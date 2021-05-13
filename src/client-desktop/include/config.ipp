namespace melon::client_desktop
{

using user_ap = UserConfigSingletone::Appearance;

[[nodiscard]] inline const user_ap::FontParams& user_ap::sender_font_params() const
{
    return m_sender_font_params;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::message_text_font_params() const
{
    return m_message_text_font_params;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::timestamp_font_params() const
{
    return m_timestamp_font_params;
}

[[nodiscard]] inline const QColor& user_ap::receive_message_color() const
{
    return m_receive_message_color;
}

[[nodiscard]] inline const QColor& user_ap::sended_message_color() const
{
    return m_sended_message_color;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::chat_name_font_params() const
{
    return m_chat_name_font_params;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::chat_timestamp_font_params() const
{
    return m_chat_timestamp_font_params;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::last_message_font_params() const
{
    return m_last_message_font_params;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::unread_counter_font_params() const
{
    return m_unread_counter_font_params;
}

[[nodiscard]] inline const user_ap::FontParams& user_ap::last_message_sender_font_params() const
{
    return m_last_message_sender_font_params;
}

[[nodiscard]] inline QColor user_ap::selected_chat_color() const
{
    return m_selected_chat_color;
}

[[nodiscard]] inline QColor user_ap::unread_background_color() const
{
    return m_unread_background_color;
}

[[nodiscard]] inline const QString& user_ap::font_family() const
{
    return m_font_family;
}

[[nodiscard]] inline user_ap::FontSize user_ap::message_font_size() const
{
    return m_message_font_size;
}

[[nodiscard]] inline user_ap::FontSize user_ap::chatlist_font_size() const
{
    return m_chatlist_font_size;
}

inline void user_ap::set_sender_font_params(const FontParams& pm)
{
    m_sender_font_params = pm;
}

inline void user_ap::set_message_text_font_params(const FontParams& pm)
{
    m_message_text_font_params = pm;
}

inline void user_ap::set_timestamp_font_params(const FontParams& pm)
{
    m_timestamp_font_params = pm;
}

inline void user_ap::set_receive_message_color(const QColor& c)
{
    m_receive_message_color = c;
}

inline void user_ap::set_sended_message_color(const QColor& c)
{
    m_sended_message_color = c;
}

inline void user_ap::set_chat_name_font_params(const FontParams& pm)
{
    m_chat_name_font_params = pm;
}

inline void user_ap::set_chat_timestamp_font_params(const FontParams& pm)
{
    m_chat_timestamp_font_params = pm;
}

inline void user_ap::set_last_message_font_params(const FontParams& pm)
{
    m_last_message_font_params = pm;
}

inline void user_ap::set_unread_counter_font_params(const FontParams& pm)
{
    m_unread_counter_font_params = pm;
}

inline void user_ap::set_last_message_sender_font_params(const FontParams& pm)
{
    m_last_message_sender_font_params = pm;
}

inline void user_ap::set_selected_chat_color(const QColor& c)
{
    m_selected_chat_color = c;
}

inline void user_ap::set_unread_background_color(const QColor& c)
{
    m_unread_background_color = c;
}

inline void user_ap::set_font_family_var(const QString& family)
{
    m_font_family = family;
}

inline void user_ap::set_message_font_size_var(FontSize size)
{
    m_message_font_size = size;
}

inline void user_ap::set_chatlist_font_size_var(FontSize size)
{
    m_chatlist_font_size = size;
}

using user_behav = UserConfigSingletone::Behaviour;

[[nodiscard]] inline bool user_behav::remove_whitespaces_around() const
{
    return m_remove_whitespaces_around_message;
}

[[nodiscard]] inline bool user_behav::replace_hyphens() const
{
    return m_replace_hyphens;
}

[[nodiscard]] inline bool user_behav::send_message_by_enter() const
{
    return m_send_message_by_enter;
}

inline void user_behav::set_remove_whitespaces_around(bool new_config)
{
    m_remove_whitespaces_around_message = new_config;
}

inline void user_behav::set_replace_hyphens(bool new_config)
{
    m_replace_hyphens = new_config;
}

inline void user_behav::set_send_message_by_enter(bool new_config)
{
    m_send_message_by_enter = new_config;
}

[[nodiscard]] inline UserConfigSingletone::Appearance& UserConfigSingletone::appearance()
{
    return m_appearance;
}

[[nodiscard]] inline const UserConfigSingletone::Appearance& UserConfigSingletone::appearance() const
{
    return m_appearance;
}

[[nodiscard]] inline UserConfigSingletone::Behaviour& UserConfigSingletone::behaviour()
{
    return m_behaviour;
}

[[nodiscard]] inline const UserConfigSingletone::Behaviour& UserConfigSingletone::behaviour() const
{
    return m_behaviour;
}

using dev_ap = DevelopConfigSingletone::Appearance;

[[nodiscard]] inline const QColor& dev_ap::selected_message_color() const
{
    return m_selected_message_color;
}

[[nodiscard]] inline int dev_ap::min_message_width() const
{
    return m_min_message_width;
}

[[nodiscard]] inline qreal dev_ap::scale_message_width() const
{
    return m_scale_message_width;
}

[[nodiscard]] inline int dev_ap::base_margin() const
{
    return m_base_margin;
}

[[nodiscard]] inline int dev_ap::icon_diameter() const
{
    return m_icon_diameter;
}

[[nodiscard]] inline int dev_ap::message_round_radius() const
{
    return m_message_round_radius;
}

[[nodiscard]] inline int dev_ap::chat_base_margin() const
{
    return m_chat_base_margin;
}

[[nodiscard]] inline int dev_ap::chat_icon_radius() const
{
    return m_chat_icon_radius;
}

[[nodiscard]] inline int dev_ap::unread_indicator_round() const
{
    return m_unread_indicator_round;
}

[[nodiscard]] inline const QColor& dev_ap::item_under_mouse_color() const
{
    return m_item_under_mouse_color;
}

[[nodiscard]] inline DevelopConfigSingletone::Appearance& DevelopConfigSingletone::appearance()
{
    return m_appearance;
}

[[nodiscard]] inline const DevelopConfigSingletone::Appearance& DevelopConfigSingletone::appearance() const
{
    return m_appearance;
}

}  // namespace melon::client_desktop
