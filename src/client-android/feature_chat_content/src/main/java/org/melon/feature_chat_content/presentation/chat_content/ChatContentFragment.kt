package org.melon.feature_chat_content.presentation.chat_content

import android.database.Cursor
import android.os.Bundle
import android.provider.OpenableColumns
import android.view.Menu
import android.view.MenuItem
import android.view.View
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.view.ActionMode
import androidx.core.os.bundleOf
import androidx.core.view.isVisible
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
import org.melon.feature_chat_content.presentation.chat_content.items.AttachedFileItem
import org.melon.feature_chat_content.presentation.chat_content.items.OtherMessageItem
import org.melon.feature_chat_content.presentation.chat_content.items.UserMessageItem
import org.melon.feature_chat_content.presentation.chat_content.model.FileUi

@AndroidEntryPoint
class ChatContentFragment : BaseFragment(R.layout.fragment_chat_content) {

    companion object {
        const val REQUEST_KEY_CHAT_UPDATE = "chat_update_key"
        const val BUNDLE_KEY_MESSAGE_UI = "key_message_ui"
    }

    private val viewModel: ChatContentViewModel by viewModels()

    private var actionMode: ActionMode? = null

    private val args: ChatContentFragmentArgs by navArgs()

    private val chatAdapter = GroupAdapter<GroupieViewHolder>()
    private lateinit var chatLayoutManager: LinearLayoutManager

    private val attachedFilesAdapter = GroupAdapter<GroupieViewHolder>()

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        chatLayoutManager = LinearLayoutManager(requireContext())

        initChatRv()
        initAttachedFilesRv()

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
            chatLayoutManager.scrollToPosition(chatAdapter.itemCount - 1)
        }

        val getDocument =
            registerForActivityResult(ActivityResultContracts.OpenMultipleDocuments()) { files ->
                files.forEach { uri ->
                    val cursor: Cursor? = requireContext().contentResolver.query(
                        uri, null, null, null, null, null
                    )

                    cursor?.use {
                        if (it.moveToFirst()) {

                            // Note it's called "Display Name". This is
                            // provider-specific, and might not necessarily be the file name.
                            val fileName: String =
                                it.getString(it.getColumnIndex(OpenableColumns.DISPLAY_NAME))

                            viewModel.onFileSelected(
                                FileUi(
                                    fileName = fileName,
                                    uri = uri
                                )
                            )
                        }
                    }
                }
            }

        attachBtn.setOnClickListener {
            getDocument.launch(arrayOf("*/*"))
        }

        messageEt.addTextChangedListener {
            viewModel.onDraftMessageChanged(it?.toString())
        }

        viewModel.liveMessagesList.observe(
            viewLifecycleOwner,
            {
                chatAdapter.update(
                    it.map { message ->
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

        viewModel.liveAttachedFiles.observe(
            viewLifecycleOwner,
            {
                attachedFilesRv.isVisible = it.isNotEmpty()
                attachedFilesAdapter.update(
                    it.map { file ->
                        AttachedFileItem(file, viewModel::onFileClose)
                    }
                )
            }
        )

        viewModel.onViewCreated(args.chatId)
    }

    private fun initChatRv() {
        chatContentRv.adapter = chatAdapter
        chatContentRv.layoutManager = chatLayoutManager
    }

    private fun initAttachedFilesRv() {
        attachedFilesRv.layoutManager =
            LinearLayoutManager(requireContext(), LinearLayoutManager.HORIZONTAL, false)
        attachedFilesRv.adapter = attachedFilesAdapter
    }
}
