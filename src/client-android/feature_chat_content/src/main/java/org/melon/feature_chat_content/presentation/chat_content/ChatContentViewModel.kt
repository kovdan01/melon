package org.melon.feature_chat_content.presentation.chat_content

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.launch
import org.melon.feature_chat_content.domain.chat_content.ChatContentUseCase
import javax.inject.Inject

@HiltViewModel
class ChatContentViewModel @Inject constructor(
        private val chatContentUseCase: ChatContentUseCase,
        private val chatContentUiTransformer: ChatContentUiTransformer
) : ViewModel() {

    private var chatId: Int? = null

    private var messagesList: MutableList<MessageUi> = mutableListOf()
    private var messageToEdit: MessageUi? = null

    private val _liveMessagesList: MutableLiveData<List<MessageUi>> = MutableLiveData()
    val liveMessagesList: LiveData<List<MessageUi>>
        get() = _liveMessagesList

    private val _liveActionMode: MutableLiveData<Boolean> = MutableLiveData()
    val liveActionMode: LiveData<Boolean>
        get() = _liveActionMode

    private val _liveMessageToEdit: MutableLiveData<MessageUi?> = MutableLiveData()
    val liveMessageToEdit: LiveData<MessageUi?>
        get() = _liveMessageToEdit

    private val _liveChatDraft: MutableLiveData<String?> = MutableLiveData()
    val liveChatDraft: LiveData<String?>
        get() = _liveChatDraft

    fun onViewCreated(chatId: Int) {
        this.chatId = chatId
        viewModelScope.launch {
            chatContentUseCase.getMessage(chatId).collect {
                messagesList = it.map(chatContentUiTransformer::transform).toMutableList()
                _liveMessagesList.value = messagesList
            }

            _liveChatDraft.value = chatContentUseCase.getChatDraft(chatId)
        }
    }

    fun onSendClick(messageText: String?) {
        if (messageText.isNullOrBlank().not()) {
            viewModelScope.launch {
                if (messageToEdit != null) {

                    chatContentUseCase.editMessage(
                            chatContentUiTransformer.transform(
                                    messageToEdit!!.copy(
                                            messageText = messageText!!
                                    )
                            )
                    )

                    _liveMessageToEdit.value = null
                } else {
                    chatContentUseCase.sendMessage(messageText!!, chatId!!)
                }
            }
        }
    }

    fun onDraftMessageChanged(messageText: String?) {
        viewModelScope.launch {
            chatContentUseCase.saveChatDraft(chatId!!, messageText)
        }
    }

    fun onMessageClick(message: MessageUi) {
        if (messagesList.any { it.isSelected }) {
            messagesList.toggleMessageSelection(message)

            _liveMessagesList.value = messagesList
        }
    }

    fun onMessageLongClick(message: MessageUi) {
        messagesList.toggleMessageSelection(message)

        _liveMessagesList.value = messagesList
    }

    fun onActionModeDestroy() {
        if (messagesList.any { it.isSelected }) {
            messagesList.deselectAll()
            _liveMessagesList.value = messagesList
        }
    }

    fun onActionDeleteClick() {
        viewModelScope.launch {
            chatContentUseCase.deleteMessages(
                    messagesList
                            .filter { it.isSelected }
                            .map(chatContentUiTransformer::transform)
            )
        }
    }

    fun onActionEditClick() {
        messageToEdit = messagesList.first { it.isSelected }
        messagesList.deselectAll()

        _liveMessagesList.value = messagesList
        _liveMessageToEdit.value = messageToEdit
    }
}