#include <config.hpp>
#include <melon/core.hpp>
#include <storage_singletones.hpp>

#include <yaml-cpp/yaml.h>

#include <fstream>

// for encoding and decoding from YAML::Node to needed classes automatically
namespace YAML
{

template<>
struct convert<melon::client_desktop::UserConfigSingletone::Appearance::FontParams>
{
  static Node encode(const melon::client_desktop::UserConfigSingletone::Appearance::FontParams& fp)
  {
      YAML::Node node;
      node["family"] = fp.family.toStdString();
      node["size"] = fp.size;
      node["weight"] = fp.weight;
      return node;
  }

  static bool decode(const Node& node, melon::client_desktop::UserConfigSingletone::Appearance::FontParams& fp)
  {
    if(!node.IsMap() || node.size() != 3)
    {
      return false;
    }

    QString family = QString::fromStdString(node["family"].as<std::string>());
    int size = node["size"].as<int>();
    int weight = node["weight"].as<int>();
    fp = melon::client_desktop::UserConfigSingletone::Appearance::FontParams({family, size, weight});
    return true;
  }

};

template<>
struct convert<QColor>
{
  static Node encode(const QColor& rhs)
  {
    Node node;
    node["r"] = rhs.red();
    node["g"] = rhs.green();
    node["b"] = rhs.blue();
    node["a"] = rhs.alpha();
    return node;
  }

  static bool decode(const Node& node, QColor& rhs)
  {
    if (!node.IsMap())
    {
      return false;
    }

    int red = node["r"].as<int>();
    int green = node["g"].as<int>();
    int blue = node["b"].as<int>();
    int alpha = node["a"].as<int>();
    rhs = QColor(red, green, blue, alpha);

    return true;
  }
};

}  // namespace YAML


namespace melon::client_desktop
{

namespace meco = melon::core;

static void parameter_abnormalities_reaction(const std::vector<std::string>& missing_params, const std::vector<std::string>& superfluous_params, const std::string& where);

void parse_settings_appearance(const std::string& title, const YAML::Node& node);

void config_to_yaml()
{
    auto& config = UserConfigSingletone::get_instance();

    YAML::Node node;

    UserConfigSingletone::Appearance user_ap = config.appearance();

    // For Chat items
    node["Appearance"]["chat_name_font_params"] = user_ap.chat_name_font_params();
    node["Appearance"]["chat_timestamp_font_params"] = user_ap.chat_timestamp_font_params();
    node["Appearance"]["last_message_font_params"] = user_ap.last_message_font_params();
    node["Appearance"]["last_message_sender_font_params"] = user_ap.last_message_sender_font_params();
    node["Appearance"]["unread_counter_font_params"] = user_ap.unread_counter_font_params();

    node["Appearance"]["selected_chat_color"] = user_ap.selected_chat_color();
    node["Appearance"]["unread_background_color"] = user_ap.unread_background_color();

    // For Message items
    node["Appearance"]["sender_font_params"] = user_ap.sender_font_params();
    node["Appearance"]["message_text_font_params"] = user_ap.message_text_font_params();
    node["Appearance"]["timestamp_font_params"] = user_ap.timestamp_font_params();

    node["Appearance"]["sended_message_color"] = user_ap.sended_message_color();
    node["Appearance"]["receive_message_color"] = user_ap.receive_message_color();

    auto& storage = StorageNameSingletone::get_instance();
    std::string filename = storage.user_settings_file_name();
    std::ofstream fout(filename);
    fout << node;
}

void set_standart_settings()
{
    auto& config = UserConfigSingletone::get_instance();

    //Appearance

    // Chat settings
    config.appearance().set_chat_name_font_params({QStringLiteral("Cantarell"), 9, QFont::DemiBold});
    config.appearance().set_chat_timestamp_font_params({QStringLiteral("Cantarell"), 7, 35});
    config.appearance().set_last_message_font_params({QStringLiteral("Cantarell"), 7, 35});
    config.appearance().set_last_message_sender_font_params({QStringLiteral("Cantarell"), 7, 40});
    config.appearance().set_unread_counter_font_params({QStringLiteral("Cantarell"), 7, 45});

    config.appearance().set_selected_chat_color({/*r*/ 255, /*g*/ 243, /*b*/ 223});
    config.appearance().set_unread_background_color({/*r*/ 235, /*g*/ 235, /*b*/ 235});

    // Message settings
    config.appearance().set_sender_font_params({QStringLiteral("Cantarell"), 9, QFont::DemiBold});
    config.appearance().set_message_text_font_params({QStringLiteral("Cantarell"), 9, 41});
    config.appearance().set_timestamp_font_params({QStringLiteral("Cantarell"), 6, 35});

    config.appearance().set_receive_message_color({/*r*/ 255, /*g*/ 243, /*b*/ 223});
    config.appearance().set_sended_message_color({/*r*/ 235, /*g*/ 235, /*b*/ 235});
}

void parse_settings(const YAML::Node& conf_file)
{
    // Parameters we want on top level of YAML
    static const std::array<std::string, 1> required_parameters =
    {
        "Appearance",
    };

    auto parsed_level_categories = meco::yaml_conf::parse_one_level_down(conf_file, required_parameters.begin(), required_parameters.end());
    auto [missing_params, superfluous_params] = parsed_level_categories.second;
    parameter_abnormalities_reaction(missing_params, superfluous_params, "top level");

    for (auto& [title, node] : parsed_level_categories.first)
    {
        if (title == "Appearance")
            parse_settings_appearance(title, node);
        else
            throw std::runtime_error("Parsing logic failure");  // Something went really wrong, let's throw an exception...
    }
}

void parse_settings_appearance(const std::string& title, const YAML::Node& node)
{
    using FP = UserConfigSingletone::Appearance::FontParams;

    // Parameters we want on top level of Appearance
    static const std::array<std::string, 12> required_parameters =
    {
        "chat_name_font_params",
        "chat_timestamp_font_params",
        "last_message_font_params",
        "last_message_sender_font_params",
        "unread_counter_font_params",
        "selected_chat_color",
        "unread_background_color",
        "sender_font_params",
        "message_text_font_params",
        "timestamp_font_params",
        "sended_message_color",
        "receive_message_color",
    };

    auto [parsed, abnormal] = meco::yaml_conf::parse_one_level_down(node,
                                                                    required_parameters.begin(),
                                                                    required_parameters.end());
    auto& [missing_params, superfluous_params] = abnormal;
    parameter_abnormalities_reaction(missing_params, superfluous_params, title);

    auto& config = UserConfigSingletone::get_instance();
    UserConfigSingletone::Appearance& user_ap = config.appearance();
    for (auto& [title, node2] : parsed)
    {
        if (title == "chat_name_font_params")
            user_ap.set_chat_name_font_params(node2.as<FP>());

        else if (title == "chat_timestamp_font_params")
            user_ap.set_chat_timestamp_font_params(node2.as<FP>());

        else if (title == "last_message_font_params")
            user_ap.set_last_message_font_params(node2.as<FP>());

        else if (title == "last_message_sender_font_params")
            user_ap.set_last_message_sender_font_params(node2.as<FP>());

        else if (title == "unread_counter_font_params")
            user_ap.set_unread_counter_font_params(node2.as<FP>());

        else if (title == "selected_chat_color")
            user_ap.set_selected_chat_color(node2.as<QColor>());

        else if (title == "unread_background_color")
            user_ap.set_unread_background_color(node2.as<QColor>());

        else if (title == "sender_font_params")
            user_ap.set_sender_font_params(node2.as<FP>());

        else if (title == "message_text_font_params")
            user_ap.set_message_text_font_params(node2.as<FP>());

        else if (title == "timestamp_font_params")
            user_ap.set_timestamp_font_params(node2.as<FP>());

        else if (title == "sended_message_color")
            user_ap.set_sended_message_color(node2.as<QColor>());

        else if (title == "receive_message_color")
            user_ap.set_receive_message_color(node2.as<QColor>());

        else
            throw std::runtime_error("Parsing logic failure");  // Something went really wrong, let's throw an exception...
    }
}

static void parameter_abnormalities_reaction(const std::vector<std::string>& missing_params, const std::vector<std::string>& superfluous_params, const std::string& where)
{
    if (!missing_params.empty())
        throw meco::yaml_conf::MissingParamsException("missing required params at " + where);
    if (!superfluous_params.empty())
        throw meco::yaml_conf::ExtraParamsException("extra params at " + where);
}


}  // namespace melon::client_desktop
