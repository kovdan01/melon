package org.melon.feature_chats_list.presentation.viewmodels

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import javax.inject.Inject

class ChatCreationViewModel @Inject constructor() : ViewModel() {

    private val _showCreateBtn: MutableLiveData<Boolean> = MutableLiveData()
    val showCreateBtn: LiveData<Boolean>
        get() = _showCreateBtn

    private val _createChat: MutableLiveData<String> = MutableLiveData()
    val createChat: LiveData<String>
        get() = _createChat

    private var chatName: String? = null

    fun onChatNameEntered(chatName: String?) {
        this.chatName = chatName
        _showCreateBtn.value = chatName.isNullOrEmpty().not()
    }

    fun onChatCreateClick() {
        _createChat.value = chatName
    }
}