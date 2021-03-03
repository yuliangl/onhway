#include "log.h"
#include <map>
#include <iostream>
#include <functional>
#include <time.h>
#include <string.h>

namespace onhway{

const char* logLevel::toString(logLevel::level level){
    switch(level){
#define xx(name)\
    case logLevel::name:\
        return #name;\
        break;

    xx(INFO);
    xx(DEBUG);
    xx(WARN);
    xx(ERROR);
    xx(FATAL);
#undef xx
    default:
        return "UNKNOW";

    }
    return "UNKNOW";
}
logEvent::logEvent(std::shared_ptr<logger> logger, logLevel::level level
        , const char* file, int32_t line, uint32_t elapse 
        ,uint32_t thread_id, uint32_t fiber_id, uint64_t time)
    :m_file(file)
    ,m_line(line)
    ,m_elapse(elapse)
    ,m_threadId(thread_id)
    ,m_fiberId(fiber_id)
    ,m_time(time) 
    ,m_logger(logger) 
    ,m_level(level) {}


logEventWrap::logEventWrap(logEvent::ptr e)
    :m_event(e){
}
logEventWrap::~logEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& logEventWrap::getSS(){
    return m_event->getSS();
}

logger::logger(const std::string& name)
    :m_name(name)
     ,m_level(logLevel::DEBUG){
    m_formatter.reset(new logFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

}
void logger::log(logLevel::level level, logEvent::ptr event){
    if(level >= m_level){
        auto self = shared_from_this();
    for( auto& i : m_appenders){
        i->log(self,level, event);
    }
    }
}
void logger::addAppender(logAppender::ptr appender){
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);

}
void logger::delAppender(logAppender::ptr appender){
    for(auto it = m_appenders.begin();
            it != m_appenders.end(); it++){
        if (*it == appender){
            m_appenders.erase(it);
            break;
        }

    }
}
void logger::info(logEvent::ptr event){
    log(logLevel::INFO, event);
}

void logger::debug(logEvent::ptr event){
    log(logLevel::DEBUG, event);
}
void logger::warn(logEvent::ptr event){
    log(logLevel::WARN, event);
}

void logger::error(logEvent::ptr event){
    log(logLevel::ERROR, event);
}
void logger::fatal(logEvent::ptr event){
    log(logLevel::FATAL, event);
}


fileLogAppender::fileLogAppender(const std::string& filename)
    :m_filename(filename){}


bool fileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;  // why use double !! ??????????????
}
void fileLogAppender::log(logger::ptr logger,logLevel::level level, logEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_formatter->format(logger, level, event);
    }
} 
void stdoutLogAppender::log(logger::ptr logger,logLevel::level level, logEvent::ptr event) {
    if(level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
}

logFormatter::logFormatter(const std::string& pattern)
    :m_pattern(pattern){
        init();
}

std::string logFormatter::format(logger::ptr logger, logLevel::level level, logEvent::ptr event){
    std::stringstream ss;
    for(auto i : m_items){
       i->format(ss, logger, level, event); 
    }
    return ss.str();
}

class messageFormatItem: public logFormatter::formatItem{
public:
    messageFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << event->getContent();
    }
};

class levelFormatItem: public logFormatter::formatItem{
public:
    levelFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << logLevel::toString(level);

    }

};

class elapseFormatItem: public logFormatter::formatItem{
public:
    elapseFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << event->getElapse();
    }
};

class nameFormatItem: public logFormatter::formatItem{
public:
    nameFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << logger->getName();
    }
};

class threadIdFormatItem: public logFormatter::formatItem{
public:
    threadIdFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class fiberIdFormatItem: public logFormatter::formatItem{
public:
    fiberIdFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class dateTimeFormatItem: public logFormatter::formatItem{
public:
    dateTimeFormatItem(const std::string& format = "%Y-%M-%D %H:%M:%S")
        :m_format(format){
        if(m_format.empty()) {
                   m_format = "%Y-%m-%d %H:%M:%S";
               }
    }
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};


class fileNameFormatItem: public logFormatter::formatItem{
public:
    fileNameFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << event->getFile();
    }
};

class lineFormatItem: public logFormatter::formatItem{
public:
    lineFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << event->getLine();
    }
};

class newLineFormatItem: public logFormatter::formatItem{
public:
    newLineFormatItem(const std::string& str="") {}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << std::endl;
    }
};

class stringFormatItem: public logFormatter::formatItem{
public:
    stringFormatItem(const std::string& str)
        :m_string(str){
        }
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class tabFormatItem: public logFormatter::formatItem{
public:
    tabFormatItem(const std::string& str=""){}
    void format(std::ostream& os, logger::ptr logger, logLevel::level level, logEvent::ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};

void logFormatter::init(){
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
     static std::map<std::string, std::function<formatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return formatItem::ptr(new C(fmt));}}

        XX(m, messageFormatItem),           //m:消息
        XX(p, levelFormatItem),             //p:日志级别
        XX(r, elapseFormatItem),            //r:累计毫秒数
        XX(c, nameFormatItem),              //c:日志名称
        XX(t, threadIdFormatItem),          //t:线程id
        XX(n, newLineFormatItem),           //n:换行
        XX(d, dateTimeFormatItem),          //d:时间
        XX(f, fileNameFormatItem),          //f:文件名
        XX(l, lineFormatItem),              //l:行号
        XX(T, tabFormatItem),               //T:Tab
        XX(F, fiberIdFormatItem),           //F:协程id
       // XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };
    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(formatItem::ptr(new stringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(formatItem::ptr(new stringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }

}

}

