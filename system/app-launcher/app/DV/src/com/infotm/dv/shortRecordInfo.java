package com.infotm.dv;

import android.graphics.Bitmap;

public class shortRecordInfo
{
	    private  Bitmap mbmp;
		private long mTS;
	    public  shortRecordInfo(Bitmap bmp,long ts)
		{
		   mbmp = bmp;
		   mTS = ts;
		}

		public Bitmap getBmp()
		  {
		    return mbmp;
		  }
	
		  public long getTS()
		  {
		    return mTS;
		  }
	
}
