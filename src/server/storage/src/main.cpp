#include <melon_core.hpp>

#include <yaml-cpp/yaml.h>

#include <array>
#include <exception>
#include <iostream>
#include <string>
#include <vector>


void low_level_parameter_irregularities_reaction(const std::vector<std::string>& missing_params, const std::vector<std::string>& superfluous_params, const std::string& where)
{
    if (!missing_params.empty())
    {
        std::cout << "[error]:\tsome " << where << " params are missing!\n";
        throw std::runtime_error("Reqired parameters are missing");
    }
    if (!superfluous_params.empty())
    {
        std::cout << "[warning]:\tsome " << where << " params are unused!\n";
    }
}

void parse_locations(const std::string& title, const YAML::Node& node, const std::array<std::string, 4>& required_parameters_locations)
{
    for (const YAML::Node& node_of_list : node)
    {
        auto parsed_level_2 = melon::core::yaml_conf::parse_one_level_down(node_of_list,
                                                                    required_parameters_locations.begin(),
                                                                    required_parameters_locations.end());
        auto [missing_params, superfluous_params] = parsed_level_2.second;
        low_level_parameter_irregularities_reaction(missing_params, superfluous_params, title);
        std::string kind, ip;
        for (auto& [title, node2] : parsed_level_2.first)
        {
            if (title == "service_kind")
                kind = node2.as<std::string>();
            else if (title == "ip")
                ip = node2.as<std::string>();

            // else if (title == "...")
            // ...

        }
        std::cout << "The " << kind << " service is at " << ip << "\n";
    }
}

void parse_networking(const std::string& title, const YAML::Node& node, const std::array<std::string, 3>& required_parameters_networking)
{
    auto parsed_level_2 = melon::core::yaml_conf::parse_one_level_down(node,
                                                                required_parameters_networking.begin(),
                                                                required_parameters_networking.end());
    auto& [missing_params, superfluous_params] = parsed_level_2.second;
    low_level_parameter_irregularities_reaction(missing_params, superfluous_params, title);
    for (auto& [title, node2] : parsed_level_2.first)
    {
        if (title == "ip")
        {
            std::string ip = node2.as<std::string>();
            std::cout << "I am at " << ip << "\n";
        }

        // else if ( title == "...")
        // ...
    }
}


void auth_config_parse(const YAML::Node& conf_file)
{
    namespace meco = melon::core;

    // Parameters we want on top level of YAML
    static const std::array<std::string, 3> required_parameters =
    {
        "networking",
        "locations",
        "db-location",
    };
    // Networking parameters we want
    static const std::array<std::string, 3> required_parameters_networking =
    {
        "ip",
        "port",
        "protocol",
    };
    // Other service locations parameters we want
    static const std::array<std::string, 4> required_parameters_locations =
    {
        "service_kind",
        "ip",
        "port",
        "protocol",
    };

    //  Parse the top level of YAML and notify about parameter abnormalities
    auto parsed_level_1 = meco::yaml_conf::parse_one_level_down(conf_file, required_parameters.begin(), required_parameters.end());
    auto [missing_params, superfluous_params] = parsed_level_1.second;
    if (!missing_params.empty())
    {
        for (std::string& key : missing_params)
            std::cout << "[error]:\t\"" << key << "\" key is missing from config" << std::endl;
        throw std::runtime_error("Reqired parameters are missing");
    }
    if (!superfluous_params.empty())
    {
        for (std::string& key : superfluous_params)
            std::cout << "[warning]:\t\"" << key << "\" key is unused" << std::endl;
    }
    // Go deeper
    for (auto& [title, node] : parsed_level_1.first)
    {
        if (title == "networking")
            parse_networking(title, node, required_parameters_networking);
        else if (title == "locations")
            parse_locations(title, node, required_parameters_locations);
        else if (title == "db-location")
        {
            std::string db_loc = node.as<std::string>();
            std::cout << "Database is located at " << db_loc << "\n";
        }
        else
        {
            // Something went really wrong, let's throw an exception...
            throw std::runtime_error("Parsing logic failure");
        }
    }
}


int main()
{
    // Parameter parsing example

    // Note that this is an example:
    // - all required parameters are hardcoded
    // - nothing is returned from function
    try
    {
        YAML::Node conf_file = YAML::LoadFile("m_config.yaml");
        auth_config_parse(conf_file);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Configuration file parsing exception: " << e.what() << "\n";
        // ...
    }

    melon::core::hello();
    return 0;
}
