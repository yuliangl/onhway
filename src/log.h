#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <ostream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <list>


namespace onhway {
	

enum logLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

#if 0
std::string toString(logLevel level){
    switch(level) {
#define XX(name) \
        case name: \
            return #name;\
            break;

        XX(DEBUG);
        XX(INFO);
        XX(WARNING);
        XX(ERROR);
        XX(FATAL);

#undef XX 
        default:
            return "UNKONW";
    }
}
#endif

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    //LogEvent();
    LogEvent(uint64_t time, const std::string& fileName, uint32_t line, const std::string& threadName, 
            uint32_t threadId, uint32_t fiberId);
    ~LogEvent();

    uint64_t getTime()          const {return m_time;}
    std::string getFileName()   const {return m_fileName;}
    uint32_t getLine()          const {return m_line;}
    std::string getThreadName() const {return m_threadName;}
    uint32_t getThreadId()      const {return m_threadId;}
    uint32_t getFiberId()       const {return m_fiberId;}
    //std::stringstream getSs() {return m_ss;}
private:
    uint64_t m_time = 0;
    std::string m_fileName = "log.txt";
    uint32_t m_line = 0;
    std::string m_threadName = "mainThread";
    uint32_t m_threadId = 0;
    uint32_t m_fiberId = 0;
    std::stringstream m_ss;
};

class LogFormat{
public:
    typedef std::shared_ptr<LogFormat> ptr;
    LogFormat();
    ~LogFormat();

    virtual void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event);
private:
    std::stringstream m_ss;
};

class LevelFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
       // os << toString(level); // have not imp yet
        std::cout << level << std::endl;
    }

};

class FileFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << event->getFileName();
    }

};

class TimeFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << event->getTime();
    }

};
class LineFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << event->getLine();
    }

};

class ThreadNameFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << event->getThreadName();
    }

};

class ThreadIdFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << event->getThreadId();
    }

};

class FiberIdFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << event->getFiberId();
    }

};

class TabFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << "\t";
    }

};

class EnterFormatItem : public LogFormat {
public:
    void formatLog(std::ostream& os, logLevel level, LogEvent::ptr event) override{
        os << "\n";
    }

};

class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    LogAppender();
    virtual ~LogAppender();
    virtual void log(logLevel level, LogEvent::ptr logEvent, std::vector<LogFormat::ptr>& vec_format);
};

class StdoutAppender : public LogAppender {
public: 
    typedef std::shared_ptr<StdoutAppender> ptr;
    StdoutAppender();
    ~StdoutAppender();

    void log(logLevel level, LogEvent::ptr logEvent, std::vector<LogFormat::ptr>& vec_format) override;
};

class FileAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileAppender> ptr;
    FileAppender();
    ~FileAppender();

    void log(logLevel level, LogEvent::ptr logEvent, std::vector<LogFormat::ptr>& vec_format) override;

};

class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name, logLevel level, const std::string& pattern, LogEvent::ptr logEvent);
    ~Logger();

    void log(logLevel level);
    void addAppender(LogAppender::ptr appender);
    void addFormat(LogFormat::ptr logFormat);
    void initLog();
    
private:
    std::string m_name = "root";
    logLevel m_level = DEBUG;
    std::string m_pattern;
    LogEvent::ptr m_logEvent;
    std::vector<LogFormat::ptr> m_format;
    std::vector<LogAppender::ptr> m_appender;

};



}

#endif
