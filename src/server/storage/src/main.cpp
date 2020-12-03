#include <melon_core.hpp>

#include <yaml-cpp/yaml.h>

#include <exception>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

void low_level_parameter_irregularities_reaction(std::vector<std::string> missing_params, std::vector<std::string> superfluous_params, std::string where)
{
    if (!missing_params.empty())
    {
        std::cout << "[error]:\tsome " << where << " params are missing!\n";

        // I'd throw an exception here, but I leave it as it is
        // for the sake of a demonstration
    }
    if (!superfluous_params.empty())
    {
        std::cout << "[warning]:\tsome " << where << " params are unused!\n";
    }
}

void auth_config_parse(YAML::Node conf_file)
{
    namespace meco = melon::core;

    // Parameters we want on top level of YAML
    const static std::unordered_set<std::string> required_parameters =
    {
    "networking",
    "locations",
    "db-location",
    "password"
    };
    // Networking parameters we want
    const static std::unordered_set<std::string> required_parameters_networking =
    {
    "ip",
    "port",
    "protocol"
    };
    // Other service locations parameters we want
    const static std::unordered_set<std::string> required_parameters_locations =
    {
    "service_kind",
    "ip",
    "port",
    "protocol"
    };

    //  Parse the top level of YAML and notify about parameter abnormalities
    auto parsed_level_1 = meco::yaml_conf::parse_one_level_down(conf_file,required_parameters.begin(),required_parameters.end());
    auto [missing_params, superfluous_params] = parsed_level_1.second;
    if (!missing_params.empty())
    {
        for (std::string& key : missing_params)
            std::cout << "[error]:\t\"" << key << "\" key is missing from config" << std::endl;

        // Throw exception (or do whatever you want) here...
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
        {
            auto parsed_level_2 = meco::yaml_conf::parse_one_level_down(node,
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

                // if ( title == "...")
                // ...
            }
        }
        else if (title == "locations")
        {

            for (const YAML::Node& node_of_list : node)
            { 
                auto parsed_level_2 = meco::yaml_conf::parse_one_level_down(node_of_list,
                                                                            required_parameters_locations.begin(),
                                                                            required_parameters_locations.end());
                auto [missing_params, superfluous_params] = parsed_level_2.second;
                low_level_parameter_irregularities_reaction(missing_params, superfluous_params, title);
                std::string kind, ip;
                for (auto& [title, node2] : parsed_level_2.first)
                {
                    if (title == "service_kind")
                    {
                        kind = node2.as<std::string>();
                    }
                    if (title == "ip")
                    {
                        ip = node2.as<std::string>();
                    }

                    // if (title == "...")
                    // ...
                }
                std::cout << "The " << kind << " service is at " << ip << "\n";
            }
        }
        else if (title == "db-location")
        {
            std::string db_loc = node.as<std::string>();
            std::cout << "Database is located at " << db_loc << "\n";
        }
        else
        {
            // Something went really wrong, you should throw exception or something...
        }
    }
}


int main()
{
    // Parameter parsing example

    // Note that this is an example:
    // - all required parameters are hardcoded
    // - nothing is returned from function
    // - no exceptions are thrown
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
