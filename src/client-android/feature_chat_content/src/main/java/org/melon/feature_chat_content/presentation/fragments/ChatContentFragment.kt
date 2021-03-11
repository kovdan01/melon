package org.melon.feature_chat_content.presentation.fragments

import android.os.Bundle
import android.view.View
import androidx.core.widget.addTextChangedListener
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupAdapter
import com.xwray.groupie.GroupieViewHolder
import kotlinx.android.synthetic.main.fragment_chat_content.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chat_content.R
import org.melon.feature_chat_content.di.DaggerChatContentComponent
import org.melon.feature_chat_content.presentation.items.OtherMessageItem
import org.melon.feature_chat_content.presentation.items.UserMessageItem
import org.melon.feature_chat_content.presentation.viewmodels.ChatContentViewModel
import javax.inject.Inject


class ChatContentFragment : BaseFragment(R.layout.fragment_chat_content) {

    @Inject
    lateinit var viewModel: ChatContentViewModel

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

        adapter.add(UserMessageItem("This is "))
        adapter.add(OtherMessageItem("your test"))

        addChatBtn.setOnClickListener {
            viewModel.onSendClick()
            layoutManager.scrollToPosition(adapter.itemCount - 1)
        }

        messageEt.addTextChangedListener {
            viewModel.onMessageChanged(it?.toString())
        }

        viewModel.sendOtherMessage.observe(viewLifecycleOwner, {
            adapter.add(OtherMessageItem(it))
        })

        viewModel.sendUserMessage.observe(viewLifecycleOwner, {
            adapter.add(UserMessageItem(it))
            messageEt.setText("")
            messageEt.clearFocus()
        })
    }
}
