#ifndef __ONHWAY_THREAD_H
#define __ONHWAY_THREAD_H

#include <memory>
#include <functional>
#include <pthread.h>


namespace onhway {

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> call_back, const std::string& name_);
    ~Thread();

    pid_t getId() const {return m_id;}
    std::string getName() {return m_name;}

    void join();
    static void* run(void* arg);

    static Thread* GetThis();
    static void SetName(std::string& name_); 
    static std::string& GetName();

private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::string m_name;
    std::function<void()> m_callBack;
};



}

#endif
