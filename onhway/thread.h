#ifndef __ONHWAY_THREAD_H
#define __ONHWAY_THREAD_H

#include <memory>
#include <functional>
#include <pthread.h>
#include <semaphore.h>


namespace onhway {

class Semaphore {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
private:
    sem_t m_semaphore;

};

template<class T>
struct ScopeLockImpl {
public:
    ScopeLockImpl(T& mutex_)
        :m_mutex(mutex_){
            lock();
          //m_locked = true;
        }
    ~ScopeLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked = false;
};

template<class T>
struct ReadScopeLockImpl {
public:
    ReadScopeLockImpl(T& mutex_)
        :m_mutex(mutex_){
            rdlock();
          //m_locked = true;
        }
    ~ReadScopeLockImpl(){
        unlock();
    }

    void rdlock(){
        if(!m_locked){
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked = false;
};

template<class T>
struct WriteScopeLockImpl {
public:
    WriteScopeLockImpl(T& mutex_)
        :m_mutex(mutex_){
            wrlock();
            //m_locked = true;
        }
    ~WriteScopeLockImpl(){
        unlock();
    }

    void wrlock(){
        if(!m_locked){
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked = false;
};

class Mutex {
public:
    typedef ScopeLockImpl<Mutex> Lock;
    Mutex(){
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class RWMutex {
public:
    typedef ReadScopeLockImpl<RWMutex> readlock;
    typedef WriteScopeLockImpl<RWMutex> writelock;

    RWMutex(){
        pthread_rwlock_init(&m_lock, nullptr);
    }
    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }
    void wrlock(){
        pthread_rwlock_wrlock(&m_lock);
    }
    void unlock(){
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

class SpinLock {
public:
    typedef ScopeLockImpl<SpinLock> Lock;
    SpinLock(){
        pthread_spin_init(&m_mutex, 0);
    }
    ~SpinLock(){
        pthread_spin_destroy(&m_mutex);
    }

    void lock(){
        pthread_spin_lock(&m_mutex);
    }
    void unlock(){
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

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
    Semaphore m_semaphore;
};



}

#endif
