package com.infotm.dv.connect;

import android.R.integer;
import android.util.Log;
import android.util.Pair;
import com.infotm.dv.camera.ICameraCommandSender;
import com.infotm.dv.camera.InfotmCamera;

public class CommandSender implements ICameraCommandSender{//仿照LegacyCameraCommandSender
    private final static String tag = "CommandSender";
    private final String DV_IP = "192.168.0.113";
    private final int sendport = 2000;
    private final InfotmCamera mCamera;
    private String mCameraAddress = null;
    private Pair<Boolean, Integer> RESPONSE_FAILED;
    private Object lock = new Object();
    
    public CommandSender(InfotmCamera infotmCamera){
        mCamera = infotmCamera;
//        mCameraAddress = "http://" + infotmCamera.getIpAddress() + ":" + infotmCamera.getPort();
        RESPONSE_FAILED = new Pair<Boolean, Integer>(true, 1);
    }
    
    public boolean send(){//阻塞式的还是非阻塞？？
        
        return true;
    }

    @Override
    public boolean cancelOta() {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public Pair<Boolean, Number> enterOtaMode() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public boolean sendCommand(String paramString) {
        synchronized (lock) {
            try{
                SocketClient client2 = new SocketClient();
                String ret2 = client2.sendMsg(paramString,256);
                if (null != ret2 && ret2.contains("keepalive")){
                    Log.v(tag, ">>>>>>>>>>sendresult:keepalive" );
                } else {
                    Log.v(tag, ">>>>>>>>>>sendresult:\n" + ret2);
                }
                
                if (null != ret2 && ret2.length() != 0 && ret2.toLowerCase().contains("ok")) {
                    return true;
                }
                return false;
            } catch (Exception e) {
                e.printStackTrace();
            }
            return false;
        }
    }
    
    public String sendCommandGetString(String paramString) {
        synchronized (lock) {
            try{
                SocketClient client2 = new SocketClient();
                String ret2 = client2.sendMsg(paramString,512);
                Log.v(tag, "------sendret~~~~:" + ret2);
                return ret2;
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;
        }
  }

    @Override
    public boolean sendCommand(String paramString, int paramInt) {
        // TODO Auto-generated method stub
        return false;
    }
    
    @Override
    public boolean sendCommand(String paramString1, String paramString2, int flag) {//FLAG_NONE FLAG_TWO_DIGIT FLAG_PREPEND_LENGTH
        synchronized (lock) {
            SocketClient client2 = new SocketClient();
            String ret2 = client2.sendMsg("mmmcommandString",256);
            Log.v(tag, "------sendret2:" + ret2);
            if (null != ret2 && ret2.length() != 0){
                return true;
            }
            return false;
        }
    }

    @Override
    public Pair<Boolean, Number> sendCommandHttpResponse(String paramString, int paramInt) {
        // TODO Auto-generated method stub //my socket response
        
        return null;
    }

    @Override
    public boolean setCameraPower(boolean paramBoolean) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public boolean setWifiConfig(String paramString1, String paramString2) {
        // TODO Auto-generated method stub
        return false;
    }
    
    
    private boolean passFail(byte[] paramArrayOfByte){
        if ((paramArrayOfByte != null) && (paramArrayOfByte.length > 0)
                && (paramArrayOfByte[0] == 0)) {
            return true;
        }
        return false;
    }

    public static String commandStringBuilder(String pin, int index, String type, int number,
            String str) {
        String build = "pin:"+pin + "\r\n"
                + "cmdfrom:phone" + "\r\n" 
                + "cmdseq:"+index + "\r\n"
                + "cmd_type:" + type + "\r\n"
                + "cmd_num:" + number + "\r\n"
                + str + "\r\n\r\n";
        Log.i("commandStr","command:"+build.toString());
        return build;
    }
    
}