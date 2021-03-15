package org.melon.feature_chat_content.presentation.chat_content

import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.view.ActionMode
import androidx.core.widget.addTextChangedListener
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupAdapter
import com.xwray.groupie.GroupieViewHolder
import kotlinx.android.synthetic.main.fragment_chat_content.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.di.DaggerChatContentComponent
import javax.inject.Inject


class ChatContentFragment : BaseFragment(R.layout.fragment_chat_content) {

    @Inject
    lateinit var viewModel: ChatContentViewModel

    private var actionMode: ActionMode? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        DaggerChatContentComponent.builder().context(requireContext()).build().inject(this)
        super.onCreate(savedInstanceState)
    }

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
            viewModel.onSendClick()
            layoutManager.scrollToPosition(adapter.itemCount - 1)
        }

        messageEt.addTextChangedListener {
            viewModel.onMessageChanged(it?.toString())
        }

        viewModel.liveMessagesList.observe(
                viewLifecycleOwner,
                {
                    adapter.update(
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

                    messageEt.clearFocus()
                    messageEt.setText("")
                }
        )

        viewModel.liveActionMode.observe(
                viewLifecycleOwner,
                {
                    if (actionMode == null) {
                        actionMode = activity.startSupportActionMode(actionModeCallback)
                    }

                    actionMode?.title = getString(R.string.action_mode_selected_placeholder, it.size)

                    if (it.isEmpty()) {
                        actionMode?.finish()
                        actionMode = null
                    }

                    actionMode?.menu?.findItem(R.id.edit)?.isVisible = it.size == 1
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
    }
}
