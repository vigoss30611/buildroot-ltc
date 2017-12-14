package com.infotm.dv.wifiadapter;


    
    import java.lang.reflect.Field;  
import java.lang.reflect.Method;  
import java.util.HashMap;  
import java.util.Map;  
      
import android.content.Context;
import android.net.wifi.WifiConfiguration;  
import android.net.wifi.WifiManager;  
import android.os.Build;  
import android.util.Log;

      
  public  class WifiApMan {  
        private static final String tag = "WifiApManager";  
      
        private static final String METHOD_GET_WIFI_AP_STATE = "getWifiApState";  
        private static final String METHOD_SET_WIFI_AP_ENABLED = "setWifiApEnabled";  
        private static final String METHOD_GET_WIFI_AP_CONFIG = "getWifiApConfiguration";  
        private static final String METHOD_IS_WIFI_AP_ENABLED = "isWifiApEnabled";  
      
        private static final Map<String, Method> methodMap = new HashMap<String, Method>();  
        private static Boolean mIsSupport;  
        private static boolean mIsHtc;  
      
        public synchronized static final boolean isSupport() {  
            if (mIsSupport != null) {  
                return mIsSupport;  
            }  
      
            boolean result = Build.VERSION.SDK_INT > Build.VERSION_CODES.FROYO;  
            if (result) {  
                try {  
                    Field field = WifiConfiguration.class.getDeclaredField("mWifiApProfile");  
                    mIsHtc = field != null;  
                } catch (Exception e) {  
                }  
            }  
      
            if (result) {  
                try {  
                    String name = METHOD_GET_WIFI_AP_STATE;  
                    Method method = WifiManager.class.getMethod(name);  
                    methodMap.put(name, method);  
                    result = method != null;  
                } catch (SecurityException e) {  
                    Log.e(tag, "SecurityException"+ e);  
                } catch (NoSuchMethodException e) {  
                	Log.e(tag, "NoSuchMethodException", e);  
                }  
            }  
              
            if (result) {  
                try {  
                    String name = METHOD_SET_WIFI_AP_ENABLED;  
                    Method method = WifiManager.class.getMethod(name, WifiConfiguration.class, boolean.class);  
                    methodMap.put(name, method);  
                    result = method != null;  
                } catch (SecurityException e) {  
                	Log.e(tag, "SecurityException", e);  
                } catch (NoSuchMethodException e) {  
                	Log.e(tag, "NoSuchMethodException", e);  
                }  
            }  
      
            if (result) {  
                try {  
                    String name = METHOD_GET_WIFI_AP_CONFIG;  
                    Method method = WifiManager.class.getMethod(name);  
                    methodMap.put(name, method);  
                    result = method != null;  
                } catch (SecurityException e) {  
                	Log.e(tag, "SecurityException", e);  
                } catch (NoSuchMethodException e) {  
                	Log.e(tag, "NoSuchMethodException", e);  
                }  
            }  
      
            if (result) {  
                try {  
                    String name = getSetWifiApConfigName();  
                    Method method = WifiManager.class.getMethod(name, WifiConfiguration.class);  
                    methodMap.put(name, method);  
                    result = method != null;  
                } catch (SecurityException e) {  
                	Log.e(tag, "SecurityException", e);  
                } catch (NoSuchMethodException e) {  
                	Log.e(tag, "NoSuchMethodException", e);  
                }  
            }  
      
            if (result) {  
                try {  
                    String name = METHOD_IS_WIFI_AP_ENABLED;  
                    Method method = WifiManager.class.getMethod(name);  
                    methodMap.put(name, method);  
                    result = method != null;  
                } catch (SecurityException e) {  
                	Log.e(tag, "SecurityException", e);  
                } catch (NoSuchMethodException e) {  
                	Log.e(tag, "NoSuchMethodException", e);  
                }  
            }  
      
            mIsSupport = result;  
            return isSupport();  
        }  
      
        private  WifiManager mWifiManager;  
        public WifiApMan(Context context) {  
            if (!isSupport()) {  
                throw new RuntimeException("Unsupport Ap!");  
            }  
            Log.i(tag, "Build.BRAND -----------> " + Build.BRAND);  
              
            //mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        }  
        private WifiManager getSystemService(String wifiService) {
			// TODO Auto-generated method stub
			return null;
		}
        public void setWifiManager(WifiManager man) {  
        	
        	mWifiManager =man;
            return ;  
        }
		public WifiManager getWifiManager() {  
            return mWifiManager;  
        }  
      
        public int getWifiApState() {  
            try {  
                Method method = methodMap.get(METHOD_GET_WIFI_AP_STATE);  
                return (Integer) method.invoke(mWifiManager);  
            } catch (Exception e) {  
            	Log.e(tag, e.getMessage(), e);  
            }  
            return 4;  
        }  
          
       /* private WifiConfiguration getHtcWifiApConfiguration(WifiConfiguration standard){  
            WifiConfiguration htcWifiConfig = standard;  
            try {  
                Object mWifiApProfileValue = BeanUtils.getFieldValue(standard, "mWifiApProfile");  
      
                if (mWifiApProfileValue != null) {  
                    htcWifiConfig.SSID = (String)BeanUtils.getFieldValue(mWifiApProfileValue, "SSID");  
                }  
            } catch (Exception e) {  
            	Log.e(tag, "" + e.getMessage(), e);  
            }  
            return htcWifiConfig;  
        }*/  
      
        public  WifiConfiguration getWifiApConfiguration() {  
            WifiConfiguration configuration = null;  
            try {  
                Method method = methodMap.get(METHOD_GET_WIFI_AP_CONFIG);  
                if(mWifiManager ==null)
                    Log.i("hello","mWifiManager is null");
                    else
                    	 Log.i("hello","mWifiManager is not null");
                configuration = (WifiConfiguration) method.invoke(mWifiManager);  
                if(configuration ==null)
                Log.i("hello","configuration is null");
                else
                	 Log.i("hello","configuration is not null");
               // if(isHtc()){  
               //     configuration = getHtcWifiApConfiguration(configuration);  
               // }  
            } catch (Exception e) {  
            	Log.e(tag, e.getMessage(), e);  
            }  
            return configuration;  
        }  
      
        public boolean setWifiApConfiguration(WifiConfiguration netConfig) {  
            boolean result = false;  
            try {  
                //if (isHtc()) {  
                  //  setupHtcWifiConfiguration(netConfig);  
                //}  
      
                Method method = methodMap.get(getSetWifiApConfigName());  
                Class<?>[] params = method.getParameterTypes();  
                for (Class<?> clazz : params) {  
                	Log.i(tag, "param -> " + clazz.getSimpleName());  
                }  
      
               // if (isHtc()) {  
                 //   int rValue = (Integer) method.invoke(mWifiManager, netConfig);  
                   // Log.i(tag, "rValue -> " + rValue);  
                   // result = rValue > 0;  
                //} else {  
                    result = (Boolean) method.invoke(mWifiManager, netConfig);  
                //}  
            } catch (Exception e) {  
            	Log.e(tag, "", e);  
            }  
            return result;  
        }  
          
        public boolean setWifiApEnabled(WifiConfiguration configuration, boolean enabled) {  
            boolean result = false;  
            try {  
                Method method = methodMap.get(METHOD_SET_WIFI_AP_ENABLED);  
                result = (Boolean)method.invoke(mWifiManager, configuration, enabled);  
            } catch (Exception e) {  
            	Log.e(tag, e.getMessage(), e);  
            }  
            return result;  
        }  
      
        public boolean isWifiApEnabled() {  
            boolean result = false;  
            try {  
                Method method = methodMap.get(METHOD_IS_WIFI_AP_ENABLED);  
                result = (Boolean)method.invoke(mWifiManager);  
            } catch (Exception e) {  
            	Log.e(tag, e.getMessage(), e);  
            }  
            return result;  
        }  
      
       /* private void setupHtcWifiConfiguration(WifiConfiguration config) {  
            try {  
            	Log.d(tag, "config=  " + config);  
                Object mWifiApProfileValue = BeanUtils.getFieldValue(config, "mWifiApProfile");  
      
                if (mWifiApProfileValue != null) {  
                    BeanUtils.setFieldValue(mWifiApProfileValue, "SSID", config.SSID);  
                    BeanUtils.setFieldValue(mWifiApProfileValue, "BSSID", config.BSSID);  
                    BeanUtils.setFieldValue(mWifiApProfileValue, "secureType", "open");  
                    BeanUtils.setFieldValue(mWifiApProfileValue, "dhcpEnable", 1);  
                }  
            } catch (Exception e) {  
            	Log.e(tag, "" + e.getMessage(), e);  
            }  
        }  
          
        public static boolean isHtc() {  
            return mIsHtc;  
        }*/  
          
        private static String getSetWifiApConfigName() {  
            return mIsHtc? "setWifiApConfig": "setWifiApConfiguration";  
        }  
    }  

