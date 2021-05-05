package org.melon.feature_chats_list.data

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.flow.mapNotNull
import kotlinx.coroutines.withContext
import org.melon.feature_chat_content.domain.chat_content.BaseMessage
import org.melon.feature_chats_list.data.db.ChatDataEntity
import org.melon.feature_chats_list.data.db.ChatsListDao
import org.melon.feature_chats_list.domain.Chat
import org.melon.feature_chats_list.domain.ChatsListRepository
import java.util.*
import javax.inject.Inject

class ChatsListRepositoryImpl @Inject constructor(
        private val chatsListDao: ChatsListDao,
        private val chatsListDataTransformer: ChatsListDataTransformer
) : ChatsListRepository {

    override fun getChatsList(): Flow<List<Chat>> {
        return chatsListDao.getChatsList()
                .mapNotNull {
                    it?.map { chatData ->
                        chatsListDataTransformer.transform(chatData)
                    }
                }
                .flowOn(Dispatchers.IO)

    }

    override suspend fun createChat(chatName: String) {
        return withContext(Dispatchers.IO) {
            chatsListDao.addChat(
                    ChatDataEntity(
                            chatName = chatName,
                            chatPreview = null,
                            lastMessageDate = Date().time,
                            isRead = true
                    )
            )
        }
    }

    override suspend fun updateChat(chat: Chat) {
        withContext(Dispatchers.IO) {
            chatsListDao.updateChat(chatsListDataTransformer.transform(chat))
        }
    }

    override suspend fun updateChatInfo(message: BaseMessage) {
        withContext(Dispatchers.IO) {
            chatsListDao.updateChatInfo(message.chatId, message.messageText, message.messageDate.time)
        }
    }

    override suspend fun deleteChat(chat: Chat) {
        withContext(Dispatchers.IO) {
            chatsListDao.deleteChat(chatsListDataTransformer.transform(chat))
        }
    }
}
