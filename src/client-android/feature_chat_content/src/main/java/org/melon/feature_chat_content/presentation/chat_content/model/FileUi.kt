package org.melon.feature_chat_content.presentation.chat_content.model

import android.net.Uri
import android.os.Parcelable
import kotlinx.android.parcel.Parcelize

@Parcelize
data class FileUi(
    val fileName: String,
    val uri: Uri
) : Parcelable