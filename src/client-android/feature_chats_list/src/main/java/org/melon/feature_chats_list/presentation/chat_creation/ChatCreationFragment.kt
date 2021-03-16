package org.melon.feature_chats_list.presentation.chat_creation

import android.os.Bundle
import android.view.View
import androidx.core.os.bundleOf
import androidx.core.widget.addTextChangedListener
import androidx.fragment.app.setFragmentResult
import androidx.fragment.app.viewModels
import androidx.navigation.fragment.findNavController
import androidx.navigation.fragment.navArgs
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.android.synthetic.main.fragment_chat_creation.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chats_list.R

@AndroidEntryPoint
class ChatCreationFragment : BaseFragment(R.layout.fragment_chat_creation) {

    companion object {
        const val REQUEST_KEY_CHAT_CREATION = "chat_creation_key"
        const val REQUEST_KEY_CHAT_RENAME = "chat_rename_key"
        const val BUNDLE_KEY_CHAT_NAME = "chat_name_key"
        const val BUNDLE_KEY_CHAT_UI = "chat_ui_key"
    }

    private val viewModel by viewModels<ChatCreationViewModel>()

    private val args: ChatCreationFragmentArgs by navArgs()

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        args.chatUi?.chatName?.let {
            chatNameEt.setText(it)
            chatCreateBtn.setText(R.string.rename_chat)
        }

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

        viewModel.renameChat.observe(viewLifecycleOwner, { chatUi ->
            setFragmentResult(REQUEST_KEY_CHAT_RENAME, bundleOf(BUNDLE_KEY_CHAT_UI to chatUi))
            findNavController().popBackStack()
        })

        viewModel.onViewCreated(args.chatUi)
    }
}