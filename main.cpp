#include <iostream>
#include <thread>
#include <ranges>
#include <string>
#include <time.h>
#include "mthdsort.hpp"


using namespace std;
typedef struct {
    uint64_t value;  // hash value
    unsigned int id; // sequence id
    unsigned int pos;
    short int strand; // strand:
} kmer_t;

int main(int argc, char *argv[]) {
    kmer_t *kmerList;
    int kmerNums;
    int n0;
    kmerNums=std::atoi(argv[1]);
    n0=std::atoi(argv[2]);
    kmerList = (kmer_t *)malloc(kmerNums * sizeof(kmer_t));
    for (int i = 0; i < kmerNums; i++) {
        kmerList[i].value = rand() ;
        kmerList[i].id = i;
    }
    time_t t;
    t = time(NULL);
    std::cout<<"n0:"<<n0<<endl;
    mts::multiThreadSort(kmerList, &kmer_t::value, std::ranges::less{}, kmerNums, n0);

    double cost_t = time(NULL) - t;
    cout << "all time : " << cost_t << "s" << endl;
    //  cout<<"\nafter"<<endl;
    kmer_t lastKmer;
    for (int i = 0; i < kmerNums; i++) {
        if (i == 0) {
            lastKmer = kmerList[0];
        } else {
            if (kmerList[i].value < lastKmer.value) {
                cout << "disorder:" <<i<< endl;
                break;
            }
            if (kmerList[i].value == lastKmer.value) {
                if (kmerList[i].id < lastKmer.id) {
                    cout << "unstable:" <<i<< endl;
                    break;
                }
            }
            lastKmer = kmerList[i];
        }
        //        cout<<"index:"<<i<<":id:"<<kmer100[i].id<<",value:"<<kmer100[i].value<<".";
        //        cout<<endl;
    }
    cout << "ok" << endl;



    return 0;
}
