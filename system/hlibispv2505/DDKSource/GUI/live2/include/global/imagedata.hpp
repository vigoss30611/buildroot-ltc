#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "buffer.hpp"
#include "ispc/Shot.h"

namespace VisionLive
{

class ImageData: public CVBuffer
{
public:
	ImageData(); // Class constructor
	int bitdepth; // Image bitdepth
	bool valid; // Indicates ImageData is valid
	ISPC::Buffer origData; // Originall received data (before any format conversions)
};

} // namespace VisionLive

#endif // IMAGEDATA_H
