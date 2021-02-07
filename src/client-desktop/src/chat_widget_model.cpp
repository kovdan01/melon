#include "chat_widget_model.hpp"

namespace melon::client_desktop
{

ChatWidgetModel::ChatWidgetModel(QObject* parent)
    : QSqlTableModel{parent}
{
    setTable(QStringLiteral("messages"));
    setEditStrategy(OnFieldChange);
    select();
}

}  // namespace melon::client_desktop
