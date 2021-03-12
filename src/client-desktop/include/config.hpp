#ifndef MELON_CLIENT_DESKTOP_CONFIG_HPP_
#define MELON_CLIENT_DESKTOP_CONFIG_HPP_

#include <QColor>
#include <QString>
#include <QFont>

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
        struct FontParams
        {
            const QString family;
            const int size;
            const int weight;
        };

        [[nodiscard]] const FontParams sender_font_params() const
        {
            return M_SENDER_FONT_PARAMS;
        }

        [[nodiscard]] const FontParams message_text_font_params() const
        {
            return M_MESSAGE_TEXT_FONT_PARAMS;
        }

        [[nodiscard]] const FontParams timestamp_font_params() const
        {
            return M_TIMESTAMP_FONT_PARAMS;
        }

        [[nodiscard]] const QColor receive_message_color() const
        {
            return M_RECEIVE_MESSAGE_COLOR;
        }

        [[nodiscard]] const QColor sended_message_color() const
        {
            return M_SENDED_MESSAGE_COLOR;
        }

        friend class UserConfigSingletone;
    private:
        const FontParams M_SENDER_FONT_PARAMS = {QStringLiteral("Cantarell"), 9, QFont::DemiBold};
        const FontParams M_MESSAGE_TEXT_FONT_PARAMS = {QStringLiteral("Cantarell"), 9, 41};
        const FontParams M_TIMESTAMP_FONT_PARAMS = {QStringLiteral("Cantarell"), 6, 35};

        const QColor M_RECEIVE_MESSAGE_COLOR{/*r*/ 255, /*g*/ 243, /*b*/ 223};
        const QColor M_SENDED_MESSAGE_COLOR{/*r*/ 235, /*g*/ 235, /*b*/ 235};
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
        [[nodiscard]] const QColor& selected_message_color() const
        {
            return M_SELECTED_MESSAGE_COLOR;
        }

        [[nodiscard]] const int& min_message_width() const
        {
            return M_MIN_MESSAGE_WIDTH;
        }

        [[nodiscard]] const qreal& scale_message_width() const
        {
            return M_SCALE_MESSAGE_WIDTH;
        }

        [[nodiscard]] const int& base_margin() const
        {
            return M_BASE_MARGIN;
        }

        [[nodiscard]] const int& icon_diameter() const
        {
            return M_ICON_DIAMETER;
        }

        [[nodiscard]] const int& message_round_radius() const
        {
            return M_MESSAGE_ROUND_RADIUS;
        }

        friend class DevelopConfigSingletone;
    private:
        const QColor M_SELECTED_MESSAGE_COLOR{/*r*/30, /*g*/30, /*b*/90, /*a*/50};
        const int M_MIN_MESSAGE_WIDTH = 100;
        const qreal M_SCALE_MESSAGE_WIDTH = 0.5;

        const int M_BASE_MARGIN = 5;
        const int M_ICON_DIAMETER = 30;
        const int M_MESSAGE_ROUND_RADIUS = 10;

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

}

#endif  // MELON_CLIENT_DESKTOP_CONFIG_HPP_
