package com.infotm.dv.connect;

import android.util.Log;

import java.io.*;
import java.net.*;

public class SocketServer {
    private final String tag = "CommandReceiver";
    ServerSocket server;
    DataChangeListener mDataChangeListener = null;
    private boolean isOn = false;

    public SocketServer(int port) {
        try {
            //server = new ServerSocket(port);
//            try {
//            sever.setReceiveBufferSize(512*1024);
//            Log.d(tag, "server buffer size: " + sever.getReceiveBufferSize());
//            } catch(SocketException se) {
//                se.printStackTrace();
//            }
            server=new ServerSocket();
            server.setReuseAddress(true);      //设置ServerSocket的选项
            server.bind(new InetSocketAddress(port));   //与8000端口绑定
            Log.v(tag, "server created on port " + port);
        } catch (IOException e) {
            e.printStackTrace();
			Log.i(tag, "server fail created on port " + port);
        }
    }
    
    public void setDataChangeListener(DataChangeListener d){
        mDataChangeListener = d;
    }

    public void beginListen() {
        isOn = true;
        Log.v(tag, "beginListen ");
        while (isOn) {
            try {
                final Socket socket = server.accept();
                socket.setSoTimeout(2000);
                Log.v(tag, "accepted !");
                new Thread(new Runnable() {
                    public void run() {
                        BufferedReader in;
                        try {
                            in = new BufferedReader(new InputStreamReader(socket.getInputStream(),
                                    "UTF-8"));
                            PrintWriter out = new PrintWriter(socket.getOutputStream());

                            char[] buff = new char[1024];
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
                            Log.d(tag, "size: " + builder.length());
                            
                            if (null != mDataChangeListener){
                                mDataChangeListener.onDataChanged(builder.toString());
                            }
                            
                            out.println("from jingbo server");
                            out.flush();
                            socket.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        Log.v(tag, "beginListen done ");
    }
    
    public void closeServer(){
        if (null != server){
            try {
                isOn = false;
                server.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    
    public interface DataChangeListener {
        public void onDataChanged(String ss);
    }
}
