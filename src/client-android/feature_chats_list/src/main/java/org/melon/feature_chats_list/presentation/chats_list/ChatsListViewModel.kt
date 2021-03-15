package org.melon.feature_chats_list.presentation.chats_list

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import org.melon.feature_chats_list.domain.ChatsListUseCase
import javax.inject.Inject
import kotlin.random.Random

@HiltViewModel
class ChatsListViewModel @Inject constructor(
    private val chatsListUseCase: ChatsListUseCase
) : ViewModel() {

    private val _chatsLiveData: MutableLiveData<List<ChatUi>> = MutableLiveData()
    val chatsLiveData: LiveData<List<ChatUi>>
        get() = _chatsLiveData

    private val _openChatFragment: MutableLiveData<Boolean> = MutableLiveData()
    val openChatFragment: LiveData<Boolean>
        get() = _openChatFragment

    private val _openChatEditDialog: MutableLiveData<ChatUi?> = MutableLiveData()
    val openChatEditDialog: LiveData<ChatUi?>
        get() = _openChatEditDialog

    private val _openCreateChatFragment: MutableLiveData<Boolean> = MutableLiveData()
    val openCreateChatFragment: LiveData<Boolean>
        get() = _openCreateChatFragment

    fun getChatsList() = chatsListUseCase.getChatsList()

    fun onNewChatClick() {
        _openCreateChatFragment.value = true
    }

    fun onNewChatCreated(chatName: String) {
        //TODO: may be optimize smh this algo
        val availableChat = _chatsLiveData.value ?: emptyList()
        var newChatList = listOf(ChatUi(availableChat.getAvailableChatId(), chatName, "stg"))
            .plus(
                availableChat
            )
        _chatsLiveData.value = newChatList
    }

    fun onChatRenamed(chatUi: ChatUi) {
        val newChatsList = _chatsLiveData.value!!.toMutableList()
        val chatIndex = newChatsList.indexOfFirst { it.chatId == chatUi.chatId }
        newChatsList[chatIndex] =
            newChatsList[chatIndex].copy(chatName = chatUi.chatName)
        _chatsLiveData.value = newChatsList
    }

    fun onChatDelete(chatUi: ChatUi) {
        val newChatsList = _chatsLiveData.value!!.toMutableList()
        newChatsList.remove(chatUi)
        _chatsLiveData.value = newChatsList
    }

    fun createStubChat() {
        onNewChatCreated(Random.nextInt(1000000).toString())
    }

    fun onChatClick(chatUi: ChatUi) {
        _openChatFragment.value = true
    }

    fun onChatLongClick(chatUi: ChatUi) {
        _openChatEditDialog.value = chatUi
    }

    fun onChatEditOpened() {
        _openChatEditDialog.value = null
    }

    fun onNavigateToChatContent() {
        _openChatFragment.value = false
    }

    fun onNavigateToChatCreate() {
        _openCreateChatFragment.value = false
    }
}
