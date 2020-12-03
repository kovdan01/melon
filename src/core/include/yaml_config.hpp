#ifndef MELON_CORE_YAML_CONFIG_HPP_
#define MELON_CORE_YAML_CONFIG_HPP_

#include <melon/core/export.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <vector>

namespace melon::core::yaml_conf
{
const std::string conf_filename = "m_config.yaml";


//  Returns a vector of missing strings
template <typename It>
MELON_CORE_EXPORT std::vector<std::string> check_missing_params(YAML::Node conf, const It req_begin, const It req_end)
{
    std::vector<std::string> res;
    for( It it = req_begin; it != req_end; ++it )
    {
        if (!conf[*it])
        {
            res.emplace_back(*it);
        }
    }
    return res;
}
template <typename It>
MELON_CORE_EXPORT std::vector<std::string> check_superfluous_params(YAML::Node conf, const It req_begin, const It req_end)
{
    std::vector<std::string> res;
    for( YAML::const_iterator it = conf.begin(); it != conf.end(); ++it )
    {
        std::string key = it->first.as<std::string>();
        if(std::find(req_begin, req_end, key) == req_end)
            res.emplace_back(key);
    }
    return res;
}


//  Returns two pairs:
//  yaml-cpp map Node with it's key
//  and missing keys with superfluous keys
template <typename It>
MELON_CORE_EXPORT std::pair<
                  std::vector<std::pair<std::string,YAML::Node> >,
                  std::pair<std::vector<std::string>, std::vector<std::string> > >
                  parse_one_level_down(YAML::Node parent_node,
                                               It req_begin,
                                               It req_end,
                                             bool check_for_superfluous = true)
{
    std::vector<std::pair<std::string, YAML::Node> > node_res = {};
    std::vector<std::string> missing_res = {}, superfluous_res;
    if(check_for_superfluous)
        superfluous_res = check_superfluous_params(parent_node, req_begin, req_end);
    else
        superfluous_res = {};
    try
    {
    for( It it = req_begin; it != req_end; ++it )
        {
            if(!*parent_node[*it])
            {
                missing_res.emplace_back(*it);
            }
            else
            {
                node_res.emplace_back(std::make_pair(*it, parent_node[*it]));
            }

        }
    }
    catch (YAML::ParserException &e)
    {
       std::cerr<<"YAML parser error: " << e.what() << std::endl;
    }
    return std::make_pair(node_res, std::make_pair(missing_res, superfluous_res));
}

//MELON_CORE_EXPORT std::pair<networking, std::vector<location>> parse_common_params();

/*enum class Services // supported service types
 {
     SERVER_SERVER,
     AUTH,
     CLIENT_SERVER,
     STORAGE,
     MESSAGING
 };

enum class Locations // supported location types
 {
    LOCAL,
    REMOTE
 };


struct networking {
    std::string protocol;
    std::string ip;
    uint64_t port;
};
struct location {
    std::string service_kind;
    std::string protocol;
    std::string ip;
    uint64_t port;
};


namespace server_conf {
enum class Policies // supported location types
 {
    DIGEST_MD5,
    PLAIN
 };
}*/

}  // namespace melon::core

#endif // MELON_CORE_YAML_CONFIG_HPP_
