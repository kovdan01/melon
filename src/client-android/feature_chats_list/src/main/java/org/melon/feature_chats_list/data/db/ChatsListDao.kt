package org.melon.feature_chats_list.data.db

import androidx.room.*
import kotlinx.coroutines.flow.Flow

@Dao
interface ChatsListDao {

    @Query("select * from ChatsListTable order by case when last_message_date not null then last_message_date else chat_id end desc")
    fun getChatsList(): Flow<List<ChatDataEntity>?>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun addChat(newChat: ChatDataEntity)

    @Update
    suspend fun updateChat(chatToUpdate: ChatDataEntity)

    @Query("update ChatsListTable set chat_preview=:chatPreview, last_message_date=:lastMessageDate where chat_id=:chatId")
    suspend fun updateChatInfo(chatId: Int, chatPreview: String, lastMessageDate: Long)

    @Delete
    suspend fun deleteChat(chatToDelete: ChatDataEntity)
}