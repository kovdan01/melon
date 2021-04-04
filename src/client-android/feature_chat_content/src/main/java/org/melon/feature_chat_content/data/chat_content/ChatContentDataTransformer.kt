package org.melon.feature_chat_content.data.chat_content

import org.melon.feature_chat_content.data.db.MessageDataEntity
import org.melon.feature_chat_content.domain.chat_content.Message
import java.util.*
import javax.inject.Inject

class ChatContentDataTransformer @Inject constructor() {

    fun transform(data: MessageDataEntity): Message = with(data) {
        Message(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = Date(messageDate),
            isUserMessage = isUserMessage,
            isRead = isRead
        )
    }

    fun transform(data: Message): MessageDataEntity = with(data) {
        MessageDataEntity(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = messageDate.time,
            isUserMessage = isUserMessage,
            isRead = isRead
        )
    }
}