#include "bucket_list.h"
#include "noisy_allocator.h"
#include "compressed_pair.h"

#include <iostream>
#include <vector>
#include <list>
#include <cstdint>

using i64 = int64_t;

struct Test {
    i64 num;
    Test(i64 i) {
        num = i;
    }

};

int main() {
{
    noisy_allocator<>::set_granularity(log_granularity::all);

    std::vector<Test, noisy_allocator<Test>> vec;
    //std::list<Test, noisy_allocator<Test>> stdlist;
    bucket_list<Test, 32, noisy_allocator<Test>> list;

    //std::cout << "size of vector: " << sizeof(vec) << "\n";
    //std::cout << "size of stdlist: " << sizeof(stdlist) << "\n";
    //std::cout << "size of bucket_list: " << sizeof(list) << "\n";

    for(int i = 0; i < 100; ++i) {
        vec.push_back(Test(i));
        //stdlist.push_back(Test(i));
        list.push_back(Test(i));
    }

/*
    for(int i = 0; i < 100; ++i) {
        std::cout << vec[i].num << ", ";
    }
    std::cout << '\n';

    for(int i = 0; i < 100; ++i) {
        std::cout << list[i].num << ", ";
    }
*/
}
    noisy_allocator<>::get_report();
}