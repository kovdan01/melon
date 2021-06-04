package org.melon.feature_chat_content.presentation.chat_content.items

import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import java.text.SimpleDateFormat
import java.util.*

abstract class MessageBaseItem() : Item<GroupieViewHolder>() {

    protected val messageTimeFormatter = SimpleDateFormat("HH:mm", Locale.getDefault())

    abstract val layoutId: Int

    override fun getLayout(): Int = layoutId

    override fun bind(viewHolder: GroupieViewHolder, position: Int) {}
}