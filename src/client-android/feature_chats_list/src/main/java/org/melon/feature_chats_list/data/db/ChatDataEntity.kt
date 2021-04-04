package org.melon.feature_chats_list.data.db

import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "ChatsListTable")
data class ChatDataEntity(
    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = "chat_id")
    val chatId: Int = 0,
    @ColumnInfo(name = "chat_name")
    val chatName: String,
    @ColumnInfo(name = "chat_preview")
    val chatPreview: String?,
    @ColumnInfo(name = "last_message_date")
    val lastMessageDate: Long?,
    @ColumnInfo(name = "is_read")
    val isRead: Boolean
)