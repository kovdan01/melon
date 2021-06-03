package org.melon.feature_chat_content.presentation.chat_content.items

import android.view.View
import androidx.core.view.isVisible
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupieAdapter
import com.xwray.groupie.GroupieViewHolder
import com.xwray.groupie.Item
import kotlinx.android.synthetic.main.fragment_chat_content.*
import kotlinx.android.synthetic.main.item_other_message.view.messageTextTv
import kotlinx.android.synthetic.main.item_user_file_message.view.*
import kotlinx.android.synthetic.main.item_user_message.view.*
import kotlinx.android.synthetic.main.item_user_message.view.messageTimeTv
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.presentation.chat_content.model.FileUi
import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi

class UserMessageItem(
    private val message: MessageUi,
    private val onLongClick: (MessageUi) -> Unit,
    private val onClick: (MessageUi) -> Unit
) : MessageBaseItem() {


    override val layoutId: Int = R.layout.item_user_message

    override fun bind(viewHolder: GroupieViewHolder, position: Int) = with(viewHolder.itemView) {
        messageTextTv.text = message.messageText
        messageTextTv.isVisible = message.messageText.isNotBlank()

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

        bindFilesRv(this)
    }

    override fun isSameAs(other: Item<*>): Boolean {
        return other is UserMessageItem && other.message.messageId == message.messageId
    }

    private fun bindFilesRv(view: View) = with(view) {
        val adapter = GroupieAdapter()
        filesRv.adapter = adapter
        filesRv.layoutManager =
            LinearLayoutManager(context, LinearLayoutManager.HORIZONTAL, false)
        adapter.update(
            message.files.map {
                MessageFileItem(it)
            }
        )
        filesRv.isVisible = message.files.isNotEmpty()
    }
}