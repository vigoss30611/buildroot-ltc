package com.infotm.dv.adapter;

import java.util.ArrayList;

import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.RadioButton;
import android.widget.TextView;

import com.infotm.dv.CameraSettingsActivity;
import com.infotm.dv.R;

public class ChoseAdapter extends BaseAdapter{
    private String tag = "ChoseAdapter";
    ArrayList<String> choses = new ArrayList<String>();
    ArrayList<String> chosesVal = new ArrayList<String>();
    String selectedValue = "";
    int selectIndex = -1;
//    private Context mContext;
    CameraSettingsActivity activity;
    LayoutInflater inflater;

    public ChoseAdapter(ArrayList<String> cho,ArrayList<String> choVal, String selec,CameraSettingsActivity act){
        choses = cho;
        chosesVal = choVal;
        selectedValue = selec;
        if (null != selectedValue){
            for (int i = 0;i < chosesVal.size();i++){
                Log.v(tag, "------" + i + " "+chosesVal.get(i) + " chosed:" + selectedValue + "-------");
//                if (selectedValue.equalsIgnoreCase(chosesVal.get(i))){
//                    selectIndex = i;
//                    break;
//                }
                if (TextUtils.equals(selectedValue.toLowerCase().trim(), chosesVal.get(i).toLowerCase().trim())){
                    selectIndex = i;
                    break;
                }
            }
        }
        
        if (selectIndex == -1){
            Log.e(tag, "------do not find selected value !!");
        }
        activity = act;
        inflater = LayoutInflater.from(activity);
    }
    
    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return choses.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return choses.get(position);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Log.v(tag, "-------getview-position:" + position + " "+choses.get(position));
        convertView = inflater.inflate(R.layout.item_chose, null);
        TextView t = (TextView) convertView.findViewById(R.id.chose_text);
//        CheckBox cBox = (CheckBox) convertView.findViewById(R.id.box_itm);
        RadioButton radioButton = (RadioButton) convertView.findViewById(R.id.box_itm);
        t.setText(choses.get(position));
        radioButton.setOnCheckedChangeListener(null);
        if (position == selectIndex){
            radioButton.setChecked(true);
        } else {
            radioButton.setChecked(false);
        }
        radioButton.setClickable(false);
//        radioButton.setOnCheckedChangeListener(new RadioButton.OnCheckedChangeListener() {
//            @Override
//            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
////                activity.
//            }
//        });
        return convertView;
    }
    
}