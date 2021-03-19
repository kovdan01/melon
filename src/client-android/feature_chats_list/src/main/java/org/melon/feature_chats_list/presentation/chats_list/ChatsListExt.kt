package org.melon.feature_chats_list.presentation.chats_list

fun List<ChatUi>.getAvailableChatId(): Int {
    val max = this.maxByOrNull { it.chatId }?.chatId ?: return 1
    val min: Int = this.minByOrNull { it.chatId }?.chatId ?: return 1
    return if (min > 2) {
        1
    } else {
        for (possibleId in (min + 1)..max) {
            if (find { it.chatId == possibleId } == null) return possibleId
        }
        return max + 1
    }
}