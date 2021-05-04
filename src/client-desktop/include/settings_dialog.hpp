#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP

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
    ~SettingsDialog();

    void load_current_settings();

signals:
    void applied_appearance_settings();

private:
    void save_all();
    void set_default();
    void cancel_changes();
    void applied();
    void load_current_settings(QWidget* tab);
    void current_tab_changed(int);

    QSet<QWidget* > m_visited_tabs;

    Ui::SettingsDialog* m_ui;
};

}  // namespace melon::client_desktop

#endif // SETTINGS_DIALOG_HPP
