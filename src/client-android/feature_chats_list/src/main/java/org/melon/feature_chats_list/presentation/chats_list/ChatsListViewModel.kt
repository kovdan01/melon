package org.melon.feature_chats_list.presentation.chats_list

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import org.melon.feature_chat_content.presentation.chat_content.MessageUi
import org.melon.feature_chats_list.domain.ChatsListUseCase
import timber.log.Timber
import javax.inject.Inject
import kotlin.random.Random

@HiltViewModel
class ChatsListViewModel @Inject constructor(
    private val chatsListUseCase: ChatsListUseCase
) : ViewModel() {

    private val chatsList: MutableList<ChatUi> = mutableListOf()

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

    fun onNewChatClick() {
        _openCreateChatFragment.value = true
    }

    fun onNewChatCreated(chatName: String) {
        chatsList.add(0, ChatUi(chatsList.getAvailableChatId(), chatName, null, null, true))
        _chatsLiveData.value = chatsList
    }

    fun onChatRenamed(chatUi: ChatUi) {
        val chatIndex = chatsList.indexOfFirst { it.chatId == chatUi.chatId }
        chatsList[chatIndex] =
            chatsList[chatIndex].copy(chatName = chatUi.chatName)
        _chatsLiveData.value = chatsList
    }

    fun onChatDelete(chatUi: ChatUi) {
        chatsList.remove(chatUi)
        _chatsLiveData.value = chatsList
    }

    fun onChatMarkAsUnread(chatUi: ChatUi) {
        val chatIndex = chatsList.indexOfFirst { it.chatId == chatUi.chatId }
        chatsList[chatIndex] =
            chatsList[chatIndex].copy(isRead = false)
        _chatsLiveData.value = chatsList
    }

    fun createStubChat() {
        onNewChatCreated(Random.nextInt(1000000).toString())
    }

    fun onChatClick(chatUi: ChatUi) {
        val chatIndex = chatsList.indexOfFirst { it.chatId == chatUi.chatId }
        chatsList[chatIndex] =
            chatsList[chatIndex].copy(isRead = true)

        _chatsLiveData.value = chatsList
        _openChatFragment.value = chatUi
        Timber.tag("LOL").i("onChatClick $chatsList")
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

    fun onUpdateChatInfo(chatId: Int, messageUi: MessageUi) {
        chatsList[chatsList.indexOfFirst { it.chatId == chatId }] =
            chatsList[chatsList.indexOfFirst { it.chatId == chatId }].copy(
                chatPreview = messageUi.messageText,
                lastMessageDate = messageUi.messageDate
            )

        chatsList.sortBy { it.lastMessageDate?.time }
        chatsList.reverse()

        _chatsLiveData.value = chatsList
    }
}
