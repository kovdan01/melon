package org.melon.feature_chat_content.presentation.chat_content

import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.view.View
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.view.ActionMode
import androidx.core.os.bundleOf
import androidx.core.widget.addTextChangedListener
import androidx.fragment.app.setFragmentResult
import androidx.fragment.app.viewModels
import androidx.navigation.fragment.navArgs
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupAdapter
import com.xwray.groupie.GroupieViewHolder
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.android.synthetic.main.fragment_chat_content.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.presentation.chat_content.items.FileMessageItem
import org.melon.feature_chat_content.presentation.chat_content.items.OtherMessageItem
import org.melon.feature_chat_content.presentation.chat_content.items.UserMessageItem
import org.melon.feature_chat_content.presentation.chat_content.model.FileMessageUi
import org.melon.feature_chat_content.presentation.chat_content.model.MessageUi
import timber.log.Timber

@AndroidEntryPoint
class ChatContentFragment : BaseFragment(R.layout.fragment_chat_content) {

    companion object {
        const val REQUEST_KEY_CHAT_UPDATE = "chat_update_key"
        const val BUNDLE_KEY_MESSAGE_UI = "key_message_ui"
    }

    private val viewModel: ChatContentViewModel by viewModels()

    private var actionMode: ActionMode? = null

    private val args: ChatContentFragmentArgs by navArgs()

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val adapter = GroupAdapter<GroupieViewHolder>()
        val layoutManager = LinearLayoutManager(requireContext()).apply {
        }
        chatContentRv.adapter = adapter
        chatContentRv.layoutManager = layoutManager

        //Action mode
        val activity = requireActivity() as AppCompatActivity
        val actionModeCallback = object : ActionMode.Callback {

            override fun onCreateActionMode(mode: ActionMode?, menu: Menu?): Boolean {
                activity.menuInflater.inflate(R.menu.menu_chat_content_actions, menu)
                return true
            }

            override fun onPrepareActionMode(mode: ActionMode?, menu: Menu?): Boolean {
                return false
            }

            override fun onActionItemClicked(mode: ActionMode?, item: MenuItem?): Boolean {
                return when (item?.itemId) {
                    R.id.delete -> {
                        viewModel.onActionDeleteClick()
                        true
                    }
                    R.id.edit -> {
                        viewModel.onActionEditClick()
                        true
                    }
                    else -> false
                }
            }

            override fun onDestroyActionMode(mode: ActionMode?) {
                viewModel.onActionModeDestroy()
            }
        }

        sendMsgBtn.setOnClickListener {
            viewModel.onSendClick(messageEt.text.toString())
            layoutManager.scrollToPosition(adapter.itemCount - 1)
        }

        val getDocument = registerForActivityResult(ActivityResultContracts.GetContent()) {
            viewModel.onFileSelected()
        }
        attachBtn.setOnClickListener {
            getDocument.launch("*/*")
        }

        messageEt.addTextChangedListener {
            viewModel.onDraftMessageChanged(it?.toString())
        }

        viewModel.liveMessagesList.observe(
            viewLifecycleOwner,
            {
                adapter.update(
                    it.map { message ->
                        if (message is MessageUi) {
                            if (message.isUserMessage) {
                                UserMessageItem(
                                    message,
                                    onLongClick = { selectedMessage ->
                                        viewModel.onMessageLongClick(selectedMessage)
                                    },
                                    onClick = { clickedMessage ->
                                        viewModel.onMessageClick(clickedMessage)
                                    }
                                )
                            } else OtherMessageItem(message)
                        } else {
                            FileMessageItem((message as FileMessageUi).messageText)
                        }
                    }
                )

                if (it.any { chatsUi -> chatsUi.isSelected }) {
                    if (actionMode == null) {
                        actionMode = activity.startSupportActionMode(actionModeCallback)
                    }

                    actionMode?.title =
                        getString(
                            R.string.action_mode_selected_placeholder,
                            it.count { chatUi -> chatUi.isSelected }
                        )

                    actionMode?.menu?.findItem(R.id.edit)?.isVisible =
                        it.count { chatUi -> chatUi.isSelected } == 1
                } else {
                    actionMode?.finish()
                    actionMode = null
                }

                messageEt.clearFocus()
                messageEt.setText("")


                if (it.isNotEmpty()) {
                    setFragmentResult(
                        REQUEST_KEY_CHAT_UPDATE, bundleOf(
                            BUNDLE_KEY_MESSAGE_UI to it.last()
                        )
                    )
                }
            }
        )


        viewModel.liveMessageToEdit.observe(
            viewLifecycleOwner,
            {
                it?.let {
                    messageEt.setText(it.messageText)
                    messageEt.requestFocus()
                }
            }
        )

        viewModel.liveChatDraft.observe(
            viewLifecycleOwner,
            { chatDraft ->
                chatDraft?.let {
                    messageEt.setText(it)
                }
            }
        )

        viewModel.onViewCreated(args.chatId)
    }
}
