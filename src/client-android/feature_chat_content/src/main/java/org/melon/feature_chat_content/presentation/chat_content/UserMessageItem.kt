package org.melon.feature_chat_content.presentation.chat_content

import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import kotlinx.android.synthetic.main.item_other_message.view.messageTextTv
import kotlinx.android.synthetic.main.item_user_message.view.*
import org.melon.feature_chat_content.R

class UserMessageItem(
    private val message: MessageUi,
    private val onLongClick: (MessageUi) -> Unit,
    private val onClick: (MessageUi) -> Unit
) : MessageBaseItem() {


    override val layoutId: Int = R.layout.item_user_message

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        messageTextTv.text = message.messageText

        setOnLongClickListener {
            onLongClick(message)
            true
        }

        setOnClickListener {
            onClick(message)
        }

        if (message.isSelected) {
            viewHolder.itemView.messageTextTv.setBackgroundResource(R.drawable.background_selected_user_message)
        } else {
            viewHolder.itemView.messageTextTv.setBackgroundResource(R.drawable.background_user_message)
        }

        messageTimeTv.text = messageTimeFormatter.format(message.messageDate)
    }

    override fun isSameAs(other: Item<*>): Boolean {
        return other is UserMessageItem && other.message.messageId == message.messageId
    }
}