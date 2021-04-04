package org.melon.feature_chat_content.data.db

import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "ChatMessagesTable")
data class MessageDataEntity(
    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = "message_id")
    val messageId: Int = 0,
    @ColumnInfo(name = "chat_id")
    val chatId: Int,
    @ColumnInfo(name = "message_text")
    val messageText: String,
    @ColumnInfo(name = "message_date")
    val messageDate: Long,
    @ColumnInfo(name = "is_user_message")
    val isUserMessage: Boolean,
    @ColumnInfo(name = "is_read")
    val isRead: Boolean
)