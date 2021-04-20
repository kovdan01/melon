#include <settings_dialog.hpp>
#include <config.hpp>

#include <QPushButton>
#include <QFontComboBox>
#include <QDialog>

#include <iostream>

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
        std::cout << "Widget name is " << cur_tab->objectName().toStdString() << std::endl;

        if (cur_tab->objectName() == QStringLiteral("Appearance"))
        {
            auto* font_box = cur_tab->findChild<QComboBox* >(QStringLiteral("FontComboBox"));
            QString font_family = font_box->currentText();
            config.appearance().set_font_family_common(font_family);
            std::cout << "Now font family is " << config.appearance().chat_name_font_params().family.toStdString() << std::endl;
        }
    }

    save_settings_to_yaml();
}

}  // namespace melon::client_desktop
