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
// Copyright (c) 1996-2016, Live Networks, Inc.  All rights reserved
// A subclass of "RTSPServer" that creates "ServerMediaSession"s on demand,
// based on whether or not the specified stream name exists as a file
// Implementation

#include "DynamicRTSPServer.hh"
#include <liveMedia.hh>
#include <string.h>
#include "types.h"
#include <qsdk/audiobox.h>

extern RTSP_SERVER_INFO g_rtsp_server_info;

DynamicRTSPServer*
DynamicRTSPServer::createNew(UsageEnvironment& env, Port ourPort,
			     UserAuthenticationDatabase* authDatabase,
			     unsigned reclamationTestSeconds) {
  int ourSocket = setUpOurSocket(env, ourPort);
  if (ourSocket == -1) return NULL;

  return new DynamicRTSPServer(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds);
}

DynamicRTSPServer::DynamicRTSPServer(UsageEnvironment& env, int ourSocket,
				     Port ourPort,
				     UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds)
  : RTSPServerSupportingHTTPStreaming(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds) {
}

DynamicRTSPServer::~DynamicRTSPServer() {
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* fid); // forward

ServerMediaSession* DynamicRTSPServer
::lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession) {

  // Next, check whether we already have a "ServerMediaSession" for this file:
  ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(streamName);
  Boolean smsExists = sms != NULL;

  if (sms == NULL) {
    sms = createNewSMS(envir(), streamName, NULL); 
    addServerMediaSession(sms);
  }

  return sms;
}

#define NEW_SMS(description) do {\
char const* descStr = description\
    ", streamed by the LIVE555 Media Server";\
sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);\
} while(0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* /*fid*/) {
  if(fileName == NULL) return NULL;
  
  // Use the file name extension to determine the type of "ServerMediaSession":
  char const* h264 = strcasestr(fileName, "-stream");
  char const* isp = strcasestr(fileName, "isp");

  ServerMediaSession* sms = NULL;
  Boolean const reuseSource = False;


  if (h264 != NULL) {
    NEW_SMS("H264 or H265 Live Video and G711A Live Audio");
    OutPacketBuffer::maxSize = 2*1024*1024;
    sms->addSubsession(H264or5LiveVideoServerMediaSubsession
		       ::createNew(env, fileName, reuseSource));
    int fHandle = audio_get_channel(g_rtsp_server_info.audio_chanID,NULL, CHANNEL_BACKGROUND);
    if (fHandle < 0) {
	    printf("audio channel does not exist\n");
    }
    else
    {
	    sms->addSubsession(PCMLiveAudioServerMediaSubsession
		       ::createNew(env, g_rtsp_server_info.audio_chanID, reuseSource));
    }
  }
  else if(isp != NULL){
    NEW_SMS("YUV");
	OutPacketBuffer::maxSize = 4*1024*1024;
    sms->addSubsession(YUVLiveVideoServerMediaSubsession
		       ::createNew(env, fileName, reuseSource));
  }
  else{
  	if((strlen(g_rtsp_server_info.file_location) + strlen(fileName)) > FILE_PATH_MAXLEN)
  	{
		*(g_rtsp_server_info.env) << "full file path(" << g_rtsp_server_info.file_location \
			<< ":" << fileName << ")is too long,must <=" << FILE_PATH_MAXLEN << "\n";
		return NULL;
	}
		
  	char full_path[FILE_PATH_MAXLEN];
	sprintf(full_path,"%s/%s",g_rtsp_server_info.file_location,fileName);
	if(access(full_path,0) != 0)
	{
		printf("file %s not exits!!!\n",full_path);
		return NULL;
	}
	NEW_SMS("MKV or other video and audio");
	sms->addSubsession(H264or5LiveFileServerMediaSubsession
			::createNew(env, full_path, reuseSource));
	sms->addSubsession(PCMLiveFileServerMediaSubsession
			::createNew(env, full_path, reuseSource));
  }

  return sms;
}
