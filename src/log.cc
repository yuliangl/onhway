#include "log.h"


namespace onhway {


LogEvent::LogEvent() {
}
LogEvent::LogEvent(uint64_t time, const std::string& fileName, uint32_t line, std::string& threadName, 
                    uint32_t threadId, int32_t fiberId) 
                    :m_time(time)
                    ,m_fileName(fileName)
                    ,m_line(line)
                    ,m_threadName(threadName)
                    ,m_threadId(threadId)
                    ,m_fiberId(fiberId) {
}
LogEvent::~LogEvent() {
}

StdoutAppender::StdoutAppender(){
}

StdoutAppender::~StdoutAppender(){
}


void StdoutAppender::log(logLevel level, std::vector<LogFormat::ptr>& vec_format) {
    for(auto i : vec_format){
        i->formatLog(std::cout, level, m_logEvent);
    }
}

Logger::Logger(const std::string& name, logLevel level, const std::string& pattern, LogEvent::ptr logEvent)
    :m_name(name)
    ,m_level(level)
    ,m_pattern(pattern)
    ,m_logEvent(logEvent) {
        addAppender(appender);
        initLog();
}

Logger::~Logger(){
}
void Logger::log(logLevel level){
    if(level >= m_level){
        for(auto i : m_appender){
            i->log(level, m_format);
        }
    }
}
void Logger::addAppender(LogAppender::ptr appender){
    m_appender.push_back(appender);
}


void Logger::addFormat(LogEvent::ptr logEvent) {
    m_format.push_back(logEvent);
}


void Logger::initLog() {
    std::string strPattern = "";
    for(uint8_t i=0; i<=strPattern.size(); ++i ){
        if (m_pattern[i] != "%")
            strPattern += m_pattern[i];
    }

    for(uint8_t i=0; i<=strPattern.size(); ++i){
#define XX(str, item) \
        if(strPattern[i] == #str) \
            m_format.push_back(LogFormat::ptr(new item));
        XX(p, LevelFormatItem)

#endif
    }
}





}
