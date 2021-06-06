#ifndef MELON_CLIENT_DESKTOP_MESSAGE_LIST_VIEW_HPP_
#define MELON_CLIENT_DESKTOP_MESSAGE_LIST_VIEW_HPP_

#include <QListView>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace melon::client_desktop
{

class MessageListView : public QListView
{
    Q_OBJECT

public:
    MessageListView(QWidget* parent = nullptr);
    ~MessageListView() override = default;

    [[nodiscard]] bool multiselection_mode()
    {
        return m_multiselection_mode;
    }

    void set_multiselection_mode(bool mode);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_multiselection_mode = false;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_LIST_VIEW_HPP_

