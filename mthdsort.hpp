#include "timsort.hpp"
#include <math.h>
#include <thread>

namespace mts {
    template<typename T,
             typename Cmp =
                     std::less<typename std::iterator_traits<T>::value_type>,
             typename Proj = gfx::detail::identity>
    struct mthdSortArg {
        int first;
        int middle;
        int last;
        T *ptr;
        Cmp cmp;
        Proj proj;
    };
    template<
            typename T,
            typename Cmp = std::less<typename std::iterator_traits<T>::value_type>,
            typename Proj = gfx::detail::identity>
    void *workerSort(void *arg) {
        struct mthdSortArg<T, Cmp, Proj> *p = (struct mthdSortArg<T, Cmp, Proj> *) arg;
        //  cout << "sorting：[" << p->first << "," << p->last << ")" << endl;
        gfx::timsort(p->ptr + p->first, p->ptr + p->last, p->cmp, p->proj);
        //  cout << "sorted：[" << p->first << "," << p->last << ")" << endl;
        return nullptr;
    }
    template<
            typename T,
            typename Cmp = std::less<typename std::iterator_traits<T>::value_type>,
            typename Proj = gfx::detail::identity>
    void *workerMerge(void *arg) {
        struct mthdSortArg<T, Cmp, Proj> *p = (struct mthdSortArg<T, Cmp, Proj> *) arg;
        gfx::timmerge(p->ptr + p->first, p->ptr + p->middle, p->ptr + p->last, p->cmp,
                      p->proj);
        return nullptr;
    }
    int kmerNums = 50;
    int n0 = 2;
    template<typename Proj, typename T, typename Cmp>
    std::thread thread(void *(*param)(void *), mthdSortArg<T, Cmp, Proj> *pArg);
    // 充分的把算法的各个细节想好再去编程，具体到每一步的变量、参数
    template<
            typename T,
            typename Cmp = std::less<typename std::iterator_traits<T>::value_type>,
            typename Proj = gfx::detail::identity>
    void multiThreadSort(T *array, Proj proj, Cmp cmp, int Len, int n) {

        // 多线程排序,线程数n=2^i，i=1,2,3...
        int perLen = Len / n;
        int idxArr[n * 2];
        for (int i = 1; i <= n; i++) {
            if (i == 1) {
                idxArr[0] = 0;
                idxArr[1] = perLen;
            } else if (i == n) {
                idxArr[n * 2 - 2] = (n - 1) * perLen;
                idxArr[n * 2 - 1] = Len;
            } else {
                idxArr[i * 2 - 2] = idxArr[i * 2 - 3];
                idxArr[i * 2 - 1] = i * perLen;
            }
        }
        //        cout << "[";
        //        for (int i = 0; i < n * 2; ++i) {
        //            cout << idxArr[i] << ",";
        //        }
        //        cout << "]";
        int iMax = (int) ceil(log2((float) 2 * n));// 到此一哟
        int sort_st, sort_ed, meg_f, meg_m, meg_l;
        int jMax, blockLen;
        //        cout << "\niMax:" << iMax << endl;
        for (int i = 1; i <= iMax; ++i) {
            if (i == 1) {
                // 分块排序
                std::thread sortTid[n];
                //      pthdSortArg sArg[n];
                mthdSortArg<T, Cmp, Proj> sArg[n];
                for (int j = 1; j <= n; ++j) {
                    sort_st = 2 * j - 2;
                    sort_ed = 2 * j - 1;
                    //                    cout << "sort:[" << sort_st << "," << sort_ed << ")，["
                    //                         << idxArr[sort_st] << "," << idxArr[sort_ed] << ")" << endl;
                    //        sArg[j-1]={array+idxArr[sort_st],array+idxArr[sort_ed]};
                    sArg[j - 1].ptr = array;
                    sArg[j - 1].first = idxArr[sort_st];
                    sArg[j - 1].last = idxArr[sort_ed];
                    sArg[j - 1].proj = proj;
                    sArg[j - 1].cmp = cmp;
                    sortTid[j - 1] = std::thread(mts::workerSort<T, Cmp, Proj>, &sArg[j - 1]);
                }
                for (int j = 0; j < n; j++) {
                    sortTid[j].join();
                    //        pthread_join(sortTid[j-1],NULL);// 到此一游
                }

            } else if (i == iMax) {
                // 最后1次合并
                meg_f = 0;
                meg_l = 2 * n - 1;
                meg_m = (int) exp2(iMax) / 2;
                //                cout << "merge:"
                //                     << "i=" << i << ",last:\t" << meg_f << "," << meg_m << "," << meg_l
                //                     << endl;
                gfx::timmerge(array + idxArr[meg_f], array + idxArr[meg_m],
                              array + idxArr[meg_l], cmp, proj);
            } else {
                // 中间合并
                blockLen = exp2(i);
                jMax = 2 * n / blockLen;
                std::thread midMegTid[jMax];
                //      pthdMegArg megArg[jMax];
                mts::mthdSortArg<T, Cmp, Proj> megArg[jMax];
                for (int j = 1; j <= jMax; ++j) {
                    if (j == 1) {
                        meg_f = 0;
                    } else {
                        meg_f = (j - 1) * blockLen;
                    }
                    meg_l = j * blockLen;
                    meg_m = (meg_f + meg_l) / 2;
                    meg_l--;
                    //                    cout << "merge:"
                    //                         << "i=" << i << ",j=" << j << ":\t" << meg_f << "," << meg_m << ","
                    //                         << meg_l << ". [" << idxArr[meg_f] << "," << idxArr[meg_m] << ","
                    //                         << idxArr[meg_l] << ")" << endl;

                    megArg[j - 1].ptr = array;
                    megArg[j - 1].first = idxArr[meg_f];
                    megArg[j - 1].middle = idxArr[meg_m];
                    megArg[j - 1].last = idxArr[meg_l];
                    megArg[j - 1].cmp = cmp;
                    megArg[j - 1].proj = proj;
                    midMegTid[j - 1] = std::thread(mts::workerMerge<T, Cmp, Proj>, &megArg[j - 1]);

                }
                for (int j = 0; j < jMax; ++j) {
                    midMegTid[j].join();
                }
            }
            //            cout << endl;
        }
    }
    //    template<typename Proj, typename T, typename Cmp>
    //    std::thread thread(void *(*param)(void *), mthdSortArg<T, Cmp, Proj> *pArg) {
    //        return std::thread();
    //    }


}// namespace mts