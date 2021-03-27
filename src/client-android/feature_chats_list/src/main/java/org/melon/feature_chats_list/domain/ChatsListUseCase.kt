package org.melon.feature_chats_list.domain

import kotlinx.coroutines.flow.Flow
import org.melon.feature_chat_content.domain.chat_content.Message
import javax.inject.Inject

class ChatsListUseCase @Inject constructor(
        private val chatsListRepository: ChatsListRepository
) {

    fun getChatsList(): Flow<List<Chat>> {
        return chatsListRepository.getChatsList()
    }

    suspend fun createChat(chatName: String) {
        chatsListRepository.createChat(chatName)
    }

    suspend fun renameChat(chat: Chat) {
        chatsListRepository.updateChat(chat)
    }

    suspend fun deleteChat(chat: Chat) {
        chatsListRepository.deleteChat(chat)
    }

    suspend fun markChatUnread(chat: Chat) {
        chatsListRepository.updateChat(chat.copy(isRead = false))
    }

    suspend fun markChatRead(chat: Chat) {
        chatsListRepository.updateChat(chat.copy(isRead = true))
    }

    suspend fun changeChatPreview(message: Message) {
        chatsListRepository.updateChatInfo(message)
    }


}
