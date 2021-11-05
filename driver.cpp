#include "bucket_list.h"

#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec1;
    bucket_list<int> list;

    std::cout << sizeof(vec1);
    std::cout << '\n';
    std::cout << sizeof(list);
    std::cout << '\n';

    for(int i = 0; i < 100; ++i) {
        //vec1.push_back(i);
        list.push_back(i);
    }


/*
    for(int i = 0; i < 100; ++i) {
        std::cout << vec1[i] << ', ';
    }

    std::cout << '\n';
*/

    for(int i = 0; i < 100; ++i) {
        std::cout << list[i] << ", ";
    }
}
