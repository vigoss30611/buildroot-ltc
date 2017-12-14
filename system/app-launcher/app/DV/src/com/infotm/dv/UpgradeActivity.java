package com.infotm.dv;



import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.Socket;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Vector;

import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.connect.CommandService;
import com.infotm.dv.connect.NetService;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;

public class UpgradeActivity extends BaseActivity {
	private static Socket client = null;
    private int upgrade=0;
    private int upgradeState=0;
    public static String upgradeIP = "";
	private HorizontalProgressBarWithNumber mProgressBar;
	private static final int MSG_PROGRESS_UPDATE = 0x110;
	private static final int MSG_VERSION = 0x111;
	private static final int MSG_UPGRADE = 0x112;
	private ArrayList<String> resultList = new ArrayList<String>();
    private String tag="UpgradeActivity";
    private String cameraVersion=null;
    private String upgradeVersion=null;
    private String upgradeUrl=null;
    private Long    upgradeLen=(long) 0;
    private int upgradeShowSetForHttp=0;
    private int upgradeShowSetForCam=0;
    private TextView txt_upgrade_status=null;
	private Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
            case MSG_PROGRESS_UPDATE:
				int progress = mProgressBar.getProgress();
				if(progress<upgrade)
				mProgressBar.setProgress(++progress);
	
				if (progress >= 100) {
					mHandler.removeMessages(MSG_PROGRESS_UPDATE);
				}
				mHandler.sendEmptyMessageDelayed(MSG_PROGRESS_UPDATE, 2000);
				break;
            case MSG_UPGRADE:
				 if(msg.arg1==R.string.upgrade_image_fail_connet_http)
				 {
					 upgradeShowSetForHttp=1;
				    String text=upgradeUrl;
				    AlertDialog.Builder builder = new AlertDialog.Builder(UpgradeActivity.this);
					builder.setTitle(R.string.upgrade_image_fail_connet_http);
					builder.setMessage(text);
					builder.setPositiveButton(R.string.next, new DialogInterface.OnClickListener() {

					     @Override
					     public void onClick(DialogInterface dialog, int which) {
						        startActivity(new Intent(Settings.ACTION_SETTINGS));
					     }
					    });
					builder.create().show();
				 } else if(msg.arg1==R.string.upgrade_upload_fail_connet_camera)
				 {
					 upgradeShowSetForCam=1;
					    String text=getString(R.string.upgrade_upload_fail_connet_camera);
					    AlertDialog.Builder builder = new AlertDialog.Builder(UpgradeActivity.this);
						builder.setTitle(text);
						builder.setPositiveButton(R.string.next, new DialogInterface.OnClickListener() {

						     @Override
						     public void onClick(DialogInterface dialog, int which) {
						    	 startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
						     }
						    });
						builder.create().show();
				 }
				 else
				 {
					 txt_upgrade_status.setText(msg.arg1);
            	 }
            default:
                break;
            }
		};
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_upgrade);
		txt_upgrade_status = (TextView) findViewById(R.id.txt_upgrade_status);
		mProgressBar = (HorizontalProgressBarWithNumber) findViewById(R.id.id_progressbar01);
		mHandler.sendEmptyMessage(MSG_PROGRESS_UPDATE);
		upgradeState=1;
		upgradeIP=InfotmCamera.apIP;
		Log.i(tag, "onCreate") ;
		upgradeThread();
	}

	@Override
	public void onBackPressed() {
			  super.onBackPressed();
			  Log.i(tag,"onBackPressed");
			  upgradeState=0;
		  }
	
	public Boolean downloadUpgradeFile(URL urls)
	{
		Log.i(tag, "downloadUpgradeFile:" + urls) ;
        upgrade=20;
		try {

			HttpURLConnection urlConnection = (HttpURLConnection) urls.openConnection() ;
			urlConnection.setRequestMethod("GET") ;
			urlConnection.setConnectTimeout(3000) ;
			urlConnection.setReadTimeout(10000) ;
			urlConnection.setUseCaches(false) ;
			urlConnection.setDoInput(true) ;
			urlConnection.setRequestProperty("Accept-Encoding", "identity");
			urlConnection.connect() ;
			
			InputStream inputStream = urlConnection.getInputStream() ;

			String mFileName = urls.getFile().substring(urls.getFile().lastIndexOf(File.separator) + 1) ;
			File appDir =null;
				
			appDir = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm") ;
			

			
			if (!appDir.exists()) {
				appDir.mkdirs() ;
			}
			
			File file = new File(appDir, mFileName) ;

			Log.i(tag, "Path:"+appDir.getPath() + File.separator + mFileName) ;

			if (file.exists()) {
				file.delete() ;
			}

			file.createNewFile() ;
			FileOutputStream fileOutput = new FileOutputStream(file) ;

			byte[] buffer = new byte[1024] ;
			int bufferLength = 0 ;
			    upgradeLen=Long.valueOf(urlConnection.getContentLength());
				Log.i("progress", "Content-Length: " + Long.valueOf(urlConnection.getContentLength()));
				
				while ((bufferLength = inputStream.read(buffer)) > 0) {
					//Log.i(tag, "buffer:"+buffer);
					fileOutput.write(buffer, 0, bufferLength) ;
					
				}
			
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace() ;
			return false ;
		}

		return true ;
	}
	
	public boolean upgradeSocketClient(int port){
	        try {
	            client = new Socket(upgradeIP, port);
	            Log.v(tag, "Client is created! site:" + upgradeIP + " port:" + port);
	            client.setReuseAddress(true);
	        
	        } catch (Exception e) {
	            e.printStackTrace();
	            Log.v(tag, "Client create fail " + upgradeIP + " port:" + port);
	            return false;
	        }
	        return true;
	}
	public byte[] getBytes(int s, boolean bBigEnding) {
		 byte[] buf = new byte[4];
		  
		 if (bBigEnding) {
			  for (int i = buf.length - 1; i >= 0; i--) {
				   buf[i] = (byte) (s & 0x000000ff);
				   s >>= 8;
			  }
		 } else {
			  System.out.println("1");
			  for (int i = 0; i < buf.length; i++) {
				   buf[i] = (byte) (s & 0x000000ff);
				   s >>= 8;
			  }
		 }
		  
		 return buf;
	}
	
	private static int calcCRC(byte[] buf, int offset, int crcLen,int remain)
	{
		int MASK = 0x0001, CRCSEED = 0x0810;
		int start = offset;
		int end = offset + crcLen;
		
		byte val;
		for (int i = start; i < end; i++)
		{
			val = buf[i];
			for (int j = 0; j < 8; j++)
			{
				if (((val ^ remain) & MASK) != 0)
				{
					remain ^= CRCSEED;
					remain >>= 1;
					remain |= 0x8000;
				}
				else
				{
					remain >>= 1;
				}
				val >>= 1;
			}
		}
		return remain;
	}
	
	
    public boolean upgradeFileSocket() {  
    	File file = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator+upgradeVersion) ;
        boolean result = true;  
        FileInputStream reader = null;  
        DataOutputStream out = null;  
        byte[] buf = null;  
  
        try {  
            // 1. 读取文件输入流  
            reader = new FileInputStream(file);  
            int file_length = reader.available();
            Log.i(tag,"file_length:"+file_length);
            if(upgradeLen!=file_length)
            {
            	 Log.i(tag,"error not same upgradeLen:"+upgradeLen);
            	  reader.close();
            	  return false;
            }
            // 2. 将文件内容写到Socket的输出流中  
            out = new DataOutputStream(client.getOutputStream());  
            
            int remain=0;
            int bufferSize = 20480; // 20K  
            buf = new byte[bufferSize];  
            byte[] ret;
            ret=getBytes((int)0x12345678,true);
    		System.arraycopy (ret,0,buf,0,4);
    		ret=getBytes((int)0x1010101,true);
    		System.arraycopy (ret,0,buf,4,4);
    		ret=getBytes((int)0x2,true);
    		System.arraycopy (ret,0,buf,8,4);
    		ret=getBytes((int)0x2,true);
    		System.arraycopy (ret,0,buf,12,4);
    		ret=getBytes((int)file_length,true);
    		System.arraycopy (ret,0,buf,16,4);
    		out.write(buf, 0, 20);  
            Log.i(tag,"after file_length:"+file_length);
            
            int read = 0;  
            // 将文件输入流 循环 读入 Socket的输出流中  
            while ((read = reader.read(buf, 0, buf.length)) != -1) {
            	remain=calcCRC(buf, 0, read,remain);
                out.write(buf, 0, read);  
            }  
            buf[0] = (byte) ((remain >> 8) & 0xff);
            buf[1] = (byte) (remain & 0xff);
            out.write(buf, 0, 2);
            Log.i(tag, "socket执行完成:"+buf[0]+","+buf[1]);  
            out.flush();  
            // 一定要加上这句，否则收不到来自服务器端的消息返回  
            client.shutdownOutput();  
  
        } catch (Exception e) {  
            Log.i(tag, "socket执行异常：" + e.toString());  
            result= false;
        } finally {  
            try {  
                // 结束对象  
                buf = null;  
                out.close();  
                reader.close();  
                client.close(); 
                if (file.exists()) 
                {
    				file.delete() ;
    			}
            } catch (Exception e) {  
  
            }  
        }  
        return result;  
    }  
    public String upgradeProcessSocket() {  
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
            PrintWriter out = new PrintWriter(client.getOutputStream());
            client.setSoTimeout(2000);
//            client.setKeepAlive(true);//长连接使用
            out.println("start");
            out.flush();
            //等待信息回执
//            String readString = in.readLine();
            
            char[] buff = new char[512];
            StringBuilder builder = new StringBuilder("");
            int length = 0;
            do {
                length = in.read(buff);
                if(length > 0) {
                    builder.append(buff, 0, length);
                }
                Log.d(tag, "length: " + length + ", buf: " + builder.toString());
                if(builder.toString().endsWith("upgrade sucess"))
                    break;
            }while(length > 0);
            client.close();
            return builder.toString();
        } catch (IOException e) {
            e.printStackTrace();
            try {
                client.close();
            } catch (Exception e2) {
                e2.printStackTrace();
            }
        }
        
        return "";
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


	private Boolean isDeviceIP(String ip)
	{
     Socket client = null;
    try {
        client = new Socket(ip, upgrade_file_port);
        Log.i(tag, "Client is created! site:" + ip+ " port:" + upgrade_file_port);
        client.setReuseAddress(true);
    } catch (Exception e) {
        e.printStackTrace();
        Log.i(tag, "Client create fail " + ip + " port:" + upgrade_file_port);
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
	private void getArpIP()
   	{ 
               	Log.d(tag,"getArpIP");
               	ArrayList<String> connectedIP = getUsbIP();
               	Log.d(tag,"getArpIP size:"+connectedIP.size());
               	for (String ip : connectedIP) {
               		if(isDeviceIP(ip)==true){
               			resultList.add(ip);
               			Log.d(tag,"result:"+ip);
               			upgradeIP=ip;
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
	               			upgradeIP=ip;
	               			break;
	               		}
	               	}
               	}
 

   	}
    
    private int upgrade_file_port=8890;
    private int upgrade_process_port=8899;
	public boolean sendUpgradeFile()
	{
		boolean ret=true;
		upgrade=40;
		ret=upgradeSocketClient(upgrade_file_port);
		if(ret==true)
		{
			 ret=upgradeFileSocket();
		}
		//upgradeSocketClient(upgrade_process_port);
		//upgradeProcessSocket();
		return ret;
	}
	
	public void  upgradeReconnect()
	{
		mPref = PreferenceManager.getDefaultSharedPreferences(this);
		int isForward=mPref.getInt("connect_mode", 1);
		Intent intent1;
		
		if(isForward==2)
		{
		    intent1 = new Intent(UpgradeActivity.this, LoginActivity.class) ;
		    startActivity(intent1) ;
		}
		else if(isForward==3)
		{
			intent1 = new Intent(UpgradeActivity.this, UsbTetherSettings.class) ;
		    startActivity(intent1) ;
		}
		else
	    {
			 intent1 = new Intent(UpgradeActivity.this, ConnectActivity.class) ;
			 startActivity(intent1) ;
		}
		finish();
		Log.i(tag,"upgradeReconnect");
	}
	public String getUpgradeVersion(URL stSpaceUrl)
	{
		upgradeVersion=null;
		        HttpURLConnection storageconn = null;
				try {
					storageconn = (HttpURLConnection) stSpaceUrl.openConnection();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
					return null;
				}
		        try {
					storageconn.connect();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
					return null;
				}
		        BufferedReader storageReader = null;
				try {
					storageReader = new BufferedReader(new InputStreamReader(storageconn.getInputStream()));
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
					return null;
				}
		        String httpresponse = null;
				try {
					while ((httpresponse = storageReader.readLine()) != null) 
					{
						 if(httpresponse.contains("burn.ius_V"))
						  {
						    Log.i(tag,"httpresponse:"+httpresponse);
							 httpresponse=httpresponse.substring(httpresponse.lastIndexOf("burn.ius_V"),httpresponse.lastIndexOf("<"));
							 if(httpresponse.indexOf("<")>0)
							 {
							 	upgradeVersion=httpresponse.substring(0,httpresponse.indexOf("<")); 
							 }
							 else
							 {
							   upgradeVersion=httpresponse;
							 }
							  break;
						  }
					}
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
					return null;
				}
		   Log.i(tag,"upgradeVersion:"+upgradeVersion);
		   return upgradeVersion;
	}
	public void getCameraVersion()
	{
		upgrade=10;
		cameraVersion=null;
		 mCamera.sendCommand(new InfotmCamera.OperationResponse() {
	            @Override
	            public boolean execute() {
	                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
	                        mCamera.getNextCmdIndex(), "action", 1, 
	                        "status.camera.version"+":"+ 0  +";");
	                 String ret=mCamera.mCommandSender.sendCommandGetString(cmd);
	                 int beginIndex = ret.lastIndexOf(":");
	                 int lastIndex=ret.lastIndexOf(";");
	                 if ((beginIndex > 0)&&(lastIndex>0))
	                 {
	                	 cameraVersion = ret.substring(beginIndex+1, lastIndex);
	                 }
	                return true;
	            }
	            public void onSuccess() {
	            	  Log.i(tag, "getCameraVersion onSuccess:"+cameraVersion);
	            	  upgradeState=3;
	            }
	            
	            public void onFail()
	            {
	            	  Log.i(tag, "getCameraVersion onfail" );
	            	  upgradeState=3;
	            }
	        }, mHandler);      
		     
		     return;
	}
	
	public void getUpgradeUrl()
	{
		upgradeUrl=null;
		 mCamera.sendCommand(new InfotmCamera.OperationResponse() {
	            @Override
	            public boolean execute() {
	                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
	                        mCamera.getNextCmdIndex(), "action", 1, 
	                        "status.camera.upgradeurl"+":"+ 0  +";"); 
	                 String ret=mCamera.mCommandSender.sendCommandGetString(cmd);
	                 int beginIndex = ret.lastIndexOf("upgradeurl:");
	                 int lastIndex=ret.lastIndexOf(";");
	                 if ((beginIndex > 0)&&(lastIndex>0))
	                 {
	                	 upgradeUrl = ret.substring(beginIndex+11, lastIndex);
	                 }
	                return true;
	            }
	            public void onSuccess() {
	            	  Log.i(tag, "getUpgradeUrl onSuccess:"+upgradeUrl);
	            	  upgradeState=5;
	            }
	            
	            public void onFail()
	            {
	            	  Log.i(tag, "getUpgradeUrl onfail" );
	            	  upgradeState=5;
	            }
	        }, mHandler);      
		     
		     return;
		
	}
	public void checkVersion()
	{
		 if(cameraVersion==null)
		 {
			 Message msg = mHandler.obtainMessage();
			 msg.what=MSG_UPGRADE;
			 msg.arg1=R.string.upgrade_check_version_fail;
			 mHandler.sendMessage(msg);
			 upgradeState=0;
			 return;
		 }
		 if(upgradeUrl==null)
		 {
			 Message msg = mHandler.obtainMessage();
			 msg.what=MSG_UPGRADE;
			 msg.arg1=R.string.upgrade_image_URL_fail;
			 mHandler.sendMessage(msg);
			 upgradeState=0;
			 return;
		 }else
		 {
			 Message msg = mHandler.obtainMessage();
			 msg.what=MSG_UPGRADE;
			 msg.arg1=R.string.upgrade_image_verion;
			 mHandler.sendMessage(msg);
		 }
		 try {
			getUpgradeVersion(new URL(upgradeUrl));
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if(upgradeVersion==null)
		{
			 if(upgradeShowSetForHttp==0)
			 {
				 Message msg = mHandler.obtainMessage();
				 msg.what=MSG_UPGRADE;
				 msg.arg1=R.string.upgrade_image_fail_connet_http;
				 mHandler.sendMessage(msg);
			 }
			 upgradeState=5;
			 return;
		}else
		{
			if(upgradeVersion.contains(cameraVersion))
			{
				Message msg = mHandler.obtainMessage();
				 msg.what=MSG_UPGRADE;
    			 msg.arg1=R.string.upgrade_newest;
    			 mHandler.sendMessage(msg);
				 upgradeState=0;
				 return;
			}else
			{
				upgradeState=7;
				return;
			}
		}
	}
	
	public void upgradeThread()
	{
		 new Thread(new Runnable() {
             public void run() {
            	while(upgradeState!=0)
            	{
            	 if(upgradeState==1)
            	 {
            		 upgradeState=2;
            		 getCameraVersion();
            	 }else  if(upgradeState==3)
            	 {
            		 if(cameraVersion==null)
            		 {
            			 Message msg = mHandler.obtainMessage();
            			 msg.what=MSG_UPGRADE;
	        			 msg.arg1=R.string.upgrade_check_version_fail;
	        			 mHandler.sendMessage(msg);
            			 upgradeState=0;
            			 break;
            		 }
            		 upgradeState=4;
            		 getUpgradeUrl();
            	 }else  if(upgradeState==5)
            	 {
            		 upgradeState=6;
            		 checkVersion();
            	 }else  if(upgradeState==7)
            	 {
            		 upgradeState=8;
            		 try {
            			 Message msg = mHandler.obtainMessage();
            			 msg.what=MSG_UPGRADE;
	        			 msg.arg1=R.string.upgrade_downloading;
	        			 mHandler.sendMessage(msg);
	        			 String url=upgradeUrl+upgradeVersion;
	        			 Log.i(tag,"url="+url);
						boolean ret=downloadUpgradeFile(new URL(url));
						if(ret==false)
						{
							 msg = mHandler.obtainMessage();
							 msg.what=MSG_UPGRADE;
		        			 msg.arg1=R.string.upgrade_download_fail;
		        			 mHandler.sendMessage(msg);
							break;
						}else
						{
							upgradeState=9;
						}
					} catch (MalformedURLException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						 Message msg = mHandler.obtainMessage();
						msg.what=MSG_UPGRADE;
	        			 msg.arg1=R.string.upgrade_download_fail;
	        			 mHandler.sendMessage(msg);
						break;
					}
            	 }else  if(upgradeState==9)
            	 {
            		 upgradeState=10;
            		 Message msg = mHandler.obtainMessage();
            		 msg.what=MSG_UPGRADE;
        			 msg.arg1=R.string.upgrade_uploading;
        			 mHandler.sendMessage(msg);
            		 boolean ret=sendUpgradeFile();
            		 if(ret==true)
            		 {
            			 msg = mHandler.obtainMessage();
            			 msg.what=MSG_UPGRADE;
            			 msg.arg1=R.string.upgrade_processing;
            			 mHandler.sendMessage(msg);
            			 upgradeState=11;
            		 }else
            		 {
            			 if(upgradeShowSetForCam==0)
            			 {
	            			 msg = mHandler.obtainMessage();
	            			 msg.what=MSG_UPGRADE;
	            			 msg.arg1=R.string.upgrade_upload_fail_connet_camera;
	            			 mHandler.sendMessage(msg);
            			 }
            			 resultList.clear();
            			 getArpIP();
            			 upgradeState=9;
            		 }
            		 
            	 }else if(upgradeState==11)
            	 {
            		 upgradeState=12;
            		 upgrade=90;
            		 try {
     					Thread.sleep(100*1000);
     					} catch (InterruptedException e) {
     						// TODO Auto-generated catch block
     						e.printStackTrace();
     					}
            		 Message msg = mHandler.obtainMessage();
            		 msg = mHandler.obtainMessage();
        			 msg.what=MSG_UPGRADE;
        			 msg.arg1=R.string.upgrade_ok;
        			 mHandler.sendMessage(msg);
        			 mProgressBar.setProgress(99);
                 	 upgrade=100;
                 	try {
     					Thread.sleep(4*1000);
     					} catch (InterruptedException e) {
     						// TODO Auto-generated catch block
     						e.printStackTrace();
     					}
                 	upgradeReconnect();
            	 }
            	 
            	    try {
					Thread.sleep(100);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
            	}
            	mProgressBar.setProgress(99);
            	upgrade=100;
             	}}).start();

	}
}

