package org.melon.feature_chats_list.data

import org.melon.melonmessenger.data.base.BaseRepository
import javax.inject.Inject

interface ChatsListRepository : BaseRepository {
    fun getChatsList(): List<String>
}

class ChatsListRepositoryImpl @Inject constructor() : ChatsListRepository {
    override fun getChatsList(): List<String> {
        return listOf("123", "321", "235")
    }

}
