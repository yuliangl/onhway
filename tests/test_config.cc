#include <yaml-cpp/yaml.h>
#include "log.h"
#include "config.h"

onhway::ConfigVar<int>::ptr g_int_value = onhway::Config::LookUp(
                                            "system.port"
                                            ,int(8080)
                                            ,"system port");
onhway::ConfigVar<std::vector<int> >::ptr g_vec_int_value = onhway::Config::LookUp(
                                            "system.int_vec"
                                            ,std::vector<int>{1, 2}
                                            ,"system vector");
void testyaml() {
    YAML::Node root = YAML::LoadFile("../bin/conf/log.yml");
    ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << root;
}
void testConfig() {
    ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "before:" <<  g_int_value->toString();
    auto v = g_vec_int_value->getValue();
    for(auto& i : v){
        ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "before:" << i; 
    }

    YAML::Node root = YAML::LoadFile("../bin/conf/log.yml");
    onhway::Config::LoadFromYaml(root);
    v = g_vec_int_value->getValue();
    for(auto& i : v){
        ONHWAY_LOG_INFO(ONHWAY_LOG_ROOT) << "after:" << i; 
    }
}
int main() {

    //testyaml();
    testConfig();
    return 0;
}

