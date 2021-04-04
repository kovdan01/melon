package org.melon.feature_chats_list.domain

import java.util.*

data class Chat(
    val chatId: Int,
    val chatName: String,
    val chatPreview: String?,
    val lastMessageDate: Date?,
    val isRead: Boolean
)