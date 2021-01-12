package org.melon.feature_chats_list.presentation.fragments

import android.os.Bundle
import android.view.View
import androidx.core.os.bundleOf
import androidx.core.widget.addTextChangedListener
import androidx.fragment.app.setFragmentResult
import androidx.navigation.fragment.findNavController
import kotlinx.android.synthetic.main.fragment_chat_creation.*
import org.melon.feature_chats_list.R
import org.melon.feature_chats_list.di.DaggerChatsListComponent
import org.melon.feature_chats_list.presentation.viewmodels.ChatCreationViewModel
import org.melon.melonmessenger.presentation.base.BaseFragment
import javax.inject.Inject

class ChatCreationFragment : BaseFragment(R.layout.fragment_chat_creation) {

    companion object {
        const val REQUEST_KEY_CHAT_CREATION = "chat_creation_key"
        const val BUNDLE_KEY_CHAT_NAME = "chat_name_key"
    }

    @Inject
    lateinit var viewModel: ChatCreationViewModel

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        DaggerChatsListComponent.builder().context(requireContext()).build().inject(this)
    }

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