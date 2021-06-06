package org.melon.feature_chat_content.domain.model

import android.net.Uri

data class File(
    val fileName: String,
    val uri: Uri
)