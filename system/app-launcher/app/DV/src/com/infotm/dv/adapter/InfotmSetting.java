package com.infotm.dv.adapter;

import android.text.TextUtils;
import android.util.Log;

import java.util.ArrayList;

public class InfotmSetting{
    
    private static final String DELETE_FILES = "delete_all_files";
    private static final String DELETE_LAST_FILE = "delete_last_file";
//    private SparseArray<ArrayList<Integer>> mBlackList = new SparseArray();
    public  String mKey;
    private int mMaxLength;
    private int mMinLength;
    public ArrayList<String> mOptions = new ArrayList<String>();
    public ArrayList<String> mOptionsValue = new ArrayList<String>();
    protected int mPrecedence;//主导
//    protected SettingOption mSelected;
    protected String mSelectedString;
    private String mValidationRegex;
    private Boolean mVisibilityOverride;
    public boolean mVisible;
    private int mType = 0;
    
    public  String mDisplayName = "dddddddd";
    
    public InfotmSetting(String keyString, String nameString, String opString, int paramInt, String type){
        mVisible = true;
        mVisibilityOverride = null;
        mDisplayName = nameString;
//        mOperation = opString;
        mPrecedence = paramInt;
        mKey = keyString;
        mType = changeTypeToInt(type);
    }
    
    private int changeTypeToInt(String ty){
        if (ty.equals("group")){
            return 0;
        }else if (ty.equals("select")){
            return 1;
        }else if (ty.equals("toggle")){
            return 2;
        }else if (ty.equals("sliderdddddd")){//?
            return 3;
        }else if (ty.equals("button")){
            return 4;
        }else if (ty.equals("readonly")){
            return 5;
        }else if (ty.equals("child")){
            return 6;
        }else if (ty.equals("text")){//edittext??
            return 7;
        }
        return 0;
    }
    
    public int getType(){
        return mType;
    }
    
    public boolean isVisible(){
        return mVisible;
    }
    
    public String getDisplayName(){
        return mDisplayName;
    }
    
    public String getSelectedName(){//is in value
        return mSelectedString;
    }
    
    public int getSelectedIndex(){
        int index = -1;
        for(int i = 0; i < mOptionsValue.size(); i ++){
            if (null != mSelectedString){
                if (mSelectedString.trim().equalsIgnoreCase(mOptionsValue.get(i).trim())){
                    index = i;
                }
            }
        }
        return index;
    }
    
    public String getChosedItmShowName(){//text show to user
        int selectIndex = getSelectedIndex();
        if (selectIndex > -1 && selectIndex < mOptions.size()){
            return mOptions.get(selectIndex);
        }
        return "";
    }
    
    public void setSelectedValue(String selectedStr){
      if (!TextUtils.equals(mSelectedString, selectedStr)){
          mSelectedString = selectedStr;
//          notifySelectedChanged();
      }
    }
    
    public void addOption(String paramSettingOption) {
        mOptions.add(paramSettingOption);
    }
    
    public void addOptionValue(String paramSettingOption) {
        mOptionsValue.add(paramSettingOption);
    }
    
    public String getKey() {
        return mKey;
    }
}