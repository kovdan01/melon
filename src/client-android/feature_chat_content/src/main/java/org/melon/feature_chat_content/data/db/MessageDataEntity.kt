package org.melon.feature_chat_content.data.db

import android.provider.ContactsContract
import androidx.room.*
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken

@Entity(tableName = "ChatMessagesTable")
@TypeConverters(DataConverter::class)
data class MessageDataEntity(
    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = "message_id")
    val messageId: Int = 0,
    @ColumnInfo(name = "chat_id")
    val chatId: Int,
    @ColumnInfo(name = "message_text")
    val messageText: String,
    @ColumnInfo(name = "message_date")
    val messageDate: Long,
    @ColumnInfo(name = "is_user_message")
    val isUserMessage: Boolean,
    @ColumnInfo(name = "is_read")
    val isRead: Boolean,
    @ColumnInfo(name = "files")
    val files: List<MessageFileDataEntity>
)

class DataConverter {
    @TypeConverter
    fun toDb(filesList: List<MessageFileDataEntity>): String? {
        return Gson().toJson(filesList)
    }

    @TypeConverter
    fun fromDb(countryLangString: String?): List<MessageFileDataEntity> {
        return Gson().fromJson(
            countryLangString,
            object : TypeToken<List<MessageFileDataEntity>>() {}.type
        )
    }
}