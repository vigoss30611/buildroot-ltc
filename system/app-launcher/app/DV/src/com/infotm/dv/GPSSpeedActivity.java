package com.infotm.dv;

import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import com.infotm.dv.view.PanelView;

import android.app.Activity;
import android.content.Context;
import android.location.GpsSatellite;
import android.location.GpsStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

public class GPSSpeedActivity extends Activity {
	private static final String TAG="GPSTEST";
	private TextView mGPSTime,mLatitude,mLongitude;
	private List numSatelliteList=new ArrayList<GpsSatellite>();
	private int mSatelliteNum;
	private  Location location;
	private PanelView mPanelView;
	//获取定位服务
	private LocationManager mLocationManager;
	
	private final GpsStatus.Listener statusListener = new GpsStatus.Listener()
	{
	    public void onGpsStatusChanged(int event){
	    		// GPS状态变化时的回调，获取当前状态
	    	GpsStatus status = mLocationManager.getGpsStatus(null);
	    	//自己编写的方法，获取卫星状态相关数据
	       GetGPSStatus(event, status);
	    }
	};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.gpsspeed_layout);

        mGPSTime=(TextView) findViewById(R.id.gps_time);
        mLatitude=(TextView) findViewById(R.id.gps_latitude);
        mLongitude=(TextView) findViewById(R.id.gps_longitude);
        mPanelView=(PanelView) findViewById(R.id.gps_panel);
      //获取定位服务
        mLocationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
      //判断是否已经打开GPS模块
        if(mLocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)){
        	//GPS模块打开，可以定位操作   

        }else{
        	Toast.makeText(this, "GPS not open!!!", 2000).show();
        }
        
        
        
        // 通过GPS定位
       location = mLocationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
       
        // 设置监听器，设置自动更新间隔这里设置1000ms，移动距离：0米。
        mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 0, locationListener);
        // 设置状态监听回调函数。statusListener是监听的回调函数。
        mGPSTime.setText("时间:"+DateFormat.format("yyyy-MM-dd hh:mm:ss", new Date()));
        mLocationManager.addGpsStatusListener(statusListener); 
        if(location!=null){
        mLatitude.setText("纬度:"+location.getLatitude());
        mLongitude.setText("经度:"+location.getLongitude());
        mPanelView.setPercent(0);
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
    
    
    private void GetGPSStatus(int event, GpsStatus status)
    {
        Log.d(TAG, "enter the updateGpsStatus()");
        if (status == null)
        {
        	return;
        } else if (event == GpsStatus.GPS_EVENT_SATELLITE_STATUS)
        {
            //获取最大的卫星数（这个只是一个预设值）
            int maxSatellites = status.getMaxSatellites();
            Iterator<GpsSatellite> it = status.getSatellites().iterator();
            numSatelliteList.clear();
            //记录实际的卫星数目
            int count = 0;
            while (it.hasNext() && count <= maxSatellites)
            {
                //保存卫星的数据到一个队列，用于刷新界面
                GpsSatellite s = it.next();
                numSatelliteList.add(s);
                count++;

          
            }
            mSatelliteNum = numSatelliteList.size();
            Log.d(TAG, "updateGpsStatus----count="+mSatelliteNum);
         }
         else if(event==GpsStatus.GPS_EVENT_STARTED)
         { 
             //定位启动  
         }
         else if(event==GpsStatus.GPS_EVENT_STOPPED)
         { 
             //定位结束  
         }
    }
    
    
    private final LocationListener locationListener = new LocationListener()
    {
            public void onLocationChanged(Location location){
                //当坐标改变时触发此函数，如果Provider传进相同的坐标，它就不会被触发 
        		double lat = location.getLatitude(); 
        		double lng = location.getLongitude(); 
        		double direction = location.getBearing();
        		double height = location.getAltitude();
               // Log.d(TAG, "LocationListener  onLocationChanged");
                mLatitude.setText("纬度:"+location.getLatitude());
                mLongitude.setText("经度:"+location.getLongitude());
                mGPSTime.setText("时间:"+DateFormat.format("yyyy-MM-dd hh:mm:ss", new Date()));
                mPanelView.setPercent((int)(location.getSpeed()*3.6));
                mPanelView.setText((int)(location.getSpeed()*3.6)+"Km/H");
            }
            public void onProviderDisabled(String provider){
                //Provider被disable时触发此函数，比如GPS被关闭
                Log.d(TAG, "LocationListener  onProviderDisabled");
            }
            public void onProviderEnabled(String provider){
                // Provider被enable时触发此函数，比如GPS被打开
                Log.d(TAG, "LocationListener  onProviderEnabled");
            }
            public void onStatusChanged(String provider, int status, Bundle extras){
                Log.d(TAG, "LocationListener  onStatusChanged");
                // Provider的转态在可用、暂时不可用和无服务三个状态直接切换时触发此函数
                if (status == LocationProvider.OUT_OF_SERVICE || status == LocationProvider.TEMPORARILY_UNAVAILABLE){
                	
                }
            }
        };
    
}
