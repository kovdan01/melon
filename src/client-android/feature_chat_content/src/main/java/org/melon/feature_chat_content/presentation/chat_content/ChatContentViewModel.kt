package org.melon.feature_chat_content.presentation.chat_content

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import java.util.*
import javax.inject.Inject
import kotlin.random.Random

class ChatContentViewModel @Inject constructor() : ViewModel() {
    private var messageToBeSend: String? = null

    private val messagesList: MutableList<MessageUi> = mutableListOf()
    private val selectedMessagesList: MutableList<MessageUi> = mutableListOf()
    private var messageToEdit: MessageUi? = null

    private val _liveMessagesList: MutableLiveData<List<MessageUi>> = MutableLiveData()
    val liveMessagesList: LiveData<List<MessageUi>>
        get() = _liveMessagesList

    private val _liveActionMode: MutableLiveData<List<MessageUi?>> = MutableLiveData()
    val liveActionMode: LiveData<List<MessageUi?>>
        get() = _liveActionMode

    private val _liveMessageToEdit: MutableLiveData<MessageUi?> = MutableLiveData()
    val liveMessageToEdit: LiveData<MessageUi?>
        get() = _liveMessageToEdit

    fun onViewCreated() {

    }

    fun onMessageChanged(messageText: String?) {
        this.messageToBeSend = messageText
    }

    fun onMessageClick(message: MessageUi) {
        if (selectedMessagesList.isEmpty().not()) {
            selectedMessagesList.addOrRemoveMessageSelection(message)
            messagesList.toggleMessageSelection(message)

            _liveActionMode.value = selectedMessagesList
            _liveMessagesList.value = messagesList
        }
    }

    fun onMessageLongClick(message: MessageUi) {
        selectedMessagesList.add(message)
        messagesList.toggleMessageSelection(message)

        _liveActionMode.value = selectedMessagesList
        _liveMessagesList.value = messagesList
    }

    fun onActionModeDestroy() {
        if (selectedMessagesList.isEmpty().not()) {
            selectedMessagesList.clear()
            messagesList.deselectAll()

            _liveActionMode.value = selectedMessagesList
            _liveMessagesList.value = messagesList
        }
    }

    fun onActionDeleteClick() {
        selectedMessagesList.forEach { selectedMessage ->
            messagesList.removeAll { it.messageId == selectedMessage.messageId }
        }
        selectedMessagesList.clear()

        _liveActionMode.value = selectedMessagesList
        _liveMessagesList.value = messagesList
    }

    fun onActionEditClick() {
        messageToEdit = selectedMessagesList.first()

        selectedMessagesList.clear()
        messagesList.deselectAll()

        _liveActionMode.value = selectedMessagesList
        _liveMessagesList.value = messagesList
        _liveMessageToEdit.value = messageToEdit
    }

    fun onSendClick() {
        if (messageToBeSend.isNullOrEmpty().not()) {
            if (messageToEdit == null) {
                messagesList.add(
                    MessageUi(
                        messageId = messagesList.getAvailableMessageId(),
                        messageText = messageToBeSend!!,
                        messageDate = Date(),
                        isUserMessage = true,
                        isRead = true
                    )
                )
            } else {
                messagesList[messagesList.indexOf(messageToEdit)] = messageToEdit!!.copy(messageText = messageToBeSend!!)
                messageToEdit = null
                _liveMessageToEdit.value = null
            }
        } else {
            messagesList.add(
                MessageUi(
                    messageId = messagesList.getAvailableMessageId(),
                    messageText = "Stub",
                    messageDate = Date(),
                    isUserMessage = Random.nextBoolean(),
                    isRead = true
                )
            )
        }

        _liveMessagesList.value = messagesList
    }
}