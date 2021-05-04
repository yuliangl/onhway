#include "util.h"
#include "log.h"


namespace onhway {

onhway::logger::ptr g_logger = ONHWAY_LOG_NAME("system");

pid_t GetThreadId(){
    return syscall(SYS_gettid);
    
}

uint32_t GetFiberId(){
    return 0;
}

void Backtrace (std::vector<std::string>& bt, int size, int skip){
    void **array = (void**)malloc(sizeof(void*)*size);
    if(array == NULL){
        ONHWAY_LOG_ERROR(g_logger) << "backtrace array malloc error";
        return;
    }
    int nptrs = ::backtrace(array, size);

    char **strings = backtrace_symbols(array ,nptrs);
    if(strings == NULL){
        ONHWAY_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for(int i=skip; i<nptrs; i++){
        bt.push_back(strings[i]);
    }
    free(array);
    free(strings);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);

    std::stringstream ss;
    for(auto i : bt){
        ss << prefix << i << std::endl;
    }
    return ss.str();
}

}//end flag
