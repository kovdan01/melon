#include <yaml_config.hpp>
#include <iostream>
#include <algorithm>
#include <yaml-cpp/yaml.h>

namespace melon::core::yaml_conf
{

std::vector<std::string> check_reqired_params(const std::vector<std::string> & keys )
{
    YAML::Node conf = YAML::LoadFile(conf_filename);
    std::vector<std::string> res;
    for(const std::string& key : keys)
    {
        if (!conf[key])
        {
            res.push_back(key);
        }
    }
    return res;
}

std::vector<std::string> check_superfluous_params(const std::vector<std::string> & keys )
{
    YAML::Node conf = YAML::LoadFile(conf_filename);
    std::vector<std::string> res;
    for(YAML::const_iterator it=conf.begin();it != conf.end();++it)
    {
        std::string key = (it->first).as<std::string>();
        if(std::find(keys.begin(),keys.end(),key)==keys.end())
            res.push_back(key);
    }
    return res;
}

std::pair<networking, std::vector<location>> parse_common_params()
{

    YAML::Node conf = YAML::LoadFile(conf_filename);
    networking net_res = {conf["networking"]["protocol"].as<std::string>(),
                          conf["networking"]["ip"].as<std::string>(),
                          conf["networking"]["port"].as<uint64_t>()};
    std::vector<location> loc_res;
    YAML::Node locations_node = conf["locations"];
    try {
    for(auto&& mapNode : locations_node )
    {
        location loc;
        if(mapNode["location_kind"].as<std::string>()=="local")
        {

            loc.location_kind = "local";
            loc.service_kind = mapNode["service_kind"].as<std::string>();
            loc.path = mapNode["path"].as<std::string>();
        }
        else
        {

            loc.location_kind = "remote";
            loc.service_kind = mapNode["service_kind"].as<std::string>();
            loc.ip = mapNode["ip"].as<std::string>();
            loc.protocol = mapNode["protocol"].as<std::string>();
            loc.port = mapNode["port"].as<uint64_t>();
        }
        loc_res.push_back(loc);
    }

    }
    catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    return {net_res,loc_res};
}

}  // namespace melon::core
