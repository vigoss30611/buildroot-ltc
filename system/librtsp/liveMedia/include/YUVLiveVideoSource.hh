/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2016 Live Networks, Inc.  All rights reserved.
// Live frames
// C++ header

#ifndef _YUV_LIVE_VIDEO_SOURCE_HH
#define _YUV_LIVE_VIDEO_SOURCE_HH

#include "FramedSource.hh" 
#include "Log.hh"
#include <fr/libfr.h>
#include <qsdk/videobox.h>

#define VIDEO_BUF_SIZE	(1024*1024*4)

class YUVLiveVideoSource: public FramedSource {
public:
  static YUVLiveVideoSource* createNew(UsageEnvironment& env,const char *channelID);
protected:
  YUVLiveVideoSource(UsageEnvironment& env,const char *channelID = "isp-out");
	// called only by createNew()

  virtual ~YUVLiveVideoSource();
	
private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

protected:

private:
	struct fr_buf_info	fFrBuf;
	char const*	fChanID;
	char *fTruncatedBytes;
	unsigned int fTruncatedBytesNum;
};

#endif
