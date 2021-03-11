#ifndef MELON_CLIENT_DESKTOP_CONFIG_HPP_
#define MELON_CLIENT_DESKTOP_CONFIG_HPP_

#include <QColor>
#include <QString>
#include <QFont>

namespace melon::client_desktop
{

namespace user_config
{

namespace appearance
{

struct FontParams
{
    const QString font_family;
    const int font_size;
    const int weight;
};

static const FontParams m_sender_font_params = {QStringLiteral("Cantarell"), 9, QFont::DemiBold};
static const FontParams m_message_text_font_params = {QStringLiteral("Cantarell"), 9, 41};
static const FontParams m_timestamp_font_params = {QStringLiteral("Cantarell"), 6, 35};

constexpr static QColor M_RECEIVE_MESSAGE_COLOR{/*r*/ 255, /*g*/ 243, /*b*/ 223};
constexpr static QColor M_SENDED_MESSAGE_COLOR{/*r*/ 235, /*g*/ 235, /*b*/ 235};

}

}

namespace develop_config
{

namespace appearance
{

constexpr static QColor M_SELECTED_MESSAGE_COLOR{/*r*/30, /*g*/30, /*b*/90, /*a*/50};
const static int M_MIN_MESSAGE_WIDTH = 100;
constexpr static qreal M_SCALE_MESSAGE_LENGTH = 0.5;

constexpr static int M_BASE_MARGIN = 5;
constexpr static int M_ICON_DIAMETER = 30;
constexpr static int M_MESSAGE_ROUND_RADIUS = 10;

}

}

}

#endif  // MELON_CLIENT_DESKTOP_CONFIG_HPP_
