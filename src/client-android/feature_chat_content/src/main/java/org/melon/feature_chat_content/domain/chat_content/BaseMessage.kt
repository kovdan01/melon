package org.melon.feature_chat_content.domain.chat_content

import java.util.*

data class BaseMessage(
    val messageId: Int,
    val chatId: Int,
    val messageText: String,
    val messageDate: Date,
    val isUserMessage: Boolean,
    val isRead: Boolean = false
)