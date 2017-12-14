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

  ServerMediaSession* sms = NULL;
  Boolean const reuseSource = True;


  if (h264 != NULL) {
    NEW_SMS("H264 Live Video and G711A Live Audio");
    sms->addSubsession(H264or5LiveVideoServerMediaSubsession
		       ::createNew(env, fileName, reuseSource));
	sms->addSubsession(PCMLiveAudioServerMediaSubsession
		       ::createNew(env, "default_mic", reuseSource));
  }else{
	NEW_SMS("MKV or other video and audio");
	sms->addSubsession(H264or5LiveFileServerMediaSubsession
			::createNew(env, fileName, reuseSource));
	sms->addSubsession(PCMLiveFileServerMediaSubsession
			::createNew(env, fileName, reuseSource));
  }

  return sms;
}
