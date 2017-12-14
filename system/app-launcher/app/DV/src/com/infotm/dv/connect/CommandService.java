
package com.infotm.dv.connect;

import android.R.string;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import com.infotm.dv.CameraSettingsActivity;
import com.infotm.dv.Utils;
import com.infotm.dv.adapter.InfotmSetting;
import com.infotm.dv.adapter.SettingSection;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.SocketServer.DataChangeListener;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;

public class CommandService extends Service {
    private static final String tag = "CommandReceiver";
    public static final String ACTION = "com.lql.service.ServiceDemo";
    public static final String ACTION_DATACHANGE = "com.infotm.dv.DATACHANGED";//InfotmCamera要在所有情况下都更新到,所以
    SocketServer mServer;
    InfotmCamera mCamera;
    
    @Override
    public IBinder onBind(Intent intent) {
        Log.v(tag, "ServiceDemo onBind");
        return null;
    }

    @Override
    public void onCreate() {//在哪start 在哪销毁？？！
        super.onCreate();
        mCamera = InfotmCamera.getInstance();
        Log.v(tag, "-------onCreate------");
        new Thread(new Runnable() {
            public void run() {
                mServer = new SocketServer(12345);
                mServer.setDataChangeListener(mDataChangeListener);
                mServer.beginListen();
            }
        }).start();
    }
    
    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.v(tag, "----ondestroy");
        if (null != mServer){
            mServer.closeServer();
        }
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.v(tag, "ServiceDemo onStart");
        super.onStart(intent, startId);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.v(tag, "ServiceDemo onStartCommand");
        return super.onStartCommand(intent, flags, startId);
    }
    
    Comparator<String> mComparator = new Comparator<String>() {
        @Override
        public int compare(String lhs, String rhs) {
            // TODO Auto-generated method stub
            if(lhs == null && rhs == null)
                return 0;
            if(lhs == null)
                return -1;
            return lhs.compareTo(rhs);
        }
    };
    
    DataChangeListener mDataChangeListener = new DataChangeListener() {
        @Override
        public void onDataChanged(String ss) {
            if(ss.startsWith("filelist")) {
                int validIndex = ss.indexOf("list:");
                Log.d(tag, "index: " + validIndex);
                if(validIndex < 0)
                    return;
                mCamera.mVideoList.clear();
                mCamera.mLockedVideoList.clear();
                mCamera.mPhotoList.clear();
                String validString = ss.substring(validIndex + 5);
                validString.trim();
                if(!validString.isEmpty()) {
                    String[] files = validString.split("/");
                    if(files.length > 0) {
                        for(String fileName : files) {
                            if(fileName.contains(".jpg")) {
                                mCamera.mPhotoList.add(fileName);
                            } else if(fileName.contains("LOCK")) {
                                mCamera.mLockedVideoList.add(fileName);
                            } else if(fileName.contains(".mp4")){
                                mCamera.mVideoList.add(fileName);
                            }else if(fileName.contains(".mkv")){
                                mCamera.mVideoList.add(fileName);
                            }
                        }
                        Log.d(tag, "File list parsed succeed, videoCount: " + mCamera.mVideoList.size() + 
                                ", lockedCount: " + mCamera.mLockedVideoList.size() + ", photoCount: " + mCamera.mPhotoList.size());
                    }
                }
                sendBroadcast(new Intent(InfotmCamera.ACTION_PLAYBACK_LIST_UPDATED));
                return;
            }
            //Log.v(tag, "<<<<<<<<<<onDataChanged<<<<<<<<<<\n" + ss);

            String[] lines = ss.split("\n");
            for (int j = 0;j < lines.length; j ++){
                Log.v(tag, "----line"+j+" :"+ lines[j]);
            }
            int len = lines.length;
            int usefulIndex = 0;
            for (int i = 0; i < len; i ++){
                if (null != lines[i] || lines[i].trim().length() != 0){
                    usefulIndex = i;
                    if (usefulIndex > 6){
                        break;
                    }
                }
            }
            if (usefulIndex < 6){
                Log.e(tag, "-----nousefulIndex:"+usefulIndex);
            }
            Intent intent = new Intent(ACTION_DATACHANGE);
            intent.putExtra("data", ss);
            intent.setPackage("com.infotm.dv");
            
            ArrayList<String> changedStrings = new ArrayList<String>();
            try {
                if (lines[0].startsWith("pin:") && lines[0].length() >= 10){
                    if (lines[0].substring(4, 10).equals(InfotmCamera.getPin(getApplicationContext()))){
                        if (lines[1].startsWith("cmdfrom:phone")){
                            intent.putExtra("from", "phone");
                            String cmdSeq = InfotmCamera.getLineValue(lines[2].trim());
                            int cmdIndex =  Integer.valueOf(cmdSeq);
                            intent.putExtra("cmdseq", cmdSeq);
                            String type = InfotmCamera.getLineValue(lines[3]).trim();
                            if ("setprop".equalsIgnoreCase(type)){
                                //忽略cmd_num lines[4]
                                int cmdNum = lines[5].length() - lines[5].replace(";", "").length();
                                String cmdString = lines[5];
                                intent.putExtra("cmd", cmdString);
                                for (int i = 0; i < cmdNum; i ++){
                                    int start1 = cmdString.indexOf(":");
                                    int end1 = cmdString.indexOf(";");
                                    String tmpKey = cmdString.substring(0,start1);
                                    String tmpValue = cmdString.substring(start1 + 1, end1);
                                    Log.v(tmpKey, "----onDataChanged---fromphone-key:"+ tmpKey + " value:"+ tmpValue);
                                    updateData(tmpKey, tmpValue);
                                    changedStrings.add(tmpKey + ":" + tmpValue);
                                    cmdString = cmdString.substring(end1 + 1, cmdString.length());
                                }
                            }
                        } else if (lines[1].startsWith("cmdfrom:spv")){
                            intent.putExtra("from", "spv");
                            String cmdSeq = InfotmCamera.getLineValue(lines[2].trim());
                            intent.putExtra("cmdseq", cmdSeq);
                            String type = InfotmCamera.getLineValue(lines[3]).trim();
                            if ("setprop".equalsIgnoreCase(type)){
                                //忽略cmd_num lines[4]
                                int cmdNum = lines[5].length() - lines[5].replace(";", "").length();
                                String cmdString = lines[5];
                                intent.putExtra("cmd", cmdString);
                                for (int i = 0; i < cmdNum; i ++){
                                    int start1 = cmdString.indexOf(":");
                                    int end1 = cmdString.indexOf(";");
                                    String tmpKey = cmdString.substring(0,start1);
                                    String tmpValue = cmdString.substring(start1 + 1, end1);
                                    Log.v(tmpKey, "----onDataChanged----key:"+ tmpKey + " value:"+ tmpValue);
                                    updateData(tmpKey, tmpValue);
                                    changedStrings.add(tmpKey + ":" + tmpValue);
                                    cmdString = cmdString.substring(end1 + 1, cmdString.length());
                                }
                            }
                        }
                    } else {
                        Log.e(tag, "-----line0=" + lines[0].substring(4, 10) + " PIN=" + InfotmCamera.getPin(getApplicationContext()));
                    }
                } else {
                    Log.e(tag, "-----line0:" + lines[0]);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            Bundle bundle = new Bundle();
            bundle.putStringArrayList("bundle", changedStrings);
            intent.putExtras(bundle);
            sendBroadcast(intent);
        }
    };
    
    private void updateData(String key,String value){
        if (Utils.KEY_BIG_CURRENTMODE.equalsIgnoreCase(key)){
            mCamera.mCurrentMode = value;
            return;
        }
        if (Utils.KEY_WORKING.equalsIgnoreCase(key)){
            mCamera.mCurrentWorking = value;
            return;
        }
        if (Utils.KEY_CAMERA_REAR_STATUS.equalsIgnoreCase(key)){
            mCamera.mCurrentCameraRearStatus= value;
            return;
        }
        if (CameraSettingsActivity.KEY_CAPACITY.equalsIgnoreCase(key)){
            mCamera.mCurrentCapaity = value;
            return;
        }
        if (CameraSettingsActivity.KEY_BATTERY.equalsIgnoreCase(key)){
            mCamera.mCurrentBatteryLv = value;
            return;
        }
        Iterator iterator = mCamera.mSettingData.keySet().iterator();
        while(iterator.hasNext()){
            SettingSection s = (SettingSection) iterator.next();
            ArrayList<InfotmSetting> asSettings = mCamera.mSettingData.get(s);
            for (int i = 0; i < asSettings.size(); i ++){
                if (asSettings.get(i).getKey().trim().equalsIgnoreCase(key.trim())){
                    asSettings.get(i).setSelectedValue(value);
                    return;
                }
            }
        }
        Log.e(tag, "updateData not found key:" + key + " value:"+ value);
    }
    
    private void updateModeDetail(String bigMode,InfotmCamera iCamera){
        if (bigMode.equalsIgnoreCase(Utils.MODE_G_VIDEO)){
            iCamera.mCurrentResolution = iCamera.getValueFromSetting(Utils.KEY_VIDEO_RESOLUTION);
            iCamera.mCurrentFps = iCamera.getValueFromSetting(Utils.KEY_VIDEO_FPS);
        } else if (bigMode.equalsIgnoreCase(Utils.MODE_G_CAR)){
            iCamera.mCurrentResolution = iCamera.getValueFromSetting(Utils.KEY_CAR_RESOLUTION);
            iCamera.mCurrentFps = iCamera.getValueFromSetting(Utils.KEY_CAR_FPS);
        } else  if (bigMode.equalsIgnoreCase(Utils.MODE_G_PHOTO)){
            iCamera.mCurrentResolution = iCamera.getValueFromSetting(Utils.KEY_PHOTO_MEGA);
            iCamera.mCurrentFps = iCamera.getValueFromSetting(Utils.KEY_PHOTO_INTERVAL);
        }
    }
}
