#ifndef MELON_CORE_YAML_CONFIG_HPP_
#define MELON_CORE_YAML_CONFIG_HPP_

#include <melon/core/exception.hpp>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


namespace melon::core::yaml_conf
{

class MissingParamsException : public melon::core::Exception
{
    using melon::core::Exception::Exception;
};

class ExtraParamsException : public melon::core::Exception
{
    using melon::core::Exception::Exception;
};

/**
 * Lists top-level keys in a yaml-cpp node, that are missing
 * @tparam It Iterator that satisfies C++ named requirement LegacyForwardIterator
 * @param[in] conf yaml-cpp node we want to check
 * @param[in] begin Iterator, specifying the beginning of a range of std::strings containing keys,
 * are expected then parsing this level
 * @param[in] end Iterator, specifying past-the-end of a range of std::strings containing keys,
 * are expected then parsing this level
 * @return A list of missing keys
 **/
template <typename It>
std::vector<std::string> check_missing_params(const YAML::Node& conf, It begin, It end)
{
    std::vector<std::string> res;
    for (It it = begin; it != end; ++it)
        if (!static_cast<bool>(conf[*it]))
            res.emplace_back(*it);
    return res;
}

/**
 * Lists top-level keys in a yaml-cpp node, that are not excepted
 * @tparam It Iterator that satisfies C++ named requirement LegacyForwardIterator
 * @param[in] conf yaml-cpp node we want to check
 * @param[in] begin Iterator, specifying the beginning of a range of std::strings containing keys,
 * not expected then parsing this level
 * @param[in] end Iterator, specifying past-the-end of a range of std::strings containing keys,
 * not expected then parsing this level
 * @return A list of extra keys
 **/
template <typename It>
std::vector<std::string> check_superfluous_params(const YAML::Node& conf, It begin, It end)
{
    std::vector<std::string> res;
    for (YAML::const_iterator it = conf.begin(); it != conf.end(); ++it)
    {
        auto key = it->first.as<std::string>();
        if (std::find(begin, end, key) == end)
            res.emplace_back(key);
    }
    return res;
}


using ParsedNodes = std::unordered_map<std::string, YAML::Node>;
struct KeyAbnormalities
{
    std::vector<std::string> missing;
    std::vector<std::string> superfluous;
};

/**
 * Parses the yaml-cpp map node with a list of expected keys into lower level nodes.
 * Lists missing and extra parameters
 * @tparam It Iterator that satisfies C++ named requirement LegacyForwardIterator
 * @param[in] parent_node yaml-cpp map node we want to parse
 * @param[in] begin Iterator, specifying the beginning of a range of std::strings containing keys,
 * expected then parsing this level
 * @param[in] end Iterator, specifying past-the-end of a range of std::strings containing keys,
 * expected then parsing this level
 * @param[in] check_for_superfluous Flag. If true, the returns returns a list of extra keys.
 * If false, returns an empty list
 * @return A pair of ParsedNodes and KeyAbnormalities:
 * ParsedNodes is a parsed map.
 * KeyAbnormalities is a list of missing keys and a list of extra keys
 **/
template <typename It>
std::pair<ParsedNodes, KeyAbnormalities>
parse_one_level_down(const YAML::Node& parent_node,
                     It begin,
                     It end,
                     bool check_for_superfluous = true)
{
    std::unordered_map<std::string, YAML::Node> node_res;
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
                node_res.insert({*it, parent_node[*it]});
        }
    }
    catch (const YAML::ParserException& e)
    {
       std::cerr << "YAML parser error: " << e.what() << std::endl;
    }
    return {node_res, {missing_res, superfluous_res}};
}

}  // namespace melon::core::yaml_conf

#endif  // MELON_CORE_YAML_CONFIG_HPP_
