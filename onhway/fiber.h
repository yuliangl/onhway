#ifndef _ONHWAY_FIBER_H_
#define _ONHWAY_FIBER_H_

#include <memory>
#include <functional>
#include <ucontext.h>

namespace onhway{

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum state{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };
private:
    Fiber();
public:
    Fiber(std::function<void()> c_b, uint32_t stack_size = 0);
    ~Fiber();

    void reset(std::function<void()> c_b);
    void swapIn();
    void swapOut();
    uint64_t getFid() const {return m_fid;}

    static void YieldToHold();
    static void YieldToReady();
    static uint64_t TotalFibers();
    static Fiber::ptr GetThis();
    static void SetThis(Fiber* f);
    static void MainFunc();
private:
    ucontext_t m_ctx;
    uint64_t m_fid = 0;
    uint32_t m_stackSize = 0;
    state m_state = INIT;

    std::function<void()> m_cb;
    void* m_stack = nullptr;
};


}//end flag

#endif
