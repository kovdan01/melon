#ifndef MELON_CORE_YAML_CONFIG_HPP_
#define MELON_CORE_YAML_CONFIG_HPP_

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <vector>

namespace melon::core::yaml_conf
{
inline const std::string conf_filename = "m_config.yaml";

/**
 * Lists top-level keys in a yaml-cpp node, that are missing
 * @tparam It Iterator that satisfies C++ named requirement LegacyForwardIterator
 * @param[in] yaml-cpp node we want to check
 * @param[in] begin,end Iterators, specifying a range of std::strings containing keys,
 * are expected then parsing this level
 * @return A list of missing keys
 **/
template <typename It>
std::vector<std::string> check_missing_params(const YAML::Node& conf, const It begin, const It end)
{
    std::vector<std::string> res;
    for (It it = begin; it != end; ++it)
        if (conf[*it] == false)
            res.emplace_back(*it);
    return res;
}

/**
 * Lists top-level keys in a yaml-cpp node, that are not excepted
 * @tparam It Iterator that satisfies C++ named requirement LegacyForwardIterator
 * @param[in] yaml-cpp node we want to check
 * @param[in] begin,end Iterators, specifying a range of std::strings containing keys,
 * not expected then parsing this level
 * @return A list of extra keys
 **/
template <typename It>
std::vector<std::string> check_superfluous_params(const YAML::Node& conf, It begin, It end)
{
    std::vector<std::string> res;
    for (YAML::const_iterator it = conf.begin(); it != conf.end(); ++it)
    {
        std::string key = it->first.as<std::string>();
        if (std::find(begin, end, key) == end)
            res.emplace_back(key);
    }
    return res;
}

/**
 * Parses the yaml-cpp map node with a list of expected keys into lower level nodes.
 * Lists missing and extra parameters
 * @tparam It Iterator that satisfies C++ named requirement LegacyForwardIterator
 * @param[in] parent_node yaml-cpp map node we want to parse
 * @param[in] begin,end Iterators, specifying a range of std::strings containing keys,
 * expected then parsing this leve
 * @param[in] check_for_superfluous Flag. If true, the returns returns a list of extra keys.
 * If false, returns an empty list
 * @return A pair of pairs:
 * Node key with corresponding lower-leve node
 * List of missing keys with list of extra keys
 **/
template <typename It>
std::pair<
std::vector<std::pair<std::string,YAML::Node>>,
std::pair<std::vector<std::string>, std::vector<std::string>>>
parse_one_level_down(const YAML::Node& parent_node,
                     It begin,
                     It end,
                     bool check_for_superfluous = true)
{
    std::vector<std::pair<std::string, YAML::Node> > node_res = {};
    std::vector<std::string> missing_res, superfluous_res;
    if (check_for_superfluous)
        superfluous_res = check_superfluous_params(parent_node, begin, end);
    try
    {
        for (It it = begin; it != end; ++it)
        {
            if (!*parent_node[*it])
                missing_res.emplace_back(*it);
            else
                node_res.emplace_back(*it, parent_node[*it]);
        }
    }
    catch (const YAML::ParserException& e)
    {
       std::cerr<<"YAML parser error: " << e.what() << std::endl;
    }
    return std::make_pair(node_res, std::make_pair(missing_res, superfluous_res));
}
}  // namespace melon::core

#endif  // MELON_CORE_YAML_CONFIG_HPP_
