package org.melon.melonmessenger.data

import androidx.room.Database
import androidx.room.RoomDatabase
import org.melon.feature_chat_content.data.db.ChatDraftDataEntity
import org.melon.feature_chat_content.data.db.ChatContentDao
import org.melon.feature_chat_content.data.db.MessageDataEntity
import org.melon.feature_chats_list.data.db.ChatDataEntity
import org.melon.feature_chats_list.data.db.ChatsListDao

@Database(version = 1, entities = [ChatDraftDataEntity::class, ChatDataEntity::class, MessageDataEntity::class])
abstract class AppDatabase : RoomDatabase() {
    companion object {
        const val NAME = "melon_app_database"
    }

    abstract fun chatContentDao(): ChatContentDao

    abstract fun chatsListDao(): ChatsListDao
}