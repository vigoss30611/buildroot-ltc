package com.infotm.dv;

import java.util.List;

import android.app.Application;

import com.baidu.mapapi.SDKInitializer;
import com.danale.video.sdk.Danale;
import com.danale.video.sdk.device.entity.LanDevice;
//import com.danale.video.sdk.platform.constant.ApiType;
import com.danale.video.sdk.platform.entity.Device;

public class App extends Application {
	public static String DEVICE_UID="2a5face6affbea0d5408f3d351aa356a";//"8d9cda22f6e5b239d23e1ec59f40f768";

	@Override
	public void onCreate() {
		super.onCreate();
		
		// ��������ʱ����Ҫ��Danale SDK���г�ʼ����ÿ������ֻ��Ҫ��ʼ��
		Danale.initialize(getApplicationContext(),"ApiType.VIDEO");
		SDKInitializer.initialize(this);
	}
	
	@Override
	protected void finalize() throws Throwable {
		super.finalize();
	}
	
	private List<Device> platformDevices;
	private List<LanDevice> lanDevices;
	
	public void setPlatformDeviceList(List<Device> list){
		this.platformDevices = list;
	}
	
	public Device getPlatformDeviceById(String deviceId){
		for (Device device: platformDevices){
			if (device.getDeviceId().equals(deviceId)){
				return device;
			}
		}
		return null;
	}
	
	public void setLanDeviceList(List<LanDevice> list){
		this.lanDevices = list;
	}
	
	public LanDevice getLanDeviceById(String deviceId){
	    if(lanDevices == null)
	        return null;
	    
		for (LanDevice device: lanDevices){
		    String id = device.getDeviceId();
			if (id != null && id.equals(deviceId)){
				return device;
			}
		}
		return null;
	}
	public LanDevice getLanDeviceByIp(String deviceip){
	    if(lanDevices == null)
	        return null;
	    
		for (LanDevice device: lanDevices){
		    String id = device.getIp();
			if (id != null && id.equals(deviceip)){
				return device;
			}
		}
		return null;
	}
	public int setUID(String UID)
	{
	   DEVICE_UID = UID;
	   return 0;
	}
}
