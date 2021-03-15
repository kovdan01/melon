package org.melon.feature_chats_list.presentation.chat_creation

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import org.melon.feature_chats_list.presentation.chats_list.ChatUi
import javax.inject.Inject

@HiltViewModel
class ChatCreationViewModel @Inject constructor() : ViewModel() {

    private val _showCreateBtn: MutableLiveData<Boolean> = MutableLiveData()
    val showCreateBtn: LiveData<Boolean>
        get() = _showCreateBtn

    private val _createChat: MutableLiveData<String> = MutableLiveData()
    val createChat: LiveData<String>
        get() = _createChat

    private val _renameChat: MutableLiveData<ChatUi> = MutableLiveData()
    val renameChat: LiveData<ChatUi>
        get() = _renameChat

    private var chatName: String? = null
    private var chatUi: ChatUi? = null
    private var isChatRenaming: Boolean = false

    //TODO: maybe chatName is redundant, because we can create here new ChatUi() with 0 in chatIndex
    fun onViewCreated(chatUi: ChatUi?) {
        isChatRenaming = chatUi != null
        this.chatUi = chatUi
        this.chatName = chatUi?.chatName
    }

    fun onChatNameEntered(chatName: String?) {
        this.chatName = chatName
        _showCreateBtn.value = chatName.isNullOrEmpty().not()
    }

    fun onChatCreateClick() {
        if (isChatRenaming && chatName != null) {
            _renameChat.value = chatUi?.copy(chatName = chatName!!)
        } else {
            _createChat.value = chatName
        }
    }
}