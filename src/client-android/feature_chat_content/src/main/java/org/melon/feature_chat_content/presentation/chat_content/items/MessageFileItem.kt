package org.melon.feature_chat_content.presentation.chat_content.items

import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import kotlinx.android.synthetic.main.item_message_file_item.view.*
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.presentation.chat_content.model.FileUi

class MessageFileItem(private val file: FileUi) : Item<GroupieViewHolder>() {

    override fun getLayout(): Int = R.layout.item_message_file_item

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        fileNameTv.text = file.fileName
    }

    override fun isSameAs(other: Item<*>): Boolean {
        return other is MessageFileItem && other.file.uri == file.uri
    }
}