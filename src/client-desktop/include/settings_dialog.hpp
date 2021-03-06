#ifndef MELON_CLIENT_DESKTOP_SETTINGS_DIALOG_HPP_
#define MELON_CLIENT_DESKTOP_SETTINGS_DIALOG_HPP_

#include <ui_settings_dialog.h>

#include <QDialog>
#include <QSet>

QT_BEGIN_NAMESPACE
namespace Ui  // NOLINT (readability-identifier-naming)
{
    class SettingsDialog;
}  // namespace Ui
QT_END_NAMESPACE

namespace melon::client_desktop
{

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

    void load_current_settings();

signals:
    void applied_appearance_settings();

private:
    void set_default();
    void closeEvent(QCloseEvent* event) override;
    void applied();
    void load_current_settings(QWidget* tab);
    void current_tab_changed(int /*new_tab_index*/);

    QSet<QWidget*> m_visited_tabs;

    Ui::SettingsDialog* m_ui;
};

}  // namespace melon::client_desktop

#endif // MELON_CLIENT_DESKTOP_SETTINGS_DIALOG_HPP_
