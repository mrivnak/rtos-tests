#define _GNU_SOURCE
#include <chrono>
#include <cstring>
#include <iostream>
#include <limits.h>
#include <linux/sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

typedef struct {
    int thread_id;
    int priority;
    std::string policy_string;
    int runtime;
    int deadline;
    int period;
} thread_msg;

struct sched_attr {
    uint32_t size;

    uint32_t sched_policy;
    uint64_t sched_flags;

    /* SCHED_NORMAL, SCHED_BATCH */
    int32_t sched_nice;

    /* SCHED_FIFO, SCHED_RR */
    uint32_t sched_priority;

    /* SCHED_DEADLINE (nsec) */
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
   return syscall(__NR_sched_setattr, pid, attr, flags);
}

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
        int runtime;
        int deadline;
        int period;

    public:
        RTThread(std::string sched_policy);
        RTThread(std::string sched_policy, int priority);
        RTThread(std::string, int runtime, int deadline, int period);
        int getID();
        void join();
        void suspend();
        void resume();
};

int RTThread::id = 0;

RTThread::RTThread(std::string sched_policy, int priority) {
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

RTThread::RTThread(std::string sched_policy, int runtime, int deadline, int period) {
    if (sched_policy == "DEADLINE") {
            this->policy_string = "SCHED_DEADLINE";
            this->policy = SCHED_DEADLINE;
            this->runtime = runtime;
            this->deadline = deadline;
            this->period = period;
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
    msg->runtime = this->period;
    msg->deadline = this->deadline;
    msg->period = this->period;

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
    ret = pthread_create(&thread, &attr, deadline_thread_func, msg);
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

void * deadline_thread_func(void * ptr) {
    thread_msg data = *(thread_msg *)ptr;

    int ret;
    int flags = 0;
    struct sched_attr attr;

    memset(&attr, 0, sizeof(attr)); 
    attr.size = sizeof(attr);

    /* This creates a 200ms / 1s reservation */
    attr.sched_policy   = SCHED_DEADLINE;
    attr.sched_runtime  = data.runtime;
    attr.sched_deadline = data.deadline;
    attr.sched_period   = data.period;

    ret = sched_setattr(0, &attr, flags);
    if (ret < 0) {
        perror("sched_setattr failed to set the priorities");
        exit(-1);
    }


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
    int duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch() - start_time).count();
    std::cout << '[' << duration << ']' << \
        " Thread: " << data.thread_id << \
        "\tSched Policy: " << data.policy_string << \
        "\tRuntime: " << data.runtime << \
        "\tDeadline: " << data.deadline << \
        "\tPeriod: " << data.period << \
        "\tDeadline: " << (duration < data.deadline ? GREEN "Made" RESET : RED "Missed" RESET) << std::endl;

    return NULL;
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