package org.melon.feature_chats_list.presentation.chats_list

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.launch
import org.melon.feature_chat_content.presentation.chat_content.ChatContentUiTransformer
import org.melon.feature_chat_content.presentation.chat_content.MessageUi
import org.melon.feature_chats_list.domain.ChatsListUseCase
import javax.inject.Inject
import kotlin.random.Random

@HiltViewModel
class ChatsListViewModel @Inject constructor(
    private val chatsListUseCase: ChatsListUseCase,
    private val chatsListUiTransformer: ChatsListUiTransformer,
    private val chatContentUiTransformer: ChatContentUiTransformer
) : ViewModel() {

    private val _chatsLiveData: MutableLiveData<List<ChatUi>> = MutableLiveData()
    val chatsLiveData: LiveData<List<ChatUi>>
        get() = _chatsLiveData

    private val _openChatFragment: MutableLiveData<ChatUi?> = MutableLiveData()
    val openChatFragment: LiveData<ChatUi?>
        get() = _openChatFragment

    private val _openChatEditDialog: MutableLiveData<ChatUi?> = MutableLiveData()
    val openChatEditDialog: LiveData<ChatUi?>
        get() = _openChatEditDialog

    private val _openCreateChatFragment: MutableLiveData<Boolean> = MutableLiveData()
    val openCreateChatFragment: LiveData<Boolean>
        get() = _openCreateChatFragment

    init {
        viewModelScope.launch {
            chatsListUseCase.getChatsList().collect {
                _chatsLiveData.value = it.map(chatsListUiTransformer::transform)
            }
        }
    }

    fun onViewCreated() {

    }

    fun onNewChatClick() {
        _openCreateChatFragment.value = true
    }

    fun onNewChatCreated(chatName: String) {
        viewModelScope.launch {
            chatsListUseCase.createChat(chatName)
        }
    }

    fun onChatRenamed(chatUi: ChatUi) {
        viewModelScope.launch {
            chatsListUseCase.renameChat(chatsListUiTransformer.transform(chatUi))
        }
    }

    fun onChatDelete(chatUi: ChatUi) {
        viewModelScope.launch {
            chatsListUseCase.deleteChat(chatsListUiTransformer.transform(chatUi))
        }
    }

    fun onChatMarkAsUnread(chatUi: ChatUi) {
        viewModelScope.launch {
            chatsListUseCase.markChatUnread(chatsListUiTransformer.transform(chatUi))
        }
    }

    fun onUpdateChatInfo(messageUi: MessageUi) {
        viewModelScope.launch {
            chatsListUseCase.changeChatPreview(chatContentUiTransformer.transform(messageUi))
        }
    }

    fun onChatClick(chatUi: ChatUi) {
        viewModelScope.launch {
            chatsListUseCase.markChatRead(chatsListUiTransformer.transform(chatUi))
        }
        _openChatFragment.value = chatUi
    }

    fun createStubChat() {
        onNewChatCreated(Random.nextInt(1000000).toString())
    }

    fun onChatLongClick(chatUi: ChatUi) {
        _openChatEditDialog.value = chatUi
    }

    fun onChatEditOpened() {
        _openChatEditDialog.value = null
    }

    fun onNavigateToChatContent() {
        _openChatFragment.value = null
    }

    fun onNavigateToChatCreate() {
        _openCreateChatFragment.value = false
    }
}
