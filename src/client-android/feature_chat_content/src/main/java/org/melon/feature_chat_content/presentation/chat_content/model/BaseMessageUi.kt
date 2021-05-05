package org.melon.feature_chat_content.presentation.chat_content.model

import android.os.Parcelable
import java.util.*

interface BaseMessageUi : Parcelable {
    val messageId: Int
    val chatId: Int
    val messageText: String
    val messageDate: Date
    val isUserMessage: Boolean
    val isRead: Boolean
    val isSelected: Boolean
}