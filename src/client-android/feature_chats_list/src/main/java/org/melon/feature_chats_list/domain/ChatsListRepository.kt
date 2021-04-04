package org.melon.feature_chats_list.domain

import kotlinx.coroutines.flow.Flow
import org.melon.core.data.base.BaseRepository
import org.melon.feature_chat_content.domain.chat_content.Message

interface ChatsListRepository : BaseRepository {
    fun getChatsList(): Flow<List<Chat>>
    suspend fun createChat(chatName: String)
    suspend fun updateChat(chat: Chat)
    suspend fun updateChatInfo(message: Message)
    suspend fun deleteChat(chat: Chat)
}