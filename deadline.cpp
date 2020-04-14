#include <iostream>
#include <new>

#include "rtthread.hpp"

int main(int argc, char **argv) {

    std::cout << "\nNew Test\n" << std::endl;

    constexpr const int NUM_GROUPS = 16;
    RTThread* threads = (RTThread*)std::malloc(2 * NUM_GROUPS * sizeof(RTThread));

    for (int i = 0; i < 2 * NUM_GROUPS; i += 2) {
        new (threads + i) RTThread ("OTHER");
        new (threads + i + 1) RTThread("DEADLINE", 200000000, 1000000000, 1000000000);
    }

    for (int i = 0; i < 2 * NUM_GROUPS; ++i) {
        threads[i].join();
    }

    std::free(threads);

    return 0;
}