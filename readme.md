#### 背景-Introduction

An C++ implementation for multi-threaded sorting, using TimSort algorithm

对大规模数据使用单线程进行排序非常耗间，这在一些追求效率的场景下变的不适用，多线程排序可以充分利用计算机多核资源，提高排序效率。于是老师让写个代码，对大规模数据使用多线程进行排序。在领到任务后，翻阅有没有相关的源码，果然不出所料，没有！！遂决定自己实现。由于TimSort排序方法是一种面向真实数据且稳定高效的排序方法，选择对TimSort进行多线程排序。在完成代码后打算将其开源，为开源增砖添瓦。

#### 使用-Usage

需要搭配TimSort文件使用，已在源码中提供。

函数说明：

```C++
multiThreadSort(T *array, Proj proj, Cmp cmp, int Len, int n)
```

**T*array：**Array pointer.

**Proj proj, Cmp cmp：**[projection function](https://ezoeryou.github.io/blog/article/2019-01-22-ranges-projection.html) 和 [std::less](https://en.cppreference.com/w/cpp/utility/functional/identity)

**int Len：**length of array

**int n：**线程数为2的幂（ a number of threads that is a power of 2.），例如1、2、4、8、16、32 等

下面给出了具体代码演示：

```C++
#include <ranges>
#include "mthdsort.hpp"
typedef struct {
    uint64_t value;  // hash value
    unsigned int id; // sequence id
    unsigned int pos;
    short int strand; // strand:
} kmer_t;
int main() {
  	kmer_t *kmerList;
    int kmerNums=100;
    int n0=8;
    kmerList = (kmer_t *)malloc(kmerNums * sizeof(kmer_t));
    for (int i = 0; i < kmerNums; i++) {
        kmerList[i].value = rand() ;
        kmerList[i].id = i;
    }
  mts::multiThreadSort(kmerList, &kmer_t::value, std::ranges::less{}, kmerNums, n0);
  return 0;
}
```

#### 环境-Prerequisites

- [TimSort](https://github.com/timsort/cpp-TimSort)
- C++20

#### 测试-Results

多线程TimSort对结构体排序，单位秒(seconds)、随机分布（使用rand函数）

| 数据规模\线程数thread num                | 1    | 2    | 4    | 8    | 16   | 32   |
| ---------------------------------------- | ---- | ---- | ---- | ---- | ---- | ---- |
| 1亿（100 million）                       | 254s | 139s | 76s  | 47s  | 32s  | 25s  |
| 5亿（500 million）                       | --   | --   | 397s | 242s | 166s | 129s |
| -- 时间长，不再测了（ too long to test） |      |      |      |      |      |      |

具体测试过程

提供一个测试的程序（multisotr），来进行多线程排序测试，使用方法如下：

第一个参数为数据规模，第二个参数为线程数。下面给出了一个具体的运行例子作为参考

```bash
./multiSotr 500000000 4
```

#### 方法-Methods

<img src="https://cdn.51dream.top/blog/image-20230331194516128.png" alt="image-20230331194516128" style="zoom:50%;" />

#### 存在问题

静态编译后，出现段地址错误（segmentation-fault）：解决方法使用的是动态编译

https://stackoverflow.com/questions/35116327/when-g-static-link-pthread-cause-segmentation-fault-why

#### 参考文献

[[1]范兴国,卫泽刚,张豪等.常见排序算法多线程实现与比较[J].宝鸡文理学院学报(自然科学版),2022,42(04):42-49.DOI:10.13467/j.cnki.jbuns.2022.04.007.](https://cdn.51dream.top/blog/%E5%B8%B8%E8%A7%81%E6%8E%92%E5%BA%8F%E7%AE%97%E6%B3%95%E5%A4%9A%E7%BA%BF%E7%A8%8B%E5%AE%9E%E7%8E%B0%E4%B8%8E%E6%AF%94%E8%BE%83_%E8%8C%83%E5%85%B4%E5%9B%BD.pdf)