#ifndef __ONHWAY__UTIL_H__
#define __ONHWAY__UTIL_H__

#include <stdint.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#include <vector>
#include <string>

namespace onhway{
    
pid_t GetThreadId();

uint32_t GetFiberId();

void Backtrace (std::vector<std::string>& bt, int size, int skip=1);
std::string BacktraceToString(int size, int skip=2, const std::string& prefix="    ");

}

#endif
