package com.infotm.dv.connect;

import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;

import android.util.Log;

import com.infotm.dv.camera.InfotmCamera;
public class streamRecv{

	public boolean isStreamStart=false;
	public String tag="streamRecv";
	public static String apIP = "";
	public static StreamReceiver strReceiverF=null;
	public boolean startrecv(String connectIP,StreamReceiver strReceiver)
    {
		Log.i(tag,"startrecv:"+connectIP);
		apIP=connectIP;
		strReceiverF=strReceiver;
		isStreamStart=true;
		streamThread();
		return true;
    }
	
	public boolean stoprecv()
    {
		Log.i(tag,"stoprecv");
		isStreamStart=false;
		return true;
    }
	
	public static abstract interface StreamReceiver 
	{
	    //ok   nRet=data.length
	    //stop nRet=0
	    //fail and stop nRet=-1
	    public abstract void onStreamReceive(int channel, int format, long timeStamp, boolean isKeyFrame,byte[] data,int nRet);
	}
	
	

	
	public boolean streamSocketCreate(){
        try {
            client = new Socket(apIP, 8891);
            Log.v(tag, "Client is created! site:" + InfotmCamera.apIP + " port:" + 8891);
            client.setReuseAddress(true);
        } catch (Exception e) {
            e.printStackTrace();
            Log.v(tag, "Client create fail " + InfotmCamera.apIP + " port:" + 8891);
            return false;
        }
         return true;
	}
	
	public int byteToInt(byte[] b,int index)
	{

        int targets = (b[index+3] & 0xff) | ((b[index+2] << 8) & 0xff00)  | ((b[index+1] << 24) >>> 8) | (b[index+0] << 24);   
        		return targets;  
	}
	public int byteToShort(byte[] b,int index)
	{
        int targets = (b[index+1] & 0xff) | ((b[index+0] << 8) & 0xff00) ;   
        		return targets;  
	}
private static Socket client = null;



public String streamDeal() {  
	
    String result = null;  
    DataInputStream in = null;
   
    	try {
			in = new DataInputStream(client.getInputStream());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			try {
				client.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			client=null;
			strReceiverF.onStreamReceive(0, 0, 0, false,null,-1);
			return null;
		}  
        try {
			client.setSoTimeout(2000);
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			try {
				client.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			client=null;
			strReceiverF.onStreamReceive(0, 0, 0, false,null,-1);
			return null;
		}

        byte[] buff = new byte[1024*1024];
        int length=0;
        int req_len=0;
        int index=0;
		 int offset=0;
		 int stream_magic=0;
        int stream_ts=0;
        int stream_len=0;
        boolean stream_key=true;
        int stream_codec=0;
        int i=0;
        while(isStreamStart)
        {
        	 req_len=16+offset-index;
        	 try {
				length = in.read(buff,index,req_len);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				Log.i(tag,"IOException length:"+length);
				e.printStackTrace();
				break;
			} 
        	// Log.i(tag,"length:"+length);
             if(length == req_len) {
			 	 stream_magic=byteToInt(buff,offset);
				 if(stream_magic!=0x12345678)
				 {
                   for( i=1;i<16;i++)
				    {
					    if(buff[i+offset]==0x12)
						{
						   offset+=i;
				          index=offset+16-i;
				          Log.v(tag,"magic not right");
						   break;
						}
				   }
				   if(i==16)
				   {
				      offset=0;
					  index=0;
				   }
				   continue;
				 }
            	 stream_key=(byteToShort(buff,offset+4)==1)?true:false;
            	 stream_codec=byteToShort(buff,offset+6);
            	 stream_ts=byteToInt(buff,offset+8);
            	 stream_len=byteToInt(buff,offset+12);
            	 //Log.v(tag,"magic:"+Integer.toHexString(stream_magic)+" key:"+stream_key+" codec:"+stream_codec+" ts:"+stream_ts+" len:"+stream_len);
            	 req_len=stream_len;
            	 index=0;
             }else if(length<0)
             {
            	 break;
             }else
             {
            	 index+=length;
            	 continue;
             }
             
             while(isStreamStart)
             {
             	 req_len=stream_len-index;
             	 try {
					length = in.read(buff,index,req_len);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					Log.i(tag,"stream IOException length:"+length);
					e.printStackTrace();
					break;
				} 
             	// Log.i(tag,"stream length:"+length);
                  if(length == req_len) {
				  	 byte [] dataBuff = new byte[stream_len];
				  	System.arraycopy(buff,0,dataBuff,0,stream_len);
				  	strReceiverF.onStreamReceive(0, stream_codec, stream_ts, stream_key,dataBuff,dataBuff.length);
                	  break;
                  }else if(length<0)
                  {
                 	 break;
                  }else
                  {
                 	 index+=length;
                 	 continue;
                  }
             }
             index=0;
             offset=0;
        }
        buff=null;  

    Log.i(tag,"streamDeal end");
    try {
		client.close();
		client=null;
		if(isStreamStart)
	    {
	       strReceiverF.onStreamReceive(0, 0, 0, false,null,-1);
		}
		else
		{
		    strReceiverF.onStreamReceive(0, 0, 0, false,null,0);
       }
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}  
    return result;  
}  
	
	public boolean streamThread()
	{
		 new Thread(new Runnable() {
             public void run() {
            	 if(client==null)
            	 {
            		boolean ret= streamSocketCreate();
            		if(ret==true)
            		{
            			streamDeal();
            		}else
            		{
            		    strReceiverF.onStreamReceive(0, 0, 0, false,null,-1);
            		}
            	 }
             	}}).start();
        return true;
	}
}