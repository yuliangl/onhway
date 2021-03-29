#include <thread.h>
#include "log.h"
#include "util.h"

namespace onhway{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)){
        throw std::logic_error("sem_init error");
    }
}
Semaphore::~Semaphore(){
    if(sem_destroy(&m_semaphore)){
        ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "sem_destroy error";
    }
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        throw std::logic_error("sem_wait error");
    }
}
void Semaphore::notify(){
    if(sem_post(&m_semaphore)){
        throw std::logic_error("sem_post error");
    }
}

Thread* Thread::GetThis(){
    return t_thread;
}
void Thread::SetName(std::string& name_){
    if(t_thread){
        t_thread_name = name_;
    }
    t_thread_name = name_;
}

std::string& Thread::GetName(){
    return t_thread_name;
}


Thread::Thread(std::function<void()> call_back, const std::string& name_ )
        :m_name(name_)
        ,m_callBack(call_back){
    if(name_.empty()){
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt){
        ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "pthread_create fail, rt = " << rt
                                          << "name = " << m_name;
        throw std::logic_error("pthread_create error(self)");
    }
    m_semaphore.wait();
}

Thread::~Thread(){
    if(m_thread){
        pthread_detach(m_thread);
    }
}

void Thread::join(){
    if(m_thread){
        int rt = pthread_join(m_thread,nullptr);
    if(rt){
        ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "pthread_join fail, rt = " << rt
                                          << "name = " << m_name;
        throw std::logic_error("pthread_join error(self)");
    }
    m_thread = 0;
}
}

void* Thread::run(void* arg){
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    thread->m_id = GetThreadId();
    t_thread_name = thread->m_name;
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> callBack;
    callBack.swap(thread->m_callBack);

    thread->m_semaphore.notify();
    callBack();

    return 0;

}

}




