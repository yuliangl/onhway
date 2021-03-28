#include "thread.h"
#include "log.h"
#include "util.h"

void fuc1(){
    
    ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "name = " << onhway::Thread::GetName()
                                     << "id = " << onhway::GetThreadId()
                                     << "this.name = " << onhway::Thread::GetThis()->GetName()
                                     << "this.id = " << onhway::Thread::GetThis()->getId();
    
}


int main(){
    ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "thread start";
    std::vector<onhway::Thread::ptr> thrs;
    for(int i=0; i<5; i++){
        onhway::Thread::ptr thr(new onhway::Thread(&fuc1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    for(int i=0; i<5; i++){
        thrs[i]->join();
    }

    ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "thread end";
    return 0;
}
