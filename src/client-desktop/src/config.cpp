#include <config.hpp>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>

namespace melon::client_desktop
{

YAML::Node font_to_yaml(const UserConfigSingletone::Appearance::FontParams& fp);
YAML::Node color_to_yaml(const QColor& color);

void config_to_yaml()
{
    auto& config = UserConfigSingletone::get_instance();

    YAML::Node node;

    UserConfigSingletone::Appearance user_ap = config.appearance();

    // For Chat items
    node["Appearance"]["chat_name_font_params"] = font_to_yaml(user_ap.chat_name_font_params());
    node["Appearance"]["chat_timestamp_font_params"] = font_to_yaml(user_ap.chat_timestamp_font_params());
    node["Appearance"]["last_message_font_params"] = font_to_yaml(user_ap.last_message_font_params());
    node["Appearance"]["last_message_sender_font_params"] = font_to_yaml(user_ap.last_message_sender_font_params());
    node["Appearance"]["unread_counter_font_params"] = font_to_yaml(user_ap.unread_counter_font_params());

    node["Appearance"]["selected_chat_color"] = color_to_yaml(user_ap.selected_chat_color());
    node["Appearance"]["unread_background_color"] = color_to_yaml(user_ap.unread_background_color());

    // For Message items
    node["Appearance"]["sender_font_params"] = font_to_yaml(user_ap.sender_font_params());
    node["Appearance"]["message_text_font_params"] = font_to_yaml(user_ap.message_text_font_params());
    node["Appearance"]["timestamp_font_params"] = font_to_yaml(user_ap.timestamp_font_params());

    node["Appearance"]["sended_message_color"] = color_to_yaml(user_ap.sended_message_color());
    node["Appearance"]["receive_message_color"] = color_to_yaml(user_ap.receive_message_color());

    std::ofstream fout("user_config.yaml");
    fout << node;
}

YAML::Node font_to_yaml(const UserConfigSingletone::Appearance::FontParams& fp)
{
    YAML::Node node;
    node["family"] = fp.family.toStdString();
    node["size"] = fp.size;
    node["weight"] = fp.weight;
    return node;
}

YAML::Node color_to_yaml(const QColor& color)
{
    YAML::Node node;
    node["r"] = color.red();
    node["g"] = color.green();
    node["b"] = color.blue();
    node["a"] = color.alpha();
    return node;
}


}  // namespace melon::client_desktop
