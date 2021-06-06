package org.melon.feature_chat_content.domain.chat_content

import kotlinx.coroutines.flow.Flow
import org.melon.feature_chat_content.data.db.MessageFileDataEntity
import org.melon.feature_chat_content.domain.model.File
import org.melon.feature_chat_content.domain.model.Message

interface ChatContentRepository {

    suspend fun getChatDraft(chatId: Int): String?
    suspend fun setChatDraft(chatId: Int, chatDraft: String?)

    fun getMessages(chatId: Int): Flow<List<Message>>
    suspend fun addMessage(messageText: String, chatId: Int, files: List<File>)
    suspend fun updateMessage(message: Message)
    suspend fun deleteMessages(messages: List<Message>)
}