#include "config.h"

namespace onhway {

Config::ConfigVarMap Config::s_datas;

ConfigVarBase::ptr Config::LookUpBase(const std::string& name_){
    auto it = s_datas.find(name_);
    return it == s_datas.end() ? nullptr : it->second;

}
static void ListAllMemmber(const std::string& prefix_
                          ,const YAML::Node& node_ 
                          ,std::list<std::pair<std::string, const YAML::Node> >& output_) {
    if(prefix_.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") 
            != std::string::npos){
        ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "config invalid name :" << prefix_ << ":" << node_;
           return;
    }
    output_.push_back(std::make_pair(prefix_, node_));
    if(node_.IsMap() ){
        for(auto it = node_.begin(); it != node_.end(); it++){
        ListAllMemmber(prefix_.empty() ? it->first.Scalar() : 
                prefix_ + "." + it->first.Scalar(), it->second, output_);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node& root_){
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    ListAllMemmber("", root_, all_nodes);


    for(auto i : all_nodes) {
        std::string key = i.first;
        if(key.empty())
            continue;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookUpBase(key);
        if(var){
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }


}

}
