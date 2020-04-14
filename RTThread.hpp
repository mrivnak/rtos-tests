#define _GNU_SOURCE
#include <string>

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

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags);
int sched_getattr(pid_t pid, const struct sched_attr *attr, unsigned int size, unsigned int flags);

void * priority_thread_func(void * ptr);
void * basic_thread_func(void * ptr);
void * deadline_thread_func(void * ptr);

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