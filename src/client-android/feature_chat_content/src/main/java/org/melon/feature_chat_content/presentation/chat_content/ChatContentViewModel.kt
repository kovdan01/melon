package org.melon.feature_chat_content.presentation.chat_content

import androidx.lifecycle.*
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.launch
import org.melon.feature_chat_content.domain.chat_content.ChatContentUseCase
import org.melon.feature_chat_content.presentation.chat_content.model.FileUi
import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi
import javax.inject.Inject

@HiltViewModel
class ChatContentViewModel @Inject constructor(
    private val state: SavedStateHandle,
    private val chatContentUseCase: ChatContentUseCase,
    private val chatContentUiTransformer: ChatContentUiTransformer
) : ViewModel() {

    companion object {
        private const val KEY_SAVED_ATTACHMENTS = "key_saved_attachments"
    }

    private var chatId: Int? = null

    private var messagesList: MutableList<MessageUi> = mutableListOf()
    private var messageToEdit: MessageUi? = null
    private lateinit var attachedFiles: MutableList<FileUi>

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

    private val _liveAttachedFiles: MutableLiveData<List<FileUi>> = MutableLiveData()
    val liveAttachedFiles: LiveData<List<FileUi>>
        get() = _liveAttachedFiles

    fun onViewCreated(chatId: Int) {
        this.chatId = chatId
        attachedFiles =
            state.get<List<FileUi>>(KEY_SAVED_ATTACHMENTS)?.toMutableList() ?: mutableListOf()

        viewModelScope.launch {
            chatContentUseCase.getMessage(chatId).collect {
                messagesList = it.map(chatContentUiTransformer::transform).toMutableList()
                _liveMessagesList.value = messagesList
            }

            _liveChatDraft.value = chatContentUseCase.getChatDraft(chatId)
        }
    }

    fun onSendClick(messageText: String?) {
        if (messageText.isNullOrBlank().not() || attachedFiles.isNotEmpty()) {
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
                    chatContentUseCase.sendMessage(
                        messageText ?: "",
                        chatId!!,
                        attachedFiles.map(chatContentUiTransformer::transform)
                    )
                    attachedFiles.clear()
                    _liveAttachedFiles.value = attachedFiles
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

    fun onFileSelected(file: FileUi) {
        attachedFiles.add(file)
        state.set(KEY_SAVED_ATTACHMENTS, attachedFiles)
        _liveAttachedFiles.value = attachedFiles
    }

    fun onFileClose(file: FileUi) {
        attachedFiles.remove(file)
        state.set(KEY_SAVED_ATTACHMENTS, attachedFiles)
        _liveAttachedFiles.value = attachedFiles
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
        messageToEdit = messagesList.first { it.isSelected } as MessageUi
        messagesList.deselectAll()

        _liveMessagesList.value = messagesList
        _liveMessageToEdit.value = messageToEdit
    }
}