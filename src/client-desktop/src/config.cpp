#include <config.hpp>
#include <melon/core.hpp>
#include <storage_singletones.hpp>

#include <yaml-cpp/yaml.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <fstream>

// for encoding and decoding from YAML::Node to needed classes automatically
namespace YAML
{

using user_conf_ap = melon::client_desktop::UserConfigSingletone::Appearance;

template<>
struct convert<user_conf_ap::FontParams>
{
    static Node encode(const user_conf_ap::FontParams& fp)
    {
        YAML::Node node;
        node["family"] = fp.family.toStdString();
        node["size"] = fp.size;
        node["weight"] = fp.weight;
        return node;
    }

    static bool decode(const Node& node, user_conf_ap::FontParams& fp)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        QString family = QString::fromStdString(node["family"].as<std::string>());
        int size = node["size"].as<int>();
        int weight = node["weight"].as<int>();
        fp = user_conf_ap::FontParams({family, size, weight});
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

void UserConfigSingletone::Appearance::set_chatlist_font_size(FontSize size)
{
    switch (size)
    {
    case FontSize::SMALL:
        m_chat_name_font_params.size = 8;            // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_chat_timestamp_font_params.size = 6;       // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_last_message_font_params.size = 6;         // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_last_message_sender_font_params.size = 6;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_unread_counter_font_params.size = 6;       // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        break;
    case FontSize::STANDARD:
        m_chat_name_font_params.size = 9;            // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_chat_timestamp_font_params.size = 7;       // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_last_message_font_params.size = 7;         // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_last_message_sender_font_params.size = 7;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_unread_counter_font_params.size = 7;       // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        break;
    case FontSize::BIG:
        m_chat_name_font_params.size = 10;           // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_chat_timestamp_font_params.size = 8;       // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_last_message_font_params.size = 8;         // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_last_message_sender_font_params.size = 8;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_unread_counter_font_params.size = 8;       // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        break;
    }
    m_chatlist_font_size = size;
}

void UserConfigSingletone::Appearance::set_message_font_size(FontSize size)
{
    switch (size)
    {
    case FontSize::SMALL:
        m_sender_font_params.size = 7;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_message_text_font_params.size = 7;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_timestamp_font_params.size = 6;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        break;
    case FontSize::STANDARD:
        m_sender_font_params.size = 8;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_message_text_font_params.size = 8;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_timestamp_font_params.size = 7;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        break;
    case FontSize::BIG:
        m_sender_font_params.size = 9;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_message_text_font_params.size = 9;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        m_timestamp_font_params.size = 8;  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
        break;
    }
    m_message_font_size = size;
    BOOST_LOG_TRIVIAL(info) << "In config msg size is " << static_cast<int>(m_message_font_size);
}

void UserConfigSingletone::Appearance::set_font_family_common(const QString& family)
{
    m_sender_font_params.family = family;
    m_message_text_font_params.family = family;
    m_timestamp_font_params.family = family;

    m_chat_name_font_params.family = family;
    m_chat_timestamp_font_params.family = family;
    m_last_message_font_params.family = family;
    m_unread_counter_font_params.family = family;
    m_last_message_sender_font_params.family = family;

    m_font_family = family;
}

static void parameter_abnormalities_reaction(const std::vector<std::string>& missing_params, const std::vector<std::string>& superfluous_params, const std::string& where);

void parse_settings_appearance(const std::string& title, const YAML::Node& node);
void parse_settings_behaviour(const std::string& title, const YAML::Node& node);

void save_settings_to_yaml()
{
    auto& config = UserConfigSingletone::get_instance();

    YAML::Node node;

    UserConfigSingletone::Appearance user_ap = config.appearance();

    node["Appearance"]["common_font_family"] = user_ap.font_family().toStdString();
    node["Appearance"]["message_font_size"] = static_cast<int>(user_ap.message_font_size());
    node["Appearance"]["chatlist_font_size"] = static_cast<int>(user_ap.chatlist_font_size());

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

    UserConfigSingletone::Behaviour user_behav = config.behaviour();
    node["Behaviour"]["remove_white_around_message"] = user_behav.remove_whitespaces_around();
    node["Behaviour"]["send_message_by_enter"] = user_behav.send_message_by_enter();
    node["Behaviour"]["replace_hyphens"] = user_behav.replace_hyphens();

    std::string settings_filename = StorageNameSingletone::get_instance().user_settings_file_name();
    std::ofstream settings_file(settings_filename);
    settings_file << node;
}

void set_standart_settings()
{
    auto& config = UserConfigSingletone::get_instance();

    // Appearance

    UserConfigSingletone::Appearance& app = config.appearance();

    app.set_font_family_var(QStringLiteral("Cantarell"));
    app.set_message_font_size_var(UserConfigSingletone::Appearance::FontSize::STANDARD);
    app.set_chatlist_font_size_var(UserConfigSingletone::Appearance::FontSize::STANDARD);

    // Chat settings
    app.set_chat_name_font_params({QStringLiteral("Cantarell"), 9, QFont::DemiBold});  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_chat_timestamp_font_params({QStringLiteral("Cantarell"), 7, 35});          // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_last_message_font_params({QStringLiteral("Cantarell"), 7, 35});            // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_last_message_sender_font_params({QStringLiteral("Cantarell"), 7, 40});     // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_unread_counter_font_params({QStringLiteral("Cantarell"), 7, 45});          // NOLINT (cppcoreguidelines-avoid-magic-numbers)

    app.set_selected_chat_color({/*r*/ 255, /*g*/ 243, /*b*/ 223});                    // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_unread_background_color({/*r*/ 235, /*g*/ 235, /*b*/ 235});                // NOLINT (cppcoreguidelines-avoid-magic-numbers)

    // Message settings
    app.set_sender_font_params({QStringLiteral("Cantarell"), 8, QFont::DemiBold});     // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_message_text_font_params({QStringLiteral("Cantarell"), 8, 41});            // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_timestamp_font_params({QStringLiteral("Cantarell"), 7, 35});               // NOLINT (cppcoreguidelines-avoid-magic-numbers)

    app.set_receive_message_color({/*r*/ 255, /*g*/ 243, /*b*/ 223});                  // NOLINT (cppcoreguidelines-avoid-magic-numbers)
    app.set_sended_message_color({/*r*/ 235, /*g*/ 235, /*b*/ 235});                   // NOLINT (cppcoreguidelines-avoid-magic-numbers)

    // Behaviour
    UserConfigSingletone::Behaviour& user_behav = config.behaviour();
    user_behav.set_remove_whitespaces_around(true);
    user_behav.set_send_message_by_enter(true);
    user_behav.set_replace_hyphens(true);
}

void parse_settings(const YAML::Node& conf_file)
{
    // Parameters we want on top level of YAML
    static const std::array<std::string, 2> required_parameters =
    {
        "Appearance",
        "Behaviour"
    };

    auto parsed_level_categories = meco::yaml_conf::parse_one_level_down(conf_file, required_parameters.begin(), required_parameters.end());
    auto [missing_params, superfluous_params] = parsed_level_categories.second;
    parameter_abnormalities_reaction(missing_params, superfluous_params, "top level");

    for (auto& [title, node] : parsed_level_categories.first)
    {
        BOOST_LOG_TRIVIAL(info) << "Parsing " << title;
        if (title == "Appearance")
            parse_settings_appearance(title, node);
        else if (title == "Behaviour")
            parse_settings_behaviour(title, node);
        else
            throw std::runtime_error("Parsing logic failure");  // Something went really wrong, let's throw an exception...
    }
}

void parse_settings_behaviour(const std::string& title, const YAML::Node& node)
{
    // Parameters we want on top level of Behaviour
    static const std::array<std::string, 3> required_parameters =
    {
        "remove_white_around_message",
        "send_message_by_enter",
        "replace_hyphens"
    };

    auto [parsed, abnormal] = meco::yaml_conf::parse_one_level_down(node,
                                                                    required_parameters.begin(),
                                                                    required_parameters.end());
    auto& [missing_params, superfluous_params] = abnormal;
    parameter_abnormalities_reaction(missing_params, superfluous_params, title);

    auto& config = UserConfigSingletone::get_instance();
    UserConfigSingletone::Behaviour& user_behav = config.behaviour();
    for (auto& [title, node2] : parsed)
    {
        if (title == "remove_white_around_message")
            user_behav.set_remove_whitespaces_around(node2.as<bool>());
        else if (title == "send_message_by_enter")
            user_behav.set_send_message_by_enter(node2.as<bool>());
        else if (title == "replace_hyphens")
            user_behav.set_replace_hyphens(node2.as<bool>());
        else
            throw std::runtime_error("Parsing logic failure");  // Something went really wrong, let's throw an exception...
    }
}

void parse_settings_appearance(const std::string& title, const YAML::Node& node)
{
    using FP = UserConfigSingletone::Appearance::FontParams;

    // Parameters we want on top level of Appearance
    static const std::array<std::string, 15> required_parameters =
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
        "common_font_family",
        "message_font_size",
        "chatlist_font_size"
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

        else if (title == "common_font_family")
            user_ap.set_font_family_var(QString::fromStdString(node2.as<std::string>()));

        else if (title == "message_font_size")
        {
            auto fs = static_cast<UserConfigSingletone::Appearance::FontSize>(node2.as<int>());
            user_ap.set_message_font_size_var(fs);
            BOOST_LOG_TRIVIAL(info) << "Parsing settings: message font size is " << static_cast<int>(fs);
        }

        else if (title == "chatlist_font_size")
        {
            auto fs = static_cast<UserConfigSingletone::Appearance::FontSize>(node2.as<int>());
            user_ap.set_chatlist_font_size_var(fs);
            BOOST_LOG_TRIVIAL(info) << "Parsing settings: chatlist font size is " << static_cast<int>(fs);
        }

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
