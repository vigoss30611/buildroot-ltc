
package com.infotm.dv.camera.operation;

import android.util.Log;

import com.infotm.dv.camera.InfotmCamera;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

public class SetDateTimeOperation implements InfotmCamera.CameraOperation {
    private static final String FORMAT_SET_DATE = "%%%02x%%%02x%%%02x%%%02x%%%02x%%%02x";
    private InfotmCamera mCamera;
    private final Timer mTimer = new Timer();
    SimpleDateFormat newman = new SimpleDateFormat("mm:ss:SSS", Locale.ENGLISH);

    public SetDateTimeOperation(InfotmCamera iCamera) {
        mCamera = iCamera;
    }

    public boolean execute() {
        Calendar localCalendar = Calendar.getInstance();
        localCalendar.get(1);
        int i = Integer.parseInt(new SimpleDateFormat("yy", Locale.US).format(localCalendar
                .getTime()));
        localCalendar.add(13, 5);
        localCalendar.set(14, 0);
        Object[] arrayOfObject = new Object[6];
        arrayOfObject[0] = Integer.valueOf(i);
        arrayOfObject[1] = Integer.valueOf(1 + localCalendar.get(2));
        arrayOfObject[2] = Integer.valueOf(localCalendar.get(5));
        arrayOfObject[3] = Integer.valueOf(localCalendar.get(11));
        arrayOfObject[4] = Integer.valueOf(localCalendar.get(12));
        arrayOfObject[5] = Integer.valueOf(localCalendar.get(13));
        String str = String.format(FORMAT_SET_DATE, arrayOfObject);
        Date localDate1 = localCalendar.getTime();
        localCalendar.add(14, -100);
        Date localDate2 = localCalendar.getTime();
        Log.d("newman", "setting time to: " + newman.format(localDate1));
        mTimer.schedule(new SetDateTimeHelper(str), localDate2);
        return true;
    }

    private class SetDateTimeHelper extends TimerTask {
        private final String mDate;

        public SetDateTimeHelper(String paramString) {
            mDate = paramString;
        }

        public void run() {
            mCamera.remoteSetCameraDateTime(mDate);
            Log.d("newman", "called fn at after: " + newman.format(new Date()));
        }
    }
}
