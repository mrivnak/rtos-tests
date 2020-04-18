#include <iostream>
#include <new>

#include "RTThread.hpp"

int main(int argc, char **argv) {

    std::cout << "\nNew Test\n" << std::endl;

    constexpr const int NUM_GROUPS = 8;
    RTThread* threads = (RTThread*)std::malloc(6 * NUM_GROUPS * sizeof(RTThread));

    for (int i = 0; i < 6 * NUM_GROUPS; i += 6) {
        new (threads + i) RTThread ("OTHER");
        new (threads + i + 1) RTThread("FIFO", 5);
        new (threads + i + 2) RTThread("FIFO", 25);
        new (threads + i + 3) RTThread("FIFO", 45);
        new (threads + i + 4) RTThread("FIFO", 65);
        new (threads + i + 5) RTThread("FIFO", 85);
    }

    for (int i = 0; i < 6 * NUM_GROUPS; ++i) {
        threads[i].join();
    }

    std::free(threads);

    return 0;
}
