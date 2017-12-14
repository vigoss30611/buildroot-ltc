package com.infotm.dv.wifiadapter;

import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.infotm.dv.R;

import java.util.ArrayList;
import java.util.List;

public class WifiAdapter extends BaseAdapter {
    ArrayList<ScanResult> mWifiList;
    Context mContext;
    LayoutInflater mInflator;
    WifiManager wifiManager;
    
    public WifiAdapter(ArrayList<ScanResult> as, Context c,WifiManager w){
        mWifiList = as;
        mContext = c;
        wifiManager = w;
        mInflator = ((LayoutInflater) c.getSystemService("layout_inflater"));
    }
    
    @Override
    public int getCount() {
        return mWifiList.size();
    }

    @Override
    public Object getItem(int position) {
        return mWifiList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        convertView = mInflator.inflate(R.layout.wifi_item, parent, false);
        TextView mTextView = (TextView) convertView.findViewById(R.id.wifi_itm_name);
        ScanResult scanResult = mWifiList.get(position);
        mTextView.setText(scanResult.SSID);
        ImageView imgV = (ImageView) convertView.findViewById(R.id.wifi_itm_lev);
//        imgV.setImageLevel(scanResult.level/20);
        int lv = wifiManager.calculateSignalLevel(scanResult.level, 4);
        imgV.setImageLevel(lv);//0123
        return convertView;
    }
    
}