#ifndef __ONHWAY_LOG_H__
#define __ONHWAY_LOG_H__

#include <string>
#include <memory>
#include <stdint.h>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "util.h"
#include "singleton.h"


#define ONHWAY_LOG_LEVEL(logger, level)\
    if(logger->getLevel() <= level)\
        onhway::logEventWrap(onhway::logEvent::ptr(new onhway::logEvent(logger, level,__FILE__, __LINE__, 0, onhway::GetThreadId(), onhway::GetFiberId(), time(0)))).getSS()

#define ONHWAY_LOG_INFO(logger)  ONHWAY_LOG_LEVEL(logger, onhway::logLevel::INFO)
#define ONHWAY_LOG_DEBUG(logger) ONHWAY_LOG_LEVEL(logger, onhway::logLevel::DEBUG)
#define ONHWAY_LOG_WARN(logger)  ONHWAY_LOG_LEVEL(logger, onhway::logLevel::WARN)
#define ONHWAY_LOG_ERROR(logger) ONHWAY_LOG_LEVEL(logger, onhway::logLevel::ERROR)
#define ONHWAY_LOG_FATAL(logger) ONHWAY_LOG_LEVEL(logger, onhway::logLevel::FATAL)

#define ONHWAY_LOG_FMT_LEVEL(logger, level, fmt, ...)\
    if(logger->getLevel() <= level)\
        onhway::logEventWrap(onhway::logEvent::ptr(new onhway::logEvent(logger, level,__FILE__, __LINE__, 0, onhway::GetThreadId(), onhway::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define ONHWAY_LOG_FMT_INFO(logger , fmt, ...) ONHWAY_LOG_FMT_LEVEL(logger, onhway::logLevel::INFOi, fmt, __VA_ARGS__)
#define ONHWAY_LOG_FMT_DEBUG(logger, fmt, ...) ONHWAY_LOG_FMT_LEVEL(logger, onhway::logLevel::DEBUG, fmt, __VA_ARGS__)
#define ONHWAY_LOG_FMT_WARN(logger , fmt, ...) ONHWAY_LOG_FMT_LEVEL(logger, onhway::logLevel::WARN , fmt, __VA_ARGS__)
#define ONHWAY_LOG_FMT_ERROR(logger, fmt, ...) ONHWAY_LOG_FMT_LEVEL(logger, onhway::logLevel::ERROR, fmt, __VA_ARGS__)
#define ONHWAY_LOG_FMT_FATAL(logger, fmt, ...) ONHWAY_LOG_FMT_LEVEL(logger, onhway::logLevel::FATAL, fmt, __VA_ARGS__)

#define ONHWAY_LOG_ROOT onhway::loggerMgr::GetInstance()->getRoot()

namespace onhway{

class logger;

class logLevel{
public:
	typedef std::shared_ptr<logLevel> ptr;
	enum level{
        UNKNOW = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
		
	};
    static const char* toString(logLevel::level);
};

class logEvent{
public:
    typedef std::shared_ptr<logEvent> ptr;
    logEvent(std::shared_ptr<logger> logger, logLevel::level level
            ,const char* file, int32_t line, uint32_t elapse,
            uint32_t thread_id, uint32_t fiber_id, uint64_t time);

    const char* getFile()              const {return m_file;}
    int32_t getLine()                  const {return m_line;}
    uint32_t getElapse()               const {return m_elapse;}
    uint32_t getThreadId()             const {return m_threadId;}
    uint32_t getFiberId()              const {return m_fiberId;}
    uint64_t getTime()                 const {return m_time;}

    logLevel::level getLevel()         const {return m_level;}
    std::string getContent()           const {return m_ss.str();}
    std::shared_ptr<logger> getLogger()const {return m_logger;}

    std::stringstream& getSS()               {return m_ss;}
    void format (const char* fmt, ...);
    void format (const char* fmt, va_list al);
private:
    const char* m_file = nullptr;
    int32_t m_line = 0;
    uint32_t m_elapse =0;
    uint32_t m_threadId =0;
    uint32_t m_fiberId =0;
    uint64_t m_time = 0;
    std::stringstream m_ss;
    std::shared_ptr<logger> m_logger;
    logLevel::level m_level;

};

class logEventWrap{
public:
    logEventWrap(logEvent::ptr e);
    ~logEventWrap();
    std::stringstream& getSS();
    logEvent::ptr getEvent() const {return m_event;}
private:
    logEvent::ptr m_event;
};

class logFormatter{
public:
    typedef std::shared_ptr<logFormatter> ptr;
    logFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<logger> logger, logLevel::level level,logEvent::ptr event);
    class formatItem{
    public:
        typedef std::shared_ptr<formatItem> ptr;
//        virtual ~formatItem();
        virtual void format(std::ostream& os, std::shared_ptr<logger>  logger, logLevel::level level, logEvent::ptr event) = 0;

    };
    void init();
    std::string getPattern() const {return m_pattern;}

private:
    std::string m_pattern;
    std::vector<formatItem::ptr> m_items;
    bool m_error = false;

};

class logAppender {
public:
	typedef std::shared_ptr<logAppender> ptr;
//	virtual ~logAppender();
    virtual void log(std::shared_ptr<logger>  logger, logLevel::level level, logEvent::ptr event) = 0;

    void setFormatter(logFormatter::ptr val) {m_formatter = val;}
    logFormatter::ptr getFormatter() const {return m_formatter;}
    logLevel::level getLevel() const {return m_level;}
    void setLevel(logLevel::level level) {m_level = level;}
protected:
	logLevel::level m_level;
    logFormatter::ptr m_formatter;

};

class logger: public std::enable_shared_from_this<logger>{
public:
    typedef std::shared_ptr<logger> ptr;
    logger(const std::string& name = "ROOT");
    void log(logLevel::level level, logEvent::ptr event);

    void info(logEvent::ptr);
    void debug(logEvent::ptr);
    void warn(logEvent::ptr);
    void error(logEvent::ptr);
    void fatal(logEvent::ptr);

    void addAppender(logAppender::ptr appender);
    void delAppender(logAppender::ptr appender);
    logLevel::level  getLevel() const {return m_level;}
    void setLevel(logLevel::level val) {m_level = val;}

    const std::string& getName() const {return m_name;}
    logEvent::ptr getEvent() const {return m_event;}
private:
    std::string m_name;
    logLevel::level m_level;
    std::list<logAppender::ptr> m_appenders;
    logFormatter::ptr m_formatter;
    logEvent::ptr m_event;

};


class stdoutLogAppender:public logAppender{
public:
    typedef std::shared_ptr<stdoutLogAppender> ptr;
    void log(logger::ptr logger,logLevel::level level, logEvent::ptr event) override;
private:
	
};

class fileLogAppender:public logAppender{
public:
    typedef std::shared_ptr<fileLogAppender> ptr;
    fileLogAppender(const std::string& filename);
    void log(logger::ptr logger,logLevel::level level, logEvent::ptr event) override;
    bool reopen();
    
private:
    std::string m_filename;
    std::ofstream m_filestream;

};

class loggerManager{
public:
    loggerManager();
    logger::ptr getLogger(const std::string& name);

    logger::ptr getRoot() const {return m_root;}
    void init();
private:
    std::map<std::string, logger::ptr> m_loggers;
    logger::ptr m_root;
};

typedef onhway::Singleton<loggerManager> loggerMgr; 

}

#endif
