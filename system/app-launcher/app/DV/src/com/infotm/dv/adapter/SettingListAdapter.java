
package com.infotm.dv.adapter;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import android.content.Context;
import android.os.Build;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.infotm.dv.CameraSettingsActivity;
import com.infotm.dv.R;
import com.infotm.dv.Utils;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.view.SmartyProgressBar;
import com.infotm.dv.view.SwitchButton;
import com.infotm.dv.view.SwitchButton.OnChangeListener;
//import android.widget.Switch;

public class SettingListAdapter extends BaseAdapter {
    private static final String tag = "SettingListAdapter";
    private static final Handler mHandler = new Handler();
    private Context mContext;
    private LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> mData;
    private ArrayList<Object> mVisibleData;
    LayoutInflater mInflator;
    InfotmCamera mCamera;
    OnChangeListener mLocateCheckListener;
    private final SmartyProgressBar mPd;

    private String mDefaultOnText;
    private String mDefaultOffText;
    // public SettingListAdapter(Context c, SmartyProgressBar
    // paramSmartyProgressBar,
    // Map<SettingSection, ArrayList<GoProSetting>> mMap,
    // SettingChangedListener paramSettingChangedListener, GoProCamera
    // paramGoProCamera){
    //
    // }

    public SettingListAdapter(Context c, Map<SettingSection, ArrayList<InfotmSetting>> a, InfotmCamera iCamera) {
        mLocateCheckListener = new OnChangeListener() {
            @Override
            public void onChange(SwitchButton sb,boolean isChecked) {
                final boolean checked = isChecked;
                InfotmCamera.getInstance().sendCommand(new InfotmCamera.OperationResponse() {
                    
                    private void onComplete(){
                        mHandler.postDelayed(new Runnable() {
                            public void run() {
                                if (mPd.isShowing()){
                                    mPd.dismiss();
                                }
                            }
                        }, 500L);
                    }
                    
                    @Override
                    public boolean execute() {
                        String valueString;
                        if (checked){
                            valueString = "on";
                        } else {
                            valueString = "off";
                        }
                        String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                        mCamera.getNextCmdIndex(), "action", 1, 
                        "status.camera.locate"+":"+ valueString  +";");
                        return mCamera.mCommandSender.sendCommand(cmd);
                    }
                    
                    @Override
                    public void onSuccess() {
                        if (null != mPd){
                            mPd.onSuccess();
                            onComplete();
                        }
                    }
                    
                    @Override
                    public void onFail() {
                        if (null != mPd){
                            mPd.onFail();
                            onComplete();
                        }
                    }
                }, mHandler);
            }
        };
        mContext = c;
        mVisibleData = new ArrayList<Object>();
        mPd = new SmartyProgressBar(c);
        mCamera = iCamera;
//        SettingHelper mSettingHelper = new SettingHelper(mContext, InfotmCamera.getInstance());//TODO for test
//        mData = mSettingHelper.getLegacySections();
        
//        InfotmCamera.getInstance().initGetCameraSettings(c);//TODO just test ,move to other space to init
        mData = InfotmCamera.getInstance().mSettingData;
        syncVisibility(mData);
        mInflator = ((LayoutInflater) c.getSystemService("layout_inflater"));
        mDefaultOffText = c.getString(R.string.default_off_text);
        mDefaultOnText = c.getString(R.string.default_on_text);
    }

    @Override
    public int getCount() {
        return mVisibleData.size();
    }

    @Override
    public Object getItem(int position) {
        return mVisibleData.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public int getItemViewType(int paramInt) {
        
        Object localObject = getItem(paramInt);
        int i = 11;
        if (localObject instanceof SettingSection) {
            i = 0;
        } else if (localObject instanceof InfotmSetting) {
            InfotmSetting localInfotmSetting = (InfotmSetting) localObject;
            switch (localInfotmSetting.getType()) {
                case 1:
                    i = 1;
                    break;
                case 2:
                    i = 2;
                    break;
                case 3:
                    i = 3;
                    break;
                case 4:
                    i = 4;
                    break;
                case 5:
                    i = 5;
                    break;
                case 6:
                    i = 6;
                    break;
                default:
                    break;
            }
        }
//        Log.v(tag, "------getItemViewType:pos:"+paramInt + "  type:"+i);
        return i;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
//        Log.v(tag, "------getview:pos:" + position + " type:"+getItemViewType(position));
        View localView = null;
        int i = getItemViewType(position);
        if (i == 0){
            localView = getHeaderView(position, convertView, parent);
        } else if (i== 1){
            localView = getItemView(position, convertView, parent);//?
        } else if (i== 2){
            localView = getToggleView(position, convertView, parent);
        } else if (i== 3){
            localView = getSliderView(position, convertView, parent);
        } else if (i== 4){
            localView = getButtonView(position, convertView, parent);
        } else if (i== 5){
            localView = getReadonlyView(position, convertView, parent);
        } else if (i== 6){
            localView = getChildView(position, convertView, parent);
        } else if (i== 7){
            localView = getEditView(position, convertView, parent);
        } else {
            localView = getItemView(position, convertView, parent);//?
        }
        return localView;
    }

    private View getButtonView(int paramInt, View v, ViewGroup paramViewGroup) {
        InfotmSetting iSetting = (InfotmSetting) getItem(paramInt);
//        if (null == v) {
            v = mInflator.inflate(R.layout.listitem_setting_button, paramViewGroup, false);
//        }
        ((TextView) v.findViewById(R.id.txt_setting_label)).setText(iSetting.getDisplayName());
        return v;
    }

    private View getChildView(int paramInt, View v, ViewGroup paramViewGroup) {
        InfotmSetting tmpSetting = (InfotmSetting) getItem(paramInt);
//        if (v == null) {
            v = this.mInflator.inflate(R.layout.listitem_setting, paramViewGroup, false);
//        }
        initLabel(v, tmpSetting);
        ((TextView) v.findViewById(R.id.txt_setting_value)).setText("");
        return v;
    }
    
    private View getEditView(int paramInt, View paramView, ViewGroup paramViewGroup){
        InfotmSetting tSetting = (InfotmSetting)getItem(paramInt);
//      if (paramView == null){
        paramView = this.mInflator.inflate(R.layout.listitem_setting_readonly, paramViewGroup, false);
//      }
      initLabel(paramView, tSetting);
      TextView localTextView = (TextView)paramView.findViewById(R.id.txt_setting_value);
//      if (TextUtils.equals(tSetting.getOperation(), "GPCAMERA_INFO_NAME_ID")){
//        localTextView.setText(mFacade.getCameraName());
//      }
//      if (TextUtils.equals(localGoProSetting.getOperation(), "GPCAMERA_NETWORK_NAME_ID")){
//          localTextView.setText(mFacade.getBacPacSSID());
//      }
      return paramView;
    }
    
    private View getHeaderView(int paramInt, View v, ViewGroup paramViewGroup){
//        if (v == null) {
            v = this.mInflator.inflate(R.layout.include_settings_title, paramViewGroup, false);
//        }
      TextView localTextView = (TextView)v;
      localTextView.setText(((SettingSection)getItem(paramInt)).getLabel());
      return localTextView;
    }
    
    private View getItemView(int paramInt, View v, ViewGroup paramViewGroup){
//        if (v == null) {
            v = mInflator.inflate(R.layout.listitem_setting, paramViewGroup, false);
//        }
      InfotmSetting tSetting = (InfotmSetting)getItem(paramInt);
      initLabel(v, tSetting);
      ((TextView)v.findViewById(R.id.txt_setting_value)).setText(tSetting.getChosedItmShowName());
      return v;
    }
    
    private View getReadonlyView(int paramInt, View v, ViewGroup paramViewGroup){
        InfotmSetting tSetting = (InfotmSetting)getItem(paramInt);
//        if (v == null) {
            v = mInflator.inflate(R.layout.listitem_setting_readonly, paramViewGroup, false);
//        }
        if (v == null){
            Log.e(tag, "!!!!!!!!!!!getReadonlyView! null");
        }
        if (tSetting == null){
            Log.e(tag, "!!!!!!!!!!!tSetting! null");
        }
      ((TextView) (v.findViewById(R.id.txt_setting_label))).setText(tSetting.getDisplayName());
      if (null != tSetting.getSelectedName()){
          TextView valueView = (TextView) v.findViewById(R.id.txt_setting_value);
          valueView.setText(tSetting.getSelectedName());
          if (CameraSettingsActivity.KEY_BATTERY.equalsIgnoreCase(tSetting.getKey())){
              int batterylv =  Integer.parseInt(tSetting.getSelectedName());
              if (batterylv % 4 == 0){
                  valueView.setText("100%");
              } else if (batterylv % 4 == 1){
                  valueView.setText("70%");
              } else if (batterylv % 4 == 2){
                  valueView.setText("40%");
              } else if (batterylv % 4 == 3){
                  valueView.setText("10%");
              }
          }
          
      }
      return v;
    }
    
    private View getSliderView(int paramInt, View v, ViewGroup paramViewGroup){
//        if (v == null) {
            v = mInflator.inflate(R.layout.listitem_setting_slider, paramViewGroup, false);
//        }
//        mSliderSettingHelper.bindView(paramInt, v, paramViewGroup);//铔鏉�
        return v;
    }
    
    //http://tieba.baidu.com/p/3262209009
    private View getToggleView(int paramInt, View paramView, ViewGroup paramViewGroup){
      String str1;
      String str2;
//      Switch localSwitch;
      if (Build.VERSION.SDK_INT > 16){
          
      }
      
//      CheckBox localSwitch;
      SwitchButton localSwitch;
      InfotmSetting tmpSetting = (InfotmSetting)getItem(paramInt);
//      ArrayList localArrayList = tmpSetting.getOptions();
//      if (localArrayList.size() > 1){
        str1 = "jingbo-on";//((SettingOption)localArrayList.get(0)).getDisplayName();
        str2 = "jingbo-off";//((SettingOption)localArrayList.get(1)).getDisplayName();
//        if (paramView == null){
            paramView = mInflator.inflate(R.layout.listitem_setting_toggle, paramViewGroup, false);
//        }
        localSwitch = (SwitchButton) paramView.findViewById(R.id.toggle_setting_value);
//        localSwitch.setTextOn(str1);
//        localSwitch.setTextOff(str2);
//        if ((!(TextUtils.equals(str2, localSwitch.getTextOff()))) || (!(TextUtils.equals(str1, localSwitch.getTextOn()))))
//        {
//          localSwitch.setTextOff(str2);
//          localSwitch.setTextOn(str1);
//        }
//      }
      try{
//          Field localField1 = Switch.class.getDeclaredField("mOnLayout");
//          localField1.setAccessible(true);
//          localField1.set(localSwitch, null);
//          Field localField2 = Switch.class.getDeclaredField("mOffLayout");
//          localField2.setAccessible(true);
//          localField2.set(localSwitch, null);
//          localSwitch.requestLayout();
//          localSwitch.invalidate();
          initLabel(paramView, tmpSetting);
//          if ((!(TextUtils.equals(tmpSetting.getKey(), "GPCAMERA_LOCATE_ID"))) && (!(TextUtils.equals(tmpSetting.getKey(), "camera/LL"))))
//            break label321;
          //localSwitch.setOnCheckedChangeListener(null);
          if ( null != tmpSetting.getSelectedName()){
              localSwitch.setChecked("on".equalsIgnoreCase(tmpSetting.getSelectedName().trim()));
          }
          if (TextUtils.equals(tmpSetting.getKey(), "status.camera.locate")){
              //localSwitch.setOnCheckedChangeListener(null);
//              localSwitch.setChecked(this.mCamera.isLocateOn());
              localSwitch.setOnChangeListener(mLocateCheckListener);

              str1 = mDefaultOnText;
              str2 = mDefaultOffText;
          }
          return paramView;
      } catch (IllegalArgumentException e3) {
          e3.printStackTrace();/*
          localSwitch.setOnCheckedChangeListener(null);
          localSwitch.setChecked(tmpSetting.getSelectedName().equalsIgnoreCase(str1));
          localSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(this, localGoProSetting)
          {
            public void onCheckedChanged(, boolean paramBoolean)
            {
              CharSequence localCharSequence;
              if (paramBoolean)
                localCharSequence = ((Switch)paramCompoundButton).getTextOn();
              while (true)
              {
                SettingOption localSettingOption;
                String str = localCharSequence.toString();
                Iterator localIterator = this.val$setting.getOptions().iterator();
                do
                {
                  if (!(localIterator.hasNext()))
                    break label83;
                  localSettingOption = (SettingOption)localIterator.next();
                }
                while (!(localSettingOption.getDisplayName().equalsIgnoreCase(str));
                SettingListAdapter.access$000(this.this$0).onOptionSelected(this.val$setting, localSettingOption);
                label83: return;
                localCharSequence = ((Switch)paramCompoundButton).getTextOff();
              }
            }
          });*/
        
      }
      return paramView;
    }

    private void initLabel(View v, InfotmSetting t) {
        ((TextView) v.findViewById(R.id.txt_setting_label)).setText(t.getDisplayName());
    }

    private boolean hasVisibleSetting(List<InfotmSetting> paramList) {
        InfotmSetting localInfotmSetting;
        Iterator localIterator = paramList.iterator();
        boolean vis = false;
        if (localIterator.hasNext()) {
            localInfotmSetting = (InfotmSetting) localIterator.next();
            if (null != localInfotmSetting && localInfotmSetting.isVisible()) {
                vis = true;
            }
        }
//        return vis;
        return true;//TODO bug ??
    }

    public void syncVisibility(LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> tData) {
        mVisibleData.clear();
        if (null == tData || tData.size() == 0){
            Log.e(tag, "----syncVisibility----but no tData");
            return;
        }
        Iterator iterator = tData.keySet().iterator();
        while (iterator.hasNext()) {
            SettingSection tSettingSection = (SettingSection) iterator.next();
            Log.v(tag, "section map " + tSettingSection.getLabel());
            if (hasVisibleSetting((ArrayList) tData.get(tSettingSection))) {
                mVisibleData.add(tSettingSection);
            }
            Iterator iterator2 = ((ArrayList) tData.get(tSettingSection)).iterator();
            while (iterator2.hasNext()) {
                InfotmSetting tInfotmSetting = (InfotmSetting) iterator2.next();
                Log.v(tag, "section map " + tInfotmSetting.mDisplayName);
                configureVisibility(tInfotmSetting);
                if (null != tInfotmSetting && tInfotmSetting.isVisible()) {
                    mVisibleData.add(tInfotmSetting);
                    if (isAdvAndHide(tInfotmSetting)){
                        break;
                    }
                }
            }
        }
        Log.v(tag, "------visSize:"+mVisibleData.size());
    }
    
    private void configureVisibility(InfotmSetting iSetting) {
        if(null == iSetting){
            return;
        }
        if (Utils.KEY_VIDEO_MODE.equalsIgnoreCase(iSetting.getKey())
                || Utils.KEY_PHOTO_MODE.equalsIgnoreCase(iSetting.getKey())){
            iSetting.mVisible = false;
            return;
        }
        String sVideoMode = mCamera.getValueFromSetting(Utils.KEY_VIDEO_MODE);
        boolean vidIntervalVisible = Utils.MODE_VIDEO_2.equalsIgnoreCase(sVideoMode);
        if (Utils.KEY_VIDEO_INTERVAL.equalsIgnoreCase(iSetting.getKey())){
            iSetting.mVisible = vidIntervalVisible;
            return;
        }
        
        String sPhotoMode = mCamera.getValueFromSetting(Utils.KEY_PHOTO_MODE);
        if (Utils.KEY_PHOTO_TIMER.equalsIgnoreCase(iSetting.getKey())){
            iSetting.mVisible = Utils.MODE_PHOTO_1.equalsIgnoreCase(sPhotoMode);
        } else if (Utils.KEY_PHOTO_INTERVAL.equalsIgnoreCase(iSetting.getKey())){
            iSetting.mVisible = Utils.MODE_PHOTO_2.equalsIgnoreCase(sPhotoMode);
        } else if (Utils.KEY_PHOTO_SHUTTER.equalsIgnoreCase(iSetting.getKey())){
            iSetting.mVisible = Utils.MODE_PHOTO_3.equalsIgnoreCase(sPhotoMode);
        }
    }
    
    private boolean isAdvAndHide(InfotmSetting iSetting){
        if ("off".equalsIgnoreCase(iSetting.getSelectedName())){
            if ("video.advanced".equalsIgnoreCase(iSetting.getKey()) ||
                    "car.advanced".equalsIgnoreCase(iSetting.getKey()) ||
                    "photo.advanced".equalsIgnoreCase(iSetting.getKey())){
                return true;
            }
        }
        return false;
    }
    
    @Override
    public void notifyDataSetChanged() {
        syncVisibility(mData);
        super.notifyDataSetChanged();
    }
}
