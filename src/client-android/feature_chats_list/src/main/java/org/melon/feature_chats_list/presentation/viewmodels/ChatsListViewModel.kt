package org.melon.feature_chats_list.presentation.viewmodels

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import org.melon.feature_chats_list.domain.ChatsListUseCase
import org.melon.feature_chats_list.presentation.models.ChatUi
import javax.inject.Inject

class ChatsListViewModel @Inject constructor(
    private val chatsListUseCase: ChatsListUseCase
) : ViewModel() {

    private val _chatsLiveData: MutableLiveData<List<ChatUi>> = MutableLiveData()
    val chatsLiveData: LiveData<List<ChatUi>>
        get() = _chatsLiveData

    private val _openChatFragment: MutableLiveData<Boolean> = MutableLiveData()
    val openChatFragment: LiveData<Boolean>
        get() = _openChatFragment

    fun getChatsList() = chatsListUseCase.getChatsList()

    fun onNewChatClick() {
        _chatsLiveData.value =
            _chatsLiveData.value?.plus(listOf(ChatUi("Test chat", "Stick finger in ...")))
                ?: listOf(ChatUi("Test chat", "Stick finger in ..."))
    }

    fun onChatClick(chatUi: ChatUi) {
        _openChatFragment.value = true
    }

    fun onNavigateToChatContent(){
        _openChatFragment.value = false
    }
}
