package com.infotm.dv.adapter;

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

import com.danale.video.sdk.platform.constant.OnlineType;
import com.danale.video.sdk.platform.entity.Device;
import com.infotm.dv.R;

import java.util.ArrayList;
import java.util.List;

public class DeviceAdapter extends BaseAdapter {
    List<Device> mDeviceList;
    Context mContext;
    LayoutInflater mInflator;
    
    public DeviceAdapter(List<Device> as, Context c){
    	mDeviceList = as;
        mContext = c;
        mInflator = ((LayoutInflater) c.getSystemService("layout_inflater"));
    }
    
    @Override
    public int getCount() {
        return mDeviceList.size();
    }

    @Override
    public Object getItem(int position) {
        return mDeviceList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        convertView = mInflator.inflate(R.layout.device_item, parent, false);
        TextView mTextView = (TextView) convertView.findViewById(R.id.device_name);
        Device device = mDeviceList.get(position);
        mTextView.setText(device.getAlias());
        ImageView imgV = (ImageView) convertView.findViewById(R.id.device_lev);
        if(device.getOnlineType() == OnlineType.ONLINE)
        {
        	 imgV.setImageResource(R.drawable.danale_cloud_device_list_online);
        }
        else
        {
        	imgV.setImageResource(R.drawable.danale_cloud_device_list_offline);
        }
        return convertView;
    }
    
}