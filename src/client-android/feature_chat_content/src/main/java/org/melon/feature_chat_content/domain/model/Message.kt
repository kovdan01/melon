package org.melon.feature_chat_content.domain.model

import java.util.*

data class Message(
    val messageId: Int,
    val chatId: Int,
    val messageText: String,
    val messageDate: Date,
    val isUserMessage: Boolean,
    val isRead: Boolean = false,
    val files: List<File> = emptyList()
)