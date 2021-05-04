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

    connect(m_ui->SettingsButtonBox->button(QDialogButtonBox::Save),
            &QAbstractButton::clicked,
            this,
            &SettingsDialog::save_all);

    connect(m_ui->SettingsButtonBox->button(QDialogButtonBox::RestoreDefaults),
            &QAbstractButton::clicked,
            this,
            &SettingsDialog::set_default);

    connect(m_ui->SettingsButtonBox->button(QDialogButtonBox::Cancel),
            &QAbstractButton::clicked,
            this,
            &SettingsDialog::cancel_changes);

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

void SettingsDialog::save_all()
{
    auto& config = UserConfigSingletone::get_instance();
    int count = m_ui->SettingsTabs->count();
    for (int i = 0; i < count; ++i)
    {
        QWidget* cur_tab = m_ui->SettingsTabs->widget(i);

        if (cur_tab->objectName() == QStringLiteral("Appearance"))
        {
            UserConfigSingletone::Appearance& app = config.appearance();
            auto* font_box = cur_tab->findChild<QComboBox* >(QStringLiteral("FontComboBox"));
            QString font_family = font_box->currentText();
            app.set_font_family_common(font_family);
            BOOST_LOG_TRIVIAL(info) << "Now font family is " << config.appearance().chat_name_font_params().family.toStdString();

            auto* message_size_slider = cur_tab->findChild<QSlider*>(QStringLiteral("MessageFontSizeSlider"));
            int message_font_size = message_size_slider->sliderPosition();
            app.set_message_font_size(static_cast<UserConfigSingletone::Appearance::FontSize>(message_font_size));
            BOOST_LOG_TRIVIAL(info) << "Now message font size is " << message_font_size;
        }
    }

    save_settings_to_yaml();
    m_visited_tabs.clear();
    emit this->applied_appearance_settings();
    BOOST_LOG_TRIVIAL(info) << "SETTINGS SIGNAL applied appearance settings";
}

void SettingsDialog::set_default()
{
    set_standart_settings();
    this->load_current_settings(m_ui->SettingsTabs->currentWidget());
}

void SettingsDialog::cancel_changes()
{
    m_visited_tabs.clear();
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
        chat_size_slider->setSliderPosition(static_cast<int>(app.chat_font_size()));
    }
    else if (tab_name == QStringLiteral("Account"))
    {
        auto& storage = DBSingletone::get_instance();

        auto* name_edit = tab->findChild<QTextEdit*>(QStringLiteral("NameEdit"));
        name_edit->setText(storage.me().full_name());
    }
}

void SettingsDialog::current_tab_changed(int)
{
//    std::cout << "Tab changed!" << std::endl;
    BOOST_LOG_TRIVIAL(info) << "Tab changed!";

    QWidget* cur_tab = m_ui->SettingsTabs->currentWidget();
    if (!m_visited_tabs.contains(cur_tab))
    {
        m_visited_tabs.insert(cur_tab);
        load_current_settings(cur_tab);
    }
}

void SettingsDialog::applied()
{
    this->save_all();    
    BOOST_LOG_TRIVIAL(info) << "EMIT SETTINGS SIGNAL applied appearance settings";
    this->setModal(false);
    emit this->applied_appearance_settings();
    this->setModal(true);
}

}  // namespace melon::client_desktop
