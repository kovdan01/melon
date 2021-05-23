#ifndef MELON_CLIENT_DESKTOP_CONFIG_HPP_
#define MELON_CLIENT_DESKTOP_CONFIG_HPP_

#include <yaml-cpp/yaml.h>

#include <QColor>
#include <QFont>
#include <QString>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace melon::client_desktop
{

class UserConfigSingletone
{
public:
    [[nodiscard]] static UserConfigSingletone& get_instance()
    {
        static UserConfigSingletone instance;
        return instance;
    }

    UserConfigSingletone(const UserConfigSingletone& root) = delete;
    UserConfigSingletone& operator=(const UserConfigSingletone&) = delete;
    UserConfigSingletone(UserConfigSingletone&& root) = delete;
    UserConfigSingletone& operator=(UserConfigSingletone&&) = delete;

    class Appearance
    {
    public:
        friend class UserConfigSingletone;

        struct FontParams
        {
            QString family;
            int size;
            int weight;
        };

        enum class FontSize
        {
            SMALL,
            STANDARD,
            BIG
        };

        // Getters

        [[nodiscard]] FontSize message_font_size() const;
        [[nodiscard]] const FontParams& sender_font_params() const;
        [[nodiscard]] const FontParams& message_text_font_params() const;
        [[nodiscard]] const FontParams& timestamp_font_params() const;

        [[nodiscard]] const QColor& receive_message_color() const;
        [[nodiscard]] const QColor& sended_message_color() const;

        [[nodiscard]] FontSize chatlist_font_size() const;
        [[nodiscard]] const FontParams& chat_name_font_params() const;
        [[nodiscard]] const FontParams& chat_timestamp_font_params() const;
        [[nodiscard]] const FontParams& last_message_font_params() const;
        [[nodiscard]] const FontParams& last_message_sender_font_params() const;
        [[nodiscard]] const FontParams& unread_counter_font_params() const;

        [[nodiscard]] QColor selected_chat_color() const;
        [[nodiscard]] QColor unread_background_color() const;

        [[nodiscard]] const QString& font_family() const;

        // Setters

        void set_message_font_size_var(FontSize size);
        void set_message_font_size(FontSize size);
        void set_sender_font_params(const FontParams& pm);
        void set_message_text_font_params(const FontParams& pm);
        void set_timestamp_font_params(const FontParams& pm);

        void set_receive_message_color(const QColor& c);
        void set_sended_message_color(const QColor& c);

        void set_chatlist_font_size_var(FontSize size);
        void set_chatlist_font_size(FontSize size);
        void set_chat_name_font_params(const FontParams& pm);
        void set_chat_timestamp_font_params(const FontParams& pm);
        void set_last_message_font_params(const FontParams& pm);
        void set_unread_counter_font_params(const FontParams& pm);
        void set_last_message_sender_font_params(const FontParams& pm);

        void set_selected_chat_color(const QColor& c);
        void set_unread_background_color(const QColor& c);

        void set_font_family_common(const QString& family);
        void set_font_family_var(const QString& family);

    private:
        Appearance() = default;

        QString m_font_family;
        FontSize m_message_font_size;
        FontSize m_chatlist_font_size;

        // for message item delegate
        FontParams m_sender_font_params;
        FontParams m_message_text_font_params;
        FontParams m_timestamp_font_params;

        QColor m_receive_message_color;
        QColor m_sended_message_color;

        // for chat item delegate
        FontParams m_chat_name_font_params;
        FontParams m_chat_timestamp_font_params;
        FontParams m_last_message_font_params;
        FontParams m_unread_counter_font_params;
        FontParams m_last_message_sender_font_params;

        QColor m_selected_chat_color;
        QColor m_unread_background_color;
    };

    class Behaviour
    {
    public:
        friend class UserConfigSingletone;

        // Getters

        [[nodiscard]] bool remove_whitespaces_around() const;
        [[nodiscard]] bool replace_hyphens() const;
        [[nodiscard]] bool send_message_by_enter() const;

        // Setters

        void set_remove_whitespaces_around(bool new_config);
        void set_replace_hyphens(bool new_config);
        void set_send_message_by_enter(bool new_config);

    private:
        Behaviour() = default;

        bool m_remove_whitespaces_around_message;
        bool m_replace_hyphens;
        bool m_send_message_by_enter;
    };

    [[nodiscard]] Appearance& appearance();
    [[nodiscard]] const Appearance& appearance() const;

    [[nodiscard]] Behaviour& behaviour();
    [[nodiscard]] const Behaviour& behaviour() const;

private:
    UserConfigSingletone() = default;

    Appearance m_appearance;
    Behaviour m_behaviour;
};


class DevelopConfigSingletone
{
public:
    [[nodiscard]] static DevelopConfigSingletone& get_instance()
    {
        static DevelopConfigSingletone instance;
        return instance;
    }

    DevelopConfigSingletone(const DevelopConfigSingletone& root) = delete;
    DevelopConfigSingletone& operator=(const DevelopConfigSingletone&) = delete;
    DevelopConfigSingletone(DevelopConfigSingletone&& root) = delete;
    DevelopConfigSingletone& operator=(DevelopConfigSingletone&&) = delete;

    class Appearance
    {
    public:
        friend class DevelopConfigSingletone;

        [[nodiscard]] const QColor& selected_message_color() const;
        [[nodiscard]] int min_message_width() const;
        [[nodiscard]] qreal scale_message_width() const;
        [[nodiscard]] int base_margin() const;
        [[nodiscard]] int icon_diameter() const;
        [[nodiscard]] int message_round_radius() const;
        [[nodiscard]] int chat_base_margin() const;
        [[nodiscard]] int chat_icon_radius() const;
        [[nodiscard]] int unread_indicator_round() const;
        [[nodiscard]] const QColor& item_under_mouse_color() const;

    private:
        Appearance() = default;

        // for message item delegate
        const QColor m_selected_message_color{/*r*/30, /*g*/30, /*b*/90, /*a*/50};
        const int m_min_message_width = 120;
        const qreal m_scale_message_width = 0.75;

        const int m_base_margin = 5;
        const int m_icon_diameter = 30;
        const int m_message_round_radius = 10;

        // for chat item delegate
        const int m_chat_base_margin = 6;
        const int m_chat_icon_radius = 20;
        const int m_unread_indicator_round = 10;
        const QColor m_item_under_mouse_color{/*r*/230, /*g*/230, /*b*/230};
    };

    [[nodiscard]] Appearance& appearance();
    [[nodiscard]] const Appearance& appearance() const;

private:
    DevelopConfigSingletone() = default;

    Appearance m_appearance;
};

void save_settings_to_yaml();
void set_standart_settings();
void parse_settings(const YAML::Node& conf_file);

}  // namespace melon::client_desktop

#include "config.ipp"

#endif  // MELON_CLIENT_DESKTOP_CONFIG_HPP_
