package org.melon.feature_chats_list.presentation.chats_list

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize
import java.util.*

@Parcelize
data class ChatUi(
    //TODO: remade with chatId
    val chatId: Int,
    val chatName: String,
    val chatPreview: String?,
    val lastMessageDate: Date?,
    val isRead: Boolean
) : Parcelable
