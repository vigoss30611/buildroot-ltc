
package com.infotm.dv;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
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
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ImageView;
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
import com.infotm.dv.wifiadapter.WifiAdapter;
import com.infotm.dv.wifiadapter.WifiAdmin;
import com.infotm.dv.wifiadapter.WifiApMan;

public class SSIDSettingActivity extends Activity implements DeviceResultHandler{
    private static final String tag = "ConnectActivity";
	private ArrayList<String> resultList = new ArrayList<String>();
    private Boolean bfind=false;
    private List<LanDevice> deviceList;
	private ListView mListView;
    private int mUIStatus = 1;
    private WifiManager mWifiManager;
    private ConnectivityManager mConnectMgr;
    private  BroadcastReceiver mNetReceiver;
    private  ArrayList<ScanResult> mListData = new ArrayList<ScanResult>();
    private WifiAdapter mWifiAdapter;
    private  Scanner mScanner;
    private  ScanResult mSelectedResult;
    private  InfotmCamera mCamera;
    private  SharedPreferences mPref;
    private   String setName;
    private  String setPassWord;
    private   String mSelectPassWord;
    private  Handler mHandler;


    private AdapterView.OnItemClickListener mOnItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
            Log.d(tag, "item choose " + arg2);
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
        		    Intent intent = new Intent(SSIDSettingActivity.this, PassworldInputDialogActivity.class);
        		   startActivityForResult(intent, 2);
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
										        setContentView(R.layout.connect_activity);
										        mCamera = InfotmCamera.getInstance();
										        mListView = (ListView) findViewById(R.id.list1);
										        mPref = PreferenceManager.getDefaultSharedPreferences(this);
										        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
										        mConnectMgr = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
										        mScanner = new Scanner();
										        showSerchDeviceDialog();
										        setUiVisibility(1);
										        IntentFilter mFilter = new IntentFilter();
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
										        registerReceiver(mNetReceiver, mFilter);;
										        
										        mHandler=new Handler(){
										            public void handleMessage(android.os.Message msg) {
										                    switch (msg.what) {
										        			case 1:
										        				Editor e=mPref.edit();
										        				e.remove(mSelectedResult.SSID);
										        				e.commit();
										        				destroyConnectDialog();
										        				Toast.makeText(SSIDSettingActivity.this, "连接失败，请重试！", 2000).show();
										        				mScanner.resume();
										        				break;
										
										        			default:
										        				break;
										        			}
										
										                };
										            };
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
														                new Thread(new Runnable() {
																			@Override
																			public void run() {
																                String passWord = mSelectPassWord;//"admin888";//wifiPassWord.getText().toString();
																                WifiAdmin mWifiAdmin = new WifiAdmin(SSIDSettingActivity.this);
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
																                WifiConfiguration wcg = mWifiAdmin.CreateWifiInfo(mSelectedResult,
																                        passWord, type);
																                mWifiAdmin.addNetwork(wcg);
																			}
																		}).start();
														                showConnectDialog();
														            }
														        } else if (mUIStatus == 3) {// pin ui
														            			mScanner.pause();
														                    	searchforwardLanDevice();//正向
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
    
													    public void initWifiList() {
													        stopAp();
													        mScanner.resume();
													        mWifiAdapter = new WifiAdapter(mListData, this, mWifiManager);
													        mListView.setAdapter(mWifiAdapter);
													    }

																	    @Override
																	    protected void onDestroy() {
																	    	if(mScanner!=null)
																	    	mScanner.pause();
																	    	destroyConnectDialog();
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
												                Toast.makeText(SSIDSettingActivity.this, R.string.wifi_fail_to_scan,
												                        Toast.LENGTH_LONG).show();
												                return;
												            }
												            sendEmptyMessageDelayed(0, 10 * 1000);
												        }
												    }

										    private void handleEvent(Context context, Intent intent) {
										        String action = intent.getAction();
										        if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)
										                || ConnectivityManager.CONNECTIVITY_ACTION.equals(action)
										                ||WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(action)) {
										            //int wifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, 0);
										            if (isSelectedConnected()) {
										                mCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;//STAT_YES_NET_NO_PIN;
										                
										                Log.i(tag, "-----------------handleEvent ok");
										                mCamera.IP = getIp();
										                mCamera.apIP = getApIp();
														Log.i(tag,"IP:"+mCamera.IP+" apIP:"+mCamera.apIP);
										                Editor mEditor = mPref.edit();
										                mEditor.putString("wifi", mSelectedResult.SSID);
										                mEditor.commit();
										                setUiVisibility(3);
										            } else {
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
										                Log.i(tag, "SelectResult , SSID:" + mSelectedResult.SSID + "wifi:" + mWifiManager.getConnectionInfo().getSSID());
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

								    boolean isSetSsidDialogShow=false;
								    public void showSetSsidDialog(){
								    	if(!isSetSsidDialogShow){
								    		isSetSsidDialogShow=true;
										Intent intent = new Intent(SSIDSettingActivity.this, SSIDSettingDialogActivity.class);
										startActivityForResult(intent, 1);
								    	}
								     	if(null!=mScanner){
								    		mScanner.pause();
								    	}
								}
    
								    @Override
								    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
								    	// TODO Auto-generated method stub
								    	
								    	switch (requestCode) {
										case 1:
											isSetSsidDialogShow=false;
											if(data!=null){
											setName =data.getStringExtra("name");
											setPassWord =data.getStringExtra("password");
											setAP();
											}
											break;
										case 2:
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
    


								@Override
								protected void onStart() {
									// TODO Auto-generated method stub
									bfind=false;
									super.onStart();
								}
								
								
								@Override
								protected void onStop() {
									// TODO Auto-generated method stub
									bfind=true;
							    	if(mNetReceiver!=null){
										unregisterReceiver(mNetReceiver);
								}
							    	stopAp();
									super.onStop();
								}
	

                      
								private void searchforwardLanDevice(){
									Log.i(tag, "searchforwardLanDevice-------->");
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
																if(deviceList.size()>0){
																	    mHandler.removeMessages(1);
																	   destroyConnectDialog();
																		showSetSsidDialog();
																}else{
																	searchforwardLanDevice();
																}
															}
														}
									    public void setAP()
										{
									    	String SSID=mSelectedResult.SSID;
									    	String pwd="admin888";
											String KEYMGMT="WPA-PSK";
									    	if(setName!=null&&!"".equals(setName)&&setPassWord!=null&&!"".equals(setPassWord)){
									    		SSID="INFOTM_cardv-"+setName;
									    		pwd=setPassWord;
									    	}
											  //mPref = PreferenceManager.getDefaultSharedPreferences(this);
									    	  //  String storedSsid = mPref.getString("wifi", null);
									        final String string = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
									                mCamera.getNextCmdIndex(), "action", 3, "action.wifi.set:"+"AP;;"+SSID+";;"+pwd+";;"+KEYMGMT+";;");
									        final String  saveSSID=SSID;
									        final String  savePwd=pwd;
									        mCamera.sendCommand(new InfotmCamera.OperationResponse() {
									            @Override
									            public boolean execute() {
									                return mCamera.mCommandSender.sendCommand(string);
									            }
            
						            @Override
						            public void onSuccess() {
						            	destroySsidSettingDialog();
										Toast.makeText(SSIDSettingActivity.this, "设置成功!", 2000).show();
										Editor e=mPref.edit();
										e.remove(mSelectedResult.SSID);
										e.commit();
										Editor e2=mPref.edit();
										e2.putString(saveSSID, savePwd);
										e2.commit();
										finish();
						            }
						            @Override
						            public void onFail() {
						            	destroySsidSettingDialog();
						            	Toast.makeText(SSIDSettingActivity.this, "设置失败!", 2000).show();
						             	if(null!=mScanner){
						             		mScanner.resume();
						            	}
						            }
						        },mHandler);
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
					    	mHandler.sendEmptyMessageDelayed(1,15*1000);
					
					    }
					    
					    public void destroyConnectDialog(){
					    	if(mConnectionDialog!=null&&mConnectionDialog.isShowing()){
					    		mConnectionDialog.dismiss();
					    	}
					    }
					    
					    //search dialog
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

				    //setting dialog
				    Dialog mSsidSettingDialog;
				    public void showSsidSettingDialog(){
				    	if(mSsidSettingDialog==null){
				    		mSsidSettingDialog=new AlertDialog.Builder(this).create();
				    	}
				    	mSsidSettingDialog.show();
				    	Window window = mSsidSettingDialog.getWindow();  
				    	window.setContentView(R.layout.device_ssid_setting_layout);  
				    		
				    }

				    public void destroySsidSettingDialog(){
				    	if(mSsidSettingDialog!=null){
				    		mSsidSettingDialog.dismiss();
				    	}
				    }
}
