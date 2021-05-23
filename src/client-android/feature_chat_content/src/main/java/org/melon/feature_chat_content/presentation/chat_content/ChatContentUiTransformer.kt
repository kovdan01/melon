package org.melon.feature_chat_content.presentation.chat_content

import org.melon.feature_chat_content.domain.model.File
import org.melon.feature_chat_content.domain.model.Message
import org.melon.feature_chat_content.presentation.chat_content.model.FileUi
import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi
import javax.inject.Inject

class ChatContentUiTransformer @Inject constructor() {

    fun transform(data: MessageUi): Message = with(data) {
        return Message(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = messageDate,
            isUserMessage = isUserMessage,
            isRead = isRead,
            files = files.map(::transform)
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
            isSelected = false,
            files = files.map(::transform)
        )
    }

    fun transform(data: FileUi): File = with(data) {
        return File(
            fileName = fileName,
            uri = uri
        )
    }

    fun transform(data: File): FileUi = with(data) {
        return FileUi(
            fileName = fileName,
            uri = uri
        )
    }
}