/*                                                                  
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */
 
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include <chrono>
#include <new>

typedef struct {
    int thread_id;
    int priority;
    std::string policy_string;
    int runtime;
    int deadline;
} thread_msg;

void * priority_thread_func(void * ptr);
void * basic_thread_func(void * ptr);

class RTThread {
    private:
        static int id;
        int thread_id = 0;
        struct sched_param param;
        pthread_attr_t attr;
        pthread_t thread;
        int policy;
        std::string policy_string;
        bool suspended = false;

    public:
        RTThread(std::string sched_policy);
        RTThread(int priority, std::string sched_policy);
        int getID();
        void join();
        void suspend();
        void resume();
};

int RTThread::id = 0;

RTThread::RTThread(int priority, std::string sched_policy) {
    if (sched_policy == "FIFO") {
        this->policy_string = "SCHED_FIFO";
        this->policy = SCHED_FIFO;
    }
    else if (sched_policy == "RR") {
        this->policy_string = "SCHED_RR";
        this->policy = SCHED_RR;
    }
    else {
        std::cerr << "Invalid policy/arguments" << std::endl;
        exit(-1);
    }

    this->thread_id = RTThread::id;
    RTThread::id++;

    thread_msg * msg = new thread_msg;
    msg->thread_id = this->thread_id;
    msg->policy_string = this->policy_string;
    msg->priority = priority;

    int ret;

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        exit(ret);
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("pthread setstacksize failed\n");
        exit(ret);
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, this->policy);
    if (ret) {
        printf("pthread setschedpolicy failed\n");
        exit(ret);
    }
    param.sched_priority = priority;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        printf("pthread setschedparam failed\n");
        exit(ret);
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        exit(ret);
    }

    /* Create a pthread with specified attributes */
    ret = pthread_create(&thread, &attr, priority_thread_func, msg);
    if (ret) {
        printf("create pthread failed\n");
        exit(ret);
    }
}

RTThread::RTThread(std::string sched_policy) {
    if (sched_policy == "OTHER") {
            this->policy_string = "SCHED_OTHER";
            this->policy = SCHED_OTHER;
    }
    else if (sched_policy == "BATCH") {
            this->policy_string = "SCHED_BATCH";
            this->policy = SCHED_BATCH;
    }
    else if (sched_policy == "IDLE") {
            this->policy_string = "SCHED_IDLE";
            this->policy = SCHED_IDLE;
    }
    else {
        std::cerr << "Invalid policy/arguments" << std::endl;
        exit(-1);
    }

    this->thread_id = RTThread::id;
    RTThread::id++;

    thread_msg * msg = new thread_msg;
    msg->thread_id = this->thread_id;
    msg->policy_string = this->policy_string;

    int ret;

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        exit(ret);
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("pthread setstacksize failed\n");
        exit(ret);
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, this->policy);
    if (ret) {
        printf("pthread setschedpolicy failed\n");
        exit(ret);
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        exit(ret);
    }

    /* Create a pthread with specified attributes */
    ret = pthread_create(&thread, &attr, basic_thread_func, msg);
    if (ret) {
        printf("create pthread failed\n");
        exit(ret);
    }
}

int RTThread::getID() {
    return this->thread_id;
}

void RTThread::join() {
    pthread_join(this->thread, NULL);
}

void * priority_thread_func(void * ptr) {
    thread_msg data = *(thread_msg *)ptr;

    float * p;
    int * q;

    auto start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    for (int i = 0; i < 1000000; i++) {
        for(int j = 0; j < 1000; j++) {
            (*p++)+1.144564542315345;
            (*q++)+1;
        }
    }

    std::cout << '[' << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch() - start_time).count() << ']' << \
        " Thread: " << data.thread_id << \
        "\tSched Policy: " << data.policy_string << \
        "\tPriority: " << data.priority << std::endl;

    return NULL;
}

void * basic_thread_func(void * ptr) {
    thread_msg data = *(thread_msg *)ptr;
    float * p;
    int * q;

    auto start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    for (int i = 0; i < 1000000; i++) {
        for(int j = 0; j < 1000; j++) {
            (*p++)+1.144564542315345;
            (*q++)+1;
        }
    }

    std::cout << '[' << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch() - start_time).count() << ']' << \
        " Thread: " << data.thread_id << \
        "\tSched Policy: " << data.policy_string << std::endl;

    return NULL;
}

int main(int argc, char **argv) {

    std::cout << "\nNew Test\n" << std::endl;

    constexpr const int NUM_GROUPS = 8;
    RTThread* threads = (RTThread*)std::malloc(3 * NUM_GROUPS * sizeof(RTThread));

    for (int i = 0; i < 3 * NUM_GROUPS; i += 3) {
        new (threads + i) RTThread ("OTHER");
        new (threads + i + 1) RTThread(1, "FIFO");
        new (threads + i + 2) RTThread(99, "FIFO");
    }

    for (int i = 0; i < 3 * NUM_GROUPS; ++i) {
        threads[i].join();
    }

    std::free(threads);

    return 0;
}
