package org.melon.feature_chat_content.presentation.viewmodels

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import org.melon.feature_chat_content.presentation.items.OtherMessageItem
import org.melon.feature_chat_content.presentation.items.UserMessageItem
import javax.inject.Inject
import kotlin.random.Random

class ChatContentViewModel @Inject constructor() : ViewModel() {
    private var messageText: String? = null

    private val _sendUserMessage: MutableLiveData<String> = MutableLiveData()
    val sendUserMessage: LiveData<String>
        get() = _sendUserMessage

    private val _sendOtherMessage: MutableLiveData<String> = MutableLiveData()
    val sendOtherMessage: LiveData<String>
        get() = _sendOtherMessage

    fun onMessageChanged(messageText: String?) {
        this.messageText = messageText
    }

    fun onSendClick() {
        if(messageText.isNullOrEmpty().not()) {
            _sendUserMessage.value = messageText
        }else{
            if (Random.nextBoolean()) {
                _sendUserMessage.value = "Hallo mfka???"
            } else {
                _sendOtherMessage.value = "Go fck ur self, pls!)"
            }
        }
    }
}