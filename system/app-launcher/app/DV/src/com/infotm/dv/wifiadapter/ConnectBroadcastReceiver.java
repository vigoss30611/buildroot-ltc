package com.infotm.dv.wifiadapter;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.infotm.dv.connect.CommandService;

public class ConnectBroadcastReceiver extends BroadcastReceiver{
    private String tag = "ConnectBroadcastReceiver";
    public final static String STARTSERIVE_CONNECT= "infotm.STARTSERIVE";
    public final static String STOPSERIVE_CONNECT= "infotm.STOPSERIVE";
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.v(tag, "------"+ intent.getAction());
        if (STARTSERIVE_CONNECT.equals(intent.getAction())){
            context.startService(new Intent(context,CommandService.class));
        } else if (STOPSERIVE_CONNECT.equals(intent.getAction())){
            context.stopService(new Intent(context,CommandService.class));
        }
    }
    
}