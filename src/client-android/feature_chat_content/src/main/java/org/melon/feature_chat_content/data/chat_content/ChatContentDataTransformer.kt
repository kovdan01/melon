package org.melon.feature_chat_content.data.chat_content

import android.net.Uri
import org.melon.feature_chat_content.data.db.MessageDataEntity
import org.melon.feature_chat_content.data.db.MessageFileDataEntity
import org.melon.feature_chat_content.domain.model.File
import org.melon.feature_chat_content.domain.model.Message
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
            isRead = isRead,
            files = files.map(::transform)
        )
    }

    fun transform(data: Message): MessageDataEntity = with(data) {
        MessageDataEntity(
            messageId = messageId,
            chatId = chatId,
            messageText = messageText,
            messageDate = messageDate.time,
            isUserMessage = isUserMessage,
            isRead = isRead,
            files = files.map(::transform)
        )
    }

    fun transform(data: File): MessageFileDataEntity = with(data) {
        MessageFileDataEntity(
            fileName = fileName
        )
    }

    fun transform(data: MessageFileDataEntity): File = with(data) {
        File(
            fileName = fileName,
            uri = Uri.parse("test://uri.com")
        )
    }
}