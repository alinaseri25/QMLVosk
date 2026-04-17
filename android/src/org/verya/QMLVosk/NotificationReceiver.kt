package org.verya.QMLVosk

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.util.Log

class NotificationReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        when(intent.action) {
        "org.verya.QMLVosk.ACTION_STOP" -> {
            TestBridge.nativeOnNotificationAction("stop")
        }
        "org.verya.QMLVosk.ACTION_START" -> {
            TestBridge.nativeOnNotificationAction("start")
        }
    }
    }
}
