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
// A file source that is a plain byte stream (rather than frames)
// C++ header

#ifndef _H264or5_LIVE_FILE_SOURCE_HH
#define _H264or5_LIVE_FILE_SOURCE_HH

#include "FramedSource.hh"
#include "Log.hh"
#include <fr/libfr.h>
#include <qsdk/videobox.h>
#include <qsdk/demux.h>

#define VIDEO_BUF_SIZE	(1024*600)

class H264or5LiveFileSource: public FramedSource {
public:
  static H264or5LiveFileSource* createNew(UsageEnvironment& env,struct demux_t *demux);
protected:
  H264or5LiveFileSource(UsageEnvironment& env,struct demux_t *demux);
	// called only by createNew()

  virtual ~H264or5LiveFileSource();
	
private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

protected:

private:
	char *fTruncatedBytes;
	unsigned int fTruncatedBytesNum;
	int fSendHeaderCount;
	struct demux_t * fDemux;
	struct demux_frame_t fFrame;
};

#endif
