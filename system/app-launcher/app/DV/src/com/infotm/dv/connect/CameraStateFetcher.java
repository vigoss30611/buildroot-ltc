package com.infotm.dv.connect;

import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.camera.ICameraStateFetcher;

import java.util.HashMap;


//copy from gopro LegacyCameraStateFetcher.class
public class CameraStateFetcher implements ICameraStateFetcher{
    InfotmCamera mCamera;
    
    public CameraStateFetcher(InfotmCamera iCamera){
        mCamera = iCamera;
    }
    
    private HashMap<String, String> buildCommandMap() {
        HashMap<String, String> localHashMap = new HashMap<String, String>();
        localHashMap.put("GPCAMERA_SET_DATE_AND_TIME_ID", "camera/TM");
        localHashMap.put("GPCAMERA_SHUTTER", "bacpac/SH");
        localHashMap.put("GPCAMERA_MODE", "camera/CM");
        localHashMap.put("GPCAMERA_BATTERY_LEVEL_ID", "camera/blx");
        localHashMap.put("GPCAMERA_WIFI_BACPAC_BATTERY_LEVEL_ID", "bacpac/blx");
        localHashMap.put("GPCAMERA_DELETE_ALL_FILES_ID", "camera/DA");
        localHashMap.put("GPCAMERA_DELETE_LAST_FILE_ID", "camera/DL");
        localHashMap.put("GPCAMERA_DELETE_FILE_ID", "camera/DF");
        localHashMap.put("GPCAMERA_INFO_NAME_ID", "camera/CN");
        localHashMap.put("GPCAMERA_INFO_VERSION_ID", "camera_version");
        localHashMap.put("GPCAMERA_LOCATE_ID", "camera/LL");
        localHashMap.put("GPCAMERA_VIDEO_PROTUNE_RESET_TO_DEFAULT", "camera/PT");
        localHashMap.put("GPCAMERA_NETWORK_NAME_ID", "bacpac_ssid");
        localHashMap.put("GPCAMERA_NETWORK_VERSION_ID", "bacpac_version");
        localHashMap.put("GPCAMERA_POWER_ID", "bacpac/PW");
        localHashMap.put("GPCAMERA_WIFI_RESET_MODULE", "bacpac/RS");
        localHashMap.put("GPCAMERA_USE_CURRENT_WIRELESS_REMOTE_ID", "bacpac/WI");
        localHashMap.put("GPCAMERA_USE_NEW_WIRELESS_REMOTE_ID", "bacpac/WI");
        localHashMap.put("GPCAMERA_FWUPDATE_DOWNLOAD_DONE", "camera/OF");
        localHashMap.put("GPCAMERA_FWUPDATE_DOWNLOAD_CANCEL", "camera/OM");
        return localHashMap;
    }
    
    private HashMap<String, String> buildSettingsMap(){
        HashMap<String, String> localHashMap = new HashMap<String, String>();
        localHashMap.put("29", "camera/BU");
        localHashMap.put("2", "camera/VV");
        localHashMap.put("30", "camera/TI");
        localHashMap.put("18", "camera/CS");
        localHashMap.put("17", "camera/PR");
        localHashMap.put("20", "camera/EX");
        localHashMap.put("4", "camera/FV");
        localHashMap.put("3", "camera/FS");
        localHashMap.put("6", "camera/LO");
        localHashMap.put("8", "camera/LW");
        localHashMap.put("7", "camera/PN");
        localHashMap.put("10", "camera/PT");
        localHashMap.put("12", "camera/CO");
        localHashMap.put("15", "camera/EV");
        localHashMap.put("13", "camera/GA");
        localHashMap.put("14", "camera/SP");
        localHashMap.put("11", "camera/WB");
        localHashMap.put("57", "camera/VM");
        return localHashMap;
    }
    
    @Override
    public void destroy() {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void fetchState(InfotmCamera iCamera) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public boolean init() {
        //先检查网路
        mCamera.setCommandMap(buildCommandMap());
        mCamera.setSettingMap(buildSettingsMap());
        return false;
    }

    @Override
    public void reset() {
        // TODO Auto-generated method stub
        
    }
    
}