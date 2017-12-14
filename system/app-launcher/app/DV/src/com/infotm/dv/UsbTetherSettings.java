package com.infotm.dv;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

import com.danale.video.sdk.device.entity.LanDevice;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.NetService;

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
import android.hardware.usb.UsbManager;
import android.net.ConnectivityManager;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;



public class UsbTetherSettings extends Activity 
{
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
        public String tag ="UsbTetherSettings";
        private SharedPreferences mPref;
	    private final static String ACTION ="android.hardware.usb.action.USB_STATE";  
	    

	    BroadcastReceiver usBroadcastReceiver = new BroadcastReceiver()
	    {  


			@Override
			public void onReceive(Context arg0, Intent arg1) {
				// TODO Auto-generated method stub  
		    	String action = arg1.getAction();  
		    	Toast.makeText(UsbTetherSettings.this,"aciton ="+ action, Toast.LENGTH_SHORT).show();  
		    	Log.i(tag,"USB :"+action);  
		    	if (action.equals(ACTION))
		    	{  
		    	
		    	boolean connected = arg1.getExtras().getBoolean("connected");  
		    	Toast.makeText(UsbTetherSettings.this,"aciton ="+ connected, Toast.LENGTH_SHORT).show();  
		    	readCurtUsbFunction();
			    	if (connected) 
			    	{  
			    		Log.i(tag,"USB Connected!");  
			    		getUsbTether();
			    	} else {  
			    		Log.i(tag,"USB DisConnected!"); 
			    	}  
		    	}  
				
			}  
	    };
	    
	private Button button; 
	
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        app = (App) getApplication();
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        setContentView(R.layout.connect_activity);
       // IntentFilter filter = new IntentFilter();  
       // filter.addAction(ACTION);    
       // registerReceiver(usBroadcastReceiver, filter);  

        
        if(isUsb0()==true)
        {
        	searchLanDevice();
        	showSerchDeviceDialog();
        }else{
        	
        	AlertDialog.Builder builder = new AlertDialog.Builder(UsbTetherSettings.this);
			builder.setTitle(R.string.device_usb_config);
			builder.setPositiveButton(R.string.next, new DialogInterface.OnClickListener() {

			     @Override
			     public void onClick(DialogInterface dialog, int which) {
			    	 	showSerchDeviceDialog();
			    	    Intent intent = new Intent();
				        intent.setClassName("com.android.settings", "com.android.settings.TetherSettings");
				        intent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);
				        startActivity(intent);
				        searchLanDevice();
			     }
			    });
			builder.create().show();
        }
    }
	
    private boolean getUsbTether() {
        //ConnectivityManager cm =(ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        //UsbManager usbManager = (UsbManager)getSystemService(Context.USB_SERVICE); 
        //SystemProperties.set("sys.usb.config", "rndis");  
    	String []mUsbRegexs =getTetherableUsbReg();
    	String []available =getTetherableIface();
    	for (String s : available) {
    		Log.i(tag,"face:"+s);
            for (String regex : mUsbRegexs) {
            	Log.i(tag,"regex:"+regex);
                if (s.matches(regex)) {
                    return true;
                }
            }
        }

        return false;
  }  

    public String [] getTetherableUsbReg()
    {
    	ConnectivityManager conMgr = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);

        // ConnectivityManager类  
      Class<?> conMgrClass = null;  
       // ConnectivityManager类中的字段  
      Field iConMgrField = null;  
       // IConnectivityManager类的引用  
        Object iConMgr = null;  
       // IConnectivityManager类  
        Class<?> iConMgrClass = null;  
       // setMobileDataEnabled方法  
        Method setMobileDataEnabledMethod = null;  
       try {  
            // 取得ConnectivityManager类  
            conMgrClass = Class.forName(conMgr.getClass().getName());  
            // 取得ConnectivityManager类中的对象Mservice  
            iConMgrField = conMgrClass.getDeclaredField("mService");  
            // 设置mService可访问  
            iConMgrField.setAccessible(true);  
            // 取得mService的实例化类IConnectivityManager  
            iConMgr = iConMgrField.get(conMgr);  
            // 取得IConnectivityManager类  
            iConMgrClass = Class.forName(iConMgr.getClass().getName());  
            // 取得IConnectivityManager类中的setMobileDataEnabled(boolean)方法  
            setMobileDataEnabledMethod = iConMgrClass.getDeclaredMethod(  
                    "getTetherableUsbRegexs");  
            // 设置setMobileDataEnabled方法是否可访问  
            setMobileDataEnabledMethod.setAccessible(true);  
            // 调用setMobileDataEnabled方法  
            String []UsbRegexs=(String[])setMobileDataEnabledMethod.invoke(iConMgr);  
           // Log.i(Tag,"usbRegexs:"+UsbRegexs);
            //Toast.makeText(UsbTetherSettings.this,"UsbRegexs ="+ UsbRegexs, Toast.LENGTH_SHORT).show();
            return UsbRegexs;
      } catch (ClassNotFoundException e) {  
            e.printStackTrace();  
      } catch (NoSuchFieldException e) {  
           e.printStackTrace();  
      } catch (SecurityException e) {  
           e.printStackTrace();  
      } catch (NoSuchMethodException e) {  
            e.printStackTrace();  
      } catch (IllegalArgumentException e) {  
            e.printStackTrace();  
      } catch (IllegalAccessException e) {  
           e.printStackTrace();  
      } catch (InvocationTargetException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	  }
	return null; 
    }
    public String [] getTetherableIface()
    {
    	ConnectivityManager conMgr = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);

        // ConnectivityManager类  
      Class<?> conMgrClass = null;  
       // ConnectivityManager类中的字段  
      Field iConMgrField = null;  
       // IConnectivityManager类的引用  
        Object iConMgr = null;  
       // IConnectivityManager类  
        Class<?> iConMgrClass = null;  
       // setMobileDataEnabled方法  
        Method setMobileDataEnabledMethod = null;  
       try {  
            // 取得ConnectivityManager类  
            conMgrClass = Class.forName(conMgr.getClass().getName());  
            // 取得ConnectivityManager类中的对象Mservice  
            iConMgrField = conMgrClass.getDeclaredField("mService");  
            // 设置mService可访问  
            iConMgrField.setAccessible(true);  
            // 取得mService的实例化类IConnectivityManager  
            iConMgr = iConMgrField.get(conMgr);  
            // 取得IConnectivityManager类  
            iConMgrClass = Class.forName(iConMgr.getClass().getName());  
            // 取得IConnectivityManager类中的setMobileDataEnabled(boolean)方法  
            setMobileDataEnabledMethod = iConMgrClass.getDeclaredMethod(  
                    "getTetherableIfaces");  
            // 设置setMobileDataEnabled方法是否可访问  
            setMobileDataEnabledMethod.setAccessible(true);  
            // 调用setMobileDataEnabled方法  
            String []UsbRegexs=(String[])setMobileDataEnabledMethod.invoke(iConMgr);  
           // Log.i(Tag,"usbRegexs:"+UsbRegexs);
            //Toast.makeText(UsbTetherSettings.this,"UsbRegexs ="+ UsbRegexs, Toast.LENGTH_SHORT).show();
            return UsbRegexs;
      } catch (ClassNotFoundException e) {  
            e.printStackTrace();  
      } catch (NoSuchFieldException e) {  
           e.printStackTrace();  
      } catch (SecurityException e) {  
           e.printStackTrace();  
      } catch (NoSuchMethodException e) {  
            e.printStackTrace();  
      } catch (IllegalArgumentException e) {  
            e.printStackTrace();  
      } catch (IllegalAccessException e) {  
           e.printStackTrace();  
      } catch (InvocationTargetException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	  }
	return null; 
    }
    
    private String readCurtUsbFunction() {  
        String curtFunction = "";  
        Runtime runTime = Runtime.getRuntime();  
        List<String> processList = new ArrayList<String>();  
  
        try {  
            Process pros = runTime.exec("getprop persist.sys.usb.config");  
              
            BufferedReader reader = new BufferedReader(new InputStreamReader(pros.getInputStream()));  
            String line = "";  
            while((line = reader.readLine()) != null){  
                processList.add(line);  
            }  
              
            reader.close();  
        } catch (IOException e) {  
            Log.e(tag, "" + e);  
        }  
          
        if(processList == null || processList.isEmpty()) {  
            // 获取失败，反回null，后续将设置usbFuncation为default  
            return curtFunction;  
        }  
          
        curtFunction = processList.get(0);  
        Log.i(tag,"readCurtUsbFunction resault [" + curtFunction + "]");  
  
        return curtFunction;  
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
               	ArrayList<String> connectedIP = getUsbIP();
               	Log.d(tag,"getArpIP size:"+connectedIP.size());
               	for (String ip : connectedIP) {
               		if(isDeviceIP(ip)==true){
               			resultList.add(ip);
               			Log.d(tag,"result:"+ip);
               			break;
               		}
               	}
               	if(resultList.size()==0)
               	{
	               	connectedIP = getConnectedIP();
	               	Log.d(tag,"getArpIP size:"+connectedIP.size());
	               	for (String ip : connectedIP) {
	               		if(isDeviceIP(ip)==true){
	               			resultList.add(ip);
	               			Log.d(tag,"result:"+ip);
	               			break;
	               		}
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
               	}}).start();

   	}
	private boolean isUsb0()
	{
			Log.i(tag, "isUsb0");
			ArrayList<String> connectedIP = new ArrayList<String>();
			BufferedReader br;
			try {
				br = new BufferedReader(new FileReader(
				"/proc/net/arp"));
			    String line;
				while ((line = br.readLine()) != null) 
				{
					if(line.contains("usb0")||line.contains("rndis0"))
					{
					   	return true;
					}
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
			return false;
	}
	
	private ArrayList<String> getUsbIP() {
			Log.i(tag, "getUsbIP");
			ArrayList<String> connectedIP = new ArrayList<String>();
			try {
			BufferedReader br = new BufferedReader(new FileReader(
			"/proc/net/arp"));
			String line;
			while ((line = br.readLine()) != null)
			{
				if(line.contains("usb0")||line.contains("rndis0"))
				{
					String[] splitted = line.split(" ");
					if (splitted != null && splitted.length >= 4)
					{
					String ip = splitted[0];
					if(ip.length()>6)
						connectedIP.add(ip);
					}
					break;
				}
			}
			} catch (Exception e) {
				e.printStackTrace();
			}
			return connectedIP;
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
                		// Toast.makeText(context, getResources().getString(R.string.search_noresult) , Toast.LENGTH_SHORT).show();
						 new Handler().post(new Runnable() {
			                @Override
			                public void run() {
			                    // TODO Auto-generated method stub
			                    Log.i(tag,"Handler searchLanDevice");
			                    //if(mConnectionDialog!=null&&mConnectionDialog.isShowing())
			                    searchLanDevice();
			                }
			            });
                		 
                    }else{
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
						
						if(resultList.size()>=1)
						{  Log.i(tag,"connect success!");
						   bfind =true;
						   //destroyConnectDialog();
							Log.i(tag, "---------------------------reverd connect success!----------------------------");
							InfotmCamera.mNetStatus = InfotmCamera.STAT_YES_NET_YES_PIN;//STAT_YES_NET_NO_PIN;
							LanDevice device = deviceList.get(0);
							InfotmCamera.apIP = device.getIp();
							startService(new Intent(getApplicationContext(), NetService.class));
				        	sendBroadcast(new Intent(NetService.ACTION_START_KEEPALIVE));
			                 Log.i(tag, "-----------------already connect");
			                 getUsbTether();
							//
							Editor e = mPref.edit();
							//e.putString(mSelectedResult.SSID, mSelectPassWord);
							e.putString("deviceip", device.getIp());
							e.commit();
							destroySerchDeviceDialog();
							
							Intent intent = new Intent(UsbTetherSettings.this, MainActivity.class);
							intent.putExtra("isPlatformDevice", false);
							intent.putExtra("deviceIp", device.getIp());
							startActivity(intent);
							finish();
						}
                    }
					 
					break;
                default:
                    break;
            }
        };
    };
    //end add for auto connection
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
    
}