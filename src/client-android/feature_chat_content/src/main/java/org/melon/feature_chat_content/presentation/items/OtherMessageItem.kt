package org.melon.feature_chat_content.presentation.items

import com.xwray.groupie.kotlinandroidextensions.GroupieViewHolder
import com.xwray.groupie.kotlinandroidextensions.Item
import kotlinx.android.synthetic.main.item_other_message.view.*
import org.melon.feature_chat_content.R

class OtherMessageItem(private val messageText: String) : Item() {
    override fun getLayout(): Int = R.layout.item_other_message

    override fun bind(viewHolder: GroupieViewHolder, position: Int) {
        viewHolder.itemView.messageTextTv.text = messageText
    }
}