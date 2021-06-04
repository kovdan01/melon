package org.melon.feature_chat_content.presentation.chat_content.model

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize
import kotlinx.android.parcel.RawValue
import java.util.*

@Parcelize
data class MessageUi(
    val messageId: Int,
    val chatId: Int,
    val messageText: String,
    val messageDate: Date,
    val isUserMessage: Boolean,
    val isRead: Boolean = false,
    val isSelected: Boolean = false,
    val files: @RawValue List<FileUi> = emptyList()
) : Parcelable