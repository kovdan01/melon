package org.melon.feature_chat_content.data.db

import androidx.room.*
import kotlinx.coroutines.flow.Flow

@Dao
interface ChatContentDao {

    @Query("select * from ChatDraftsTable where chat_id=:chatId")
    suspend fun getChatDraft(chatId: Int): ChatDraftDataEntity?

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun saveChatDraft(chatDraftDataEntity: ChatDraftDataEntity)


    @Query("select * from ChatMessagesTable where chat_id=:chatId")
    fun getMessages(chatId: Int): Flow<List<MessageDataEntity>?>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun addMessage(messageToAdd: MessageDataEntity)

    @Update
    suspend fun updateMessage(messageToUpdate: MessageDataEntity)

    @Delete
    suspend fun deleteMessage(vararg messageToDelete: MessageDataEntity)
}