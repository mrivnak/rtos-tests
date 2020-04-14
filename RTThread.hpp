#define _GNU_SOURCE
#include <string>

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