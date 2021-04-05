#include "log.h"
#include "config.h"
#include <map>
#include <iostream>
#include <functional>
#include <time.h>
#include <string.h>
#include <iostream>

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

logLevel::level logLevel::fromString(const std::string& str) {
#define XX(level, v) \
    if(str == #v) { \
        return logLevel::level; \
    } 
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);
 
    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return logLevel::UNKNOW;
#undef XX
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

void logEvent::format (const char* fmt, ...){
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void logEvent::format (const char* fmt, va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if( len != -1){
        m_ss << std::string(buf,len);
        free(buf);
    }
}

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
    m_formatter.reset(new logFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

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

void logger::clearAppenders() {
    m_appenders.clear();
}


void logger::setFormatter(logFormatter::ptr val) {
    m_formatter = val;

    for(auto& i : m_appenders) {
        if(!i->m_hasFormatter) {
            i->m_formatter = m_formatter;
        }
    }
}
void logger::setFormatter(const std::string& val) {
    onhway::logFormatter::ptr new_val(new onhway::logFormatter(val));
    if(new_val->isError()) {
        std::cout << "logger setFormatter name =" << m_name
                  << "value= " << val << "invalid formatter"
                  << std::endl;
    }
    setFormatter(new_val);
}
logFormatter::ptr logger::getFormatter() {
    return m_formatter;
}
std::string logger::toYamlString() {
    YAML::Node node;
    node["name"] = m_name;
    if(m_level != logLevel::UNKNOW) {
        node["level"] = logLevel::toString(m_level);
    }
    if(m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }

    for(auto& i : m_appenders) { 
        node["appender"].push_back(YAML::Load(i->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
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
    :m_filename(filename){
        reopen();
    }

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
std::string fileLogAppender::toYamlString() {
    YAML::Node node;
    node["type"] = "fileLogAppender";
    node["file"] = m_filename;
    if(m_level != logLevel::UNKNOW) {
        node["level"] = logLevel::toString(m_level);
    }
    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}
void stdoutLogAppender::log(logger::ptr logger,logLevel::level level, logEvent::ptr event) {
    if(level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
}

std::string stdoutLogAppender::toYamlString() {
    YAML::Node node;
    node["type"] = "stdoutLogAppender";
    if(m_level != logLevel::UNKNOW) {
        node["level"] = logLevel::toString(m_level);
    }
    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
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
        //std::cout<< "str" << i << ": " << str << std::endl; 
        //std::cout<< "fmt" << i << ": " << fmt <<  std::endl; 
        //std::cout<< "nstr" << i << ": " <<  nstr <<std::endl; 

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
    //for(auto& i : vec ){
    //    std::cout << "vec: " << std::get<0>(i) << " , " << std::get<1>(i) << " , " << std::get<2>(i) <<std::endl;
    //}
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

loggerManager::loggerManager(){
    m_root.reset(new logger);
    m_root->addAppender(logAppender::ptr(new stdoutLogAppender));

    m_loggers[m_root->m_name] = m_root;
}

logger::ptr loggerManager::getLogger(const std::string& name){
        auto it = m_loggers.find(name);
        if(it != m_loggers.end()){
            return it->second;
        }

        logger::ptr loggerO(new logger(name));
        loggerO->m_root = m_root;
        m_loggers[name] = loggerO;
        return loggerO;
}

std::string loggerManager::toYamlString() { 
    YAML::Node node;
    for(auto& i : m_loggers) {
        node.push_back(YAML::Load(i.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

void loggerManager::init() {
}

struct logAppenderDefine {
    int type = 0;
    logLevel::level level = logLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const logAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct logDefine {
    std::string name;
    logLevel::level level = logLevel::UNKNOW;
    std::string formatter;
    std::vector<logAppenderDefine> appenders;

    bool operator==(const logDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }

    bool operator<(const logDefine& oth) const {
        return name < oth.name;
    }

    bool isValid() const {
        return !name.empty();
    }

};

template<>
class LexicalCast<std::string, logDefine> {
public:
    logDefine operator()(const std::string& v) {
        YAML::Node n = YAML::Load(v);
        logDefine ld;
        if(!n["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << n
                      << std::endl;
            throw std::logic_error("log config name is null");
        }
        ld.name = n["name"].as<std::string>();
        ld.level = logLevel::fromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
        if(n["formatter"].IsDefined()) {
            ld.formatter = n["formatter"].as<std::string>();
        }
 
        if(n["appenders"].IsDefined()) {
            //std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
            for(size_t x = 0; x < n["appenders"].size(); ++x) {
                auto a = n["appenders"][x];
                if(!a["type"].IsDefined()) {
                    std::cout << "log config error: appender type is null, " << a
                              << std::endl;
                    continue;
                }
                std::string type = a["type"].as<std::string>();
                logAppenderDefine lad;
                if(type == "FileLogAppender") {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) {
                        std::cout << "log config error: fileappender file is null, " << a
                              << std::endl;
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                    if(a["formatter"].IsDefined()) {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } else if(type == "StdoutLogAppender") {
                    lad.type = 2; 
                    if(a["formatter"].IsDefined()) {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } else {
                    std::cout << "log config error: appender type is invalid, " << a
                              << std::endl;
                    continue;
                }
 
                ld.appenders.push_back(lad);
            }
        }
        return ld;
    }
};

template<>
class LexicalCast<logDefine, std::string> {
public:
    std::string operator()(const logDefine& i) {
        YAML::Node n;
        n["name"] = i.name;
        if(i.level != logLevel::UNKNOW) {
            n["level"] = logLevel::toString(i.level);
        }
        if(!i.formatter.empty()) {
            n["formatter"] = i.formatter;
        }
        for(auto& a : i.appenders) {
            YAML::Node na;
            if(a.type == 1) {
                na["type"] = "FileLogAppender";
                na["file"] = a.file;
            } else if(a.type == 2) {
                na["type"] = "StdoutLogAppender";
            }
            if(a.level != logLevel::UNKNOW) {
                na["level"] = logLevel::toString(a.level);
            }

            if(!a.formatter.empty()) {
                na["formatter"] = a.formatter;
            }

            n["appenders"].push_back(na);
        }
        
        std::stringstream ss;
        ss << n;
        return ss.str();
    }
        
};

onhway::ConfigVar<std::set<logDefine> >::ptr g_log_defines = 
    onhway::Config::LookUp("logs", std::set<logDefine>(), "logs config");
struct logIniter {
    logIniter() {
        g_log_defines->addListener(100, [](const std::set<logDefine>& old_value,
                    const std::set<logDefine>& new_value) {
            ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "on_logger_conf_changed";
            for(auto& i : new_value) {
                auto it = old_value.find(i);
                onhway::logger::ptr loggerO;
                if(it == old_value.end()){
                    loggerO = ONHWAY_LOG_NAME(i.name); // add new logger
                } else {
                    if(!(i == *it)) {
                        loggerO = ONHWAY_LOG_NAME(i.name); // mod logger
                    } else {
                        continue;
                    }
                }
                loggerO->setLevel(i.level);
                if(!i.formatter.empty()) {
                    loggerO->setFormatter(i.formatter);
                }

                loggerO->clearAppenders();
                for(auto& a : i.appenders) {
                    onhway::logAppender::ptr ap;
                    if(a.type == 1){
                        ap.reset(new fileLogAppender(a.file));
                    } else if(a.type == 2) {
                        ap.reset(new stdoutLogAppender);
                    }
                    ap->setLevel(a.level);
                    loggerO->addAppender(ap);
                }
            }

            for(auto& i : old_value) {
                auto it = new_value.find(i);
                if(it == new_value.end()) {
                    auto loggerO = ONHWAY_LOG_NAME(i.name); // delete logger 
                    loggerO->setLevel((logLevel::level)0);
                    loggerO->clearAppenders();
    
                }
            }
        });
    }
};

static logIniter __log_init;











































}

