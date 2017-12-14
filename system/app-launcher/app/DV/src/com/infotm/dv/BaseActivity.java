package com.infotm.dv;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.Toast;

import com.infotm.dv.adapter.InfotmSetting;
import com.infotm.dv.adapter.SettingSection;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.connect.NetService;

public class BaseActivity extends Activity{
    private static final String tag = "BaseActivity";
    public static InfotmCamera mCamera = null;
    private BroadcastReceiver mCameraNetworkStateListener;
    public static final int DIALOG_SELECT = 1;
    public static final int DIALOG_LOST = 4;
    SharedPreferences mPref;

	
    public BaseActivity(){
        mCameraNetworkStateListener = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (NetService.ACTION_NETUPDATE.equals(intent.getAction())){
                    onNetworkStateUpdate(intent);
                }
            }
        };
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
//        getSupportActionBar().setDisplayHomeAsUpEnabled(true);//FEATURE_NO_TITLE in mainactivity will cause nullpointer
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        getCamera(null, null);
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        Log.i(tag, "----onResume----");
        registerReceiver(mCameraNetworkStateListener, new IntentFilter(NetService.ACTION_NETUPDATE));
        Intent i = new Intent(NetService.ACTION_NETUPDATE);
        i.putExtra("BaseActivity", 1);
        sendBroadcast(i);
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        Log.i(tag, "----onPause----");
        unregisterReceiver(mCameraNetworkStateListener);
    }
    
    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
         Log.i(tag, "----onDestroy----");
        super.onDestroy();
    }
    
    protected void onNetworkStateUpdate(Intent i){
        if (mCamera.mNetStatus < InfotmCamera.STAT_YES_NET_YES_PIN){
            showDialog(DIALOG_LOST);
        }
    }
    
    public static InfotmCamera getCamera(Bundle paramBundle, Intent paramIntent){
        mCamera = InfotmCamera.getInstance();
        return mCamera;
    }
    
//    public void showDialog(String paramString)//)TODO
//    {
////      showDialog(paramString, paramDialogFactory, true);
//    }
    
    public void startInitConfig(final Handler handler,int type){
    	final int mType=type;
        //init
        handler.post(new Runnable() {
            public void run() {
               // smartyProgressBar.show();
            }
        });
        mCamera.sendCommand(new InfotmCamera.OperationResponse() {
            @Override
            public boolean execute() {
                try {
                    String str = CommandSender.commandStringBuilder(InfotmCamera.PIN, mCamera.getNextCmdIndex(), 
                            "init", 1, null);
                    String ret = mCamera.mCommandSender.sendCommandGetString(str);
                    int index =ret.indexOf("model_infotm");
                    Log.i(tag,"len:"+ret.length()+" index:"+index);
                    String strJS=ret;
                    if(index>0)
                    { 
                       //get model_infotm.js
                       strJS=ret.substring(0,index);
                       mCamera.initGetCameraSettings(strJS);
                       strJS=ret.substring(index);
                    }
                    HashMap<String, String> lineMap = getLinesFromString(strJS);
                    
					Log.i(tag, "after getLinesFromString");
                    mCamera.mCurrentMode = lineMap.get(Utils.KEY_BIG_CURRENTMODE);
					Log.i(tag, "KEY_BIG_CURRENTMODE");
                    mCamera.mCurrentBatteryLv = lineMap.get(Utils.KEY_BATTRTY);
                    Log.i(tag, "KEY_BATTRTY");
                    mCamera.mCurrentCapaity = lineMap.get(Utils.KEY_CAPACITY);
					mCamera.mCurrentWorking= lineMap.get(Utils.KEY_WORKING);
                    mCamera.mCurrentCameraRearStatus= lineMap.get(Utils.KEY_CAMERA_REAR_STATUS);
                    mCamera.mCurrentCameraFormat= lineMap.get(Utils.KEY_CAMERA_FORMAT_STATUS);
                    Log.i(tag, "init----mCurrentCameraFormat:" + mCamera.mCurrentCameraFormat);
                    Iterator iterator = mCamera.mSettingData.keySet().iterator();
                    boolean foundOne = false;
                    while(iterator.hasNext()){
						  Log.i("iinniitt", "hasNext:");
                        SettingSection sec = (SettingSection) iterator.next();
                        ArrayList<InfotmSetting> isetings = mCamera.mSettingData.get(sec);
                        for (int i = 0; i < isetings.size(); i ++){
                            String key = isetings.get(i).getKey();
                            isetings.get(i).setSelectedValue(lineMap.get(key));
                            Log.i("iinniitt", "key:" + key + " ---selcected:"+ lineMap.get(key));
                            
                            foundOne = true;
                        }
                    }
                    if (null == ret || ret.length() == 0 || !foundOne){
                        Log.e(tag, "-----init error:" + ret);
                        return false;
                    }
                    return true;
                } catch (Exception e) {
                    e.printStackTrace();
                    return false;
                }
                
            }
            
            @Override
            public void onSuccess() {
            	if(mType!=0)
            	Toast.makeText(BaseActivity.this,  R.string.device_setting_success, 2000).show();
                //smartyProgressBar.onSuccess();
/*                handler.postDelayed(new Runnable() {
                    public void run() {
                        if (smartyProgressBar.isShowing()){
                            smartyProgressBar.dismiss();
                        }
                    }
                }, 500L);*/
                mCamera.mNetStatus = InfotmCamera.STAT_Y_Y_Y_INIT;
                //sendBroadcast(new Intent(NetService.ACTION_NETUPDATE));
            }
            
            @Override
            public void onFail() {
            	if(mType!=0)
            	Toast.makeText(BaseActivity.this,  R.string.device_setting_fail, 2000).show();
                //smartyProgressBar.onFail();
/*                handler.postDelayed(new Runnable() {
                    public void run() {
                        if (smartyProgressBar.isShowing()){
                            smartyProgressBar.dismiss();
                        }
                    }
                }, 500L);*/
//                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN ;
            }
        }, handler);
    }
    
    public static HashMap<String, String> getLinesFromString(String ret){
        String[] lines = ret.split("\n");
        HashMap<String, String> cfgMap = new HashMap<String, String>();
        for (int i = 0; i < lines.length; i ++){
            if (null != lines[i] && lines[i].trim().length() != 0 && !lines[i].trim().startsWith("#")){
                Log.i(tag, "---getLinesFromString");
                String ttString = lines[i].trim();
                int keyendIndex = ttString.indexOf("=");
                if (keyendIndex > 0){
                    String key = ttString.substring(0, keyendIndex);
                    ttString = ttString.substring(keyendIndex + 1);
                    int endIndex = ttString.indexOf("#");
                    String value = ttString;
                    if (endIndex > 0){
                        value = ttString.substring(0,endIndex).trim();
                    } else {
                        value = ttString.trim();
                    }
                    cfgMap.put(key, value);
                    Log.i(tag, "---getLinesFromString---key:" + key + " value:" + value + "----" + i + " total:" + lines.length);
                }
            }
        }
        return cfgMap;
    }
    
    protected void goToHome(){
        Intent localIntent = new Intent(this, HomeActivity.class);
        localIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(localIntent);
    }
    
    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
            case DIALOG_SELECT:
                return new AlertDialog.Builder(this)
//                .set
                .create();
            case DIALOG_LOST:
                String msg = getResources().getString(R.string.reconnecting_msg, mPref.getString("wifi", "a camera"));
                return new AlertDialog.Builder(this)
                        .setTitle(R.string.reconnecting)
                        .setMessage(msg)
                        .setNegativeButton(android.R.string.cancel,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        goToHome();
                                    }
                                })
                        .create();
            default:
                break;
        }
        // TODO Auto-generated method stub
        return super.onCreateDialog(id);
    }
    
    @Override
    protected Dialog onCreateDialog(int id, Bundle args) {
        // TODO Auto-generated method stub
        return super.onCreateDialog(id, args);
    }
}