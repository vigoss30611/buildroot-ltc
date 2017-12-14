package com.infotm.dv.view;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Checkable;
import android.widget.CheckedTextView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.infotm.dv.R;
import com.infotm.dv.Utils;
import com.infotm.dv.adapter.InfotmSetting;
import com.infotm.dv.adapter.SettingHelper;
import com.infotm.dv.adapter.SettingSection;
import com.infotm.dv.camera.CameraMode;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class CameraModePopup extends PopupWindow{
    private String tag = "CameraModePopup";
    private CheckableImageView mBtnVideo;
    private CheckableImageView mBtnCar;
    private CheckableImageView mBtnPhoto;
    ListView mListView;
    private Context mContext;
    private Handler mHandler = new Handler();
    private SmartyProgressBar pd;
    InfotmCamera mCamera;
    ControlModeAdapter mCameraModeAdapter;
    private HashMap<String, ArrayList<CameraMode>> mMap;
    
    public static HashMap<String, Integer> RESIDMAP = new HashMap<String, Integer>();
    static {
        buildResIdMap();
    }
    
    public CameraModePopup(Context c){
        super(LayoutInflater.from(c).inflate(R.layout.include_mode_tree_popup, null), -2, -2);//include_mode_popup 废弃
        setAnimationStyle(R.style.ModePopupList);
        setBackgroundDrawable(c.getResources().getDrawable(R.drawable.border_controls_dialog));
        setOutsideTouchable(true);
        setTouchable(true);
        setFocusable(true);
        mContext = c;
        pd = new SmartyProgressBar(c);
    }
    
    public void init(InfotmCamera iCamera,Display d, OnPopupDismissListener dismissListen){
        mCamera = iCamera;
        mListView = (ListView) getContentView().findViewById(R.id.list_mode);
//        buildModeMap();
//        ArrayList<CameraMode> localGroupList = mMap.get(mCamera.mCurrentMode.toLowerCase());
//        if (null == localGroupList){
        ArrayList<CameraMode>  localGroupList = new ArrayList<CameraMode>();
//        }
        mCameraModeAdapter = new ControlModeAdapter(mListView.getContext(), localGroupList, "");
        mListView.setAdapter(mCameraModeAdapter);
        mCameraModeAdapter.setOnClickListener(new OnModeClickedListener() {
            @Override
            public void onModeClick(Checkable checkable, int position) {
                if (0 == 1){//if is shutter on
                    return;
                }
                final CameraMode mode = (CameraMode) mCameraModeAdapter.getItem(position);
                checkable.setChecked(true);
                mCameraModeAdapter.setSelectedIndex(position);
                mCameraModeAdapter.notifyDataSetChanged();
                mCamera.sendCommand(new InfotmCamera.CameraOperation() {
                    @Override
                    public boolean execute() {
                        String key = "";
                        if (Utils.MODE_G_VIDEO.equalsIgnoreCase(mCameraModeAdapter.modeGroup)){
                            key = "video.recordmode";
                        } else if (Utils.MODE_G_CAR.equalsIgnoreCase(mCameraModeAdapter.modeGroup)){
                            key = "car.mode";
                        } else if (Utils.MODE_G_PHOTO.equalsIgnoreCase(mCameraModeAdapter.modeGroup)){
                            key = "photo.capturemode";
                        }
                        String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                mCamera.getNextCmdIndex(), "setprop", 1, 
                                key+":" + mode.textValue+";");
                        return mCamera.mCommandSender.sendCommand(cmd);
                    }
                }, null);
//                dismiss();
                mHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        dismiss();
                    }
                }, 300L);
            }
        });
        initChildViews();
    }
    
    public void initChildViews(){
//      setOrientation(1);
        mListView = ((ListView)getContentView().findViewById(R.id.list_mode));
        mBtnVideo = ((CheckableImageView) getContentView().findViewById(R.id.btn_video));
        mBtnCar = ((CheckableImageView) getContentView().findViewById(R.id.btn_car));
        mBtnPhoto = ((CheckableImageView) getContentView().findViewById(R.id.btn_photo));
        mBtnVideo.setOnClickListener(createGroupClickListener(Utils.MODE_G_VIDEO));
        mBtnCar.setOnClickListener(createGroupClickListener(Utils.MODE_G_CAR));
        mBtnPhoto.setOnClickListener(createGroupClickListener(Utils.MODE_G_PHOTO));
    }
    
    private View.OnClickListener createGroupClickListener(String modeGroup) {
        final String modG = modeGroup;
        return new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if ((v instanceof Checkable) && ((Checkable) v).isChecked()){
                    return;
                }
//                mCameraModeAdapter.setModes(mMap.get(mod));
//                updateModeBtns(modG);
                //TODO socket ,2 updateModeBtns should called here ?
                mCamera.sendCommand(new InfotmCamera.CameraOperation() {
                    @Override
                    public boolean execute() {
                        String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                mCamera.getNextCmdIndex(), "setprop", 1, 
                                Utils.KEY_BIG_CURRENTMODE+":" + modG+";");
                        return mCamera.mCommandSender.sendCommand(cmd);
                    }
                }, null);
            }
        };
//            ModeSelectorView.access$200(this.this$0).setModeGroup(this.val$group);
//            SmartyApp.getTracker().trackEvent("Camera Mode - " + ModeSelectorView.access$000(this.this$0).getCameraVersion(), this.val$group.toString(), "", 0L);
//          }
//        }
//      };
    }


    
    private void buildModeMap(){
//        mMap = new HashMap();
//        ArrayList<CameraMode> aArrayList = new ArrayList<CameraMode>();
//        aArrayList.add(MODE_VIDEO_1);
//        aArrayList.add(MODE_VIDEO_2);
//        aArrayList.add(MODE_VIDEO_3);
//        mMap.put(MODE_G_VIDEO, aArrayList);
//        ArrayList<CameraMode> bArrayList = new ArrayList<CameraMode>();
//        bArrayList.add(MODE_PHOTO_1);
//        bArrayList.add(MODE_PHOTO_2);
//        bArrayList.add(MODE_PHOTO_3);
//        mMap.put(MODE_G_PHOTO, bArrayList);
//        ArrayList<CameraMode> cArrayList = new ArrayList<CameraMode>();
//        cArrayList.add(MODE_CAR_1);
//        mMap.put(MODE_G_CAR, cArrayList);
        Iterator iterator = mCamera.mSettingData.keySet().iterator();
        ArrayList<InfotmSetting> settings = new ArrayList<InfotmSetting>();
        while(iterator.hasNext()){
            SettingSection sec = (SettingSection) iterator.next();
            if (sec.getId().equals(Utils.MODE_G_VIDEO)){
                settings = mCamera.mSettingData.get(sec);

                ArrayList<CameraMode> modes = new ArrayList<CameraMode>();
                int selectedIndex = -1;
                if (null != settings && settings.size() != 0) {
                    modes = createModesFromInfotmSetting(settings.get(0));
                } else {
                    Log.e(tag, " settings == null or size0");
                }
                break;
            }
        }
    }
    
    private ArrayList<CameraMode> createModesFromInfotmSetting(InfotmSetting setting){
        ArrayList<CameraMode> modes = new ArrayList<CameraMode>();
        int selectedIndex = -1;
        int size = setting.mOptionsValue.size();
        for (int i = 0; i < size; i++) {
            modes.add(new CameraMode(RESIDMAP.get(setting.mOptionsValue.get(i)
                    .toLowerCase().trim()),
                    setting.mOptions.get(i), setting.mOptionsValue
                            .get(i)));
        }
        selectedIndex = setting.getSelectedIndex();

        if (null == modes || modes.size() == 0) {
            Log.e(tag, "modes ==  null or 0");
        }
        return modes;
    }
    
    private static void buildResIdMap(){
        RESIDMAP.put(Utils.MODE_VIDEO_1.toLowerCase().trim(), R.drawable.detail_mode_icon_video);
        RESIDMAP.put(Utils.MODE_VIDEO_2.toLowerCase().trim(), R.drawable.detail_mode_icon_video_timelapse);
        RESIDMAP.put(Utils.MODE_VIDEO_3.toLowerCase().trim(), R.drawable.detail_mode_icon_video_slowmotion);
        RESIDMAP.put(Utils.MODE_PHOTO_1.toLowerCase().trim(), R.drawable.detail_mode_icon_photo);
        RESIDMAP.put(Utils.MODE_PHOTO_2.toLowerCase().trim(), R.drawable.detail_mode_icon_continuous);
        RESIDMAP.put(Utils.MODE_PHOTO_3.toLowerCase().trim(), R.drawable.detail_mode_icon_nightphoto);
        RESIDMAP.put(Utils.MODE_CAR_1.toLowerCase().trim(), R.drawable.detail_mode_icon_car);
    }
    
    @Override
    public void showAtLocation(View parent, int gravity, int x, int y) {
        if (null != mCamera && null != mCamera.mCurrentMode){
            updateModeBtns(mCamera.mCurrentMode.toLowerCase());
        }
        super.showAtLocation(parent, gravity, x, y);
    }
    
    public void updateModeBtns(String modeGroup){
        Log.v(tag, "---updateModeBtns---modeGroup:"+modeGroup);
        if (modeGroup.equalsIgnoreCase(Utils.MODE_G_VIDEO) || modeGroup.equalsIgnoreCase(Utils.MODE_G_CAR) || 
                modeGroup.equalsIgnoreCase(Utils.MODE_G_PHOTO)){
            setSelectedGroup(modeGroup);
        } else {
            dismiss();
            Log.e(tag, "----in mode not show popup:" + modeGroup);
        }
    }
    
    public static interface OnPopupDismissListener {
        public abstract void onDismiss();
    }
    
    private void setSelectedGroup(String modeGroup){//video photo car
//        if (null == mMap.get(modeGroup)){
//            Log.e(tag, "error modegroup :" + modeGroup);
//            return;
//        }
        Iterator iterator = mCamera.mSettingData.keySet().iterator();
        ArrayList<InfotmSetting> settings = new ArrayList<InfotmSetting>();
        while(iterator.hasNext()){
            SettingSection sec = (SettingSection) iterator.next();
            if (sec.getId().equalsIgnoreCase(modeGroup)){
                settings = mCamera.mSettingData.get(sec);
                break;
            }
        }
        
        ArrayList<CameraMode> modes = new ArrayList<CameraMode>();
//        String selectedValue = "";
        int selectedIndex = -1;
        if (modeGroup.equalsIgnoreCase(Utils.MODE_G_CAR)){
            String text = mContext.getResources().getString(R.string.setting_car_mode);
            modes.add(new CameraMode(R.drawable.detail_mode_icon_car, text, Utils.MODE_CAR_1));
            selectedIndex = 0;
        } else {
            if (null != settings && settings.size() != 0) {
                // modes = settings.get(0).mOptions;//多语言显示
                int size = settings.get(0).mOptionsValue.size();
				  for (int i = 0; i < size; i++) 
				  	{
				  	   Log.v(tag, "----mode:"+settings.get(0).mOptionsValue.get(i).toLowerCase().trim());
				  	}
                for (int i = 0; i < size; i++) {
                    // TODO
                    Log.v(tag, "----modeadd:selectname:" + settings.get(0).getKey()+" :"+size);
                    Log.v(tag, "----modeadd:"+settings.get(0).mOptionsValue.get(i).toLowerCase().trim());
                    modes.add(new CameraMode(RESIDMAP.get(settings.get(0).mOptionsValue.get(i)
                            .toLowerCase().trim()),
                            settings.get(0).mOptions.get(i), settings.get(0).mOptionsValue.get(i)));
					Log.v(tag, "after modes.add");
                }
				Log.v(tag, "after for");
                selectedIndex = settings.get(0).getSelectedIndex();
                if (null == modes || modes.size() == 0) {
                    Log.e(tag, "modes ==  null or 0");
                    return;
                }
            } else {
                Log.e(tag, " settings == null or size0");
            }
        }
        
        mCameraModeAdapter.setGroup(modeGroup);
        mCameraModeAdapter.setModes(modes);
//        mCameraModeAdapter.setSelectedI2CValue(littleMode);
        mCameraModeAdapter.setSelectedIndex(selectedIndex);
        mCameraModeAdapter.notifyDataSetChanged();
        if (Utils.MODE_G_VIDEO.equalsIgnoreCase(modeGroup)){
            mBtnVideo.setChecked(true);
            mBtnPhoto.setChecked(false);
            mBtnCar.setChecked(false);
        } else if (Utils.MODE_G_CAR.equalsIgnoreCase(modeGroup)){
            mBtnCar.setChecked(true);
            mBtnVideo.setChecked(false);
            mBtnPhoto.setChecked(false);
        } else if (Utils.MODE_G_PHOTO.equalsIgnoreCase(modeGroup)){
            mBtnPhoto.setChecked(true);
            mBtnVideo.setChecked(false);
            mBtnCar.setChecked(false);
        }
    }
    
    public class ControlModeAdapter extends BaseAdapter{
        private LayoutInflater mInflator;
        private ArrayList<CameraMode> mModes = new ArrayList();
//        private String mSelectedMode = "";
        private int selectedIndex = -1;
        OnModeClickedListener mOnClickListener;
        public String modeGroup;
        
        public ControlModeAdapter(Context c,ArrayList<CameraMode> as, String modeG){
            mModes = as;
            mInflator = ((LayoutInflater) c.getSystemService("layout_inflater"));
            modeGroup = modeG;
        }

        @Override
        public int getCount() {
            return mModes.size();
        }

        @Override
        public Object getItem(int position) {
            return mModes.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }
        
//        private String getSelectedI2CValue() {
//            return mSelectedMode;
//        }
//        
//        public void setSelectedI2CValue(String paramCameraModes) {
//            mSelectedMode = paramCameraModes;
//        }
        
        public void setSelectedIndex(int i){
            selectedIndex = i;
        }
        
        public void setGroup(String str){
            modeGroup = str;
        }
        
        public void setModes(ArrayList<CameraMode> paramList){
            mModes.clear();
            if (paramList != null) {
                mModes.addAll(paramList);
            }
            notifyDataSetChanged();
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View v = mInflator.inflate(R.layout.listitem_mode, parent, false);
            CameraMode tmpMode = mModes.get(position);
            String ss = tmpMode.text;
            CheckableImageView btn_mode = (CheckableImageView) v.findViewById(R.id.btn_mode);
            final CheckedTextView txt_title = (CheckedTextView) v.findViewById(R.id.txt_title);
            txt_title.setText(ss);
            btn_mode.setImageResource(tmpMode.resId);
            if (position == selectedIndex){
                txt_title.setChecked(true);
            } else {
                txt_title.setChecked(false);
            }
            final int tmpposition = position;
            if (null != mOnClickListener){
                v.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.v(tag, "------onclick------" + tmpposition);
                        mOnClickListener.onModeClick(txt_title, tmpposition);
                    }
                });
            }
            
            return v;
        }
        
        public void setOnClickListener(OnModeClickedListener onModeClickedListener) {
            mOnClickListener = onModeClickedListener;
        }
    }

    public static interface OnModeClickedListener {
        public abstract void onModeClick(Checkable checkable, int position);
    }
    
}
