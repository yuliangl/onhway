#ifndef __ONHWAY_CONFIG_H__
#define __ONHWAY_CONFIG_H__

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "log.h"

namespace onhway {

class ConfigVarBase {
public: 
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name_, const std::string& description_)
        :m_name(name_)
        ,m_description(description_) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
         }

    const std::string getName() const {return m_name;}
    const std::string getDescription() const {return m_description;}
    
    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val_) = 0;
    virtual std::string getTypeName() const = 0;
    
private:
    std::string m_name;
    std::string m_description;
};
template <class F, class T>
class LexicalCast {
public:
    T operator() (const F& v_) {
        return boost::lexical_cast<T> (v_);
    }
};

//support vector
template <class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator() (const std::string& v_){
        YAML::Node node = YAML::Load(v_);
        std::vector<T> vec;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T>& v_){
        YAML::Node node;
        for(auto i : v_){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//support list
template <class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& v_){
        YAML::Node node = YAML::Load(v_);
        std::list<T> vec;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator() (const std::list<T>& v_){
        YAML::Node node;
        for(auto i : v_){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//support set
template <class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& v_){
        YAML::Node node = YAML::Load(v_);
        std::set<T> vec;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T>& v_){
        YAML::Node node;
        for(auto i : v_){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//support unordered_set
template <class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& v_){
        YAML::Node node = YAML::Load(v_);
        std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator() (const std::unordered_set<T>& v_){
        YAML::Node node;
        for(auto i : v_){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//support map
template <class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& v_){
        YAML::Node node = YAML::Load(v_);
        std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); it++){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                    LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator() (const std::map<std::string, T>& v_){
        YAML::Node node;
        for(auto i : v_){
            node[i->first] = 
                YAML::Load(LexicalCast<T, std::string>()(i->second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//support unordered_map
template <class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& v_){
        YAML::Node node = YAML::Load(v_);
        std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); it++){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                    LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator() (const std::unordered_map<std::string, T>& v_){
        YAML::Node node;
        for(auto i : v_){
            node[i->first] = 
                YAML::Load(LexicalCast<T, std::string>()(i->second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T, class FromStr = LexicalCast<std::string, T> 
                , class ToStr = LexicalCast<T, std::string> >
class ConfigVar :public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void(const T& old_value, const T& new_vulue)> onChangeCb;
    ConfigVar(const std::string& name_ 
            ,const T& default_value 
            ,const std::string& description_)
        :ConfigVarBase(name_, description_)
        ,m_val(default_value){
        }
    std::string toString() override {
        try{
            //return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        }catch(std::exception& e){
            ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "ConfigVar::toString() :"
                << e.what() << "convert" << typeid(m_val).name() << " toString";
        }
        return "";
    }
    
    bool fromString(const std::string& val_) override {
        try{
            //m_val = boost::lexical_cast<T>(val_);
            setValue(FromStr()(val_));
        }catch(std::exception& e){
            ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "ConfigVar::fromString() :"
                << e.what() << "convert:" << typeid(m_val).name() << " fromString";
        }
        return false;
    }

    void setValue(const T& val_) {
        if(m_val == val_){
            return;
        }
        for(auto& i : m_cbs){
            i.second(m_val, val_);
        }

        m_val = val_;
    } 

    void addListener(uint64_t key_, onChangeCb cb_){
        m_cbs[key_] = cb_;
    }

    onChangeCb getListener(uint64_t name_){
        auto it = m_cbs.find(name_);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void deListener(uint64_t key_){
        m_cbs.erase(key_);
    }

    void clearListener()              {m_cbs.clear();}
    std::string getTypeName() const override {return typeid(T).name();}
    T getValue()                const {return m_val;}
private:
    T m_val;
    std::map<uint64_t, onChangeCb> m_cbs;
};

class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    template<class T>
    static typename ConfigVar<T>::ptr LookUp(const std::string& name_){
        auto it = s_datas.find(name_);
        if(it == s_datas.end())
            return nullptr;
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }
    template<class T>
    static typename ConfigVar<T>::ptr LookUp(const std::string& name_
                                            ,const T& value_
                                            ,const std::string& description_){
        auto it = s_datas.find(name_);
        if(it != s_datas.end()){
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp){
                ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "ConfigVar " << name_ << " exist.";
                return tmp;
            }else{
                ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "LookUp name" << name_ 
                    << "exist but type is not " << typeid(T).name() 
                    << "real type is " << it->second->getTypeName();
                return nullptr;
            }
        }
        if(name_.find_first_not_of("abcdefghigklmnopqrstuvwxyz._012345678")
                != std::string::npos){
            ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << name_ << ": name is inviled";
            throw std::invalid_argument(name_);
        }
        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name_, value_, description_));
        s_datas[name_] = v;
        return v;

    }
    static void LoadFromYaml(const YAML::Node& root_);
    static ConfigVarBase::ptr LookUpBase(const std::string& name_);
private:
    static ConfigVarMap s_datas;
}; 
















}
#endif
