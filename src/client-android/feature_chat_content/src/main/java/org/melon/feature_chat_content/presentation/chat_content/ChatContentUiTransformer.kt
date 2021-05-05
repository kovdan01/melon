package org.melon.feature_chat_content.presentation.chat_content

import org.melon.feature_chat_content.domain.chat_content.BaseMessage
import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi
import javax.inject.Inject

class ChatContentUiTransformer @Inject constructor() {

    fun transform(data: MessageUi): BaseMessage = with(data) {
        return BaseMessage(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = messageDate,
            isUserMessage = isUserMessage,
            isRead = isRead
        )
    }

    fun transform(data: BaseMessage): MessageUi = with(data) {
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