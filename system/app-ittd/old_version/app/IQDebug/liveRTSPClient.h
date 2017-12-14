/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2017, Live Networks, Inc.  All rights reserved
// A demo application, showing how to create and run a RTSP client (that can potentially receive multiple streams concurrently).
//
// NOTE: This code - although it builds a running application - is intended only to illustrate how to develop your own RTSP
// client application.  For a full-featured RTSP client application - with much more functionality, and many options - see
// "openRTSP": http://www.live555.com/openRTSP/

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#define VIDEO_PROFILE_H264_MP 0x2
#define VIDEO_PROFILE_RAW_YUV 0x80
#define VIDEO_PROFILE_H265_MP 0x200

#define VIDEO_DATA_YUV_UV12 0x15

#define VIDEO_DATA_H264_IFRAME 0x21
#define VIDEO_DATA_H264_BFRAME 0x22
#define VIDEO_DATA_H264_PFRAME 0x23

#define VIDEO_DATA_H265_IFRAME 0x51
#define VIDEO_DATA_H265_BFRAME 0x52
#define VIDEO_DATA_H265_PFRAME 0x53

// Forward function definitions:
// RTSP 'response handlers':
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
// called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient);
// The main streaming routine (for each "rtsp://" URL):
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:

class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;
};

// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:

class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  void *userdata = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, void *userdata, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  void *dlg;
  StreamClientState scs;
};

// Define a data sink (a subclass of "MediaSink") to receive the data for each subsession (i.e., each audio or video 'substream').
// In practice, this might be a class (or a chain of classes) that decodes and then renders the incoming audio or video.
// Or it might be a "DataSink", for outputting the received data into a file (as is done by the "openRTSP" application).
// In this example code, however, we define a simple 'dummy' sink that receives incoming data, but does nothing with it.

class DataSink: public MediaSink {
public:
	static DataSink* createNew(UsageEnvironment& env, char const* fileName,
		void *playHandle = NULL,
		unsigned bufferSize = 20000,
		Boolean oneFilePerFrame = False);
	// "bufferSize" should be at least as large as the largest expected
	//   input frame.
	// "oneFilePerFrame" - if True - specifies that each input frame will
	//   be written to a separate file (using the presentation time as a
	//   file name suffix).  The default behavior ("oneFilePerFrame" == False)
	//   is to output all incoming data into a single file.
	virtual void addStartCode(unsigned char const* data, unsigned dataSize);
	// (Available in case a client wants to add pre data to the output file)

	virtual void addData(unsigned char const* data, unsigned dataSize,
	struct timeval presentationTime);
	// (Available in case a client wants to add extra data to the output file)

protected:
	DataSink(UsageEnvironment& env, FILE* fid, void *playHandle, unsigned bufferSize,
		char const* perFrameFileNamePrefix);
	// called only by createNew()
	virtual ~DataSink();

protected: // redefined virtual functions:
	virtual Boolean continuePlaying();

protected:
	static void afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes,
	struct timeval presentationTime,
		unsigned durationInMicroseconds);
	virtual void afterGettingFrame(unsigned frameSize,
		unsigned numTruncatedBytes,
	struct timeval presentationTime);

	FILE* fOutFid;
	void* pPlayHandle;
	unsigned char* fBuffer;
	unsigned fBufferSize;
	unsigned fBufferPos;
	char* fPerFrameFileNamePrefix; // used if "oneFilePerFrame" is True
	char* fPerFrameFileNameBuffer; // used if "oneFilePerFrame" is True
	struct timeval fPrevPresentationTime;
	unsigned fSamePresentationTimeCounter;
public:
	Boolean fPlayPause;
	Boolean fHaveKeyFrame;
};

class H264or5VideoDataSink: public DataSink {
protected:
	H264or5VideoDataSink(UsageEnvironment& env, FILE* fid, void *playHandle,
		unsigned bufferSize, char const* perFrameFileNamePrefix,
		char const* sPropParameterSetsStr1,
		char const* sPropParameterSetsStr2 = NULL,
		char const* sPropParameterSetsStr3 = NULL);
	// we're an abstract base class
	virtual ~H264or5VideoDataSink();

protected: // redefined virtual functions:
	virtual void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime);

private:
	char const* fSPropParameterSetsStr[3];
	Boolean fHaveWrittenFirstFrame;
};

class H264VideoDataSink: public H264or5VideoDataSink {
public:
	static H264VideoDataSink* createNew(UsageEnvironment& env, char const* fileName,
		void *playHandle = NULL,
		char const* sPropParameterSetsStr = NULL,
		// "sPropParameterSetsStr" is an optional 'SDP format' string
		// (comma-separated Base64-encoded) representing SPS and/or PPS NAL-units
		// to prepend to the output
		unsigned bufferSize = 100000,
		Boolean oneFilePerFrame = False);
	// See "FileSink.hh" for a description of these parameters.

protected:
	H264VideoDataSink(UsageEnvironment& env, FILE* fid, void *playHandle,
		char const* sPropParameterSetsStr,
		unsigned bufferSize, char const* perFrameFileNamePrefix);
	// called only by createNew()
	virtual ~H264VideoDataSink();
};

class H265VideoDataSink: public H264or5VideoDataSink {
public:
	static H265VideoDataSink* createNew(UsageEnvironment& env, char const* fileName,
		void *playHandle = NULL,
		char const* sPropVPSStr = NULL,
		char const* sPropSPSStr = NULL,
		char const* sPropPPSStr = NULL,
		// The "sProp*Str" parameters are optional 'SDP format' strings
		// (comma-separated Base64-encoded) representing VPS, SPS, and/or PPS NAL-units
		// to prepend to the output
		unsigned bufferSize = 100000,
		Boolean oneFilePerFrame = False);
	// See "FileSink.hh" for a description of these parameters.

protected:
	H265VideoDataSink(UsageEnvironment& env, FILE* fid, void *playHandle,
		char const* sPropVPSStr,
		char const* sPropSPSStr,
		char const* sPropPPSStr,
		unsigned bufferSize, char const* perFrameFileNamePrefix);
	// called only by createNew()
	virtual ~H265VideoDataSink();
};

class YUVVideoDataSink: public DataSink {
public:
	static YUVVideoDataSink* createNew(UsageEnvironment& env, char const* fileName,
		void *playHandle = NULL,
		unsigned bufferSize = 100000,
		Boolean oneFilePerFrame = False);
	// See "FileSink.hh" for a description of these parameters.
protected:
	YUVVideoDataSink(UsageEnvironment& env, FILE* fid, void *playHandle,
		unsigned bufferSize, char const* perFrameFileNamePrefix);
	// we're an abstract base class
	virtual ~YUVVideoDataSink();
};
