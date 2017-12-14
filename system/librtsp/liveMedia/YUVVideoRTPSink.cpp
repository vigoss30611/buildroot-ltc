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
// RTP sink for YUV video
// Implementation

#include "YUVVideoRTPSink.hh"
#include "Log.hh"

////////// YUVFragmenter definition //////////

// Because of the ideosyncracies of the YUV RTP payload format, we implement
// "YUVVideoRTPSink" using a separate "YUVFragmenter" class that delivers,
// to the "YUVVideoRTPSink", only fragments that will fit within an outgoing
// RTP packet.  I.e., we implement fragmentation in this separate "YUVFragmenter"
// class, rather than in "YUVVideoRTPSink".
// (Note: This class should be used only by "YUVVideoRTPSink", or a subclass.)

class YUVFragmenter: public FramedFilter {
public:
  YUVFragmenter(UsageEnvironment& env, FramedSource* inputSource,
		    unsigned inputBufferMax, unsigned maxOutputPacketSize);
  virtual ~YUVFragmenter();

  Boolean lastFragmentCompletedNALUnit() const { return fLastFragmentCompletedNALUnit; }

private: // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

private:
  static void afterGettingFrame(void* clientData, unsigned frameSize,
				unsigned numTruncatedBytes,
                                struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame1(unsigned frameSize,
                          unsigned numTruncatedBytes,
                          struct timeval presentationTime,
                          unsigned durationInMicroseconds);
  void reset();

private:
  unsigned fInputBufferSize;
  unsigned fMaxOutputPacketSize;
  unsigned char* fInputBuffer;
  unsigned fNumValidDataBytes;
  unsigned fCurDataOffset;
  unsigned fSaveNumTruncatedBytes;
  Boolean fLastFragmentCompletedNALUnit;
};


////////// YUVVideoRTPSink implementation //////////

YUVVideoRTPSink* YUVVideoRTPSink
::createNew(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat) {
  return new YUVVideoRTPSink(env, RTPgs, rtpPayloadFormat);
}


YUVVideoRTPSink
::YUVVideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat)
  : VideoRTPSink(env, RTPgs, rtpPayloadFormat, 90000,"YUV"),
  fOurFragmenter(NULL), fFmtpSDPLine(NULL) {

}

YUVVideoRTPSink::~YUVVideoRTPSink() {
  fSource = fOurFragmenter; // hack: in case "fSource" had gotten set to NULL before we were called
  delete[] fFmtpSDPLine;
  stopPlaying(); // call this now, because we won't have our 'fragmenter' when the base class destructor calls it later.

  // Close our 'fragmenter' as well:
  Medium::close(fOurFragmenter);
  fSource = NULL; // for the base class destructor, which gets called next
}

Boolean YUVVideoRTPSink::continuePlaying() {
  // First, check whether we have a 'fragmenter' class set up yet.
  // If not, create it now:
  if (fOurFragmenter == NULL) {
    fOurFragmenter = new YUVFragmenter(envir(), fSource, OutPacketBuffer::maxSize,
					   ourMaxPacketSize() - 12/*RTP hdr size*/);
  } else {
    fOurFragmenter->reassignInputSource(fSource);
  }
  fSource = fOurFragmenter;

  // Then call the parent class's implementation:
  return MultiFramedRTPSink::continuePlaying();
}

void YUVVideoRTPSink::doSpecialFrameHandling(unsigned /*fragmentationOffset*/,
						 unsigned char* /*frameStart*/,
						 unsigned /*numBytesInFrame*/,
						 struct timeval framePresentationTime,
						 unsigned /*numRemainingBytes*/) {
  // Set the RTP 'M' (marker) bit iff
  // 1/ The most recently delivered fragment was the end of (or the only fragment of) an NAL unit, and
  // 2/ This NAL unit was the last NAL unit of an 'access unit' (i.e. video frame).
  if (fOurFragmenter != NULL) {
    // This relies on our fragmenter's source being a "YUVVideoStreamFramer".
    if (((YUVFragmenter*)fOurFragmenter)->lastFragmentCompletedNALUnit()) {
      setMarkerBit();
    }
  }

  setTimestamp(framePresentationTime);
}

Boolean YUVVideoRTPSink
::frameCanAppearAfterPacketStart(unsigned char const* /*frameStart*/,
				 unsigned /*numBytesInFrame*/) const {
  return False;
}


////////// YUVFragmenter implementation //////////

YUVFragmenter::YUVFragmenter( UsageEnvironment& env, FramedSource* inputSource,
				     unsigned inputBufferMax, unsigned maxOutputPacketSize)
  : FramedFilter(env, inputSource),
    fInputBufferSize(inputBufferMax+1), fMaxOutputPacketSize(maxOutputPacketSize) {
  fInputBuffer = new unsigned char[fInputBufferSize];
  reset();
}

YUVFragmenter::~YUVFragmenter() {
  delete[] fInputBuffer;
  detachInputSource(); // so that the subsequent ~FramedFilter() doesn't delete it
}

void YUVFragmenter::doGetNextFrame() {
  if (fNumValidDataBytes == 1) {
    // We have no NAL unit data currently in the buffer.  Read a new one:
    fInputSource->getNextFrame(&fInputBuffer[1], fInputBufferSize - 1,
			       afterGettingFrame, this,
			       FramedSource::handleClosure, this);
  } else {
    if (fMaxSize < fMaxOutputPacketSize) { // shouldn't happen
      envir() << "YUVFragmenter::doGetNextFrame(): fMaxSize ("
	      << fMaxSize << ") is smaller than expected\n";
    } else {
      fMaxSize = fMaxOutputPacketSize;
    }

    fLastFragmentCompletedNALUnit = True; // by default
	unsigned numBytesToSend = fNumValidDataBytes - fCurDataOffset;
	if(fMaxSize < numBytesToSend)
	{
		memmove(fTo, &fInputBuffer[fCurDataOffset],fMaxSize);
      	fFrameSize = fMaxSize;
      	fCurDataOffset += fMaxSize;
		fLastFragmentCompletedNALUnit = False;
	}
	else
	{
		memmove(fTo, &fInputBuffer[fCurDataOffset],numBytesToSend);
      	fFrameSize = numBytesToSend;
      	fCurDataOffset += numBytesToSend;
	}

    if (fCurDataOffset >= fNumValidDataBytes) {
      // We're done with this data.  Reset the pointers for receiving new data:
      fNumValidDataBytes = fCurDataOffset = 1;
    }

    // Complete delivery to the client:
    FramedSource::afterGetting(this);
  }
}

void YUVFragmenter::doStopGettingFrames() {
  // Make sure that we don't have any stale data fragments lying around, should we later resume:
  reset();
  FramedFilter::doStopGettingFrames();
}

void YUVFragmenter::afterGettingFrame(void* clientData, unsigned frameSize,
					  unsigned numTruncatedBytes,
					  struct timeval presentationTime,
					  unsigned durationInMicroseconds) {
  YUVFragmenter* fragmenter = (YUVFragmenter*)clientData;
  fragmenter->afterGettingFrame1(frameSize, numTruncatedBytes, presentationTime,
				 durationInMicroseconds);
}

void YUVFragmenter::afterGettingFrame1(unsigned frameSize,
					   unsigned numTruncatedBytes,
					   struct timeval presentationTime,
					   unsigned durationInMicroseconds) {
  fNumValidDataBytes += frameSize;
  fSaveNumTruncatedBytes = numTruncatedBytes;
  fPresentationTime = presentationTime;
  fDurationInMicroseconds = durationInMicroseconds;

  // Deliver data to the client:
  doGetNextFrame();
}

void YUVFragmenter::reset() {
  fNumValidDataBytes = fCurDataOffset = 1;
  fSaveNumTruncatedBytes = 0;
  fLastFragmentCompletedNALUnit = True;
}
