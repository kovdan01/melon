package org.melon.melonmessenger.presentation

import android.os.Bundle
import android.os.PersistableBundle
import android.util.Log
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.android.synthetic.main.activity_main.*
import org.melon.jniwrapper.TestWrapper
import org.melon.melonmessenger.R
import org.melon.melonmessenger.presentation.base.BaseActivity
import timber.log.Timber

@AndroidEntryPoint
class MainActivity : BaseActivity(R.layout.activity_main) {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        testTv.text = TestWrapper().another().toString()
    }

    override fun onResume() {
        super.onResume()
        Timber.tag("LOL").i("RESULTS IS: ${TestWrapper().another()}")
    }
}
