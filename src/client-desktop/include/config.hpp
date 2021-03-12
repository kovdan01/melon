#ifndef MELON_CLIENT_DESKTOP_CONFIG_HPP_
#define MELON_CLIENT_DESKTOP_CONFIG_HPP_

#include <QColor>
#include <QFont>
#include <QString>

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
            const QString family;
            const int size;
            const int weight;
        };

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

    private:
        const FontParams m_sender_font_params = {QStringLiteral("Cantarell"), 9, QFont::DemiBold};
        const FontParams m_message_text_font_params = {QStringLiteral("Cantarell"), 9, 41};
        const FontParams m_timestamp_font_params = {QStringLiteral("Cantarell"), 6, 35};

        const QColor m_receive_message_color{/*r*/ 255, /*g*/ 243, /*b*/ 223};
        const QColor m_sended_message_color{/*r*/ 235, /*g*/ 235, /*b*/ 235};
        Appearance() = default;

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

    private:
        const QColor m_selected_message_color{/*r*/30, /*g*/30, /*b*/90, /*a*/50};
        const int m_min_message_width = 100;
        const qreal m_scale_message_width = 0.5;

        const int m_base_margin = 5;
        const int m_icon_diameter = 30;
        const int m_message_round_radius = 10;

        Appearance() = default;

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

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_CONFIG_HPP_
