package org.melon.feature_chat_content.presentation.chat_content.items

import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import kotlinx.android.synthetic.main.item_other_message.view.*
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi

class OtherMessageItem(private val message: MessageUi) : MessageBaseItem() {

    override val layoutId: Int = R.layout.item_other_message

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        messageTextTv.text = message.messageText
        messageTimeTv.text = messageTimeFormatter.format(message.messageDate)
    }

    override fun isSameAs(other: Item<*>): Boolean {
        return other is OtherMessageItem && other.message.messageId == message.messageId
    }
}