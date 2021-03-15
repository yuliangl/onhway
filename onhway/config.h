#ifndef __ONHWAY_CONFIG_H__
#define __ONHWAY_CONFIG_H__

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
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
template<class T, class FromStr = LexicalCast<std::string, T> 
                , class ToStr = LexicalCast<T, std::string> >
class ConfigVar :public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
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

    T getValue() const {return m_val;}
    void setValue(const T& val_) {m_val = val_;} 
private:
    T m_val;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
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
        auto tmp = LookUp<T>(name_);
        if(tmp){
            ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "ConfigVar " << name_ << " exist.";
            return tmp;
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
