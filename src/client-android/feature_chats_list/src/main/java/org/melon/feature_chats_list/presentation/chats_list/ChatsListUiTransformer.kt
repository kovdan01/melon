package org.melon.feature_chats_list.presentation.chats_list

import org.melon.feature_chats_list.domain.Chat
import javax.inject.Inject

class ChatsListUiTransformer @Inject constructor() {

    fun transform(data: ChatUi): Chat = with(data) {
        return Chat(
            chatId = chatId,
            chatName = chatName,
            chatPreview = chatPreview,
            lastMessageDate = lastMessageDate,
            isRead = isRead
        )
    }

    fun transform(data: Chat): ChatUi = with(data) {
        return ChatUi(
            chatId = chatId,
            chatName = chatName,
            chatPreview = chatPreview,
            //TODO: normal parameters
            lastMessageDate = lastMessageDate,
            isRead = isRead
        )
    }
}