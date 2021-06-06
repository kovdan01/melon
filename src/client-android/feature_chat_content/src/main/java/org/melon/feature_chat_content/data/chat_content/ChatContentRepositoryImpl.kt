package org.melon.feature_chat_content.data.chat_content

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.withContext
import org.melon.feature_chat_content.data.db.ChatContentDao
import org.melon.feature_chat_content.data.db.ChatDraftDataEntity
import org.melon.feature_chat_content.data.db.MessageDataEntity
import org.melon.feature_chat_content.data.db.MessageFileDataEntity
import org.melon.feature_chat_content.domain.chat_content.ChatContentRepository
import org.melon.feature_chat_content.domain.model.File
import org.melon.feature_chat_content.domain.model.Message
import java.util.*
import javax.inject.Inject

class ChatContentRepositoryImpl @Inject constructor(
    private val chatContentDao: ChatContentDao,
    private val chatContentDataTransformer: ChatContentDataTransformer
) : ChatContentRepository {
    override suspend fun getChatDraft(chatId: Int): String? {
        return withContext(Dispatchers.IO) {
            chatContentDao.getChatDraft(chatId)?.chatDraft
        }
    }

    override suspend fun setChatDraft(chatId: Int, chatDraft: String?) {
        withContext(Dispatchers.IO) {
            chatContentDao.saveChatDraft(
                ChatDraftDataEntity(
                    chatId = chatId,
                    chatDraft = chatDraft
                )
            )
        }
    }

    override fun getMessages(chatId: Int): Flow<List<Message>> {
        return chatContentDao.getMessages(chatId)
            .map {
                it?.map(chatContentDataTransformer::transform) ?: emptyList()
            }
            .flowOn(Dispatchers.IO)
    }

    override suspend fun addMessage(
        messageText: String,
        chatId: Int,
        files: List<File>
    ) {
        withContext(Dispatchers.IO) {
            chatContentDao.addMessage(
                MessageDataEntity(
                    chatId = chatId,
                    messageText = messageText,
                    messageDate = Date().time,
                    isUserMessage = true,
                    isRead = true,
                    files = files.map(chatContentDataTransformer::transform)
                )
            )
        }
    }

    override suspend fun updateMessage(message: Message) {
        withContext(Dispatchers.IO) {
            chatContentDao.updateMessage(chatContentDataTransformer.transform(message))
        }
    }

    override suspend fun deleteMessages(messages: List<Message>) {
        withContext(Dispatchers.IO) {
            chatContentDao.deleteMessage(
                *messages.map(chatContentDataTransformer::transform).toTypedArray()
            )
        }
    }
}