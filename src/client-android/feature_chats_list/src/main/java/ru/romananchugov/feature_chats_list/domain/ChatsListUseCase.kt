package ru.romananchugov.feature_chats_list.domain

import ru.romananchugov.feature_chats_list.data.ChatsListRepository
import ru.romananchugov.melonmessenger.domain.base.BaseUseCase
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