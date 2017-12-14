package com.infotm.dv;

import java.util.ArrayList;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.infotm.dv.adapter.ChoseAdapter;
import com.infotm.dv.adapter.InfotmSetting;
import com.infotm.dv.adapter.SettingListAdapter;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.connect.CommandService;
import com.infotm.dv.connect.NetService;

//主Setting的activity
public class CameraSettingsActivity extends BaseActivity {
    private static final String tag = "CameraSettingsActivity";
    public static final String KEY_TIME_SET = "action.time.set";
    public static final String KEY_RECORD_TIME = "status.camera.recordtime";
    public static final String KEY_VIDEO_RESET = "video.reset";
    public static final String KEY_CAR_RESET = "car.reset";
    public static final String KEY_PHOTO_RESET = "photo.reset";
    public static final String KEY_DELETE_LAST = "action.file.delete.last";
    public static final String KEY_DELETE_ALL = "action.file.delete.all";
    public static final String KEY_WIFI_MODE = "setup.wifi";
    public static final String KEY_WIFI_SET = "action.wifi.set";
    public static final String KEY_CAPACITY = "status.camera.capacity";
    public static final String KEY_SETUP_FORMAT = "setup.format";
    public static final String KEY_SETUP_RESET = "setup.reset";
    
    public static final String KEY_BATTERY = "status.camera.battery";
    ListView mListView;
    SettingListAdapter mListAdapter;
    String lastCmdString = "";
    private static final int MSG_FAIL_DISMISS_PB = 0;
    private static final int MSG_SUCCESS_DISMISS_PB = 1;
    private Handler handler = new Handler(){
        public void handleMessage(android.os.Message msg) {
            Log.v(tag, "---handleMessage---" + msg.what);
            switch (msg.what) {
                case MSG_FAIL_DISMISS_PB:
                	destroySettingProcessDialog();
                	Toast.makeText(CameraSettingsActivity.this,  R.string.device_setting_fail, 2000).show();
                    break;
                case MSG_SUCCESS_DISMISS_PB:
                    handler.removeMessages(MSG_FAIL_DISMISS_PB);
                	destroySettingProcessDialog();
                	Toast.makeText(CameraSettingsActivity.this, R.string.device_setting_success, 2000).show();
                    break;
                default:
                    break;
            }
        };
    };
    //SmartyProgressBar mSmartyProgressBar;
    private AlertDialog mSelectDialog;
    
    BroadcastReceiver mCommandBroadcastReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context context, Intent intent) {
            String ss = intent.getStringExtra("data");
            Log.v(tag, "------datachanged:" + ss);
            String from = intent.getStringExtra("from");
            String indexStr = intent.getStringExtra("cmdseq");
            String keyValueString = intent.getStringExtra("cmd").trim();
            Log.v(tag, "---indexStr----" + indexStr);
            if ("phone".equalsIgnoreCase(from) && null != indexStr && lastCmdString.trim().equalsIgnoreCase(keyValueString)){
//                chulong excecute end
                handler.sendEmptyMessage(MSG_SUCCESS_DISMISS_PB);
            }
            mListAdapter.notifyDataSetChanged();
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.a_settings);
        getActionBar().setDisplayHomeAsUpEnabled(true);
        registerReceiver(mCommandBroadcastReceiver, new IntentFilter(CommandService.ACTION_DATACHANGE));
        initUiVars();
    }

    public void initUiVars() {
        //mSmartyProgressBar = new SmartyProgressBar(this);
        //mClingView = (TextView) findViewById(R.id.cling);
        mListView = (ListView) findViewById(android.R.id.list);
//        mListAdapter = new SettingListAdapter(this, this.mPd, this.mCamera.getSettings(),
//                this.mSettingChangedListener, this.mCamera);
        mListAdapter = new SettingListAdapter(this, mCamera.getSettings(),mCamera);
        mListView.setAdapter(mListAdapter);
        mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            	
                if ("on".equalsIgnoreCase(mCamera.mCurrentWorking)){
                	Log.v(tag, "------onItemClick:pos:"+position + " mCamera:busy!!!!!!!!!!!" );
                   showSettingBusyDialog();
                   }else{
                Log.v(tag, "------onItemClick:pos:"+position + " id:" + id);
                int i = mListAdapter.getItemViewType(position);
                Object obj = mListAdapter.getItem(position);
                if (obj instanceof InfotmSetting){
                    String name = ((InfotmSetting) obj).getDisplayName();
                    String key = ((InfotmSetting) obj).getKey();
                    Log.v(tag, "--click:"+key);
                    int type = ((InfotmSetting) obj).getType();
                    if (type == 1){//select
                        Bundle args = new Bundle();
                        args.putStringArrayList("chosesentry", ((InfotmSetting) obj).mOptions);
                        args.putStringArrayList("chosesvalue", ((InfotmSetting) obj).mOptionsValue);
                        args.putString("selected", ((InfotmSetting) obj).getSelectedName());
                        args.putString("title", ((InfotmSetting) obj).getDisplayName());
                        chosesShow((InfotmSetting) obj,args);
                    } else if (type == 2){//"toggle"
                        String selectedValue = ((InfotmSetting) obj).getSelectedName();
                        String valStr = "off";
                        if ("on".equalsIgnoreCase(selectedValue)){
                            valStr = "off";
                        } else if ("off".equalsIgnoreCase(selectedValue)){
                            valStr = "on";
                        }
                        final String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                mCamera.getNextCmdIndex(), "setprop", 1, 
                                key+":"+ valStr  +";");
                        sendNormalCommand(cmd,key+":"+ valStr  +";");
                    } else if (type == 4){//"button"
                        if (KEY_TIME_SET.trim().equalsIgnoreCase(key.trim())){
                            setDateAndTime();
                        } else if (KEY_VIDEO_RESET.equalsIgnoreCase(key.trim()) ||
                                KEY_CAR_RESET.equalsIgnoreCase(key.trim()) ||
                                KEY_PHOTO_RESET.equalsIgnoreCase(key.trim())){
                            reSetAdvance(key.trim());
                        } else if (KEY_WIFI_MODE.equalsIgnoreCase(key.trim())){
                            showDialog(DIALOG_WIFI_MODE);
                        }else if(KEY_SETUP_FORMAT.equalsIgnoreCase(key.trim())){
                        	 showDialog(DIALOG_FORMAT_MODE);
                        }else if(KEY_SETUP_RESET.equalsIgnoreCase(key.trim())){
                        	 showDialog(DIALOG_RESET_MODE);
                        }
                    } else if (type == 6){//"child"
                        if (KEY_WIFI_SET.equalsIgnoreCase(key.trim())){
                            showDialog(DIALOG_WIFI_SET);
                        } else if (KEY_DELETE_LAST.equalsIgnoreCase(key.trim())){
                            showDialog(DIALOG_DELETE_LAST);
//                            deleteDVFile(0);
                        } else if (KEY_DELETE_ALL.equalsIgnoreCase(key.trim())){
                            showDialog(DIALOG_DELETE_ALL);
//                            deleteDVFile(1);
                        } else if (KEY_CAPACITY.equalsIgnoreCase(key.trim())){
                            showDialog(DIALOG_CAPACITY);
                        }
                    }
                        
                }
            }
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        int mInitialOrientation = getResources().getConfiguration().orientation;
    }
    
    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
    }
    
    @Override
    protected void onDestroy() {
        new Thread(new Runnable() {
            public void run() {
//                SocketClient.getInstance().closeSocket();
            }
        }).start();
        unregisterReceiver(mCommandBroadcastReceiver);
        super.onDestroy();
    }
    
    private void setDateAndTime(){
        Log.v(tag, "------setDateAndTime()");
        handler.post(new Runnable() {
            public void run() {
                showSettingProcessDialog();
            }
        });
        /*Date curDate = new Date(System.currentTimeMillis());//获取当前时间 
        int year = curDate.getYear(); 
        int month = curDate.getMonth(); 
        int day = curDate.getDate(); 
        int hour = curDate.getHours(); 
        int minute = curDate.getMinutes(); 
        int second = curDate.getSeconds(); 
        long time_UTC = java.util.Date.UTC(year, month, day, hour, minute, second);
        String time_uString = Long.toString(time_UTC / 1000 - 28800);*/
        String time_uString = Long.toString(System.currentTimeMillis()/1000);
        Log.v(tag, "----UTC----" + time_uString);
        final String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                        mCamera.getNextCmdIndex(), "action", 1, 
                        KEY_TIME_SET+":"+ time_uString  +";");
//        mCamera.sendCommand(new OperationResponseWrapper(new SetDateTimeOperation(mCamera)), handler);
        mCamera.sendCommand(new OperationResponseWrapper(new InfotmCamera.CameraOperation() {
            
            @Override
            public boolean execute() {
                // TODO Auto-generated method stub
                return mCamera.mCommandSender.sendCommand(cmd);
            }
        }), handler);
    }
    
    public void deleteDVFile(int flag){
        showSettingProcessDialog();
        final String value = flag == 1 ? "all" : "last";
        mCamera.sendCommand(new OperationResponseWrapper(new InfotmCamera.CameraOperation() {
            @Override
            public boolean execute() {
                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                        mCamera.getNextCmdIndex(), "action", 1, 
                        "action.file.delete"+":"+ value  +";");
                return mCamera.mCommandSender.sendCommand(cmd);
            }
        }),handler);
    }
    
    private void reSetAdvance(String key){
        String keyValueStr = "";
        if (KEY_VIDEO_RESET.equalsIgnoreCase(key)){
            keyValueStr = Utils.KEY_VIDEO_WHITEBALANCE+":"+ "Auto"  +";"
                    +Utils.KEY_VIDEO_COLOR+":"+ "Advanced Color"  +";"
                    +Utils.KEY_VIDEO_ISO+":"+ "1600"  +";"
                    +Utils.KEY_VIDEO_SHARPNESS+":"+ "high"  +";"
                    +Utils.KEY_VIDEO_EV+":"+ "0"  +";";
        } else if (KEY_CAR_RESET.equalsIgnoreCase(key)){
            keyValueStr = Utils.KEY_CAR_WHITEBALANCE+":"+ "Auto"  +";"
                    +Utils.KEY_CAR_COLOR+":"+ "Advanced Color"  +";"
                    +Utils.KEY_CAR_ISO+":"+ "1600"  +";"
                    +Utils.KEY_CAR_SHARPNESS+":"+ "high"  +";"
                    +Utils.KEY_CAR_EV+":"+ "0"  +";";
        } else if (KEY_PHOTO_RESET.equalsIgnoreCase(key)){
            keyValueStr = Utils.KEY_PHOTO_ADVANCE+":"+ "Auto"  +";"
                    +Utils.KEY_PHOTO_COLOR+":"+ "Advanced Color"  +";"
                    +Utils.KEY_PHOTO_ISO+":"+ "1600"  +";"
                    +Utils.KEY_PHOTO_SHARPNESS+":"+ "high"  +";"
                    +Utils.KEY_PHOTO_EV+":"+ "0"  +";";
        }
        if (null == keyValueStr || keyValueStr.length() == 0){
            return;
        }
        final String str = keyValueStr;
        final String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                mCamera.getNextCmdIndex(), "setprop", 5, 
                str);
        sendNormalCommand(cmd,str);
    }
    
    private synchronized void sendNormalCommand(String cmd, String keyValueString){//,wait next socket type
        lastCmdString = keyValueString;
        final String str = cmd;
        showSettingProcessDialog();
        handler.sendEmptyMessageDelayed(MSG_FAIL_DISMISS_PB, 5000);
        mCamera.sendCommand(new InfotmCamera.OperationResponse() {
            
            @Override
            public boolean execute() {
                return mCamera.mCommandSender.sendCommand(str);
            }
            
            @Override
            public void onSuccess() {
                //net ok but we have to wait chulong
                Log.v(tag, "------on success---wait for chulong");
            }
            
            @Override
            public void onFail() {
                handler.removeMessages(MSG_FAIL_DISMISS_PB);
                destroySettingProcessDialog();
                Toast.makeText(CameraSettingsActivity.this,  R.string.device_setting_fail, 2000).show();
/*               mSmartyProgressBar.onFail();
                 handler.postDelayed(new Runnable() {
                    public void run() {
                        if (mSmartyProgressBar.isShowing()){
                            mSmartyProgressBar.dismiss();
                        }
                    }
                }, 500L);*/
            }
        },handler);
    }
    
    public synchronized void sendDirectCommand(String cmd){//only wait this socket return
        final String str = cmd;
        showSettingProcessDialog();
        mCamera.sendCommand(new OperationResponseWrapper(new InfotmCamera.CameraOperation() {
            @Override
            public boolean execute() {
                return mCamera.mCommandSender.sendCommand(str);
            }
        }),handler);
    }
    
    public class OperationResponseWrapper implements InfotmCamera.OperationResponse{
      private final InfotmCamera.CameraOperation mOperation;

        public OperationResponseWrapper(InfotmCamera.CameraOperation paramCameraOperation) {
            mOperation = paramCameraOperation;
        }

        private void onComplete() {
            handler.postDelayed(new Runnable() {
                public void run() {
                	destroySettingProcessDialog();
                }
            }, 500L);
        }

        public boolean execute() {
            return this.mOperation.execute();
        }

        public void onFail() {
        	Toast.makeText(CameraSettingsActivity.this,  R.string.device_setting_fail, 2000).show();
            onComplete();
            Log.v(tag, "----fail");
        }

        public void onSuccess() {
        	Toast.makeText(CameraSettingsActivity.this, R.string.device_setting_success, 2000).show();
            onComplete();
            Log.v(tag, "----success");
        }
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
    
    @Override
    protected void onNetworkStateUpdate(Intent i) {
        Log.v(tag, "---onNetworkStateUpdate---stat:" + mCamera.mNetStatus);
//        super.onNetworkStateUpdate(i);
        if (mCamera.mNetStatus < InfotmCamera.STAT_YES_NET_YES_PIN){
           // startService(new Intent(getApplicationContext(), NetService.class));
            showDialog(DIALOG_LOST);
        } else if (mCamera.mNetStatus == InfotmCamera.STAT_YES_NET_YES_PIN){
            startInitConfig(handler,1);
            removeDialog(DIALOG_LOST);
        } else if (mCamera.mNetStatus == InfotmCamera.STAT_Y_Y_Y_INIT) {
            removeDialog(DIALOG_LOST);
            mListAdapter.notifyDataSetChanged();
        }
        
    }
    
    public void chosesShow(InfotmSetting infotmSetting,Bundle args){
        AlertDialog.Builder builder =  new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT).setNegativeButton(android.R.string.cancel, 
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.v(tag, "---onclick selectdialog button");
                    }
                });
        LayoutInflater inflater = LayoutInflater.from(this);
        View convertView = inflater.inflate(R.layout.dialog_chose, null);
        builder.setView(convertView);
        builder.setTitle(args.getString("title"));
        mSelectDialog = builder.create();
        mSelectDialog.show();
        Log.v(tag, "-----button height"+mSelectDialog.getButton(DialogInterface.BUTTON_NEGATIVE).getHeight());
        
        ListView l = (ListView) convertView.findViewById(R.id.list_chose);
        final ArrayList<String> tmpOption = args.getStringArrayList("chosesentry");
        final ArrayList<String> tmpOptionValue = args.getStringArrayList("chosesvalue");
        final String selectedVal = args.getString("selected");
        final String key = infotmSetting.getKey();
        l.setAdapter(new ChoseAdapter(tmpOption, tmpOptionValue,selectedVal,CameraSettingsActivity.this));
        l.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int posi, long arg3) {
                Log.v(tag, "------onItemClick"+posi);
                mSelectDialog.dismiss();
                if (!tmpOptionValue.get(posi).equals(selectedVal)){//if changed
                    //command
                    String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, mCamera.getNextCmdIndex(), "setprop", 1, 
                            ""+key+":"+tmpOptionValue.get(posi) +";");
                    sendNormalCommand(cmd,key+":"+tmpOptionValue.get(posi) +";");
                }
            }
        });
    }
    
    public static final int DIALOG_WIFI_MODE = 11;
    public static final int DIALOG_WIFI_SET = 12;
    public static final int DIALOG_DELETE_LAST = 13;
    public static final int DIALOG_DELETE_ALL = 14;
    public static final int DIALOG_CAPACITY = 15;
    public static final int DIALOG_RESET_MODE = 16;
    public static final int DIALOG_FORMAT_MODE = 17;
    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
            case DIALOG_WIFI_MODE:
//                String msg = getResources().getString(R.string.reconnecting_msg);
                return new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT)
                        .setTitle(R.string.dialog_wifi_off)
                        .setNegativeButton(android.R.string.cancel,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                    }
                                })
                        .setPositiveButton(android.R.string.ok,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                                mCamera.getNextCmdIndex(), "setprop", 1, 
                                                KEY_WIFI_MODE+":"+ "off"  +";");
                                        sendDirectCommand(cmd);
                                    }
                                })
                        .create();
            case DIALOG_WIFI_SET:
                LayoutInflater inflater = LayoutInflater.from(this);
                View dialogView = inflater.inflate(R.layout.dialog_wifi_set, null);
                final EditText nameTxt = (EditText) dialogView.findViewById(R.id.wifi_set_name);
                final EditText pswdTxt = (EditText) dialogView.findViewById(R.id.wifi_set_pswd);
                AlertDialog.Builder builder = new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT);//, android.R.style.Theme_Dialog);
                builder.setView(dialogView);
                builder.setTitle(R.string.setting_setup_changewifi);
                builder.setNegativeButton(android.R.string.cancel,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                            }
                        })
                        .setPositiveButton(android.R.string.ok,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        String cmd = CommandSender.commandStringBuilder(
                                                InfotmCamera.PIN,
                                                mCamera.getNextCmdIndex(), "action", 1,
                                                KEY_WIFI_SET + ":" + nameTxt.getText().toString()
                                                        + "\r\n" + pswdTxt.getText().toString()
                                                        + ";");
                                        sendDirectCommand(cmd);
                                    }
                                });
                AlertDialog wifiSetDialog = builder.create();
                return wifiSetDialog;
            case DIALOG_DELETE_LAST:
                return new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT)
                        .setTitle(R.string.delete_last_file)
                        .setNegativeButton(android.R.string.cancel,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                    }
                                })
                        .setPositiveButton(android.R.string.ok,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        deleteDVFile(0);
                                    }
                                })
                        .create();
            case DIALOG_DELETE_ALL:
                return new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT)
                        .setTitle(R.string.delete_all_files_from_sd_card)
                        .setNegativeButton(android.R.string.cancel,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                    }
                                })
                        .setPositiveButton(android.R.string.ok,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        deleteDVFile(1);
                                    }
                                })
                        .create();
            case DIALOG_CAPACITY:
                LayoutInflater inflat = LayoutInflater.from(this);
                View sdView = inflat.inflate(R.layout.dialog_sdcard, null);
                TextView videoTxt = (TextView) sdView.findViewById(R.id.videooncard);
                TextView videoRemainTxt = (TextView) sdView.findViewById(R.id.videominiutesavail);
                TextView picTxt = (TextView) sdView.findViewById(R.id.photooncard);
                TextView picRemainTxt = (TextView) sdView.findViewById(R.id.photoavailable);
                String sdString = mCamera.getValueFromSetting(KEY_CAPACITY);
                String[] capacities = sdString.split("/");
                if (null == capacities || capacities.length < 3){
                    Log.e(tag, "--capacities error:"+capacities);
                    return null;
                }
                videoTxt.setText(capacities[1]);
                videoRemainTxt.setText(getCapacityRemainVideoText(capacities[0], mCamera.mCurrentMode));
                picTxt.setText(capacities[2]);
                picRemainTxt.setText(getCapacityRemainPhotoText(capacities[0], mCamera.mCurrentMode));
                AlertDialog.Builder sdBuilder = new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT);
                sdBuilder.setView(sdView);
                sdBuilder.setTitle(R.string.setting_setup_sdcard);
                sdBuilder.setPositiveButton(android.R.string.ok,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                            }
                        });
                AlertDialog sdDialog = sdBuilder.create();
                return sdDialog;
            case DIALOG_FORMAT_MODE:
                return new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT)
                .setTitle(R.string.setting_adv_format)
                .setNegativeButton(android.R.string.cancel,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                            }
                        })
                .setPositiveButton(android.R.string.ok,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                        mCamera.getNextCmdIndex(), "action", 1, 
                                        "action.setup.format"+":"+ "on"  +";");
                                sendDirectCommand(cmd);
                            }
                        })
                .create();
            case DIALOG_RESET_MODE:
                return new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT)
                .setTitle(R.string.setting_adv_reset)
                .setNegativeButton(android.R.string.cancel,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                            }
                        })
                .setPositiveButton(android.R.string.ok,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                        mCamera.getNextCmdIndex(), "action", 1, 
                                        "action.setup.reset"+":"+ "on"  +";");  
                                sendDirectCommand(cmd);
                            }
                        })
                .create();
            default:
                break;
        }
        return super.onCreateDialog(id);
    }
    
    //even if not in video mode ,we calculate a result !
    public String getCapacityRemainVideoText(String capacityString, String bigMode){
        int seconds = 0;
        try {
            
            long remainSize = Long.parseLong(capacityString);
            
            if (Utils.MODE_G_PHOTO.equalsIgnoreCase(bigMode) || //no wrong !
                    Utils.MODE_G_VIDEO.equalsIgnoreCase(bigMode)){
                String resolution = mCamera.getValueFromSetting(Utils.KEY_VIDEO_RESOLUTION);
                int pixels = Integer.parseInt(resolution);
                int fps = Integer.parseInt(mCamera.getValueFromSetting(Utils.KEY_VIDEO_FPS));
                seconds = Utils.GetVideoTimeRemain(remainSize, pixels, fps);
            } else if (Utils.MODE_G_CAR.equalsIgnoreCase(bigMode)){
                String resolution = mCamera.getValueFromSetting(Utils.KEY_CAR_RESOLUTION);
                int pixels = Integer.parseInt(resolution);
                int fps = Integer.parseInt(mCamera.getValueFromSetting(Utils.KEY_CAR_FPS));
                seconds = Utils.GetVideoTimeRemain(remainSize, pixels, fps);
            } else if (Utils.MODE_G_PHOTO.equalsIgnoreCase(bigMode)){//
                String resolution = mCamera.getValueFromSetting(Utils.KEY_PHOTO_MEGA);
                if (resolution.contains("8")){
                    seconds = Utils.GetPictureRemain(remainSize, 8);
                } else if (resolution.contains("5")){
                    seconds = Utils.GetPictureRemain(remainSize, 5);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        String txt = seconds / 3600 + "H " + seconds / 60;
        return txt;
    }
    
    public String getCapacityRemainPhotoText(String capacityString, String bigMode){
        try {
            long remainSize = Long.parseLong(capacityString);
            String resolution = mCamera.getValueFromSetting(Utils.KEY_PHOTO_MEGA);
            if (resolution.contains("8")){
                return ""+Utils.GetPictureRemain(remainSize, 8);
            } else if (resolution.contains("5")){
                return ""+Utils.GetPictureRemain(remainSize, 5);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }

    //process dialog
    Dialog mSettingProcessDialog;
    public void showSettingProcessDialog(){
    	if(mSettingProcessDialog==null){
    		mSettingProcessDialog=new AlertDialog.Builder(this).create();
    	}
    	mSettingProcessDialog.show();
    	Window window = mSettingProcessDialog.getWindow();  
    	window.setContentView(R.layout.setting_process_dialog_layout);  
    		
    }
    
    public void destroySettingProcessDialog(){
    	if(mSettingProcessDialog!=null){
    		mSettingProcessDialog.dismiss();
    	}
    }
    
    //busy dialog
    Dialog mSettingBusyDialog;
    public void showSettingBusyDialog(){
    	if(mSettingBusyDialog==null){
    		mSettingBusyDialog=new AlertDialog.Builder(this).create();
    	}
    	mSettingBusyDialog.show();
    	Window window = mSettingBusyDialog.getWindow();  
    	window.setContentView(R.layout.setting_busy_dialog_layout);  
    	mSettingBusyDialog.findViewById(R.id.ok).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				destroySettingBusyDialog();
			}
		});
    		
    }
    
    public void destroySettingBusyDialog(){
    	if(mSettingBusyDialog!=null){
    		mSettingBusyDialog.dismiss();
    	}
    }
    
}
