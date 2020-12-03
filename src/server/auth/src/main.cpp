#include <melon_core.hpp>

#include <vector>
#include <string>

#include <iostream>
#include <unordered_set>
#include <exception>
#include <yaml-cpp/yaml.h>


int main()
{


    //  Parameter parsing example

    YAML::Node conf_file = YAML::LoadFile("m_config.yaml");


    //  Parameters we want on top level of YAML
    const std::unordered_set<std::string> required_parameters = {"networking","locations","sasl-policy","password"};


    //  Networking parameters we want
    const std::unordered_set<std::string> required_parameters_networking = {"ip","port","protocol"};


    //  Other service locations parameters we want
    const std::unordered_set<std::string> required_parameters_locations = {"service_kind", "ip","port","protocol"};

    //  Parse the top level of YAML and notify about parameter abnormalities
    auto parsed_level_1 = melon::core::yaml_conf::parse_one_level_down(conf_file,required_parameters.begin(),required_parameters.end());
    auto [missing_params, superfluous_params] = parsed_level_1.second;
    if(missing_params.size())
    {
        for(std::string& key : missing_params)
            std::cout <<"[error]:\t\"" << key <<"\" key is missing from config" << std::endl;

        // Throw exception (or do whatever you want) here...
    }
    if(superfluous_params.size())
    {
        for(std::string& key : superfluous_params)
            std::cout <<"[warning]:\t\"" << key <<"\" key is unused" << std::endl;
    }

    // Go deeper
    for(auto [title, node] : parsed_level_1.first )
    {
        if(title == "networking")
        {

            auto parsed_level_2 = melon::core::yaml_conf::parse_one_level_down(node,
                                                                               required_parameters_networking.begin(),
                                                                               required_parameters_networking.end());
            auto [missing_params, superfluous_params] = parsed_level_2.second;
            if(missing_params.size())
            {
                std::cout<<"[error]:\tsome networking params are missing!\n";


                // Once again, throw exception (or do whatever you want) here...
            }
            if(superfluous_params.size())
            {
                std::cout<<"[warning]:\tsome networking params are unused!\n";
            }
            for(auto [title, node] : parsed_level_2.first )
            {
                if(title == "ip")
                {

                    std::string ip = node.as<std::string>();
                    std::cout << "I am at " << ip << "\n";
                }


                // if( title == "..." )
                // ...
            }

        }
        else if(title == "locations")
        {

            for(size_t i = 0; i < node.size(); ++i)
            {
                YAML::Node node_of_list = node[i];
                auto parsed_level_2 = melon::core::yaml_conf::parse_one_level_down(node_of_list,
                                                                                   required_parameters_locations.begin(),
                                                                                   required_parameters_locations.end());
                auto [missing_params, superfluous_params] = parsed_level_2.second;
                if(missing_params.size())
                {
                    std::cout<<"[error]:\tsome networking params are missing on service "<<i<<" !\n";


                    // Once again, throw exception (or do whatever you want) here...
                }
                if(superfluous_params.size())
                {
                    std::cout<<"[warning]:\tsome networking params are unused on service "<<i<<" !\n";
                }
                std::string kind, ip;
                for(auto [title, node] : parsed_level_2.first )
                {
                    if(title == "service_kind")
                    {

                        kind = node.as<std::string>();
                    }
                    if(title == "ip")
                    {

                        ip = node.as<std::string>();
                    }


                    // if( title == "..." )
                    // ...
                }
                std::cout << "The " << kind << " service is at " << ip << "\n";
            }
        }
        else if(title == "policy")
        {

            std::string policy = node.as<std::string>();
            std::cout << "SASL policy is " << policy << "\n";
        }
        else
        {
            //  something went really wrong, you should throw exception or something..//  Extra and missing parameter handling can easily be put in a separate function too!.
        }
    }

    //  Extra and missing parameter handling can easily be put in a separate function too!

    //help in pasing common params?
    //auto parsed =  melon::core::yaml_conf::parse_common_params();
    //std::cout << "Info: " << parsed.second[0].service_kind << " service is at " << parsed.second[0].ip << "\n";


    melon::core::hello();
    return 0;
}
