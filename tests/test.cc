#include <iostream>
#include "../onhway/log.h"
#include "../onhway/util.h"


int main(int argc, char** argv) {
    onhway::logger::ptr logger(new onhway::logger);
    logger->addAppender(onhway::logAppender::ptr(new onhway::stdoutLogAppender));

    onhway::logAppender::ptr fileAppender(new onhway::fileLogAppender("./log.txt"));
    logger->addAppender(fileAppender);
//    onhway::logEvent::ptr event(new onhway::logEvent(__FILE__, __LINE__, 0, onhway::GetThreadId(),  onhway::GetFiberId(), time(0)));
//    event->getSS() << "on the way";

//    logger->log(onhway::logLevel::DEBUG, event);
    ONHWAY_LOG_DEBUG(logger) << "on the way to hea";
    ONHWAY_LOG_FMT_DEBUG(logger, " test fmt %s", " hello fmt"); 
    return 0;
}

