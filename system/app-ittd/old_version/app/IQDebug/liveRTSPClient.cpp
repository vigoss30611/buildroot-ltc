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

#include "liveRTSPClient.h"
#include "H264VideoRTPSource.hh" // for "parseSPropParameterSets()"

#include "PlaySDK.h"
#include "h264or5parser.h"


// Implementation of "ourRTSPClient":

ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
					int verbosityLevel, char const* applicationName, void *userdata, portNumBits tunnelOverHTTPPortNum) {
  return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, userdata, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
			     int verbosityLevel, char const* applicationName, void *userdata, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1), dlg(userdata) {
}

ourRTSPClient::~ourRTSPClient() {
}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
  : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
  delete iter;
  if (session != NULL) {
    // We also need to delete "session", and unschedule "streamTimerTask" (if set)
    UsageEnvironment& env = session->envir(); // alias

    env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
    Medium::close(session);
  }
}


// Implementation of "DataSink":

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#include <io.h>
#include <fcntl.h>
#endif
#include "GroupsockHelper.hh"
#include "OutputFile.hh"

////////// DataSink //////////

#define SINK_PRE_BUFSZ 16 /* »º³åÔ¤Áô16×Ö½Ú´æ·ÅNALUÆðÊ¼Âë */

DataSink::DataSink(UsageEnvironment& env, FILE* fid, void *playHandle,
	unsigned bufferSize, char const* perFrameFileNamePrefix)
	: MediaSink(env), fOutFid(fid), pPlayHandle(playHandle), fBufferSize(bufferSize), fBufferPos(SINK_PRE_BUFSZ), fSamePresentationTimeCounter(0),
	fPlayPause(False), fHaveKeyFrame(False) {
		fBuffer = new unsigned char[bufferSize + SINK_PRE_BUFSZ];
		if (perFrameFileNamePrefix != NULL) {
			fPerFrameFileNamePrefix = strDup(perFrameFileNamePrefix);
			fPerFrameFileNameBuffer = new char[strlen(perFrameFileNamePrefix) + 100];
		} else {
			fPerFrameFileNamePrefix = NULL;
			fPerFrameFileNameBuffer = NULL;
		}
		fPrevPresentationTime.tv_sec = ~0; fPrevPresentationTime.tv_usec = 0;
}

DataSink::~DataSink() {
	delete[] fPerFrameFileNameBuffer;
	delete[] fPerFrameFileNamePrefix;
	delete[] fBuffer;
	if (fOutFid != NULL) fclose(fOutFid);
}

DataSink* DataSink::createNew(UsageEnvironment& env, char const* fileName, void *playHandle,
	unsigned bufferSize, Boolean oneFilePerFrame) {
		do {
			FILE* fid;
			char const* perFrameFileNamePrefix;
			if (oneFilePerFrame) {
				// Create the fid for each frame
				fid = NULL;
				perFrameFileNamePrefix = fileName;
			} else {
				// Normal case: create the fid once
				fid = OpenOutputFile(env, fileName);
				if (fid == NULL) break;
				perFrameFileNamePrefix = NULL;
			}

			return new DataSink(env, fid, playHandle, bufferSize, perFrameFileNamePrefix);
		} while (0);

		return NULL;
}

Boolean DataSink::continuePlaying() {
	if (fSource == NULL) return False;

	fSource->getNextFrame(fBuffer + SINK_PRE_BUFSZ, fBufferSize,
		afterGettingFrame, this,
		onSourceClosure, this);

	return True;
}

void DataSink::afterGettingFrame(void* clientData, unsigned frameSize,
	unsigned numTruncatedBytes,
struct timeval presentationTime,
	unsigned /*durationInMicroseconds*/) {
		DataSink* sink = (DataSink*)clientData;
		sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

void DataSink::addStartCode(unsigned char const* data, unsigned dataSize) {
	if (dataSize <= SINK_PRE_BUFSZ)
	{
		fBufferPos = SINK_PRE_BUFSZ - dataSize;
		memcpy(fBuffer + fBufferPos, data, dataSize);
	}
}

void DataSink::addData(unsigned char const* data, unsigned dataSize,
struct timeval presentationTime) {
	if (fPerFrameFileNameBuffer != NULL && fOutFid == NULL) {
		// Special case: Open a new file on-the-fly for this frame
		if (presentationTime.tv_usec == fPrevPresentationTime.tv_usec &&
			presentationTime.tv_sec == fPrevPresentationTime.tv_sec) {
				// The presentation time is unchanged from the previous frame, so we add a 'counter'
				// suffix to the file name, to distinguish them:
				sprintf(fPerFrameFileNameBuffer, "%s-%lu.%06lu-%u", fPerFrameFileNamePrefix,
					presentationTime.tv_sec, presentationTime.tv_usec, ++fSamePresentationTimeCounter);
		} else {
			sprintf(fPerFrameFileNameBuffer, "%s-%lu.%06lu", fPerFrameFileNamePrefix,
				presentationTime.tv_sec, presentationTime.tv_usec);
			fPrevPresentationTime = presentationTime; // for next time
			fSamePresentationTimeCounter = 0; // for next time
		}
		fOutFid = OpenOutputFile(envir(), fPerFrameFileNameBuffer);
	}

	if (!fPlayPause)
	{
		if (pPlayHandle != NULL && data != NULL && dataSize > 4) {
			do {
				DWORD dataType;
				H264VideoDataSink *pH264Sink = dynamic_cast<H264VideoDataSink*>(this);
				if (pH264Sink) {
					h264_type_t *h264 = (h264_type_t*)&data[4];
					switch (h264->u5Type) {
					case H264_SPS_TYPE:
					case H264_PPS_TYPE:
					case H264_I_TYPE:
						dataType = VIDEO_DATA_H264_IFRAME;
						break;
					case H264_P_TYPE:
					default:
						dataType = VIDEO_DATA_H264_PFRAME;
						break;
					}
				} else {
					H265VideoDataSink *pH265Sink = dynamic_cast<H265VideoDataSink*>(this);

					if (pH265Sink)
					{
						switch (data[4]) {
						case H265_VPS_TYPE:
						case H265_SPS_TYPE:
						case H265_PPS_TYPE:
						case H265_I_TYPE:
							dataType = VIDEO_DATA_H265_IFRAME;
							break;
						case H265_P_TYPE:
						default:
							dataType = VIDEO_DATA_H265_PFRAME;
							break;
						}
					} else {
						YUVVideoDataSink *pYUVSink = dynamic_cast<YUVVideoDataSink*>(this);
						if (pYUVSink == NULL) {
							break;
						}
						dataType = VIDEO_DATA_YUV_UV12;
					}
				}
				if (fHaveKeyFrame)
				{
					if (dataType != VIDEO_DATA_H264_IFRAME &&
						dataType != VIDEO_DATA_H265_IFRAME &&
						dataType != VIDEO_DATA_YUV_UV12)
					{
						break;
					}
					fHaveKeyFrame = False;
				}
				play_input_data(pPlayHandle, dataType, 0, data, dataSize);
			} while (0);
		}
	}

	// Write to our file:
#ifdef TEST_LOSS
	static unsigned const framesPerPacket = 10;
	static unsigned const frameCount = 0;
	static Boolean const packetIsLost;
	if ((frameCount++)%framesPerPacket == 0) {
		packetIsLost = (our_random()%10 == 0); // simulate 10% packet loss #####
	}

	if (!packetIsLost)
#endif
		if (fOutFid != NULL && data != NULL) {
			fwrite(data, 1, dataSize, fOutFid);
		}
}

void DataSink::afterGettingFrame(unsigned frameSize,
	unsigned numTruncatedBytes,
struct timeval presentationTime) {
	if (numTruncatedBytes > 0) {
		envir() << "DataSink::afterGettingFrame(): The input frame data was too large for our buffer size ("
			<< fBufferSize << ").  "
			<< numTruncatedBytes << " bytes of trailing data was dropped!  Correct this by increasing the \"bufferSize\" parameter in the \"createNew()\" call to at least "
			<< fBufferSize + numTruncatedBytes << "\n";
	}
	addData(fBuffer + fBufferPos, SINK_PRE_BUFSZ - fBufferPos + frameSize, presentationTime);
	fBufferPos = SINK_PRE_BUFSZ;
	
	if (fPerFrameFileNameBuffer != NULL) {
		if (fOutFid != NULL) { fclose(fOutFid); fOutFid = NULL; }
	}

	// Then try getting the next frame:
	continuePlaying();
}

////////// H264or5VideoDataSink //////////

H264or5VideoDataSink
	::H264or5VideoDataSink(UsageEnvironment& env, FILE* fid, void *playHandle,
	unsigned bufferSize, char const* perFrameFileNamePrefix,
	char const* sPropParameterSetsStr1,
	char const* sPropParameterSetsStr2,
	char const* sPropParameterSetsStr3)
	: DataSink(env, fid, playHandle, bufferSize, perFrameFileNamePrefix),
	fHaveWrittenFirstFrame(False) {
		fSPropParameterSetsStr[0] = sPropParameterSetsStr1;
		fSPropParameterSetsStr[1] = sPropParameterSetsStr2;
		fSPropParameterSetsStr[2] = sPropParameterSetsStr3;
}

H264or5VideoDataSink::~H264or5VideoDataSink() {
}

void H264or5VideoDataSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime) {
	unsigned char const start_code[4] = {0x00, 0x00, 0x00, 0x01};
	if (!fHaveWrittenFirstFrame) {
		// If we have NAL units encoded in "sprop parameter strings", prepend these to the file:
		for (unsigned j = 0; j < 3; ++j) {
			unsigned numSPropRecords;
			SPropRecord* sPropRecords
				= parseSPropParameterSets(fSPropParameterSetsStr[j], numSPropRecords);
			for (unsigned i = 0; i < numSPropRecords; ++i) {
				if (sPropRecords[i].sPropLength > 4 &&
					memcmp(sPropRecords[i].sPropBytes, start_code, sizeof(start_code)) == 0)
				{
					addData(sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength, presentationTime);
				} else {
					unsigned char *pBuf = new unsigned char[4 + sPropRecords[i].sPropLength];
					if (pBuf)
					{
						memcpy(pBuf, start_code, sizeof(start_code));
						memcpy(pBuf + sizeof(start_code), sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
						addData(pBuf, sizeof(start_code) + sPropRecords[i].sPropLength, presentationTime);
						delete[] pBuf;
					}
				}
			}
			delete[] sPropRecords;
		}
		fHaveWrittenFirstFrame = True; // for next time
	}

	// Write the input data to the file, with the start code in front:
	DataSink::addStartCode(start_code, sizeof(start_code));

	// Call the parent class to complete the normal file write with the input data:
	DataSink::afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

////////// H265VideoDataSink //////////

H265VideoDataSink
	::H265VideoDataSink(UsageEnvironment& env, FILE* fid,
	void *playHandle,
	char const* sPropVPSStr,
	char const* sPropSPSStr,
	char const* sPropPPSStr,
	unsigned bufferSize, char const* perFrameFileNamePrefix)
	: H264or5VideoDataSink(env, fid, playHandle, bufferSize, perFrameFileNamePrefix,
	sPropVPSStr, sPropSPSStr, sPropPPSStr) {
}

H265VideoDataSink::~H265VideoDataSink() {
}

H265VideoDataSink*
	H265VideoDataSink::createNew(UsageEnvironment& env, char const* fileName,
	void *playHandle,
	char const* sPropVPSStr,
	char const* sPropSPSStr,
	char const* sPropPPSStr,
	unsigned bufferSize, Boolean oneFilePerFrame) {
		do {
			FILE* fid = NULL;
			char const* perFrameFileNamePrefix = NULL;
			if (oneFilePerFrame) {
				// Create the fid for each frame
				perFrameFileNamePrefix = fileName;
			} else if (fileName != NULL) {
				// Normal case: create the fid once
				fid = OpenOutputFile(env, fileName);
				if (fid == NULL) break;
			}

			return new H265VideoDataSink(env, fid, playHandle, sPropVPSStr, sPropSPSStr, sPropPPSStr, bufferSize, perFrameFileNamePrefix);
		} while (0);

		return NULL;
}

////////// H264VideoDataSink //////////

H264VideoDataSink
	::H264VideoDataSink(UsageEnvironment& env, FILE* fid,
	void *playHandle,
	char const* sPropParameterSetsStr,
	unsigned bufferSize, char const* perFrameFileNamePrefix)
	: H264or5VideoDataSink(env, fid, playHandle, bufferSize, perFrameFileNamePrefix,
	sPropParameterSetsStr, NULL, NULL) {
}

H264VideoDataSink::~H264VideoDataSink() {
}

H264VideoDataSink*
	H264VideoDataSink::createNew(UsageEnvironment& env, char const* fileName,
	void *playHandle,
	char const* sPropParameterSetsStr,
	unsigned bufferSize, Boolean oneFilePerFrame) {
		do {
			FILE* fid = NULL;
			char const* perFrameFileNamePrefix = NULL;
			if (oneFilePerFrame) {
				// Create the fid for each frame
				perFrameFileNamePrefix = fileName;
			} else if (fileName != NULL) {
				// Normal case: create the fid once
				fid = OpenOutputFile(env, fileName);
				if (fid == NULL) break;
			}

			return new H264VideoDataSink(env, fid, playHandle, sPropParameterSetsStr, bufferSize, perFrameFileNamePrefix);
		} while (0);

		return NULL;
}

////////// YUVVideoDataSink //////////

YUVVideoDataSink
	::YUVVideoDataSink(UsageEnvironment& env, FILE* fid,
	void *playHandle,
	unsigned bufferSize, char const* perFrameFileNamePrefix)
	: DataSink(env, fid, playHandle, bufferSize, perFrameFileNamePrefix) {
}

YUVVideoDataSink::~YUVVideoDataSink() {
}

YUVVideoDataSink*
	YUVVideoDataSink::createNew(UsageEnvironment& env, char const* fileName,
	void *playHandle,
	unsigned bufferSize, Boolean oneFilePerFrame) {
		do {
			FILE* fid = NULL;
			char const* perFrameFileNamePrefix = NULL;
			if (oneFilePerFrame) {
				// Create the fid for each frame
				perFrameFileNamePrefix = fileName;
			} else if (fileName != NULL) {
				// Normal case: create the fid once
				fid = OpenOutputFile(env, fileName);
				if (fid == NULL) break;
			}

			return new YUVVideoDataSink(env, fid, playHandle, bufferSize, perFrameFileNamePrefix);
		} while (0);

		return NULL;
}
