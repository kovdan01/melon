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

    [[nodiscard]] bool selection_mode()
    {
        return m_selection_mode;
    }

    void set_selection_mode(bool mode)
    {
        m_selection_mode = mode;
        if(mode)
        {
            BOOST_LOG_TRIVIAL(info) << "Now selection mode is MultiSelection";
            this->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Now selection mode is NoSelection";
            this->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        }
    }

    void disable_selection_mode();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_selection_mode = false;
};

}  // namespace melon::client_desktop

#endif  // MELON_CLIENT_DESKTOP_MESSAGE_LIST_VIEW_HPP_

