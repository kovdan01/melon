#ifndef MELON_CLIENT_DESKTOP_CONFIG_HPP_
#define MELON_CLIENT_DESKTOP_CONFIG_HPP_

#include <yaml-cpp/yaml.h>

#include <QColor>
#include <QFont>
#include <QString>

// For Log
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
            STANDART,
            BIG
        };

        // Getters

        [[nodiscard]] FontParams sender_font_params() const
        {
            return m_sender_font_params;
        }

        [[nodiscard]] FontParams message_text_font_params() const
        {
            return m_message_text_font_params;
        }

        [[nodiscard]] FontParams timestamp_font_params() const
        {
            return m_timestamp_font_params;
        }

        [[nodiscard]] const QColor& receive_message_color() const
        {
            return m_receive_message_color;
        }

        [[nodiscard]] const QColor& sended_message_color() const
        {
            return m_sended_message_color;
        }

        [[nodiscard]] FontParams chat_name_font_params() const
        {
            return m_chat_name_font_params;
        }

        [[nodiscard]] FontParams chat_timestamp_font_params() const
        {
            return m_chat_timestamp_font_params;
        }

        [[nodiscard]] FontParams last_message_font_params() const
        {
            return m_last_message_font_params;
        }

        [[nodiscard]] FontParams unread_counter_font_params() const
        {
            return m_unread_counter_font_params;
        }

        [[nodiscard]] FontParams last_message_sender_font_params() const
        {
            return m_last_message_sender_font_params;
        }

        [[nodiscard]] QColor selected_chat_color() const
        {
            return m_selected_chat_color;
        }

        [[nodiscard]] QColor unread_background_color() const
        {
            return m_unread_background_color;
        }

        [[nodiscard]] QString font_family() const
        {
            return m_font_family;
        }

        [[nodiscard]] FontSize message_font_size() const
        {
            return m_message_font_size;
        }

        // Setters

        void set_sender_font_params(const FontParams& pm)
        {
            m_sender_font_params = pm;
        }

        void set_message_text_font_params(const FontParams& pm)
        {
            m_message_text_font_params = pm;
        }

        void set_timestamp_font_params(const FontParams& pm)
        {
            m_timestamp_font_params = pm;
        }

        void set_receive_message_color(const QColor& c)
        {
            m_receive_message_color = c;
        }

        void set_sended_message_color(const QColor& c)
        {
            m_sended_message_color = c;
        }

        void set_chat_name_font_params(const FontParams& pm)
        {
            m_chat_name_font_params = pm;
        }

        void set_chat_timestamp_font_params(const FontParams& pm)
        {
            m_chat_timestamp_font_params = pm;
        }

        void set_last_message_font_params(const FontParams& pm)
        {
            m_last_message_font_params = pm;
        }

        void set_unread_counter_font_params(const FontParams& pm)
        {
            m_unread_counter_font_params = pm;
        }

        void set_last_message_sender_font_params(const FontParams& pm)
        {
            m_last_message_sender_font_params = pm;
        }

        void set_selected_chat_color(const QColor& c)
        {
            m_selected_chat_color = c;
        }

        void set_unread_background_color(const QColor& c)
        {
            m_unread_background_color = c;
        }

        void set_font_family_common(const QString& family)
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

        void set_font_family_var(const QString& family)
        {
            m_font_family = family;
        }

        void set_message_font_size_var(FontSize size)
        {
            m_message_font_size = size;
        }

        void set_message_font_size(FontSize size)
        {
            switch (size)
            {
            case FontSize::SMALL:
                m_sender_font_params.size = 7;
                m_message_text_font_params.size = 7;
                m_timestamp_font_params.size = 6;
                break;
            case FontSize::STANDART:
                m_sender_font_params.size = 8;
                m_message_text_font_params.size = 8;
                m_timestamp_font_params.size = 7;
                break;
            case FontSize::BIG:
                m_sender_font_params.size = 9;
                m_message_text_font_params.size = 9;
                m_timestamp_font_params.size = 8;
                break;
            }
            m_message_font_size = size;
            BOOST_LOG_TRIVIAL(info) << "In config msg size is " << static_cast<int>(m_message_font_size);
        }        

    private:
        Appearance() = default;

        QString m_font_family;
        FontSize m_message_font_size;

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

    [[nodiscard]] Appearance& appearance()
    {
        return m_appearance;
    }

    [[nodiscard]] const Appearance& appearance() const
    {
        return m_appearance;
    }

private:
    UserConfigSingletone() = default;

    Appearance m_appearance;
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

        [[nodiscard]] const QColor& selected_message_color() const
        {
            return m_selected_message_color;
        }

        [[nodiscard]] int min_message_width() const
        {
            return m_min_message_width;
        }

        [[nodiscard]] qreal scale_message_width() const
        {
            return m_scale_message_width;
        }

        [[nodiscard]] int base_margin() const
        {
            return m_base_margin;
        }

        [[nodiscard]] int icon_diameter() const
        {
            return m_icon_diameter;
        }

        [[nodiscard]] int message_round_radius() const
        {
            return m_message_round_radius;
        }

        [[nodiscard]] int chat_base_margin() const
        {
            return m_chat_base_margin;
        }

        [[nodiscard]] int chat_icon_radius() const
        {
            return m_chat_icon_radius;
        }

        [[nodiscard]] int unread_indicator_round() const
        {
            return m_unread_indicator_round;
        }

        [[nodiscard]] const QColor& item_under_mouse_color() const
        {
            return m_item_under_mouse_color;
        }

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

    [[nodiscard]] Appearance& appearance()
    {
        return m_appearance;
    }

    [[nodiscard]] const Appearance& appearance() const
    {
        return m_appearance;
    }

private:
    DevelopConfigSingletone() = default;

    Appearance m_appearance;
};

void save_settings_to_yaml();
void set_standart_settings();
void parse_settings(const YAML::Node& conf_file);

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CONFIG_HPP_
