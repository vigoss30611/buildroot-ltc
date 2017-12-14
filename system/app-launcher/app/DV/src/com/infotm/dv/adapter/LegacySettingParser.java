package com.infotm.dv.adapter;

import android.content.Context;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

public class LegacySettingParser{
    public static final String tag = LegacySettingParser.class.getSimpleName();
    
    private static final String KEY = "key";
    private static final String KEY_ID = "id";
    private static final String KEY_ITEMS = "items";
    private static final String KEY_KEYS = "keys";
    private static final String KEY_LABEL = "label";
    private static final String KEY_SECTION_SETTINGS = "master_settings";
    private static final String KEY_SETTING_REF = "SETTING_REF";
    private static final String KEY_TYPE = "type";
    private static final String KEY_URL = "url";
    private static final String KEY_VALUES = "values";
    
    public LegacySettingParser(){
    }
    
    public LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> parseSettingSections(InputStream ins1,InputStream ins2,
            HashMap<String, Integer> labelLookUp, Context c){
        LinkedHashMap localLinkedHashMap = new LinkedHashMap<SettingSection, ArrayList<InfotmSetting>>();
        String str1 = readAllString(ins1);
        Log.v(tag, "------len:"+str1.length());
//        Log.v(tag, "------"+str1);
        if (null == str1){
            return null;
        }
        
        try {
            JSONArray localJSONArray;
            localJSONArray = new JSONArray(str1);
            for (int i = 0; i < localJSONArray.length(); i++) {
                JSONObject groupJSONObject = localJSONArray.optJSONObject(i);
                Log.v(tag, "------jsonobject-G:"+groupJSONObject + "\n");
                String gIdString = groupJSONObject.getString("id");
//                String gLabelString = groupJSONObject.getString("label");
                groupJSONObject.getString("type");//should be group
                JSONArray itmArray = groupJSONObject.getJSONArray("items");
                SettingSection mapkey = new SettingSection(gIdString, c.getResources().getString(labelLookUp.get(gIdString)));
                ArrayList<InfotmSetting> mapValue = new ArrayList<InfotmSetting>();
                for (int j = 0; j < itmArray.length(); j++) {
                    JSONObject itmJsOb = itmArray.optJSONObject(j);
                    String idString = itmJsOb.getString("id");
                    String typeString = itmJsOb.getString("type");//select button toggle text readonly child
//                    String labelString = itmJsOb.getString("label");
                    //------------------------------
//                    Log.v(tag, "-------idstring:"+idString);
//                    Log.v(tag, "-------idstring getresid:"+labelLookUp.get(idString));
                    if (null == labelLookUp.get(idString)){
                        Log.e(tag, "-------null---"+idString);
                    }
                    String labelString = c.getResources().getString(labelLookUp.get(idString));
                    InfotmSetting tmp = new InfotmSetting(idString, labelString, "ooo", 0,typeString);
                    
                    JSONArray itm = itmJsOb.getJSONArray("itms");
                    if (null != itm){
                        for(int k = 0; k < itm.length(); k ++){
                            JSONObject chose = itm.optJSONObject(k);
                            tmp.addOption(chose.getString("chose"));
                        }
                    }
                    
                    JSONArray itmValues = itmJsOb.getJSONArray("itmsvalue");
                    if (null != itmValues){
                        for(int k = 0; k < itmValues.length(); k ++){
                            JSONObject chose = itmValues.optJSONObject(k);
                            tmp.addOptionValue(chose.getString("chose"));
                        }
                    }
                    
                    
                    //------------------------------
//                    itmJsOb.getString("url");//exception when donot have
//                    itmJsOb.getString("SETTING_REF");//
                    
                    
                    mapValue.add(tmp);
                    Log.v(tag, "------itm:id:"+idString + "  label:"+labelString);
                }
                localLinkedHashMap.put(mapkey, mapValue);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return localLinkedHashMap;
    }
    public LinkedHashMap<SettingSection, ArrayList<InfotmSetting>> parseSettingSectionsBystream(String ins1,InputStream ins2,
            HashMap<String, Integer> labelLookUp, Context c){
        LinkedHashMap localLinkedHashMap = new LinkedHashMap<SettingSection, ArrayList<InfotmSetting>>();
        String str1 = ins1;
        Log.v(tag, "------len:"+str1.length());
//        Log.v(tag, "------"+str1);
        if (null == str1){
            return null;
        }
        
        try {
            JSONArray localJSONArray;
            localJSONArray = new JSONArray(str1);
            for (int i = 0; i < localJSONArray.length(); i++) {
                JSONObject groupJSONObject = localJSONArray.optJSONObject(i);
                Log.v(tag, "------jsonobject-G:"+groupJSONObject + "\n");
                String gIdString = groupJSONObject.getString("id");
//                String gLabelString = groupJSONObject.getString("label");
                groupJSONObject.getString("type");//should be group
                JSONArray itmArray = groupJSONObject.getJSONArray("items");
                SettingSection mapkey = new SettingSection(gIdString, c.getResources().getString(labelLookUp.get(gIdString)));
                ArrayList<InfotmSetting> mapValue = new ArrayList<InfotmSetting>();
                for (int j = 0; j < itmArray.length(); j++) {
                    JSONObject itmJsOb = itmArray.optJSONObject(j);
                    String idString = itmJsOb.getString("id");
                    String typeString = itmJsOb.getString("type");//select button toggle text readonly child
//                    String labelString = itmJsOb.getString("label");
                    //------------------------------
//                    Log.v(tag, "-------idstring:"+idString);
//                    Log.v(tag, "-------idstring getresid:"+labelLookUp.get(idString));
                    if (null == labelLookUp.get(idString)){
                        Log.e(tag, "-------null---"+idString);
                    }
                    String labelString = c.getResources().getString(labelLookUp.get(idString));
                    InfotmSetting tmp = new InfotmSetting(idString, labelString, "ooo", 0,typeString);
                    
                    JSONArray itm = itmJsOb.getJSONArray("itms");
                    if (null != itm){
                        for(int k = 0; k < itm.length(); k ++){
                            JSONObject chose = itm.optJSONObject(k);
                            tmp.addOption(chose.getString("chose"));
                        }
                    }
                    
                    JSONArray itmValues = itmJsOb.getJSONArray("itmsvalue");
                    if (null != itmValues){
                        for(int k = 0; k < itmValues.length(); k ++){
                            JSONObject chose = itmValues.optJSONObject(k);
                            tmp.addOptionValue(chose.getString("chose"));
                        }
                    }
                    
                    
                    //------------------------------
//                    itmJsOb.getString("url");//exception when donot have
//                    itmJsOb.getString("SETTING_REF");//
                    
                    
                    mapValue.add(tmp);
                    Log.v(tag, "------itm:id:"+idString + "  label:"+labelString);
                }
                localLinkedHashMap.put(mapkey, mapValue);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return localLinkedHashMap;
    }
    //copy from GPStreamutil.class
    public static String readString(InputStream paramInputStream) {
        String str1 = null;
        BufferedReader localBufferedReader = new BufferedReader(new InputStreamReader(
                paramInputStream));
        StringBuilder localStringBuilder = new StringBuilder();
        try{
            String str2 = localBufferedReader.readLine();
            if (str2 == null){
                paramInputStream.close();
                str1 = localStringBuilder.toString();
            } else {
                localStringBuilder.append(str2);
            }
        } catch (IOException localIOException) {
            Log.w(tag, "Error reading json data from stream");
            str1 = null;
        }
        return localStringBuilder.toString();
    }
    
    public static String readAllString(InputStream is){
        String json = null;
        try {
            byte[] buffer = new byte[is.available()];
            is.read(buffer);
            json = new String(buffer,"UTF-8");
        } catch (Exception e) {
            e.printStackTrace();
        }
        return json;
    }
}