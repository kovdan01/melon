#include <config.hpp>
#include <settings_dialog.hpp>
#include <storage_singletones.hpp>

#include <QPushButton>
#include <QFontComboBox>
#include <QDialog>

// For Log
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace melon::client_desktop
{

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog{parent}
    , m_ui{new Ui::SettingsDialog}
{
    m_ui->setupUi(this);

    connect(m_ui->SettingsButtonBox->button(QDialogButtonBox::RestoreDefaults),
            &QAbstractButton::clicked,
            this,
            &SettingsDialog::set_default);

    connect(m_ui->SettingsButtonBox->button(QDialogButtonBox::Apply),
            &QAbstractButton::clicked,
            this,
            &SettingsDialog::applied);

    connect(m_ui->SettingsTabs,
            &QTabWidget::currentChanged,
            this,
            &SettingsDialog::current_tab_changed);

    m_ui->SettingsButtonBox->button(QDialogButtonBox::RestoreDefaults)->setText(tr("Default"));
    this->setFixedSize(this->size().width(), this->size().height());
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::applied()
{
    auto& config = UserConfigSingletone::get_instance();

    foreach (auto* tab, m_visited_tabs)
    {
        BOOST_LOG_TRIVIAL(info) << "Applying tab: " << tab->objectName().toStdString();
        if (tab->objectName() == QStringLiteral("Appearance"))
        {
            UserConfigSingletone::Appearance& app = config.appearance();

            auto* font_box = tab->findChild<QComboBox* >(QStringLiteral("FontComboBox"));
            QString font_family = font_box->currentText();
            app.set_font_family_common(font_family);
            BOOST_LOG_TRIVIAL(info) << "Now font family is " << config.appearance().chat_name_font_params().family.toStdString();

            auto* message_size_slider = tab->findChild<QSlider*>(QStringLiteral("MessageFontSizeSlider"));
            int message_font_size = message_size_slider->sliderPosition();
            app.set_message_font_size(static_cast<UserConfigSingletone::Appearance::FontSize>(message_font_size));
            BOOST_LOG_TRIVIAL(info) << "Now message font size is " << message_font_size;

            auto* chatlist_size_slider = tab->findChild<QSlider*>(QStringLiteral("ChatFontSizeSlider"));
            int chatlist_font_size = chatlist_size_slider->sliderPosition();
            app.set_chatlist_font_size(static_cast<UserConfigSingletone::Appearance::FontSize>(chatlist_font_size));
            BOOST_LOG_TRIVIAL(info) << "Now chatlist font size is " << message_font_size;

            BOOST_LOG_TRIVIAL(info) << "[apply] EMIT SETTINGS SIGNAL applied appearance settings";
            emit this->applied_appearance_settings();
        }
        else if (tab->objectName() == QStringLiteral("Behaviour"))
        {
            UserConfigSingletone::Behaviour& behav = config.behaviour();

            auto* white_check_box = tab->findChild<QCheckBox* >(QStringLiteral("RemoveWhiteCheckBox"));
            bool is_check = white_check_box->isChecked();
            BOOST_LOG_TRIVIAL(info) << "Now remove_white_around is " << is_check;
            behav.set_remove_whitespaces_around(is_check);

            auto* enter_to_send_button = tab->findChild<QRadioButton* >(QStringLiteral("EnterToSendButton"));
            is_check = enter_to_send_button->isChecked();
            BOOST_LOG_TRIVIAL(info) << "Enter to send message? " << is_check;
            behav.set_send_message_by_enter(is_check);

            auto* replace_hyphens_check_box = tab->findChild<QCheckBox* >(QStringLiteral("ReplaceHyphensCheckBox"));
            is_check = replace_hyphens_check_box->isChecked();
            BOOST_LOG_TRIVIAL(info) << "Replace hyphens? " << is_check;
            behav.set_replace_hyphens(is_check);
        }
    }

    save_settings_to_yaml();
    m_visited_tabs.clear();
}

void SettingsDialog::set_default()
{
    set_standart_settings();
    this->load_current_settings(m_ui->SettingsTabs->currentWidget());
}

void SettingsDialog::closeEvent(QCloseEvent* event)
{
    m_visited_tabs.clear();
    event->accept();
}

void SettingsDialog::load_current_settings()
{
    m_ui->SettingsTabs->setCurrentIndex(0);
    this->load_current_settings(m_ui->SettingsTabs->currentWidget());
}

void SettingsDialog::load_current_settings(QWidget* tab)
{
    QString tab_name  = tab->objectName();
    BOOST_LOG_TRIVIAL(info) << "Tab name is " << tab_name.toStdString();

    if (tab_name == QStringLiteral("Appearance"))
    {
        auto& config = UserConfigSingletone::get_instance();
        UserConfigSingletone::Appearance& app = config.appearance();
        QString cur_font_family = app.font_family();
        auto* font_combobox = tab->findChild<QComboBox*>(QStringLiteral("FontComboBox"));
        font_combobox->setCurrentText(cur_font_family);

        auto* message_size_slider = tab->findChild<QSlider*>(QStringLiteral("MessageFontSizeSlider"));
        message_size_slider->setSliderPosition(static_cast<int>(app.message_font_size()));

        auto* chat_size_slider = tab->findChild<QSlider*>(QStringLiteral("ChatFontSizeSlider"));
        chat_size_slider->setSliderPosition(static_cast<int>(app.chatlist_font_size()));
    }
    else if (tab_name == QStringLiteral("Account"))
    {
        auto& storage = DBSingletone::get_instance();

        auto* name_edit = tab->findChild<QTextEdit*>(QStringLiteral("NameEdit"));
        name_edit->setText(storage.me().full_name());
    }
    else if (tab_name == QStringLiteral("Behaviour"))
    {
        auto& config = UserConfigSingletone::get_instance();
        UserConfigSingletone::Behaviour& behav = config.behaviour();

        auto* white_check_box = tab->findChild<QCheckBox* >(QStringLiteral("RemoveWhiteCheckBox"));
        white_check_box->setChecked(behav.remove_whitespaces_around());
        BOOST_LOG_TRIVIAL(info) << "[loading settings] remove_whitespaces_around is " << behav.remove_whitespaces_around();

        auto* enter_to_send_button = tab->findChild<QRadioButton* >(QStringLiteral("EnterToSendButton"));
        auto* send_to_send_button = tab->findChild<QRadioButton* >(QStringLiteral("SendToSendButton"));
        bool is_enter_to_send = behav.send_message_by_enter();
        BOOST_LOG_TRIVIAL(info) << "[loading settings] enter to send message? " << behav.send_message_by_enter();

        auto* replace_hyphens_check_box = tab->findChild<QCheckBox* >(QStringLiteral("ReplaceHyphensCheckBox"));
        replace_hyphens_check_box->setChecked(behav.replace_hyphens());

        enter_to_send_button->setChecked(is_enter_to_send);
        send_to_send_button->setChecked(!is_enter_to_send);
    }
}

void SettingsDialog::current_tab_changed(int)
{
    BOOST_LOG_TRIVIAL(info) << "Tab changed!";

    QWidget* cur_tab = m_ui->SettingsTabs->currentWidget();
    if (!m_visited_tabs.contains(cur_tab))
    {
        m_visited_tabs.insert(cur_tab);
        load_current_settings(cur_tab);
    }
}

}  // namespace melon::client_desktop
