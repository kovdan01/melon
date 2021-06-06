package org.melon.feature_chat_content.presentation.chat_content.items

import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import kotlinx.android.synthetic.main.item_attached_file.view.*
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.presentation.chat_content.model.FileUi

class AttachedFileItem(
    val file: FileUi,
    val onCloseClick: (FileUi) -> Unit
) : Item<GroupieViewHolder>() {

    override fun getLayout(): Int = R.layout.item_attached_file

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        fileNameTv.text = file.fileName
        fileCloseIv.setOnClickListener {
            onCloseClick(file)
        }
    }

    override fun isSameAs(other: Item<*>): Boolean {
        return other is AttachedFileItem && other.file.uri == file.uri
    }
}