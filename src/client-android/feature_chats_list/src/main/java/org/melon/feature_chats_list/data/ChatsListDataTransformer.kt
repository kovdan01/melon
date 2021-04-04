package org.melon.feature_chats_list.data

import org.melon.feature_chats_list.data.db.ChatDataEntity
import org.melon.feature_chats_list.domain.Chat
import java.util.*
import javax.inject.Inject

class ChatsListDataTransformer @Inject constructor() {

    fun transform(data: ChatDataEntity): Chat = with(data) {
        return Chat(
            chatId = chatId,
            chatName = chatName,
            chatPreview = chatPreview,
            lastMessageDate = lastMessageDate?.let { Date(it) },
            isRead = isRead
        )
    }

    fun transform(data: Chat): ChatDataEntity = with(data) {
        return ChatDataEntity(
            chatId = chatId,
            chatName = chatName,
            chatPreview = chatPreview,
            lastMessageDate = lastMessageDate?.time,
            isRead = isRead
        )
    }
}