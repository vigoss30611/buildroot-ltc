
package com.infotm.dv.camera;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;

import com.infotm.dv.CameraSettingsActivity;
import com.infotm.dv.adapter.InfotmSetting;
import com.infotm.dv.adapter.SettingHelper;
import com.infotm.dv.adapter.SettingSection;
import com.infotm.dv.connect.CameraStateFetcher;
import com.infotm.dv.connect.CommandService;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.connect.SocketServer;
import com.infotm.dv.connect.SocketServer.DataChangeListener;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class InfotmCamera {
    public final static String tag = "InfotmCamera";
    public static String PIN = "222222";
    public static int cmdIndex = 1;

    public static InfotmCamera mCamera = null;
    private Map<SettingSection, ArrayList<InfotmSetting>> mUnmodifiableSettings;
    CameraStateFetcher mCameraStateFetcher;
    public CommandSender mCommandSender;
    public SocketServer mCommandReceiver;
    private final ConcurrentHashMap<String, String> mCommandUrlsById;
    private final ConcurrentHashMap<String, String> mOverrideSettingUrlsById;
    public LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> mSettingData = new LinkedHashMap<SettingSection, ArrayList<InfotmSetting>>();
    public static String IP = "";
    public static String apIP = "";
    public static int mNetStatus = 4;
    public static final int STAT_NONE = 0;//没连接过，本地没有保存值
    public static final int STAT_DISCONNECT = 1;
    public static final int STAT_NO_NET = 2;
    public static final int STAT_YES_NET_NO_PIN = 3;
    public static final int STAT_YES_NET_YES_PIN = 4;
    public static final int STAT_Y_Y_Y_INIT = 5;
//    public Handler frontHandler = null;
    
    public String mCurrentMode = "";//大模式
    public String mCurrentWorking = "off";
    public String mCurrentRecordTime = "0";
    public String mCurrentResolution = "";//由于不同的模式有不同的key导致不行
    public String mCurrentFps = "0";
    public String mCurrentBatteryLv = "";
    public String mCurrentCapaity = "";
    public String mCurrentAp = "off";
    public String mCurrentCameraRearStatus="off";
    public static String mCurrentCameraFormat="1";
    public static final String DEVICE_IP = "device.ip";
    
    public static final String ACTION_PLAYBACK_LIST_UPDATED = "action.dv.playback.list.updated";
    public static final String MEDIA_TYPE = "media.type";
    public static final int MEDIA_INVALID  = -1;
    public static final int MEDIA_VIDEO = 1;
    public static final int MEDIA_VIDEO_LOCKED = 2;
    public static final int MEDIA_PHOTO = 3;
    
    public static final int CHANNEL_FRONT = 1;
    public static final int CHANNEL_REAR = 2;
    public static final int CHANNEL_LOCK = 3;
    public static final int CHANNEL_LOCK_REAR = 4;
    public static final int CHANNEL_PHOTO = 5;
    
    public static final String MEDIA_FILE_NAME = "media.file.name";
    
    //playback list
    public static ArrayList<String> mVideoList = new ArrayList<String>();
    public static ArrayList<String> mLockedVideoList = new ArrayList<String>();
    public static ArrayList<String> mPhotoList = new ArrayList<String>();
    public static String currentSSID="";
    public static String currentIP="";
    public static SettingHelper mSettingHelper;
    public InfotmCamera() {
        mUnmodifiableSettings = new HashMap();
        setSettings(new HashMap(), new ArrayList());// init 空
        mCommandSender = new CommandSender(this);
//        new Thread(new Runnable() {
//            @Override
//            public void run() {
//                mCommandReceiver.beginListen();
//            }
//        }).start();
        
        mCommandUrlsById = new ConcurrentHashMap();
        mOverrideSettingUrlsById = new ConcurrentHashMap<String, String>();
    }
    
    public static String getLineValue(String s){
        int slashIndex = s.indexOf(":");
        if (slashIndex > -1){
            String retunString = s.substring(slashIndex + 1, s.length());
            Log.v(tag, "--getLineValue--"+s+"====="+retunString);
            return retunString;
        }
        return null;
    }
    
    public String getValueFromSetting(String keyString){
        if (null == keyString || keyString.length() == 0){
            return "";
        }
        Iterator iterator = mSettingData.keySet().iterator();
        while (iterator.hasNext()) {
            SettingSection section = (SettingSection) iterator.next();
            ArrayList<InfotmSetting> settings = mSettingData.get(section);
            for(int i = 0; i < settings.size(); i ++){
                if (keyString.equalsIgnoreCase(settings.get(i).getKey())){
                    return settings.get(i).getSelectedName();
                }
            }
        }
        return "";
    }
    
    public InfotmSetting getObjFromSetting(String keyString){
        if (null == keyString || keyString.length() == 0){
            return null;
        }
        Iterator iterator = mSettingData.keySet().iterator();
        while (iterator.hasNext()) {
            SettingSection section = (SettingSection) iterator.next();
            ArrayList<InfotmSetting> settings = mSettingData.get(section);
            for(int i = 0; i < settings.size(); i ++){
                if (keyString.equalsIgnoreCase(settings.get(i).getKey())){
                    return settings.get(i);
                }
            }
        }
        return null;
    }
    
//    public String getIpAddress(){
//        return "10.5.5.9";
//    }
//    
//    public int getPort(){
//        return 20000;
//    }
    
    public static void setPin(Context c,String pinString){
        SharedPreferences mPref = PreferenceManager.getDefaultSharedPreferences(c);
        Editor e = mPref.edit();
        e.putString("pin", pinString);
        e.commit();
        PIN = pinString;
    }
    
    public static String getPin(Context c){
        if (null == PIN || PIN.length() == 0){
            SharedPreferences mPref = PreferenceManager.getDefaultSharedPreferences(c);
            PIN = mPref.getString("pin", "");
        }
        return PIN;
    }
    
    public static InfotmCamera getInstance() {
        if (null == mCamera) {
            mCamera = new InfotmCamera();
        }
        return mCamera;
    }
    
    public int getNextCmdIndex(){
        cmdIndex ++;
        return cmdIndex;
    }
    public void setSetHelper(Context c)
    {
    	  mSettingHelper = new SettingHelper(c, this);
    	  return;
    }
    public LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> initGetCameraSettings(String str){//should called after connected pined ok
        if (mNetStatus == STAT_YES_NET_YES_PIN){
			Log.e(tag, "called in  mNetStatus:" + mNetStatus);
            mSettingData = mSettingHelper.getLegacySectionsBystream(mSettingHelper.buildLabelLookup(),str);
        } else {
            Log.e(tag, "called in error status mNetStatus:" + mNetStatus);
        }
        return mSettingData;
    }

    public void setSettings(Map<SettingSection, ArrayList<InfotmSetting>> paramMap,
            Collection<InfotmSetting> paramCollection) {
        //生成list mUnmodifiableSettings
        
//        mUnmodifiableSettings.put(key, value)
    }

    public Map<SettingSection, ArrayList<InfotmSetting>> getSettings() {
        return mUnmodifiableSettings;
    }
    
    public void sendCommand(final CameraOperation paramCameraOperation, final Handler paramHandler){
        new Thread(new Runnable() {
            public void run() {
                boolean boolRet = paramCameraOperation.execute();
                if (paramHandler == null) {
                    return;
                }
                Message localMessage = paramHandler.obtainMessage();
                if (boolRet) {
                    localMessage.what = CameraOperation.WHAT_SUCCESS;
                } else {
                    localMessage.what = CameraOperation.WHAT_FAIL;
                }
                paramHandler.sendMessage(localMessage);
            }
        }).start();
    }
    
    public void sendCommand(final OperationResponse paramOperationResponse, final Handler paramHandler){
        new Thread(new Runnable() {
            public void run() {
                Log.v(tag, "------sendCommand");
                final boolean bool = paramOperationResponse.execute();
                Log.v(tag, "------sendCommand-execute-end-" + bool);
                if (paramHandler == null) {
                    return;
                }
                paramHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (bool) {
                            paramOperationResponse.onSuccess();
                        } else {
                            paramOperationResponse.onFail();
                        }
                    }
                }, 500L);
            }
        }).start();
    }
    
    public int sendGETSocketResponse(String str){
        
        return 0;
    }
    
    public void setCommandMap(HashMap<String, String> paramHashMap){
        mCommandUrlsById.putAll(paramHashMap);
    }
    
    public void setSettingMap(HashMap<String, String> paramHashMap) {
        mOverrideSettingUrlsById.putAll(paramHashMap);
    }
    
    public String getCommandUrl(String paramString){
        String str = null;
        if (mCommandUrlsById.containsKey(paramString))
            str = (String) mCommandUrlsById.get(paramString);
        return str;
    }
    
    public boolean remoteSetCameraDateTime(String paramString) {
        return mCommandSender.sendCommand(
                (String) mCommandUrlsById.get("GPCAMERA_SET_DATE_AND_TIME_ID"), paramString, 0);
    }

    public static interface OperationResponse extends InfotmCamera.CameraOperation {
        public void onFail();
        public void onSuccess();
    }

    public static interface CameraOperation {
        public static final int WHAT_FAIL = 256;
        public static final int WHAT_SUCCESS = 512;

        public boolean execute();
    }
}
