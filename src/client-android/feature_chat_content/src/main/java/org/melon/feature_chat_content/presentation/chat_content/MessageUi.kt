package org.melon.feature_chat_content.presentation.chat_content

import java.util.*

data class MessageUi(
        val messageId: Int,
        val messageText: String,
        val messageTime: Date,
        val isUserMessage: Boolean,
        val isSelected: Boolean = false
)