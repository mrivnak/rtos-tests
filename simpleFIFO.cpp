#include <iostream>
#include <new>

#include "rtthread.hpp"

int main(int argc, char **argv) {

    std::cout << "\nNew Test\n" << std::endl;

    constexpr const int NUM_GROUPS = 8;
    RTThread* threads = (RTThread*)std::malloc(3 * NUM_GROUPS * sizeof(RTThread));

    for (int i = 0; i < 3 * NUM_GROUPS; i += 3) {
        new (threads + i) RTThread ("OTHER");
        new (threads + i + 1) RTThread("FIFO", 1);
        new (threads + i + 2) RTThread("FIFO", 99);
    }

    for (int i = 0; i < 3 * NUM_GROUPS; ++i) {
        threads[i].join();
    }

    std::free(threads);

    return 0;
}
