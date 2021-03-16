package org.melon.feature_chats_list.presentation.chats_list

import androidx.core.view.isVisible
import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import kotlinx.android.synthetic.main.item_chat.view.*
import org.melon.feature_chats_list.R
import java.util.*

class ChatItem(
    private val chatUi: ChatUi,
    private val onLongClickListener: ((ChatUi) -> Unit)? = null,
    private val onClickListener: ((ChatUi) -> Unit)? = null
) : Item<GroupieViewHolder>() {
    override fun getLayout(): Int = R.layout.item_chat

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        chatNameTv.text = chatUi.chatName
        chatPreviewText.text = chatUi.chatPreview

        chatUi.lastMessageDate?.let { messageDate ->
            val cal = Calendar.getInstance()
            cal.time = messageDate
            chatEditTimeTv.text = chatEditTimeTv.context.getString(
                R.string.chat_item_time_placeholder,
                cal.get(Calendar.HOUR_OF_DAY),
                cal.get(Calendar.MINUTE)
            )
        }

        chatUnreadIndicator.isVisible = chatUi.isRead.not()

        setOnClickListener {
            onClickListener?.invoke(chatUi)
        }
        setOnLongClickListener {
            onLongClickListener?.invoke(chatUi)
            true
        }
    }

    override fun isSameAs(other: Item<*>): Boolean {
        return other is ChatItem && other.chatUi.chatId == chatUi.chatId
    }
}
