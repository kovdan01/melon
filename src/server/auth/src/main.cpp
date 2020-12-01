#include <melon_core.hpp>

#include <vector>
#include <string>

#include <iostream>
#include <set>
#include <yaml-cpp/yaml.h>

int main()
{
    //We simulate the need for this config information
    const std::vector<std::string> required_parameters = {"networking","locations","sasl-policy","password"};

    std::vector<std::string>  missing_params = melon::core::yaml_conf::check_reqired_params(required_parameters);
    if(missing_params.size())
    {
        for(std::string& key : missing_params)
            std::cout <<"[error]:\t\"" << key <<"\" key is missing from config\n";
    }

    std::vector<std::string> superfluous_params = melon::core::yaml_conf::check_superfluous_params(required_parameters);
    if(superfluous_params.size())
    {
        for(std::string& key : superfluous_params)
            std::cout <<"[warning]:\t\"" << key <<"\" key is unused\n";
    }

    //help in pasing common params?
    auto parsed =  melon::core::yaml_conf::parse_common_params();
    std::cout << "At least one " << parsed.second[0].service_kind << " service is a " << parsed.second[0].location_kind << "\n";


    //Local parsing?
    //YAML::Node node = YAML::LoadFile("m_config.yaml");
    //std::cout << "SASL policy is " << node["sasl-policy"] << "\n";
    melon::core::hello();
    return 0;
}
