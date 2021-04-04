package org.melon.feature_chat_content.data.db

import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "ChatDraftsTable")
data class ChatDraftDataEntity(
    @PrimaryKey
    @ColumnInfo(name = "chat_id")
    val chatId: Int,
    @ColumnInfo(name = "chat_draft") val chatDraft: String?
)