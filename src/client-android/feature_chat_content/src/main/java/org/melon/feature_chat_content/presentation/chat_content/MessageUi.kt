package org.melon.feature_chat_content.presentation.chat_content

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize
import java.util.*

@Parcelize
data class MessageUi(
        val messageId: Int,
        val chatId: Int,
        val messageText: String,
        val messageDate: Date,
        val isUserMessage: Boolean,
        val isRead: Boolean = false,
        val isSelected: Boolean = false
) : Parcelable