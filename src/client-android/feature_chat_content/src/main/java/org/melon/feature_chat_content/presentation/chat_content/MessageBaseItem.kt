package org.melon.feature_chat_content.presentation.chat_content

import androidx.annotation.LayoutRes
import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item

abstract class MessageBaseItem() : Item<GroupieViewHolder>() {

    abstract val layoutId: Int

    override fun getLayout(): Int = layoutId

    override fun bind(viewHolder: GroupieViewHolder, position: Int) {}
}