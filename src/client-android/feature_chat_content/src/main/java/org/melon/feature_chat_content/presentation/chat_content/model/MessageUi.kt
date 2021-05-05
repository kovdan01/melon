package org.melon.feature_chat_content.presentation.chat_content.model

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize
import java.util.*

@Parcelize
data class MessageUi(
    override val messageId: Int,
    override val chatId: Int,
    override val messageText: String,
    override val messageDate: Date,
    override val isUserMessage: Boolean,
    override val isRead: Boolean = false,
    override val isSelected: Boolean = false
) : Parcelable, BaseMessageUi