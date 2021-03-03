#ifndef __ONHWAY_LOG_H__
#define __ONHWAY_LOG_H__

#include <string>
#include <memory>
#include <stdint.h>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>


namespace onhway{

class logLevel{
public:
	typedef std::shared_ptr<logLevel> ptr;
	enum level{
        UNKNOW = 0,
		INFO = 1,
		DEBUG = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
		
	};
    static const char* toString(logLevel::level);
};

class logger: public std::enable_shared_from_this<logger>{
public:
	typedef std::shared_ptr<logger> ptr;
    logger(const std::string& name = "ROOT");
    void log(logLevel::ptr level, logEvent::ptr event);

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
private:
	std::string m_name;
	logLevel::level m_level;
	std::list<logAppender::ptr> m_appenders; 
    logFormatter::ptr m_formatter;

}; 

class logAppender {
public:
	typedef std::shared_ptr<logAppender> ptr;
	virtual ~logAppender();
    virtual void log(logger::ptr logger, logLevel::level level, logEvent::ptr event) = 0;

    void setFormatter(logFormatter::ptr val) {return m_formatter = val;}
    logFormatter::ptr getFormatter() const {return m_formatter;}
private:
	logLevel::level m_level;
    logFormatter::ptr m_formatter;

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
class logFormatter{
public:
	typedef std::shared_ptr<logFormatter> ptr;
	logFormatter(const std::string& pattern);
	std::string format(logEvent::ptr event);
    class formatItem{
    public:
        typedef std::shared_ptr<formatItem> ptr;
        virtual ~formatItem();
        virtual void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) = 0;

    };
    void init();
    std::string getPattern() const {return m_pattern;}

private:
    std::string m_pattern;
    std::vector<formatItem::ptr> m_item;
    
    

};

class logEvent{
public:
	typedef std::shared_ptr<logEvent> ptr;
	logEvent();

    const char* getFile()          const {return m_file;}
    int32_t getLine()              const {return m_line;}
    uint32_t getElapse()           const {return m_elapse;}
    uint32_t getThreadId()         const {return m_threadId;}
    uint32_t getFiberId()          const {return m_fiberId;}
    uint64_t getTime()             const {return m_time;}
    const std::string getContent() const {return m_content;}


private:
	const char* m_file = nullptr;
	int32_t m_line = 0;
	uint32_t m_elapse =0;
	uint32_t m_threadId =0;
	uint32_t m_fiberId =0;
	uint64_t m_time = 0;
	std::string m_content; 

};



#endif
