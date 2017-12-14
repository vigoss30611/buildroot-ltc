package com.infotm.dv;

import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandService;
import com.infotm.dv.connect.NetService;

public class HomeActivity extends BaseActivity{
    String tag = "HomeActivity";
    WifiManager mWifiManager;
    ConnectivityManager mConnectMgr;
    Button mBtnRemote;
    ProgressBar mPbRemote;
    BroadcastReceiver mReceiver;
    SharedPreferences mPref;
    int mStatus = 0;
    ScanResult mSelectedResult;
    private Handler mHandler = new Handler();
    BroadcastReceiver mCommandBroadcastReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context context, Intent intent) {
            String ss = intent.getStringExtra("data");
            Log.v(tag, "------datachanged:" + ss);
        }
    };
    private boolean mCoReceiverOn = false;
    /*
    BroadcastReceiver mNetReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (NetService.ACTION_SETGONG.equals(intent.getAction())){
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        switch (mCamera.mNetStatus) {
                            case InfotmCamera.STAT_NONE:
                            case InfotmCamera.STAT_NO_NET:
                                mBtnRemote.setText("no net :" + mCamera.mNetStatus);
                                break;
                            case InfotmCamera.STAT_YES_NET_NO_PIN:
                                mBtnRemote.setText("yes net but no pin");
                                break;
                            case InfotmCamera.STAT_YES_NET_YES_PIN:
                                mBtnRemote.setText("ok");
                                break;
                            default:
                                break;
                        }
                        mPbRemote.setVisibility(View.GONE);
                    }
                });
            }
//            else if (NetService.ACTION_DISCONNECT.equals(intent.getAction())){
//                mPbRemote.setVisibility(View.VISIBLE);
//            }
        }
    };*/
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.a_home);
        getActionBar().setDisplayHomeAsUpEnabled(false);
        startService(new Intent(getApplicationContext(),CommandService.class));
        
        registerReceiver(mCommandBroadcastReceiver, new IntentFilter(CommandService.ACTION_DATACHANGE));
        mCoReceiverOn = true;
        
        mBtnRemote = (Button) findViewById(R.id.btn_remote);
        mPbRemote = (ProgressBar) findViewById(R.id.pb_remote_loading);
        setButtonClickListener();
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        mCamera = InfotmCamera.getInstance();
        mConnectMgr = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        Log.v(tag, "--------onPause-----");
        ActivityManager am = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        ComponentName cn = am.getRunningTasks(2).get(1).topActivity;
        Log.v(tag, "---------toppkg:"+cn.getPackageName());
    }
    
    @Override
    protected void onStop() {
        super.onStop();
        Log.v(tag, "--------onStop-----");
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        Log.v(tag, "----onResume----" + mCamera.mNetStatus);
        if (mCamera.mNetStatus < InfotmCamera.STAT_YES_NET_YES_PIN){
            mPbRemote.setVisibility(View.VISIBLE);
            mBtnRemote.setText(R.string.experience_cam_searching);
          //  startService(new Intent(getApplicationContext(), NetService.class));
        }
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (null != mCommandBroadcastReceiver && mCoReceiverOn){
            unregisterReceiver(mCommandBroadcastReceiver);
            mCoReceiverOn = false;
        }
        stopService(new Intent(getApplicationContext(),CommandService.class));
        //stopService(new Intent(getApplicationContext(), NetService.class));
    }
    
    @Override
    public void onBackPressed() {
//        super.onBackPressed();
        stopService(new Intent(getApplicationContext(),CommandService.class));
        //stopService(new Intent(getApplicationContext(), NetService.class));
        mCamera.mNetStatus = InfotmCamera.STAT_NONE;
        finish();
    }
    
    @Override
    protected void onNetworkStateUpdate(Intent intent) {
//        super.onNetworkStateUpdate();
        Log.v(tag, "----onNetworkStateUpdate----"+mCamera.mNetStatus + " flag:" + intent.getIntExtra("BaseActivity", 0));
        if (1 == intent.getIntExtra("BaseActivity", 0)){
            //鐢变簬onpause鏃跺凡缁忚Е鍙戯紝鑻ユ湭杩炴帴閲嶆柊杩炴帴銆傛墍浠ラ伩鍏嶉噸澶嶉�犳垚
            return;
        }
        String missingString = getResources().getString(R.string.experience_cam_missing);
        switch (mCamera.mNetStatus) {
            case InfotmCamera.STAT_NONE:
                mBtnRemote.setText(missingString + mCamera.mNetStatus);
                mPbRemote.setVisibility(View.GONE);
                break;
            case InfotmCamera.STAT_DISCONNECT://杩欎釜涓嶆槸缁撴灉
                mBtnRemote.setText(missingString + mCamera.mNetStatus);
                mPbRemote.setVisibility(View.VISIBLE);
                break;
            case InfotmCamera.STAT_NO_NET:
                mBtnRemote.setText(missingString + mCamera.mNetStatus);
                mPbRemote.setVisibility(View.GONE);
                break;
            case InfotmCamera.STAT_YES_NET_NO_PIN:
                mBtnRemote.setText(missingString + mCamera.mNetStatus);
                mPbRemote.setVisibility(View.GONE);
                break;
            case InfotmCamera.STAT_YES_NET_YES_PIN:
                mBtnRemote.setText(getResources().getString(R.string.experience_cam_found));
                mPbRemote.setVisibility(View.GONE);
                break;
            default:
                break;
        }
    }
    
    public void setButtonClickListener(){
        if (null == mBtnRemote){
            Log.e(tag, "-------null mBtnRemote");
        }
        mBtnRemote.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(tag, "----onClick----netstatus:" + mCamera.mNetStatus);
                switch (mCamera.mNetStatus) {
                    case InfotmCamera.STAT_NONE:
                    case InfotmCamera.STAT_NO_NET:
                    case InfotmCamera.STAT_YES_NET_NO_PIN:
                        Intent i = new Intent(HomeActivity.this, ConnectActivity.class);
                        startActivity(i);
                        break;
                    case InfotmCamera.STAT_YES_NET_YES_PIN:
                    case InfotmCamera.STAT_Y_Y_Y_INIT:
                        Intent imain = new Intent(HomeActivity.this, MainActivity.class);
                        startActivity(imain);
                        break;
                    default:
                        break;
                }
            }
        });
    }
}