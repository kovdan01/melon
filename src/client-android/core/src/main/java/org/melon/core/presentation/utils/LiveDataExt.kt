package org.melon.core.presentation.utils

import androidx.lifecycle.LiveData

fun <T> LiveData<T>.nonNullValue(): T = value!!