package com.infotm.dv.adapter;

import android.content.Context;
import android.content.res.Resources.NotFoundException;
import android.util.Log;

import com.infotm.dv.R;
import com.infotm.dv.camera.InfotmCamera;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringBufferInputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;

public class SettingHelper{
    private static HashMap<String, Integer> mLabelLookup = new HashMap();
    private final InfotmCamera mCamera;
    private final Context mContext;
    static{
      buildLabelLookup();
    }
    
    public SettingHelper(Context context, InfotmCamera pCamera){
      mCamera = pCamera;
      mContext =context;
    }
    
    public static HashMap<String, Integer> buildLabelLookup(){
        mLabelLookup.put("current.mode", Integer.valueOf(R.string.setting_section_video));
        
        mLabelLookup.put("current.reverseimage", Integer.valueOf(R.string.setting_reverseimage));
        
        mLabelLookup.put("video", Integer.valueOf(R.string.setting_section_video));
        mLabelLookup.put("video.mode", Integer.valueOf(R.string.setting_video_mode));
        mLabelLookup.put("video.recordmode", Integer.valueOf(R.string.setting_video_mode));
        mLabelLookup.put("video.preview", Integer.valueOf(R.string.setting_video_preview));
        
        mLabelLookup.put("video.interval", Integer.valueOf(R.string.setting_video_interval));
        mLabelLookup.put("video.resolution", Integer.valueOf(R.string.setting_video_res));
        mLabelLookup.put("video.fps", Integer.valueOf(R.string.setting_video_fps));
        mLabelLookup.put("video.lowlight", Integer.valueOf(R.string.setting_video_lowlight));
        mLabelLookup.put("video.advanced", Integer.valueOf(R.string.setting_video_adv));
        mLabelLookup.put("video.whitebalance", Integer.valueOf(R.string.setting_video_wb));
        mLabelLookup.put("video.color", Integer.valueOf(R.string.setting_video_color));
        mLabelLookup.put("video.isolimit", Integer.valueOf(R.string.setting_video_iso));
        mLabelLookup.put("video.sharpness", Integer.valueOf(R.string.setting_video_sharpness));
        mLabelLookup.put("video.ev", Integer.valueOf(R.string.setting_video_ev));

        mLabelLookup.put("video.looprecording", Integer.valueOf(R.string.setting_video_looprecording));
        mLabelLookup.put("video.timelapsedimage", Integer.valueOf(R.string.setting_video_timelapsedimage));
        mLabelLookup.put("video.highspeed", Integer.valueOf(R.string.setting_video_highspeed));
		mLabelLookup.put("video.rearcamera", Integer.valueOf(R.string.setting_video_rearcamera));
		mLabelLookup.put("video.pip", Integer.valueOf(R.string.setting_video_pip));
		mLabelLookup.put("video.frontbig", Integer.valueOf(R.string.setting_video_frontbig));
		mLabelLookup.put("video.wdr", Integer.valueOf(R.string.setting_video_wdr));
		mLabelLookup.put("video.recordaudio", Integer.valueOf(R.string.setting_video_recordaudio));
		mLabelLookup.put("video.datestamp", Integer.valueOf(R.string.setting_video_datestamp));
		mLabelLookup.put("video.gsensor", Integer.valueOf(R.string.setting_video_gsensor));
		mLabelLookup.put("video.platestamp", Integer.valueOf(R.string.setting_video_platestamp));
      
		
        mLabelLookup.put("car", Integer.valueOf(R.string.setting_section_car));
        mLabelLookup.put("car.mode", Integer.valueOf(R.string.setting_car_mode));
        mLabelLookup.put("car.resolution", Integer.valueOf(R.string.setting_car_res));
        mLabelLookup.put("car.fps", Integer.valueOf(R.string.setting_car_fps));
        mLabelLookup.put("car.lowlight", Integer.valueOf(R.string.setting_car_lowlight));
        mLabelLookup.put("car.advanced", Integer.valueOf(R.string.setting_car_adv));
        mLabelLookup.put("car.whitebalance", Integer.valueOf(R.string.setting_car_wb));
        mLabelLookup.put("car.color", Integer.valueOf(R.string.setting_car_color));
        mLabelLookup.put("car.isolimit", Integer.valueOf(R.string.setting_car_iso));
        mLabelLookup.put("car.sharpness", Integer.valueOf(R.string.setting_car_sharpness));
        mLabelLookup.put("car.ev", Integer.valueOf(R.string.setting_car_ev));
        
        mLabelLookup.put("photo", Integer.valueOf(R.string.setting_section_capture));
        mLabelLookup.put("photo.mode", Integer.valueOf(R.string.setting_photo_mode));
        mLabelLookup.put("photo.preview", Integer.valueOf(R.string.setting_photo_preview));
        mLabelLookup.put("photo.interval", Integer.valueOf(R.string.setting_photo_interval));
        mLabelLookup.put("photo.shutter", Integer.valueOf(R.string.setting_photo_shutter));
        mLabelLookup.put("photo.timer", Integer.valueOf(R.string.setting_photo_timer));
        mLabelLookup.put("photo.modetimer", Integer.valueOf(R.string.setting_photo_timer));
        mLabelLookup.put("photo.modeauto", Integer.valueOf(R.string.setting_photo_modeauto));
        mLabelLookup.put("photo.modesequence", Integer.valueOf(R.string.setting_photo_modesequence));
        
        mLabelLookup.put("photo.megapixels", Integer.valueOf(R.string.setting_photo_mega));
        mLabelLookup.put("photo.lowlight", Integer.valueOf(R.string.setting_photo_lowlight));
        mLabelLookup.put("photo.advanced", Integer.valueOf(R.string.setting_photo_adv));
        mLabelLookup.put("photo.whitebalance", Integer.valueOf(R.string.setting_photo_wb));
        mLabelLookup.put("photo.color", Integer.valueOf(R.string.setting_photo_color));
        mLabelLookup.put("photo.isolimit", Integer.valueOf(R.string.setting_photo_iso));
        mLabelLookup.put("photo.sharpness", Integer.valueOf(R.string.setting_photo_sharpness));
        mLabelLookup.put("photo.ev", Integer.valueOf(R.string.setting_photo_ev));

        mLabelLookup.put("photo.capturemode", Integer.valueOf(R.string.setting_photo_capturemode));
		mLabelLookup.put("photo.resolution", Integer.valueOf(R.string.setting_photo_resolution));
		mLabelLookup.put("photo.sequence", Integer.valueOf(R.string.setting_photo_sequence));
		mLabelLookup.put("photo.quality", Integer.valueOf(R.string.setting_photo_quality));
		mLabelLookup.put("photo.antishaking", Integer.valueOf(R.string.setting_photo_antishaking));
		mLabelLookup.put("photo.datestamp", Integer.valueOf(R.string.setting_photo_datestamp));

		
        mLabelLookup.put("setup", Integer.valueOf(R.string.setting_section_setup));
        mLabelLookup.put("setup.wifi", Integer.valueOf(R.string.setting_setup_wireless));
        mLabelLookup.put("setup.wifimode", Integer.valueOf(R.string.setting_setup_wireless));
        mLabelLookup.put("setup.ledfreq", Integer.valueOf(R.string.setting_setup_ledfreq));
        mLabelLookup.put("setup.led", Integer.valueOf(R.string.setting_setup_led));
        mLabelLookup.put("setup.cardvmode", Integer.valueOf(R.string.setting_setup_cardvmode));
        mLabelLookup.put("setup.autosleep", Integer.valueOf(R.string.setting_setup_dis_sleep));
        mLabelLookup.put("setup.sleep", Integer.valueOf(R.string.setting_setup_dis_sleep));
        mLabelLookup.put("setup.display.sleep", Integer.valueOf(R.string.setting_setup_dis_sleep));
        mLabelLookup.put("setup.display.brightness", Integer.valueOf(R.string.setting_setup_dis_bright));
        mLabelLookup.put("setup.brightness", Integer.valueOf(R.string.setting_setup_dis_bright));
        mLabelLookup.put("setup.orientation", Integer.valueOf(R.string.setting_setup_ori));
        mLabelLookup.put("setup.watermark", Integer.valueOf(R.string.setting_setup_watermark));
        mLabelLookup.put("setup.language", Integer.valueOf(R.string.setting_setup_language));
        mLabelLookup.put("setup.defaultmode", Integer.valueOf(R.string.setting_setup_defaultmode));
        mLabelLookup.put("setup.beepsound", Integer.valueOf(R.string.setting_setup_beeps));
        mLabelLookup.put("setup.autooff", Integer.valueOf(R.string.setting_setup_autooff));
        mLabelLookup.put("setup.autopoweroff", Integer.valueOf(R.string.setting_setup_autooff));
        mLabelLookup.put("setup.delayedshutdown", Integer.valueOf(R.string.setting_setup_delayedshutdown));
        
        mLabelLookup.put("setup.date", Integer.valueOf(R.string.setting_setup_date));
        mLabelLookup.put("setup.battery", Integer.valueOf(R.string.setting_setup_battery));
        mLabelLookup.put("setup.sdcard", Integer.valueOf(R.string.setting_setup_sdcard));
        mLabelLookup.put("setup.changewifi", Integer.valueOf(R.string.setting_setup_changewifi));
        mLabelLookup.put("setup.locate", Integer.valueOf(R.string.setting_setup_locate));
		
		mLabelLookup.put("setup.time.syncgps", Integer.valueOf(R.string.setting_setup_time_syncgps));
		mLabelLookup.put("setup.keytone", Integer.valueOf(R.string.setting_setup_keytone));
		mLabelLookup.put("setup.tvmode", Integer.valueOf(R.string.setting_setup_tvmode));
		mLabelLookup.put("setup.parkingguard", Integer.valueOf(R.string.setting_setup_parkingguard));
		mLabelLookup.put("setup.irled", Integer.valueOf(R.string.setting_setup_irled));
		mLabelLookup.put("setup.imagerotation", Integer.valueOf(R.string.setting_setup_imagerotation));
		mLabelLookup.put("setup.volume", Integer.valueOf(R.string.setting_setup_volume));

		mLabelLookup.put("setup.format", Integer.valueOf(R.string.setting_adv_format));
		mLabelLookup.put("setup.reset", Integer.valueOf(R.string.setting_adv_reset));
		mLabelLookup.put("setup.platenumber", Integer.valueOf(R.string.setting_setup_platenumber));
		mLabelLookup.put("setup.ap", Integer.valueOf(R.string.setting_setup_ap));

		
        mLabelLookup.put("status.camera.locate", Integer.valueOf(R.string.setting_setup_locate));
        mLabelLookup.put("status.camera.battery", Integer.valueOf(R.string.setting_setup_battery));
        mLabelLookup.put("status.camera.capacity", Integer.valueOf(R.string.setting_setup_sdcard));
        mLabelLookup.put("action.file.delete.last", Integer.valueOf(R.string.delete_last_file));
        mLabelLookup.put("action.file.delete.all", Integer.valueOf(R.string.delete_all_files_from_sd_card));
        mLabelLookup.put("action.time.set", Integer.valueOf(R.string.setting_setup_date));
        mLabelLookup.put("action.wifi.set", Integer.valueOf(R.string.setting_setup_changewifi));
        
        mLabelLookup.put("video.reset", Integer.valueOf(R.string.setting_adv_reset));
        mLabelLookup.put("car.reset", Integer.valueOf(R.string.setting_adv_reset));
        mLabelLookup.put("photo.reset", Integer.valueOf(R.string.setting_adv_reset));
        /*
      mLabelLookup.put("CAMERA_RESET_DEFAULT_ADVANCED_SETTINGS_ID", Integer.valueOf(R.string.setting_protune_reset));
      mLabelLookup.put("CAMERA_BEEPING_SOUND_ID", Integer.valueOf(R.string.setting_sound));
      mLabelLookup.put("CAMERA_BURST_RATE_ID", Integer.valueOf(R.string.setting_burst_rate));
      mLabelLookup.put("CAMERA_CONTINUOUS_SHOT_ID", Integer.valueOf(R.string.setting_continuous_shot));
      mLabelLookup.put("CAMERA_DEFAULT_POWER_ON_ID", Integer.valueOf(R.string.setting_defaultpower));
      mLabelLookup.put("CAMERA_FIELD_OF_VIEW_ID", Integer.valueOf(R.string.setting_fov));
      mLabelLookup.put("CAMERA_FRAME_RATE_ID", Integer.valueOf(R.string.setting_frame_rate));
      mLabelLookup.put("CAMERA_LED_BLINKING_ID", Integer.valueOf(R.string.setting_led));
      mLabelLookup.put("CAMERA_LOOPING_ID", Integer.valueOf(R.string.setting_looping));
      mLabelLookup.put("CAMERA_PHOTO_RESOLUTION_ID", Integer.valueOf(R.string.setting_photo_resolution));
      mLabelLookup.put("CAMERA_PREVIEW_ID", Integer.valueOf(R.string.setting_preview));
      mLabelLookup.put("CAMERA_PROTUNE_ID", Integer.valueOf(R.string.setting_protune));
      mLabelLookup.put("CAMERA_SPOT_METER_ID", Integer.valueOf(R.string.setting_spotmeter));
      mLabelLookup.put("CAMERA_TIME_INTERVAL_ID", Integer.valueOf(R.string.setting_time_lapse));
      mLabelLookup.put("CAMERA_UPSIDE_DOWN_IMAGE_CAPTURE_ID", Integer.valueOf(R.string.setting_updown));
      mLabelLookup.put("CAMERA_VIDEO_PLUS_PHOTO_ID", Integer.valueOf(R.string.setting_video_photo));
      mLabelLookup.put("CAMERA_VIDEO_RESOLUTION_ID", Integer.valueOf(R.string.setting_video_res));
      mLabelLookup.put("CAMERA_VIDEO_STANDARD_MODE_ID", Integer.valueOf(R.string.setting_ntsc));
      mLabelLookup.put("CAMERA_COLOR_ID", Integer.valueOf(R.string.setting_color));
      mLabelLookup.put("CAMERA_EXPOSURE_COMPENSATION_ID", Integer.valueOf(R.string.setting_exposure_compensation));
      mLabelLookup.put("CAMERA_GAIN_ID", Integer.valueOf(R.string.setting_gain));
      mLabelLookup.put("CAMERA_SHARPNESS_ID", Integer.valueOf(R.string.setting_sharpness));
      mLabelLookup.put("CAMERA_WHITE_BALANCE_ID", Integer.valueOf(R.string.setting_white_balance));
      mLabelLookup.put("CAMERA_LOCATE_ID", Integer.valueOf(R.string.locate_camera));
      mLabelLookup.put("CAMERA_LOW_LIGHT_ID", Integer.valueOf(R.string.setting_low_light));
      mLabelLookup.put("CAMERA_DELETE_LAST_FILE_ID", Integer.valueOf(R.string.delete_last_file));
      mLabelLookup.put("CAMERA_DELETE_ALL_FILES_ID", Integer.valueOf(R.string.delete_all_files_from_sd_card));
      mLabelLookup.put("DUAL_CAMERA_VERSION_ID", Integer.valueOf(R.string.version));
      mLabelLookup.put("GPCAMERA_GROUP_CAMERA_SETTINGS_ID", Integer.valueOf(R.string.section_camera_settings));
      mLabelLookup.put("GPCAMERA_GROUP_CAPTURE_SETTINGS_ID", Integer.valueOf(R.string.section_capture_settings));
      mLabelLookup.put("GPCAMERA_GROUP_ADVANCED_SETTINGS_ID", Integer.valueOf(R.string.section_advanced_settings));
      mLabelLookup.put("GPCAMERA_GROUP_SETUP_ID", Integer.valueOf(R.string.section_setup));
      mLabelLookup.put("GPCAMERA_GROUP_DELETE_ID", Integer.valueOf(R.string.section_delete));
      mLabelLookup.put("GPCAMERA_GROUP_WIFI_NETWORK_ID", Integer.valueOf(R.string.section_wireless_settings));
      mLabelLookup.put("GPCAMERA_GROUP_WIRELESS_CONTROLS", Integer.valueOf(R.string.section_wireless_controls));
      mLabelLookup.put("GPCAMERA_GROUP_CAMERA_STATUS", Integer.valueOf(R.string.section_camera_status));
      mLabelLookup.put("GPCAMERA_GROUP_CAMERA_INFO", Integer.valueOf(R.string.section_camera_info));*/
      return mLabelLookup;
    }
    
    private InputStream getJsonAppData() throws NotFoundException{
        return mContext.getResources().openRawResource(R.raw.model_infotm);
    }
    
    private InputStream getJsonSettingsData() throws NotFoundException{
        return mContext.getResources().openRawResource(R.raw.filters_indo_bawa);
    }
    
    public LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> getLegacySections(HashMap<String, Integer> lableLookup,Context context){
//            ILabelLookup paramILabelLookup, 
//            GPCommon.Filter<InfotmSetting> paramFilter){
        LinkedHashMap localMap;
        LegacySettingParser lega = new LegacySettingParser();
        InputStream ins1 = getJsonAppData();
        InputStream ins2 = getJsonSettingsData();
        Log.e("map","LinkedHashMap");
        localMap = lega.parseSettingSections(ins1, ins2,lableLookup, context);
        return localMap;
        //TODO
    }
    public LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> getLegacySectionsBystream(HashMap<String, Integer> lableLookup,String str){
	//      ILabelLookup paramILabelLookup, 
	//      GPCommon.Filter<InfotmSetting> paramFilter){
	  LinkedHashMap localMap;
	  LegacySettingParser lega = new LegacySettingParser();
	  String ins1 = str;
	  InputStream ins2 = getJsonSettingsData();
	  Log.e("map","LinkedHashMap");
	  localMap = lega.parseSettingSectionsBystream(ins1, ins2,lableLookup, mContext);
	  return localMap;
	  //TODO
	}
}
