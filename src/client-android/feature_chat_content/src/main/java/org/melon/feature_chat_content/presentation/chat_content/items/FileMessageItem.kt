package org.melon.feature_chat_content.presentation.chat_content.items

import com.xwray.groupie.GroupieViewHolder
import kotlinx.android.synthetic.main.item_user_file_message.view.*
import org.melon.feature_chat_content.R

//TODO: need FileUi
class FileMessageItem(val fileName: String): MessageBaseItem() {

    override val layoutId: Int = R.layout.item_user_file_message

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        messageTextTv.text = fileName
    }
}