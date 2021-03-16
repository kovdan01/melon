package org.melon.feature_chats_list.domain

import org.melon.core.domain.base.BaseUseCase
import org.melon.feature_chats_list.data.ChatsListRepository
import javax.inject.Inject

interface ChatsListUseCase{
    fun getChatsList(): List<String>
}

class ChatsListUseCaseImpl @Inject constructor(
    private val chatsListRepository: ChatsListRepository
) : ChatsListUseCase {
    override fun getChatsList(): List<String> {
        return chatsListRepository.getChatsList()
    }
}
