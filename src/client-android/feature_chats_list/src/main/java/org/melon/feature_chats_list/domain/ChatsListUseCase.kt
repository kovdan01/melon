package org.melon.feature_chats_list.domain

import org.melon.feature_chats_list.data.ChatsListRepository
import org.melon.melonmessenger.domain.base.BaseUseCase
import javax.inject.Inject

interface ChatsListUseCase : BaseUseCase {
    fun getChatsList(): List<String>
}

class ChatsListUseCaseImpl @Inject constructor(
    private val chatsListRepository: ChatsListRepository
) : ChatsListUseCase {
    override fun getChatsList(): List<String> {
        return chatsListRepository.getChatsList()
    }
}