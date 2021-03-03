#ifndef __ONHWAY__UTIL_H__
#define __ONHWAY__UTIL_H__

#include <stdint.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

namespace onhway{
    
pid_t GetThreadId();

uint32_t GetFiberId();


}

#endif
