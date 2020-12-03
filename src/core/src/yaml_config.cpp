#include <yaml_config.hpp>
#include <iostream>
#include <algorithm>


namespace melon::core::yaml_conf
{



std::pair<networking, std::vector<location>> parse_common_params()
{

    YAML::Node conf = YAML::LoadFile(conf_filename);
    networking net_res = {conf["networking"]["protocol"].as<std::string>(),
                          conf["networking"]["ip"].as<std::string>(),
                          conf["networking"]["port"].as<uint64_t>()};
    std::vector<location> loc_res;
    YAML::Node locations_node = conf["locations"];
    try {
    for(YAML::Node mapNode : locations_node )
    {
        location loc;
            loc.service_kind = mapNode["service_kind"].as<std::string>();
            loc.ip = mapNode["ip"].as<std::string>();
            loc.protocol = mapNode["protocol"].as<std::string>();
            loc.port = mapNode["port"].as<uint64_t>();
        loc_res.push_back(loc);
    }

    }
    catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    return {net_res,loc_res};
}

}  // namespace melon::core
