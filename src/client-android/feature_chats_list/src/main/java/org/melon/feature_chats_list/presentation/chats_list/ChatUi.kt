package org.melon.feature_chats_list.presentation.chats_list

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize

@Parcelize
data class ChatUi(
    //TODO: remade with chatId
    val chatId: Int,
    val chatName: String,
    val chatPreview: String,
) : Parcelable
