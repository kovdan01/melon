#include "main_window.hpp"

namespace melon::client_desktop
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);
}

}  // namespace melon::client_desktop
