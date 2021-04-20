#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP

#include <ui_settings_dialog.h>

#include <QDialog>

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

    void save_all();

private:
    Ui::SettingsDialog* m_ui;
};

}  // namespace melon::client_desktop

#endif // SETTINGS_DIALOG_HPP
