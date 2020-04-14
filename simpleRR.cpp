#include <iostream>
#include <new>

#include "RTThread.hpp"

int main(int argc, char **argv) {

    std::cout << "\nNew Test\n" << std::endl;

    constexpr const int NUM_GROUPS = 8;
    RTThread* threads = (RTThread*)std::malloc(3 * NUM_GROUPS * sizeof(RTThread));

    for (int i = 0; i < 3 * NUM_GROUPS; i += 3) {
        new (threads + i) RTThread ("OTHER");
        new (threads + i + 1) RTThread("RR", 1);
        new (threads + i + 2) RTThread("RR", 99);
    }

    for (int i = 0; i < 3 * NUM_GROUPS; ++i) {
        threads[i].join();
    }

    std::free(threads);

    return 0;
}
