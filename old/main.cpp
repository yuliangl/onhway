#include <iostream>
#include "log.h"
//#include "sylar/util.h"


int main(int argc, char** argv) {
    onhway::logger::ptr logger(new onhway::logger);
    logger->addAppender(onhway::logAppender::ptr(new onhway::stdoutLogAppender));

    onhway::logEvent::ptr event(new onhway::logEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));

    logger->log(onhway::logLevel::DEBUG, event);

    std::cout<< "on the way" <<std::endl;
    return 0;
}

