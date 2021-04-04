package org.melon.feature_chat_content.presentation.chat_content

import org.melon.feature_chat_content.domain.chat_content.Message
import javax.inject.Inject

class ChatContentUiTransformer @Inject constructor() {

    fun transform(data: MessageUi): Message = with(data) {
        return Message(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = messageDate,
            isUserMessage = isUserMessage,
            isRead = isRead
        )
    }

    fun transform(data: Message): MessageUi = with(data) {
        return MessageUi(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = messageDate,
            isUserMessage = isUserMessage,
            isRead = isRead,
            isSelected = false
        )
    }
}