package org.melon.feature_chat_content.domain.chat_content

import javax.inject.Inject

class ChatContentUseCase @Inject constructor(
        private val chatContentRepository: ChatContentRepository
) {
    suspend fun getChatDraft(chatId: Int) = chatContentRepository.getChatDraft(chatId)
    suspend fun saveChatDraft(chatId: Int, chatDraft: String?) =
            chatContentRepository.setChatDraft(chatId, chatDraft)

    fun getMessage(chatId: Int) = chatContentRepository.getMessages(chatId)

    suspend fun sendMessage(messageText: String, chatId: Int) {
        chatContentRepository.addMessage(messageText, chatId)
    }

    suspend fun editMessage(message: Message) {
        chatContentRepository.updateMessage(message)
    }

    suspend fun deleteMessages(messages: List<Message>) {
        chatContentRepository.deleteMessages(messages)
    }
}