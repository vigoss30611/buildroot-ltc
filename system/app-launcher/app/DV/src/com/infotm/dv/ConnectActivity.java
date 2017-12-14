
package com.infotm.dv;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.drawable.AnimationDrawable;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import com.danale.video.sdk.device.constant.DeviceCmd;
import com.danale.video.sdk.device.entity.Connection;
import com.danale.video.sdk.device.entity.LanDevice;
import com.danale.video.sdk.device.handler.DeviceResultHandler;
import com.danale.video.sdk.device.result.DeviceResult;
import com.danale.video.sdk.device.result.SearchLanDeviceResult;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.connect.NetService;
import com.infotm.dv.wifiadapter.WifiAdapter;
import com.infotm.dv.wifiadapter.WifiAdmin;
import com.infotm.dv.wifiadapter.WifiApMan;

public class ConnectActivity extends Activity implements DeviceResultHandler{
    private static final String tag = "ConnectActivity";
	private ArrayList<String> resultList = new ArrayList<String>();
    private final static int MSG_REFRESH = 9;
    private Boolean bfind=false;
    private List<LanDevice> deviceList;
    private LanDevice device;
    
    public String apiVersion="1.0.150815";//"1.0.160318";
	public int channelNum=0;
	public String deviceId =App.DEVICE_UID;
	public DeviceType deviceType=DeviceType.IPC;
	public String netmask="255.255.255.255";
	public int p2pPort=12349;
	public String sn="danale_2015_di3";
	private App app;
	
	private ListView mListView;
    private int mUIStatus = 1;
    private WifiManager mWifiManager;
    private ConnectivityManager mConnectMgr;
    private  BroadcastReceiver mNetReceiver;
    private  ArrayList<ScanResult> mListData = new ArrayList<ScanResult>();
    private WifiAdapter mWifiAdapter;
    private  Scanner mScanner;
    private ScanResult mSelectedResult;
    private  InfotmCamera mCamera;
    private SharedPreferences mPref;
    private  String mSelectPassWord;
    int isForward=1;
    private Boolean bsendAp=false;
    private  Handler mHandler;
   private  IntentFilter mFilter ;

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	// TODO Auto-generated method stub
    	
    	switch (requestCode) {
		case 1:
			if(data!=null){
				mSelectPassWord =data.getStringExtra("password");
				Editor mEditor = mPref.edit();
	            mEditor.putString("wifi", mSelectedResult.SSID);
	            mEditor.commit();
	            setUiVisibility(2);
			}
			break;			

		default:
			break;
		}
    	super.onActivityResult(requestCode, resultCode, data);
    }
    
    private AdapterView.OnItemClickListener mOnItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
            Log.d(tag, "item choose " + arg2);
            bfind=false;
            mSelectedResult = mListData.get(arg2);
            if (mScanner != null) {
                mScanner.pause();
            }
            mSelectPassWord= mPref.getString(mSelectedResult.SSID, "");
            if("".equals(mSelectPassWord)){
            	if(mSelectedResult.SSID.indexOf("INFOTM_cardv_")>=0){
            		mSelectPassWord="admin888";
                    Editor mEditor = mPref.edit();
                    mEditor.putString("wifi", mSelectedResult.SSID);
                    mEditor.commit();
                    setUiVisibility(2);
            	}else{
            			Intent intent = new Intent(ConnectActivity.this, PassworldInputDialogActivity.class);
            			startActivityForResult(intent, 1);
            	}
            }else{
                Editor mEditor = mPref.edit();
                mEditor.putString("wifi", mSelectedResult.SSID);
                mEditor.commit();
                setUiVisibility(2);
            }
            


        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        app = (App) getApplication();
        setContentView(R.layout.connect_activity);
        mCamera = InfotmCamera.getInstance();
        mListView = (ListView) findViewById(R.id.list1);
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        isForward=mPref.getInt("connect_mode", 1);
        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        mConnectMgr = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        Log.i(tag, "isForward:"+isForward);
        if(isForward==2)
        {
		 setWifiInfo();
		}
        else if(isForward==3)
		{
			AlertDialog.Builder builder = new AlertDialog.Builder(ConnectActivity.this);
			builder.setTitle("input ip");
			LinearLayout layout = new LinearLayout(ConnectActivity.this);
			layout.setOrientation(LinearLayout.VERTICAL);
			
			final EditText edit = new EditText(ConnectActivity.this);
			edit.setHint("remoteip:");
			edit.setPadding(30, 30, 30, 30);
			layout.addView(edit);							
			final EditText edit1 = new EditText(ConnectActivity.this);
			edit1.setHint("localip:");
			edit1.setPadding(30, 30, 30, 30);
			layout.addView(edit1);	
			
			builder.setView(layout);
			builder.setPositiveButton(R.string.next, new DialogInterface.OnClickListener() {

			     @Override
			     public void onClick(DialogInterface dialog, int which) {
			    	 mCamera.IP = edit1.getText().toString();
			         mCamera.apIP = edit.getText().toString();
			         mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;//STAT_YES_NET_NO_PIN;
			         startService(new Intent(getApplicationContext(), NetService.class));
			         sendBroadcast(new Intent(NetService.ACTION_START_KEEPALIVE));
			         reconnectDevice(null, mCamera.apIP);
			     }
			    });
			builder.create().show();
		}
      //add for reconnect device
 /*        String wifi=mPref.getString("wifi", null);
    	String deviceIp=mPref.getString("deviceip", null);
   	if(null!=wifi&&null!=deviceIp){
    		reconnectDevice(wifi,deviceIp);
    	}else{*/
        mScanner = new Scanner();
        
        if(isForward==3)
        {
        }
        else 
        {
          showSerchDeviceDialog();
          setUiVisibility(1);
        }
        mFilter = new IntentFilter();
        mFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        mFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        mFilter.addAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
         mFilter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mFilter.addAction(WifiManager.RSSI_CHANGED_ACTION);
        mFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        mNetReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.v(tag, "------" + intent.getAction());
                handleEvent(context, intent);
            }
        };
        mHandler = new Handler() {
            public void handleMessage(android.os.Message msg) {
                Log.v(tag, "------" + msg.what);
                switch (msg.what) {
    			case 1:
    				Editor e=mPref.edit();
    				e.remove(mSelectedResult.SSID);
    				e.commit();
    				destroyConnectDialog();
    				Toast.makeText(ConnectActivity.this,R.string.device_connect_fail, 2000).show();
    				bfind=true;
    				mScanner.resume();
    				break;

    			default:
    				break;
    			}

            };
        };
    	//}
    }
    
    
    public void setUiVisibility(int lv) {
        mUIStatus = lv;
        findViewById(R.id.ui_list).setVisibility(View.VISIBLE);//lv == 1 ? View.VISIBLE :
        findViewById(R.id.ui_wifi).setVisibility(View.GONE);//lv == 2 ? View.VISIBLE :
        findViewById(R.id.ui_pin).setVisibility(View.GONE);//lv == 3 ? View.VISIBLE : 
        if (mUIStatus == 1) {
            initWifiList();
        } else if (mUIStatus == 2) {// wifi config password
            mScanner.pause();
            
            if (null != mSelectedResult) {
                /*TextView wifiNameText = (TextView) findViewById(R.id.wifi_name);
                final EditText wifiPassWord = (EditText) findViewById(R.id.wifi_password);
                Button wifiButton = (Button) findViewById(R.id.button_wificonfirm);
                wifiNameText.setText(mSelectedResult.SSID);*/
            	if (isSelectedConnected())
            	{
	            	mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;//STAT_YES_NET_NO_PIN;
	                 
	                 Log.i(tag, "-----------------already connect");
	                 mCamera.IP = getIp();
	                 mCamera.apIP = getApIp();
	 			     Log.i(tag,"IP:"+mCamera.IP+" <---->apIP:"+mCamera.apIP);
	                 Editor mEditor = mPref.edit();
	                 mEditor.putString("wifi", mSelectedResult.SSID);
	                 mEditor.commit();
	                 mUIStatus=3;
                }else
                {
                 new Thread(new Runnable() {
					@Override
					public void run() {
						// TODO Auto-generated method stub
		                String passWord =mSelectPassWord;// "admin888";//wifiPassWord.getText().toString();
		                WifiAdmin mWifiAdmin = new WifiAdmin(ConnectActivity.this);
		                String capabilities = mSelectedResult.capabilities;
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
		                Log.i(tag, "connect to wifi:"+mSelectedResult.SSID);
		                WifiConfiguration wcg = mWifiAdmin.CreateWifiInfo(mSelectedResult,
		                        passWord, type);
		                mWifiAdmin.addNetwork(wcg);
					}
				  }).start();
                }
                showConnectDialog();
            }
        } 
        if (mUIStatus == 3) {// pin ui
            mUIStatus=4;
            mScanner.pause();
            startService(new Intent(getApplicationContext(), NetService.class));
        	sendBroadcast(new Intent(NetService.ACTION_START_KEEPALIVE));
            //final EditText pinEditText = (EditText) findViewById(R.id.pin_password);
            //Button pinButton = (Button) findViewById(R.id.button_pinconfirm);
          //  pinButton.setOnClickListener(new View.OnClickListener() {
            //    @Override
              //  public void onClick(View v) {
                    String pinString = "222222";//pinEditText.getText().toString();
                    //pairPin(pinString);
                    if((isForward==1)||(isForward==0)){
                    	//searchforwardLanDevice();//正向
                    	searchLanDevice();
                    }else if(isForward==2)
                    {
                       sendWifi(pinString);
                    }
                    else
                    {
                       // sendAp(pinString);//反向
                    }
                    
                //}
            //});
        }
    }
    
    public void startAp()
	{
	     if (mWifiManager.isWifiEnabled()) {
            mWifiManager.setWifiEnabled(false);
        }
	    WifiApMan apMan = new WifiApMan(this) ;
		apMan.setWifiManager(mWifiManager);
		if(!apMan.isWifiApEnabled())
		{
		   apMan.setWifiApEnabled(apMan.getWifiApConfiguration(),true);
		}
	}

	public  void stopAp()
	{
		 WifiApMan apMan = new WifiApMan(this) ;
		 apMan.setWifiManager(mWifiManager);
		if(apMan.isWifiApEnabled()){ 
		   apMan.setWifiApEnabled(apMan.getWifiApConfiguration(),false);
		}
		if (!mWifiManager.isWifiEnabled()){
            mWifiManager.setWifiEnabled(true);
          }
	}
    
    public void sendAp(String pinString){
		if(bsendAp==true)
		{
		  Log.i(tag, "sendAp already send return");
		  return;
		}
		Log.i(tag, "sendAp bsendAp:"+bsendAp);
		bsendAp= true;
        //mSmartyProgressBar.show();
        final String pin = pinString;
//        InfotmCamera.PIN = pinString;
        mCamera.setPin(this, pinString);
		String SSID;
		String pwd;
		String KEYMGMT="WPA-PSK";
		WifiApMan apMan = new WifiApMan(this) ;
		apMan.setWifiManager(mWifiManager);
		SSID=apMan.getWifiApConfiguration().SSID;
		pwd = apMan.getWifiApConfiguration().preSharedKey;
		if(pwd==null)
		{
		  KEYMGMT="WEP";//"NONE";
		  pwd="";
		}
        final String string = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                mCamera.getNextCmdIndex(), "action", 3, "action.wifi.set:"+"STA;;"+SSID+";;"+pwd+";;"+KEYMGMT+";;");
        Log.i(tag, string);
        mCamera.sendCommand(new InfotmCamera.OperationResponse() {
            @Override
            public boolean execute() {
                return mCamera.mCommandSender.sendCommand(string);
            }
            
            @Override
            public void onSuccess() {
                //mSmartyProgressBar.onSuccess();
				startAp();
				Log.i(tag, "sendAp Success!");
                Editor mEditor = mPref.edit();
                mEditor.putString("pin", pin);
                mEditor.commit();
                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;
                searchLanDevice();
            }
            
            @Override
            public void onFail() {
                Log.i(tag, "sendAp Fail!");
                bsendAp= false;
             
            }
        },mHandler);
    }
 
	public void sendWifi(String pinString){
		 if(bsendAp==true)
		 {
		   Log.i(tag, "sendWifi already send return");
		   return;
		 }
		 Log.i(tag, "sendWifi bsendAp:"+bsendAp);
		 bsendAp= true;
		 //mSmartyProgressBar.show();
		 final String pin = pinString;
 // 	   InfotmCamera.PIN = pinString;
		 mCamera.setPin(this, pinString);
		 String SSID;
		 String pwd;
		 String KEYMGMT="WPA-PSK";
		 SSID=mPref.getString("saved-ssid", null);
		 pwd = mPref.getString("saved-pwd", null);
		 if(pwd==null)
		 {
		   KEYMGMT="WEP";//"NONE";
		   pwd="";
		 }
		 final String string = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
				 mCamera.getNextCmdIndex(), "action", 3, "action.wifi.set:"+"STA;;"+SSID+";;"+pwd+";;"+KEYMGMT+";;");
		 Log.i(tag, string);
		 mCamera.sendCommand(new InfotmCamera.OperationResponse() {
			 @Override
			 public boolean execute() {
				 return mCamera.mCommandSender.sendCommand(string);
			 }
			 
			 @Override
			 public void onSuccess() {
				 //mSmartyProgressBar.onSuccess();
				 restoreLastWifiAP();
				 Log.i(tag, "sendWifi Success!");
				 Editor mEditor = mPref.edit();
				 mEditor.putString("pin", pin);
				 mEditor.commit();
				 mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;
				 Intent intent = new Intent(ConnectActivity.this, DeviceListLocalActivity.class);
				 startActivity(intent);
				 finish();
			 }
			 
			 @Override
			 public void onFail() {
				 Log.i(tag, "sendWifi Fail!");
				 bsendAp= false;
			  
			 }
		 },mHandler);
	 }

    public void initWifiList() {
        stopAp();
        mScanner.resume();
        mWifiAdapter = new WifiAdapter(mListData, this, mWifiManager);
        mListView.setAdapter(mWifiAdapter);
    }

  	 private void setWifiInfo() {
  		   String ssid = null;
	       mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
	        if(mWifiManager.isWifiEnabled()) {
	        	ssid = mWifiManager.getConnectionInfo().getSSID();
	        }

	        Log.d(tag, "current wifi is " + ssid);
			if(ssid != null && ssid.indexOf("INFOTM_cardv") >= 0) 
			{
				ssid=null;
	        }
			
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setTitle(R.string.wlan_connect);
			LinearLayout layout = new LinearLayout(this);
			layout.setOrientation(LinearLayout.VERTICAL);
			
			final EditText editSSID = new EditText(this);
			String pwd=null;
			if(ssid==null)
			{
				editSSID.setHint(R.string.input_wlan_SSID);
			}else
			{
				editSSID.setText(ssid);
				mPref = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
				pwd = mPref.getString(ssid, null);
			}
			editSSID.setPadding(30, 30, 30, 30);
			layout.addView(editSSID);
			
			final EditText editPWD = new EditText(this);
			editPWD.setHint(R.string.input_wlan_pwd);
			editPWD.setPadding(30, 30, 30, 30);
			if(pwd!=null)
		    {
				editPWD.setText(pwd);
		    }
			layout.addView(editPWD);

			builder.setView(layout);
			builder.setPositiveButton(R.string.next, new DialogInterface.OnClickListener() {

			     @Override
			     public void onClick(DialogInterface dialog, int which) {
			      // TODO Auto-generated method stub
			    	  mPref = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
			            Editor mEditor = mPref.edit();
			            mEditor.putString("saved-ssid", editSSID.getText().toString());
			            mEditor.putString("saved-pwd", editPWD.getText().toString());
			            mEditor.putString(editSSID.getText().toString(), editPWD.getText().toString());
			            mEditor.commit();
			     }
			    });
			builder.setCancelable(false);
			builder.create().show();
       
	}
	
	private void restoreLastWifiAP() {
    	    if(mWifiManager == null) {
    	        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
    	    }
    	    if(!mWifiManager.isWifiEnabled()) {
    	        return;
    	    }
    	    
    	    mPref = PreferenceManager.getDefaultSharedPreferences(this);
    	    String storedSsid = mPref.getString("saved-ssid", null);
    	    if(storedSsid == null)
    	        return;
    	    
    	    String ssid = mWifiManager.getConnectionInfo().getSSID();
    	    if(ssid != null && ssid.equals(storedSsid)) {// current is the stored wifi ap, no need to change
    	        return;
    	    }
    	    
    	    //connect to the last wifi ap
    	    List<WifiConfiguration> existingConfig = mWifiManager.getConfiguredNetworks();
    	    for(WifiConfiguration config : existingConfig) {
    	        if(storedSsid.equals(config.SSID)) {// found the last wifi config
    	            Log.d(tag, "Connect to the last wifi ap, ssid: " + storedSsid);
    	            int netID = mWifiManager.addNetwork(config);
    	            boolean ret = mWifiManager.enableNetwork(netID, true);
    	            return;
    	        }
    	    }
	}


    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        Log.i(tag,"onResume");
    }


    @Override
    protected void onDestroy() {
    	destroyConnectDialog();
    	if(mScanner!=null)
    	mScanner.pause();
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        if (mUIStatus > 1) {
            mUIStatus=1;
            setUiVisibility(mUIStatus);
        } else if (mUIStatus == 1) {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        getActionBar().setDisplayHomeAsUpEnabled(true);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private class Scanner extends Handler {
        private int mRetry = 0;

        void resume() {
            if (!hasMessages(0)) {
                sendEmptyMessage(0);
            }
        }

        void forceScan() {
            removeMessages(0);
            sendEmptyMessage(0);
        }

        void pause() {
            mRetry = 0;
            removeMessages(0);
        }

        @Override
        public void handleMessage(Message message) {
            Log.v(tag, "-----Scanner-handleMessage");
            if (mWifiManager.startScan()) {
                mRetry = 0;
            } else if (++mRetry >= 3) {
                mRetry = 0;
                Toast.makeText(ConnectActivity.this, R.string.wifi_fail_to_scan,
                        Toast.LENGTH_LONG).show();
                return;
            }
            sendEmptyMessageDelayed(0, 6 * 1000);
        }
    }

    private void handleEvent(Context context, Intent intent) {
        String action = intent.getAction();
        if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)
                || ConnectivityManager.CONNECTIVITY_ACTION.equals(action)
                ||WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(action)) {
            int wifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, 0);
            if (isSelectedConnected()) {
				 if(mUIStatus==4)
				 {
				    Log.i(tag, "-----------------handleEvent discard mUIStatus:"+mUIStatus);
				    return;
				 }
                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;//STAT_YES_NET_NO_PIN;
                Log.i(tag, "-----------------handleEvent ok");
                mCamera.IP = getIp();
                mCamera.apIP = getApIp();
				 Log.i(tag,"IP:"+mCamera.IP+" <---->apIP:"+mCamera.apIP);
                Editor mEditor = mPref.edit();
                mEditor.putString("wifi", mSelectedResult.SSID);
                mEditor.commit();
                setUiVisibility(3);
            } else {
               // mCamera.mNetStatus = InfotmCamera.STAT_NO_NET;
            }
        } else if (WifiManager.SCAN_RESULTS_AVAILABLE_ACTION.equals(action)) {
            if (mUIStatus == 1){
                updateAccessPoints();
            }
        }
    }
    
    private boolean isSelectedConnected() {
        NetworkInfo wifiNetInfo = mConnectMgr.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (null != mSelectedResult) {
            if (wifiNetInfo.isConnected() && mWifiManager.getConnectionInfo().getSSID().indexOf(mSelectedResult.SSID) >= 0) {
                Log.i(tag, "SelectResult , SSID:" + mSelectedResult.SSID + "<--->wifi:" + mWifiManager.getConnectionInfo().getSSID());
                InfotmCamera.currentSSID=mSelectedResult.SSID;
                return true;
            }
            Log.i(tag, "1SSID:" + mSelectedResult.SSID + "wifi:"+ mWifiManager.getConnectionInfo().getSSID());
        }

        return false;
    }
    
    public String getIp(){
        if (mWifiManager.isWifiEnabled()){
            if (null != mWifiManager.getConnectionInfo()){
                int i = mWifiManager.getConnectionInfo().getIpAddress();
                String ip = (i & 0xFF) + "." + ((i >> 8) & 0xFF) + "." + ((i >> 16) & 0xFF) + "." + (i >> 24 & 0xFF) ;
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

    private void updateAccessPoints() {
        List<ScanResult> list = mWifiManager.getScanResults();
        if (null == list || list.size() == 0) {
            Log.e(tag, "------problem");
        } else {
            Log.v(tag, "------updateAccessPoints total:" + list.size());
            mListView.setOnItemClickListener(null);
            mListData.clear();
            for (int i = 0; i < list.size(); i++) {
            	if(list.get(i).SSID.indexOf("INFOTM_cardv") < 0)
            	    continue;
                mListData.add(list.get(i));
            }
            
            Collections.sort(mListData, new Comparator<ScanResult>() {
                @Override
                public int compare(ScanResult o1, ScanResult o2) {
                    int lv1 = mWifiManager.calculateSignalLevel(o1.level, 4);
                    int lv2 = mWifiManager.calculateSignalLevel(o2.level, 4);
                    return lv2 - lv1;
                }
            });
            if(mListData.size()>=1){
            	destroySerchDeviceDialog();
            }
            mWifiAdapter.notifyDataSetChanged();
            mListView.setOnItemClickListener(mOnItemClickListener);
        }
    }
    
    private boolean checkCnResult(){
        List<WifiConfiguration> cfigs = mWifiManager.getConfiguredNetworks();
        for (WifiConfiguration cfig : cfigs){
            Log.v(tag, "---------checkCnResult()-cfg-bssid:"+cfig.BSSID + " sele:"+mSelectedResult.BSSID);
            Log.v(tag, "------:cfig-status:"+cfig.status);
            if (null!= mSelectedResult && mSelectedResult.BSSID.equals(cfig.BSSID) && 
                    cfig.status == WifiConfiguration.Status.DISABLED){
                return true;
            }
        }
        return false;
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
            case 1:
                return new AlertDialog.Builder(ConnectActivity.this)
                        .setMessage("ddddddddddd")
                        .create();
            default:
                break;
        }
        return super.onCreateDialog(id);
    }
    Dialog mConnectionDialog;
    public void showConnectDialog(){
    	if(null!=mScanner){
    		mScanner.pause();
    	}
    	mConnectionDialog=new AlertDialog.Builder(this).create();
    	mConnectionDialog.show();
    	Window window = mConnectionDialog.getWindow();  
    	window.setContentView(R.layout.device_connect_dialog_layout);  
    	ImageView mWelImageView = (ImageView)window.findViewById(R.id.device_connect_anim);
    	AnimationDrawable anim =(AnimationDrawable)mWelImageView.getBackground();
    	anim.start();
    	if(isForward==1){
    		mHandler.sendEmptyMessageDelayed(1,30*1000);
    	}else{
    		mHandler.sendEmptyMessageDelayed(1,60*1000);
    	}

    }
    
    public void destroyConnectDialog(){
    	if(mConnectionDialog!=null&&mConnectionDialog.isShowing()){
    		mConnectionDialog.dismiss();
    	}
    }
    
    Dialog mSerchDeviceDialog;
    public void showSerchDeviceDialog(){
    	if(mSerchDeviceDialog==null){
    			mSerchDeviceDialog=new AlertDialog.Builder(this).create();
    	}
    	mSerchDeviceDialog.show();
    	Window window = mSerchDeviceDialog.getWindow();  
    	window.setContentView(R.layout.device_search_dialog_layout);  
    		
    }
    
    public void destroySerchDeviceDialog(){
    	if(mSerchDeviceDialog!=null){
    		mSerchDeviceDialog.dismiss();
    	}
    }
    
    
    //start add for auto connection

	private void searchLanDevice(){
		Log.d(tag,"searchLanDevice");
		resultList.clear();
		getArpIP();
		
	}
	
	public void getArpIP()
	{
    new Thread(new Runnable() {
            public void run() {
            	Log.d(tag,"getArpIP");
            	ArrayList<String> connectedIP = getConnectedIP();
            	Log.d(tag,"getArpIP size:"+connectedIP.size());
            	for (String ip : connectedIP) {
            		if(isDeviceIP(ip)==true){
            			resultList.add(ip);
            			Log.d(tag,"result:"+ip);
            		}
            	}
            	if(resultList.size()==0)
                {
            		try {
    					Thread.sleep(1000);
    				} catch (InterruptedException e) {
    					// TODO Auto-generated catch block
    					e.printStackTrace();
    				}  
                }
				mConnectHandler.sendEmptyMessage(MSG_REFRESH);
				Log.d(tag,"getArpIP after");
            	}}).start();

	}
	private ArrayList<String> getConnectedIP() {
			Log.i(tag, "getConnectedIP");
			ArrayList<String> connectedIP = new ArrayList<String>();
			try {
			BufferedReader br = new BufferedReader(new FileReader(
			"/proc/net/arp"));
			String line;
			while ((line = br.readLine()) != null) {
			String[] splitted = line.split(" ");
			if (splitted != null && splitted.length >= 4) {
			String ip = splitted[0];
			if(ip.length()>6)
				connectedIP.add(ip);
			}
			}
			} catch (Exception e) {
				e.printStackTrace();
			}
			return connectedIP;
	}
	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		 registerReceiver(mNetReceiver, mFilter);
		bfind=false;
		super.onStart();
	}
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		mHandler.removeMessages(1);
		bfind=true;
    	if(mNetReceiver!=null){
			unregisterReceiver(mNetReceiver);
	}
		super.onStop();
	}
	
    public Boolean isDeviceIP(String ip)
	{
	     Socket client = null;
        try {
            client = new Socket(ip, 8889);
            Log.i(tag, "Client is created! site:" + ip+ " port:" + 8889);
            client.setReuseAddress(true);
			
        } catch (Exception e) {
            e.printStackTrace();
            Log.i(tag, "Client create fail " + ip + " port:" + 8889);
			return false;
        }
        try {
			client.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	  	return true;
	}
    
    private Handler mConnectHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_REFRESH:
                	Log.i(tag,"MSG_REFRESH");
					if(bfind==true)
					{
					   return;
					}
					Log.i(tag,"bfind:"+bfind);
                	if(resultList.size()==0)
                    {
			                    Log.i(tag,"Handler searchLanDevice");
			                    searchLanDevice();
                    }else{
                    	Log.i(tag, "forward connect success! bsendAp:"+bsendAp);

        				if(isForward==0)
        				{
        					if(bsendAp==false)
        				    {
        						String pinString = "222222";
        						sendAp(pinString);
        						break;
        					}
                           
        				}
        				bfind =true;
                        if(deviceList!=null)
                        {
                        	deviceList.clear();
                        }
                        
                        /**
                         *     public String apiVersion="1.0.150815";//"1.0.160318";
								 public int channelNum=0;
								 public String deviceId ="8d9cda22f6e5b239d23e1ec59f40f768";//"55c01bc4d59928d0077ed3c4ef475356";
								 public DeviceType deviceType=DeviceType.IPC;
								 //public String ip="192.168.43.58";//"10.42.0.1";
								 public String netmask="255.255.255.255";
								 public int p2pPort=12349;
								 public String sn="danale_2015_di3";
                         * */
            			for (String ip : resultList){
            				device= new LanDevice();
							device.setAccessAuth("zhutou","wobushi");
							device.setApiVersion(apiVersion);
							device.setChannelNum(channelNum);
							device.setDeviceId(App.DEVICE_UID);
							device.setDeviceType(deviceType);
							device.setIp(ip);
							device.setNetmask(netmask);
							device.setP2pPort(p2pPort);
							device.setSn(sn);
							if(deviceList==null)
						    {
								deviceList=new ArrayList<LanDevice>();
						    }
						    deviceList.add(device);
            			}
						app.setLanDeviceList(deviceList);
						Log.i(tag, getResources().getString(R.string.search_result)+deviceList.size()+getResources().getString(R.string.search_count));
						
						Log.i(tag,"connect success!");
					   
					    destroyConnectDialog();

						LanDevice device = deviceList.get(0);
						InfotmCamera.apIP = device.getIp();
						Editor e = mPref.edit();
						e.putString(mSelectedResult.SSID, mSelectPassWord);
						e.putString("deviceip", device.getIp());
						e.commit();
						Intent intent = new Intent(ConnectActivity.this, MainActivity.class);
						intent.putExtra("isPlatformDevice", false);
						intent.putExtra("deviceIp", device.getIp());
						startActivity(intent);
						mConnectHandler.removeMessages(MSG_REFRESH);
						finish();
						
                    }
					 
					break;
                default:
                    break;
            }
        };
    };
    //end add for auto connection
    
    
    //add for 正向 contection
	private void searchforwardLanDevice(){
		Log.i(tag, "searchforwardLanDevice-------->,isforward:"+isForward);
		Connection.searchLanDevice(0, DeviceType.ALL, this);
	}

	@Override
	public void onFailure(DeviceResult arg0, int arg1) {
		// TODO Auto-generated method stub
		searchforwardLanDevice();
	}

	@Override
	public void onSuccess(DeviceResult result) {
		// TODO Auto-generated method stub

		if (result.getRequestCommand() == DeviceCmd.searchLanDevice){
			SearchLanDeviceResult ret = (SearchLanDeviceResult) result;
			deviceList = ret.getLanDeviceList();
			
			Log.i(tag, "当前获取到"+deviceList.size()+"台设备");
			
			//List<String> deviceNames = new ArrayList<String>();
			//for (LanDevice device: deviceList){
			//	String name = device.getIp()+":"+device.getDeviceId();
			//	deviceNames.add(name);
			//}
			if(deviceList.size()>0){
				Log.i(tag, "---------------------------forward connect success!----------------------------");
				
				

				
				LanDevice device = deviceList.get(0);
				app.setUID(device.getDeviceId());
				if(isForward==0)
				{
				   Log.i(tag,"get UID:"+device.getDeviceId());
				    String pinString = "222222";
					sendAp(pinString);
                   
				}else
				{
				    if(!bfind){
					bfind=true;
					app.setLanDeviceList(deviceList);
					InfotmCamera.apIP = device.getIp();
	                //startService(new Intent(getApplicationContext(), NetService.class));
	                
					Editor e = mPref.edit();
					e.putString(mSelectedResult.SSID, mSelectPassWord);
					e.putString("deviceip", device.getIp());
					e.commit();
					Intent intent = new Intent(ConnectActivity.this, MainActivity.class);
					intent.putExtra("isPlatformDevice", false);
					intent.putExtra("deviceIp", device.getIp());
					startActivity(intent);
					finish();
				}
			   }
			}else{
				searchforwardLanDevice();
			}
		}
	
	}
	
	//add for reconnect device
	public void reconnectDevice(String wifi,String deviceIp){
		Log.d(tag, "reconnect device  is "+wifi+"deviceIp is  "+deviceIp);
	    LanDevice device= new LanDevice();
		device.setAccessAuth("zhutou","wobushi");
		device.setApiVersion(apiVersion);
		device.setChannelNum(channelNum);
		device.setDeviceId(deviceId);
		device.setDeviceType(deviceType);
		device.setIp(deviceIp);
		device.setNetmask(netmask);
		device.setP2pPort(p2pPort);
		device.setSn(sn);
		if(deviceList==null)
	    {
			deviceList=new ArrayList<LanDevice>();
	    }
	    deviceList.add(device);
	
	   app.setLanDeviceList(deviceList);
       Log.i(tag,"reconnect device success!");
		Log.i(tag, "---------------------------reconnect device  success!----------------------------");
		Intent intent = new Intent(ConnectActivity.this, MainActivity.class);
		intent.putExtra("isPlatformDevice", false);
		intent.putExtra("deviceIp", device.getIp());
		startActivity(intent);
		InfotmCamera.apIP = device.getIp();
        //startService(new Intent(getApplicationContext(), NetService.class));
		finish();
	}

}
