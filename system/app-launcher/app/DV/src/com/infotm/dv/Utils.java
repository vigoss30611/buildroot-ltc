
package com.infotm.dv;

public class Utils {
    public static final String KEY_BIG_CURRENTMODE = "current.mode";
    
    public static final String KEY_VIDEO_MODE = "video.recordmode";
    public static final String KEY_CAR_MODE = "car.mode";
    public static final String KEY_PHOTO_MODE = "photo.capturemode";
    
    public static final String KEY_VIDEO_INTERVAL = "video.interval";
    public static final String KEY_VIDEO_RESOLUTION = "video.resolution";
    public static final String KEY_VIDEO_FPS = "video.fps";
    public static final String KEY_VIDEO_LOWLIGHT = "video.lowlight";
    public static final String KEY_VIDEO_ADVANCE = "video.advanced";
    public static final String KEY_VIDEO_WHITEBALANCE = "video.whitebalance";
    public static final String KEY_VIDEO_COLOR = "video.color";
    public static final String KEY_VIDEO_ISO = "video.isolimit";
    public static final String KEY_VIDEO_SHARPNESS = "video.sharpness";
    public static final String KEY_VIDEO_EV = "video.ev";
    
    public static final String KEY_VIDEO_LOOPRECORDING = "video.looprecording";
	public static final String KEY_VIDEO_REARCAMERA = "video.rearcamera";
    public static final String KEY_VIDEO_PIP = "video.pip";
    public static final String KEY_VIDEO_FRONTBIG = "video.frontbig";
    public static final String KEY_VIDEO_WDR = "video.wdr";
	
    public static final String KEY_CAR_RESOLUTION = "car.resolution";
    public static final String KEY_CAR_FPS = "car.fps";
    public static final String KEY_CAR_LOWLIGHT = "car.lowlight";
    public static final String KEY_CAR_ADVANCE = "car.advanced";
    public static final String KEY_CAR_WHITEBALANCE = "car.whitebalance";
    public static final String KEY_CAR_COLOR = "car.color";
    public static final String KEY_CAR_ISO = "car.isolimit";
    public static final String KEY_CAR_SHARPNESS = "car.sharpness";
    public static final String KEY_CAR_EV = "car.ev";
    

	
    public static final String KEY_PHOTO_MEGA = "photo.megapixels";
    public static final String KEY_PHOTO_INTERVAL = "photo.interval";
    public static final String KEY_PHOTO_TIMER = "photo.modetimer";
    public static final String KEY_PHOTO_SHUTTER = "photo.shutter";
    public static final String KEY_PHOTO_LOWLIGHT = "photo.lowlight";
    public static final String KEY_PHOTO_ADVANCE = "photo.advanced";
    public static final String KEY_PHOTO_WHITEBALANCE = "photo.whitebalance";
    public static final String KEY_PHOTO_COLOR = "photo.color";
    public static final String KEY_PHOTO_ISO = "photo.isolimit";
    public static final String KEY_PHOTO_SHARPNESS = "photo.sharpness";
    public static final String KEY_PHOTO_EV = "photo.ev";
    




	
    public static final String KEY_WORKING = "status.camera.working";
    public static final String KEY_CAMERA_REAR_STATUS = "status.camera.rear";
    public static final String KEY_CAMERA_FORMAT_STATUS = "status.camera.format";
    public static final String KEY_BATTRTY = CameraSettingsActivity.KEY_BATTERY;
    public static final String KEY_CAPACITY = CameraSettingsActivity.KEY_CAPACITY;
    
    //----------------------
    public static final String MODE_G_VIDEO = "Video";
    public static final String MODE_VIDEO_1 = "Video";
    public static final String MODE_VIDEO_2 = "Time Lapse Video";
    public static final String MODE_VIDEO_3 = "Slow Motion Video";
    
    public static final String MODE_G_CAR = "car";
    public static final String MODE_CAR_1 = "Car";
    
    public static final String MODE_G_PHOTO = "photo";
    public static final String MODE_PHOTO_1 = "Single";
    public static final String MODE_PHOTO_2 = "Continuous";
    public static final String MODE_PHOTO_3 = "Night";
    //-------------------------
    
    public static final int BIT_RATE_1080 = 6 * 1024 * 1024;
    public static final int BIT_RATE_720 = 2 * 1024 * 1024;
    public static final int BIT_RATE_360 = 400 * 1024;
    public static final int BIT_PIXELS_8MP = 8 * 1024 * 1024;
    public static final int BIT_PIXELS_5MP = 5 * 1024 * 1024;

    //Get video time remain, in Seconds
    public static int GetVideoTimeRemain(long availableSpace, int resolution, int fps) {
        int remain = 0;
        int bitrate = BIT_RATE_1080;
        switch(resolution) {
            case 1080:
                bitrate = BIT_RATE_1080;
                break;
            case 720:
                bitrate = BIT_RATE_720;
                break;
        }
        remain =(int) (((double)availableSpace) * 60 / (fps * bitrate >> 13));
        return remain;
    }
    
  //Get picture number remain, in Piece
    public static int GetPictureRemain(long availableSpace, int megapixel){
        int piece = 0;
        piece = (int) (availableSpace >> 7) / megapixel;
        return piece;
    }
}
