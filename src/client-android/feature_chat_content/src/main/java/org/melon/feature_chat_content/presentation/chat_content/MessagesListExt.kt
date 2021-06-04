package org.melon.feature_chat_content.presentation.chat_content

import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi

fun List<MessageUi>.getAvailableMessageId(): Int {
    val max = this.maxByOrNull { it.messageId }?.messageId ?: return 1
    val min: Int = this.minByOrNull { it.messageId }?.messageId ?: return 1
    return if (min > 2) {
        1
    } else {
        for (possibleId in (min + 1)..max) {
            if (find { it.messageId == possibleId } == null) return possibleId
        }
        return max + 1
    }
}

fun MutableList<MessageUi>.addOrRemoveMessageSelection(messageUi: MessageUi) {
    if (this.firstOrNull { messageUi.messageId == it.messageId } != null) {
        val index = this.indexOfFirst { it.messageId == messageUi.messageId }
        this.removeAt(index)
    } else this.add(messageUi)
}

fun MutableList<MessageUi>.toggleMessageSelection(messageUi: MessageUi) {
    if (this.indexOf(messageUi) != -1) {
        this[this.indexOf(messageUi)] = messageUi.copy(isSelected = messageUi.isSelected.not())
    }
}

fun MutableList<MessageUi>.deselectAll() {
    for (i in 0 until size) {
        this[i] = this[i].copy(isSelected = false)
    }
}