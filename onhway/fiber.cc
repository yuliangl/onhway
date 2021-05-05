#include <atomic>
#include "fiber.h"
#include "macro.h"
#include "log.h"
#include "thread.h"
//#include "config.h"


namespace onhway{

static std::atomic<uint64_t> s_fib{0};
static std::atomic<uint64_t> s_fiber_count{0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::LookUp<uint32_t>("fiber.stacksize", 1024 * 1024, "fiber stack size");

typedef struct MallocStackAllocater{
    static void* Alloc(uint32_t stack_size){
        return malloc(stack_size);
    }
    static void DelAlloc(void* stack_p, uint32_t tack_size){
        free(stack_p);
    }

} StackAllocator;

Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);
    if(getcontext($m_ctx)){
        onhway_assert(false);
    }
    ++s_fiber_count;
}
Fiber::Fiber(std::function<void()> c_b, uint32_t stack_size)
    :m_fid(++s_fib)
    ,m_cb(c_b){
    ++s_fiber_count;
    m_stackSize = stack_size ? stack_size : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stackSize);

    if(getcontext(&m_ctx)){
        onhway_assert(false);
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;

    makecontext(&m_ctx, Fiber::MainFunc, 0);

}
Fiber::~Fiber(){
    if(m_stack){
        onhway_assert(m_state == INIT 
                || m_state == TERM
                || m_state == EXCEPT)
        StackAllocator::DelAlloc(m_stack, m_stackSize);
        m_stack = nullptr;
    }else{
        onhway_assert(m_cb);
        onhway_assert(m_state == EXEC);
        Fiber* cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
        onhway_assert1(false, "~Fiber, cur != this");
    }
    --s_fiber_count;
}

void Fiber::reset(std::function<void()> c_b){
}
void Fiber::swapIn(){
}
void Fiber::swapOut(){
}

void Fiber::YieldToHold(){
}
void Fiber::YieldToReady(){
}
void Fiber::TotalFibers(){
}
Fiber::ptr Fiber::GetThis(){
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber)
    onhway_assert(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return ;
}
void SetThis(Fiber* f){
    t_fiber = f;
}
void Fiber::MainFunc(){
}

}//end flag
