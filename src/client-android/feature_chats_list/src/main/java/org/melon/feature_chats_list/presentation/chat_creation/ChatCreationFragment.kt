package org.melon.feature_chats_list.presentation.chat_creation

import android.os.Bundle
import android.view.View
import androidx.core.os.bundleOf
import androidx.core.widget.addTextChangedListener
import androidx.fragment.app.setFragmentResult
import androidx.fragment.app.viewModels
import androidx.navigation.fragment.findNavController
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.android.synthetic.main.fragment_chat_creation.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chats_list.R

@AndroidEntryPoint
class ChatCreationFragment : BaseFragment(R.layout.fragment_chat_creation) {

    companion object {
        const val REQUEST_KEY_CHAT_CREATION = "chat_creation_key"
        const val BUNDLE_KEY_CHAT_NAME = "chat_name_key"
    }

    private val viewModel by viewModels<ChatCreationViewModel>()

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        chatNameEt.addTextChangedListener {
            viewModel.onChatNameEntered(it?.toString())
        }

        chatCreateBtn.setOnClickListener {
            viewModel.onChatCreateClick()
        }

        viewModel.showCreateBtn.observe(viewLifecycleOwner, {
            chatCreateBtn.isEnabled = it
        })

        viewModel.createChat.observe(viewLifecycleOwner, { chatName ->
            setFragmentResult(REQUEST_KEY_CHAT_CREATION, bundleOf(BUNDLE_KEY_CHAT_NAME to chatName))
            findNavController().popBackStack()
        })
    }
}