package com.infotm.dv.connect;

import android.util.Log;

import com.infotm.dv.camera.InfotmCamera;

import java.io.*;
import java.net.*;

public class SocketClient {
    private final String tag = "InfotmSocketClient";
    static Socket client = null;
    public boolean connected = false;
    private static SocketClient mSocketClient;

    /*
    public SocketClient(String site, int port) {
        try {
//            client = new Socket(site, port);
            Log.v(tag, "--------InetAddress:"+InetAddress.getByName(InfotmCamera.IP));
            client = new Socket(InfotmCamera.apIP, 8889);
//                    InetAddress.getByName(InfotmCamera.getInstance().IP), 17000);
//            client.setKeepAlive(true);
//            client = new Socket("192.168.0.113", 20000);
            Log.v(tag, "Client is created! site:" + site + " port:" + port);
            client.setReuseAddress(true);
            connected = true;
        } catch (UnknownHostException e) {
            e.printStackTrace();
            Log.e(tag, "------create fail:"+site +" port"+ port);
        } catch(ConnectException e){
            e.printStackTrace();
            Log.e(tag, "------create fail:"+site +" port"+ port);
        } catch (IOException e) {
            e.printStackTrace();
            Log.e(tag, "------create fail:"+site +" port"+ port);
        }
    }*/
    
    public SocketClient(){
        try {
            client = new Socket(InfotmCamera.apIP, 8889);
            Log.v(tag, "Client is created! site:" + InfotmCamera.apIP + " port:" + 8889);
            client.setReuseAddress(true);
            connected = true;
        } catch (Exception e) {
            e.printStackTrace();
            Log.v(tag, "Client create fail " + InfotmCamera.apIP + " port:" + 8889);
        }
    }
    /*
    public void sendMsgUDP(String str){
        byte data[] = str.getBytes();
        try {
            DatagramSocket socket = new DatagramSocket(4567);
            socket.setReuseAddress(true);
            InetAddress serverAddress = InetAddress.getByName(InfotmCamera.apIP);
            DatagramPacket packet = new DatagramPacket(data,data.length,serverAddress,4567);
            socket.send(packet);
            socket.close();
            Log.v(tag, "------sendUDP---end");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }*/
    
//    public static SocketClient getInstance(){
//        if (null == mSocketClient){
//            return new SocketClient("192.168.0.113", 20000);
//        }else {
//            return mSocketClient;
//        }
//    }

    public String sendMsg(String msg,int bufSize) {
        if (!connected){
            return "";
        }
        if (msg.contains("keepalive")){
            Log.v(tag, ">>>>>>>>>>sendMsg>>>>>>>>>>:keepalive\n");
        } else {
            Log.v(tag, ">>>>>>>>>>sendMsg>>>>>>>>>>:\n"+msg);
        }
        
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
            PrintWriter out = new PrintWriter(client.getOutputStream());
            client.setSoTimeout(2000);
//            client.setKeepAlive(true);//长连接使用
            out.println(msg);
            out.flush();
            //等待信息回执
//            String readString = in.readLine();
            
            char[] buff = new char[bufSize];
            StringBuilder builder = new StringBuilder("");
            int length = 0;
            do {
                length = in.read(buff);
                if(length > 0) {
                    builder.append(buff, 0, length);
                }
                //Log.d(tag, "length: " + length + ", buf: " + builder.toString());
                if(builder.toString().endsWith("\r\nend"))
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

    public void closeSocket() {
        if (!connected){
            return;
        }
        try {
            client.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) throws Exception {

    }

}
