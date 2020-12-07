package ru.romananchugov.feature_chats_list.presentation.items

import com.xwray.groupie.kotlinandroidextensions.GroupieViewHolder
import com.xwray.groupie.kotlinandroidextensions.Item
import kotlinx.android.synthetic.main.item_chat.view.*
import ru.romananchugov.feature_chats_list.R
import ru.romananchugov.feature_chats_list.presentation.models.ChatUi

class ChatItem(
    private val chatUi: ChatUi,
    private val onClickListener: ((ChatUi) -> Unit)? = null
) : Item() {
    override fun getLayout(): Int = R.layout.item_chat

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        chatNameTv.text = chatUi.chatName
        chatPreviewText.text = chatUi.chatPreview
        setOnClickListener {
            onClickListener?.invoke(chatUi)
        }
    }

    override fun isSameAs(other: com.xwray.groupie.Item<*>): Boolean {
        return true
    }
}