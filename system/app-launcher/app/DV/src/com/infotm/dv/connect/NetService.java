package com.infotm.dv.connect;

import android.app.ActivityManager;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;

import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.wifiadapter.WifiAdmin;

import java.util.List;

public class NetService extends Service{
    private String tag = "NetService";
    WifiManager mWifiManager;
    ConnectivityManager mConnectMgr;
    InfotmCamera mCamera;
    BroadcastReceiver mReceiver;
    BroadcastReceiver mKeepAliveReceiver;
    SharedPreferences mPref;
    ScanResult mSelectedResult = null;
    private boolean isAlive = true;
    private Handler mHandler = new Handler(){
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case 0:
                    keepAlive();
                    mHandler.removeMessages(0);
                    Log.i(tag,"keepalive");
                    mHandler.sendEmptyMessageDelayed(0, 10*1000);//TODO TODO gejingbo remove this for test,remember!
                    break;
                case 1:
                    mHandler.removeMessages(0);
                    mCamera.mNetStatus = InfotmCamera.STAT_DISCONNECT;
                    sendBroadcast(new Intent(ACTION_NETUPDATE));
                    startMyServiceConnect();
                    break;
                default:
                    break;
            }
        };
    };
//    public static final String ACTION_SETGONG = "infotm.set.gone";
//    public static final String ACTION_DISCONNECT = "infotm.disconnect";
    public static final String ACTION_NETUPDATE = "com.infotm.dv.netupdCate";
//    public static final String ACTION_WIFI_STRENTH = "infotm.wifistrenth";
    public static final String ACTION_START_KEEPALIVE = "com.infotm.dv.startkeepalive";
    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return null;
    }
    
    @Override
    public void onCreate() {
        super.onCreate();
        Log.v(tag, "------onCreate------");
        mCamera = InfotmCamera.getInstance();
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        mConnectMgr = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        if (!mWifiManager.isWifiEnabled()){
            mWifiManager.setWifiEnabled(true);
        }
       // registerWifiConnect();
        registerKeepAliveRecever();
    }
    
    @Override
    public void onStart(Intent intent, int startId) {//只有这个可被多次调用
        super.onStart(intent, startId);
       // Log.v(tag, "----onstart----startID" + startId + intent.getAction() + " status:" + mCamera.mNetStatus);
        //startMyServiceConnect();
        mHandler.sendEmptyMessageDelayed(0, 10*1000);
    }
    
    public void startMyServiceConnect(){
        if (!mWifiManager.isWifiEnabled()){
            mWifiManager.setWifiEnabled(true);
        }
        String savedApString = mPref.getString("wifi", "");
        if (mCamera.mNetStatus < InfotmCamera.STAT_YES_NET_YES_PIN){
            if (savedApString.length() != 0){
                if (isSelectedConnected(savedApString)){
                    mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_NO_PIN;
                    mCamera.IP = getIp();
                    mCamera.apIP = getApIp();
                    String pin = mPref.getString("pin", "");
                    if (pin.length() == 0){
                        sendBroadcast(new Intent(ACTION_NETUPDATE));
                    } else {
                        pairPinByKeepAlive();
                    }
                } else {//有存值，但未连上
                    mCamera.mNetStatus = InfotmCamera.STAT_NO_NET;
                    mWifiManager.startScan();//触发scanresult-connect
                }
            } else {
                mCamera.mNetStatus = InfotmCamera.STAT_NONE;
                sendBroadcast(new Intent(ACTION_NETUPDATE));
            }
        }
    }
    
    public void registerWifiConnect(){
        IntentFilter mFilter = new IntentFilter();
        mFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        mFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        mFilter.addAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
        mFilter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
//        mFilter.addAction(WifiManager.RSSI_CHANGED_ACTION);
        mFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.v(tag, "---onReceive---" + intent.getAction());
                
                String savedApString = mPref.getString("wifi", "");
                String action = intent.getAction();
                if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)
                        || ConnectivityManager.CONNECTIVITY_ACTION.equals(action)
                        || WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(action)) {
                    if (savedApString.length() != 0){//有存值
                        if (isSelectedConnected(savedApString)){//有存值且连上
                            mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_NO_PIN;
                            mCamera.IP = getIp();
                            mCamera.apIP = getApIp();
                            if (mPref.getString("pin", "").length() != 0){
                                if (isApkFront()){
                                    pairPinByKeepAlive();
                                }
                            } else {
                                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_NO_PIN;
                                sendBroadcast(new Intent(ACTION_NETUPDATE));
                            }
                        } else {//有存值，但未连上
                            mCamera.mNetStatus = InfotmCamera.STAT_NO_NET;
                            if (isApkFront()){
                                mWifiManager.startScan();//触发scanresult-connect
                            }
                        }
                    } else {
                        mCamera.mNetStatus = InfotmCamera.STAT_NONE;
                        sendBroadcast(new Intent(ACTION_NETUPDATE));
                    }
                    /*
                    if (isSelectedConnected(savedApString)) {
                        Log.v(tag, "-------handleEvent ok");
                        mCamera.IP = getIp();
                        mCamera.apIP = getApIp();
                        mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_NO_PIN;
                        if (mPref.getString("pin", "").length() != 0){
                            pairPinByKeepAlive();
                        } else {
                            mCamera.mNetStatus = InfotmCamera.STAT_NONE;
                            sendBroadcast(new Intent(ACTION_SETGONG));
                        }
                    } else {
                        mCamera.mNetStatus = InfotmCamera.STAT_NO_NET;
                        sendBroadcast(new Intent(ACTION_SETGONG));
                        sendBroadcast(new Intent(ACTION_DISCONNECT));
                    }*/
                } else if (WifiManager.SCAN_RESULTS_AVAILABLE_ACTION.equals(action)) {
                    if (!isApkFront()){
                        Log.e(tag, " apk in not in front");
                        return;
                    }
                    mSelectedResult = null;//shoud add this ?
                    if (!isSelectedConnected(savedApString)){
                        List<ScanResult> wifiList = mWifiManager.getScanResults();
                        if (null != wifiList){
                            for (ScanResult s : wifiList){
                                Log.v(tag, "ssid:" + s.SSID + " saved:" + savedApString);
                                if (("\"" + s.SSID + "\"").equalsIgnoreCase("\""+savedApString+"\"")){
                                    mSelectedResult = s;
                                    break;
                                }
                            }
                        }
                        if (null == mSelectedResult){
                            Log.v(tag, "find mSelectedResult null");
                            mCamera.mNetStatus = InfotmCamera.STAT_NO_NET;
                            sendBroadcast(new Intent(ACTION_NETUPDATE));
                        } else {//连接
                            connectThisWifi(mSelectedResult,mPref.getString("password", ""));
                        }
                    }
                }
            }
        };
        registerReceiver(mReceiver, mFilter);
    }
    
    public void registerKeepAliveRecever(){
		Log.i(tag,"registerKeepAliveRecever");
        mKeepAliveReceiver = new BroadcastReceiver(){
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.i(tag,"registerKeepAliveRecever onReceive");
                mHandler.sendEmptyMessage(0);
            }
        };
        registerReceiver(mKeepAliveReceiver, new IntentFilter(ACTION_START_KEEPALIVE));
    }
    
    private boolean isApkFront(){
        ActivityManager am = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        ComponentName cn = am.getRunningTasks(2).get(0).topActivity;
        return getPackageName().equals(cn.getPackageName());
    }
    
    private void pairPinByKeepAlive(){//通过心跳pairpin
        String pin = InfotmCamera.getPin(getApplicationContext());
        final String string = CommandSender.commandStringBuilder(pin, 
                mCamera.getNextCmdIndex(), "keepalive", 1, "pin="+pin);
        mCamera.sendCommand(new InfotmCamera.OperationResponse() {
            
            @Override
            public boolean execute() {//in thread
                boolean ret = mCamera.mCommandSender.sendCommand(string);
                if (ret){
                    mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;
                    mHandler.sendEmptyMessage(0);
                } else {
                    mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_NO_PIN;
                }
                sendBroadcast(new Intent(ACTION_NETUPDATE));
                return ret;
            }
            
            @Override
            public void onSuccess() {//in handler
//                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;
//                mBtnRemote.setText("found ok");
//                mPbRemote.setVisibility(View.GONE);
//                sendBroadcast(new Intent(ACTION_SETGONG));
            }
            
            @Override
            public void onFail() {
//                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_NO_PIN;
//                mBtnRemote.setText("net ok but pin failed");
//                mPbRemote.setVisibility(View.GONE);
//                sendBroadcast(new Intent(ACTION_SETGONG));
            }
        }, null);
    }
    
    private void keepAlive(){
        String pin = InfotmCamera.getPin(getApplicationContext());
        Log.e(tag, "----start keepalive->:"+System.currentTimeMillis());
        final String string = CommandSender.commandStringBuilder(pin,
                mCamera.getNextCmdIndex(), "keepalive", 1, "pin=" + pin);
        mCamera.sendCommand(new InfotmCamera.OperationResponse() {

            @Override
            public boolean execute() {// in thread
                isAlive = mCamera.mCommandSender.sendCommand(string);
                if (!isAlive){
                    Log.e(tag, " keep alive fail !!");
                    //mHandler.sendEmptyMessage(1);
                }
                return isAlive;
            }

            @Override
            public void onSuccess() {// in handler
            // sendBroadcast(new Intent(ACTION_SETGONG));
            	Log.i(tag, "----keepalive success:"+System.currentTimeMillis());
            }

            @Override
            public void onFail() {
                // sendBroadcast(new Intent(ACTION_SETGONG));
                Log.e(tag, "----keepalive failed");
            }
        }, mHandler);
    }
    
    public void connectThisWifi(ScanResult s,String passWord){
        Log.v(tag, "--------connect this ssid:"+s.SSID);
        WifiAdmin mWifiAdmin = new WifiAdmin(this);
        String capabilities = s.capabilities;
        int type = 1;
        if (!TextUtils.isEmpty(capabilities)) {
            if (capabilities.contains("WPA") || capabilities.contains("wpa")) {
                type = 3;
            } else if (capabilities.contains("WEP") || capabilities.contains("wep")) {
                type = 2;
            } else {
                type = 1;
            }
        }
        WifiConfiguration wcg = mWifiAdmin.CreateWifiInfo(s, passWord, type);
        mWifiAdmin.addNetwork(wcg);
    }
    
    private boolean isSelectedConnected(String ssid){
        NetworkInfo wifiNetInfo = mConnectMgr
                .getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (null != ssid
                && wifiNetInfo.isConnected()
                && mWifiManager.getConnectionInfo().getSSID()
                        .equalsIgnoreCase("\"" + ssid + "\"")) {
            return true;
        }
        return false;
    }
    
    public String getIp(){
        if (mWifiManager.isWifiEnabled()){
            if (null != mWifiManager.getConnectionInfo()){
                int i = mWifiManager.getConnectionInfo().getIpAddress();
                String ip = (i & 0xFF) + "." +
                        ((i >> 8) & 0xFF) + "." +
                        ((i >> 16) & 0xFF) + "." +
                        (i >> 24 & 0xFF) ;
                Log.v(tag, "------ip:"+ip);
                return ip;
            }
        }
        return null;
    }
    
    public String getApIp(){
        if (mWifiManager.isWifiEnabled()){
            DhcpInfo dhcpInfo = mWifiManager.getDhcpInfo();
            if (null != dhcpInfo){
                int i = dhcpInfo.serverAddress;
                String apIp = (i & 0xFF) + "." +
                        ((i >> 8) & 0xFF) + "." +
                        ((i >> 16) & 0xFF) + "." +
                        (i >> 24 & 0xFF) ;
                Log.v(tag, "------apIp:"+apIp);
                return apIp;
            }
        }
        return null;
    }
    
    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.v(tag, "------onDestroy------");
        if (null != mReceiver){
            unregisterReceiver(mReceiver);
        }
        if (null != mKeepAliveReceiver){
            unregisterReceiver(mKeepAliveReceiver);
        }
        mHandler.removeMessages(0);
        
    }
}