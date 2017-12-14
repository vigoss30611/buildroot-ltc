#include "algorithms.hpp"
#include "img_errors.h"

//
// Public Functions
//

QMap<QString, std::pair<QObject *, QString> > VisionLive::Algorithms::getChildrenObjects(QObject *root, const QString &path, const QString &objectNamePrefix, bool recursive)
{
	QMap<QString, std::pair<QObject *, QString> > objList;

	QString prefix = (objectNamePrefix == QString())? objectNamePrefix : ((objectNamePrefix == root->objectName())? QString() : (objectNamePrefix + "_"));

	if(!(root->objectName().startsWith("qt_")) && !(root->objectName().isEmpty()))
	{
		objList.insert(prefix + root->objectName(), std::make_pair(root, path));
	}

	QObjectList childrenObjects = root->children();

	if(recursive)
	{
		for(QObjectList::iterator it = childrenObjects.begin(); it != childrenObjects.end(); it++)
		{
			QMap<QString, std::pair<QObject *, QString> > tmp = getChildrenObjects(*it, path + prefix + root->objectName() + "/", objectNamePrefix);
			for(QMap<QString, std::pair<QObject *, QString> >::iterator it2 = tmp.begin(); it2 != tmp.end(); it2++)
			{
				//if(!(root->objectName().startsWith("qt_")) && !(root->objectName().isEmpty()))
				{
					objList.insert(it2.key(), it2.value());
				}
			}
		}
	}
	else
	{
		for(QObjectList::iterator it = childrenObjects.begin(); it != childrenObjects.end(); it++)
		{
			if(!(root->objectName().startsWith("qt_")) && !(root->objectName().isEmpty()))
			{
				objList.insert((*it)->objectName(), std::make_pair(*it, path));
			}
		}
	}

	return objList;
}

VisionLive::Type VisionLive::Algorithms::getObjectType(QObject *object)
{
	Type ret;

	if(qobject_cast<QSpinBox *>(object)) ret = QSPINBOX;
	else if(qobject_cast<QDoubleSpinBox *>(object)) ret = QDOUBLESPINBOX;
	else if(qobject_cast<QComboBox *>(object)) ret = QCOMBOBOX;
	else if(qobject_cast<QCheckBox *>(object)) ret = QCHECKBOX;
	else if(qobject_cast<QAction *>(object)) ret = QACTION;
	else if(qobject_cast<QPushButton *>(object)) ret = QPUSHBUTTON;
	else if(qobject_cast<QLabel *>(object)) ret = QLABEL;
	else if(qobject_cast<QTextEdit *>(object)) ret = QTEXTEDIT;
	else if(qobject_cast<QLineEdit *>(object)) ret = QLINEEDIT;
	else if(qobject_cast<QRadioButton *>(object)) ret = QRADIOBUTTON;
	else if(qobject_cast<CaptureGalleryItem *>(object)) ret = CAPTUREGALLERYITEM;
	else if(qobject_cast<CapturePreview *>(object)) ret = CAPTUREPREVIEW;
	else ret = UNKNOWN;

	return ret;
}

const QString VisionLive::Algorithms::getTypeName(Type objectType)
{
	switch(objectType)
	{
		case INT: return "INT";
		case DOUBLE: return "DOUBLE";
		case STRING: return "STRING";
		case QSPINBOX: return "QSPINBOX";
		case QDOUBLESPINBOX: return "QDOUBLESPINBOX";
		case QCOMBOBOX: return "QCOMBOBOX";
		case QPUSHBUTTON: return "QPUSHBUTTON";
		case QRADIOBUTTON: return "QRADIOBUTTON";
		case QCHECKBOX: return "QCHECKBOX";
		case QLINEEDIT: return "QLINEEDIT";
		case QACTION: return "QACTION";
		case QLABEL: return "QLABEL";
		case QTEXTEDIT: return "QTEXTEDIT";
		case CAPTUREGALLERYITEM: return "CAPTUREGALLERYITEM";
		case CAPTUREPREVIEW: return "CAPTUREPREVIEW";
		default: return "UNKNOWN";
	}
}

bool VisionLive::Algorithms::isInt(const QString &val)
{
	bool ok;
	val.toInt(&ok);
	return ok;
}

bool VisionLive::Algorithms::isDouble(const QString &val)
{
	bool ok;
	val.toDouble(&ok);
	return ok;
}

bool VisionLive::Algorithms::convert_RGB_to_CV_8UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool bgr, pxlFormat fmt)
{
    int rgb_o[3] { 0, 1, 2 };

    // Swap R with B
    if (bgr)
    {
        rgb_o[1] = 2;
        rgb_o[2] = 1;
    }

    out = cv::Mat(height, width, CV_8UC3);

    if (fmt == RGB_888_24 || fmt == BGR_888_24)
    {
        IMG_UINT8 *data = static_cast<IMG_UINT8 *>(in);

        unsigned int lineAddr = 0;
        unsigned int addr = 0;
        for (unsigned int line = 0; line < height; line++)
        {
            lineAddr = line * width * 3;
            for (unsigned int pack = 0, pixel = 0; pack < width * 3; pack += 3, pixel++)
            {
                addr = lineAddr + pack;
                out.at<cv::Vec3b>(line, pixel).val[rgb_o[0]] = data[addr];
                out.at<cv::Vec3b>(line, pixel).val[rgb_o[1]] = data[addr + 1];
                out.at<cv::Vec3b>(line, pixel).val[rgb_o[2]] = data[addr + 2];
            }
        }
    }
    else if (fmt == RGB_888_32 || fmt == BGR_888_32)
    {
        IMG_UINT32 *data = static_cast<IMG_UINT32 *>(in);

        IMG_UINT32 tmp = 0;
        unsigned int lineAddr = 0;
        unsigned int addr = 0;
        for (unsigned int line = 0; line < height; line++)
        {
            lineAddr = line*width;
            for (unsigned int pack = 0; pack < width; pack++)
            {
                addr = lineAddr + pack;
                tmp = data[addr] & 0xFF0000;
                out.at<cv::Vec3b>(line, pack).val[rgb_o[0]] = static_cast<unsigned char>(tmp >> 16);
                tmp = data[addr] & 0xFF00;
                out.at<cv::Vec3b>(line, pack).val[rgb_o[1]] = static_cast<unsigned char>(tmp >> 8);
                tmp = data[addr] & 0xFF;
                out.at<cv::Vec3b>(line, pack).val[rgb_o[2]] = static_cast<unsigned char>(tmp);
            }
        }
    }
    else if (fmt == RGB_101010_32 || fmt == BGR_101010_32)
    {
        IMG_UINT32 *data = static_cast<IMG_UINT32 *>(in);

        IMG_UINT32 tmp = 0;
        unsigned int lineAddr = 0;
        unsigned int addr = 0;
        for (unsigned int line = 0; line < height; line++)
        {
            lineAddr = line*width;
            for (unsigned int pack = 0; pack < width; pack++)
            {
                addr = lineAddr + pack;
                tmp = data[addr] & 0x3FF00000;
                out.at<cv::Vec3b>(line, pack).val[rgb_o[0]] = static_cast<unsigned char>(tmp >> 22);
                tmp = data[addr] & 0xFFC00;
                out.at<cv::Vec3b>(line, pack).val[rgb_o[1]] = static_cast<unsigned char>(tmp >> 12);
                tmp = data[addr] & 0x3FF;
                out.at<cv::Vec3b>(line, pack).val[rgb_o[2]] = static_cast<unsigned char>(tmp >> 2);
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool VisionLive::Algorithms::convert_YUV_420_422_8bit_to_YUV444_CV_8UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool yvu, pxlFormat fmt)
{
    PIXELTYPE sYUVType;
    if (PixelTransformYUV(&sYUVType, fmt) != IMG_SUCCESS ||
        (fmt != YUV_420_PL12_8 && fmt != YUV_422_PL12_8 &&
         fmt != YVU_420_PL12_8 && fmt != YVU_422_PL12_8))
        return false;

    int yuv_o[3] { 0, 1, 2 };

    // Swap Cb with Cr
    if (yvu)
    {
        yuv_o[1] = 2;
        yuv_o[2] = 1;
    }

    // Packts per line
    unsigned ppl = width / sYUVType.ui8PackedElements;
    if (width % sYUVType.ui8PackedElements) ppl++;
    // Bytes per line
    unsigned bpl = ppl * sYUVType.ui8PackedStride;
    // Offset to CbCr data
    unsigned cbcrOffset = bpl * height / sizeof(IMG_UINT8);

    out = cv::Mat(height, width, CV_8UC3);
    IMG_UINT8 *data = static_cast<IMG_UINT8 *>(in);

    unsigned int line = 0, pack = 0, i = 0, j = 0, packAddr = 0;
    for (line = 0; line < height; line++)
    {
        packAddr = line * ppl;
        for (pack = 0; pack < ppl; pack++)
            out.at<cv::Vec3b>(line, pack).val[yuv_o[0]] = data[packAddr + pack];
    }

    unsigned int cbcrLine = 0, cbcrPack = 0, cbcrPackAddr = 0;
    for (line = 0; line < height / sYUVType.ui8VSubsampling; line++)
    {
        cbcrLine = line * sYUVType.ui8VSubsampling;
        for (pack = 0; pack < ppl / sYUVType.ui8HSubsampling; pack++)
        {
            cbcrPack = pack * sYUVType.ui8HSubsampling;
            cbcrPackAddr = line * ppl + pack * 2; // 2 to move over Cr
            // Populate Cb
            for (i = 0; i < sYUVType.ui8HSubsampling; i++)
            {
                // Populate Cb
                out.at<cv::Vec3b>(cbcrLine, cbcrPack + i).val[yuv_o[1]] = data[cbcrOffset + cbcrPackAddr];
                // Populate Cr
                out.at<cv::Vec3b>(cbcrLine, cbcrPack + i).val[yuv_o[2]] = data[cbcrOffset + cbcrPackAddr + 1];
            }
        }
      
        for (i = 1; i < sYUVType.ui8VSubsampling; i++)
        {
            for (j = 0; j < ppl; j++)
            {
                // Copy Cb line for vertical subsampling
                out.at<cv::Vec3b>(cbcrLine + i, j).val[yuv_o[1]] = out.at<cv::Vec3b>(cbcrLine, j).val[yuv_o[1]];
                // Copy Cr line for vertical subsampling
                out.at<cv::Vec3b>(cbcrLine + i, j).val[yuv_o[2]] = out.at<cv::Vec3b>(cbcrLine, j).val[yuv_o[2]];
            }
        }
    }

    return true;
}

bool VisionLive::Algorithms::convert_YUV_420_422_10bit_to_YUV444_CV_16UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool yvu, pxlFormat fmt)
{
    PIXELTYPE sYUVType;
    if (PixelTransformYUV(&sYUVType, fmt) != IMG_SUCCESS ||
        (fmt != YUV_420_PL12_10 && fmt != YUV_422_PL12_10 &&
        fmt != YVU_420_PL12_10 && fmt != YVU_422_PL12_10))
        return false;

	int yuv_o[3] = {0, 1, 2};

    if (yvu)
    {
        yuv_o[1] = 2;
        yuv_o[2] = 1;
    }

	// Packs per line
	unsigned int ppl = width / sYUVType.ui8PackedElements;
	if(width % sYUVType.ui8PackedElements) ppl++;
	// Bytes per line
	unsigned int bpl = ppl * sYUVType.ui8PackedStride;
	// Where CbCr planes starts
	unsigned int cbcrOffset = bpl * height / sizeof(IMG_UINT32);

	out = cv::Mat(height, bpl, CV_16UC3);
	IMG_UINT32 *data = static_cast<IMG_UINT32 *>(in);

	IMG_UINT16 y1, y2, y3, y4, y5, y6, cb1, cr1, cb2, cr2, cb3, cr3;
	y1 =y2 = y3 = y4 = y5 = y6 = cb1 = cr1 = cb2 = cr2 = cb3 = cr3 = 0;
    IMG_UINT32 tmp = 0;
    unsigned int line = 0, pack = 0, i = 0, j = 0;
    unsigned int packAddr = 0, packPos = 0;
	for(line = 0; line < height; line++)
	{
        packAddr = line*ppl;
        for (pack = 0; pack < ppl; pack++)
        {
            packPos = pack*sYUVType.ui8PackedElements;
            tmp = data[(pack + packAddr) * 2] & 0x3FF00000;
            y1 = tmp >> 20;
            tmp = data[(pack + packAddr) * 2] & 0xFFC00;
            y2 = tmp >> 10;
            tmp = data[(pack + packAddr) * 2] & 0x3FF;
            y3 = tmp;
            tmp = data[(pack + packAddr) * 2 + 1] & 0x3FF00000;
            y4 = tmp >> 20;
            tmp = data[(pack + packAddr) * 2 + 1] & 0xFFC00;
            y5 = tmp >> 10;
            tmp = data[(pack + packAddr) * 2 + 1] & 0x3FF;
            y6 = tmp;

            // Populate Y
            out.at<cv::Vec3w>(line, packPos + 0).val[yuv_o[0]] = y3;
            out.at<cv::Vec3w>(line, packPos + 1).val[yuv_o[0]] = y2;
            out.at<cv::Vec3w>(line, packPos + 2).val[yuv_o[0]] = y1;
            out.at<cv::Vec3w>(line, packPos + 3).val[yuv_o[0]] = y6;
            out.at<cv::Vec3w>(line, packPos + 4).val[yuv_o[0]] = y5;
            out.at<cv::Vec3w>(line, packPos + 5).val[yuv_o[0]] = y4;
        }
	}

    unsigned int cbcrLine = 0;
    unsigned int cbcrLineAddr = 0;
    unsigned int cbcrLinePos = 0;
    unsigned int cbcrLineWidth = 0;
    for (line = 0; line < height / sYUVType.ui8VSubsampling; line++)
    {
        cbcrLine = line * sYUVType.ui8VSubsampling;
        cbcrLineAddr = line * ppl;
        for (pack = 0; pack < ppl; pack++)
        {
            tmp = data[cbcrOffset + (cbcrLineAddr + pack) * 2] & 0x3FF00000;
            cb2 = tmp >> 20;
            tmp = data[cbcrOffset + (cbcrLineAddr + pack) * 2] & 0xFFC00;
            cr1 = tmp >> 10;
            tmp = data[cbcrOffset + (cbcrLineAddr + pack) * 2] & 0x3FF;
            cb1 = tmp;
            tmp = data[cbcrOffset + (cbcrLineAddr + pack) * 2 + 1] & 0x3FF00000;
            cr3 = tmp >> 20;
            tmp = data[cbcrOffset + (cbcrLineAddr + pack) * 2 + 1] & 0xFFC00;
            cb3 = tmp >> 10;
            tmp = data[cbcrOffset + (cbcrLineAddr + pack) * 2 + 1] & 0x3FF;
            cr2 = tmp;

            cbcrLinePos = pack * 3 * sYUVType.ui8HSubsampling;
            for (i = 0; i < sYUVType.ui8HSubsampling; i++)
            {
                // Populate Cb
                out.at<cv::Vec3w>(cbcrLine, cbcrLinePos + i).val[yuv_o[1]] = cb1;
                out.at<cv::Vec3w>(cbcrLine, cbcrLinePos + sYUVType.ui8HSubsampling + i).val[yuv_o[1]] = cb2;
                out.at<cv::Vec3w>(cbcrLine, cbcrLinePos + 2 * sYUVType.ui8HSubsampling + i).val[yuv_o[1]] = cb3;
                // Populate Cr
                out.at<cv::Vec3w>(cbcrLine, cbcrLinePos + i).val[yuv_o[2]] = cr1;
                out.at<cv::Vec3w>(cbcrLine, cbcrLinePos + sYUVType.ui8HSubsampling + i).val[yuv_o[2]] = cr2;
                out.at<cv::Vec3w>(cbcrLine, cbcrLinePos + 2 * sYUVType.ui8HSubsampling + i).val[yuv_o[2]] = cr3;
            }
        }

        cbcrLineWidth = ppl * 3 * sYUVType.ui8HSubsampling;
        for (i = 1; i < sYUVType.ui8VSubsampling; i++)
        {
            for (j = 0; j < cbcrLineWidth; j++)
            {
                // Duplicate Cb line
                out.at<cv::Vec3w>(cbcrLine + i, j).val[yuv_o[1]] = out.at<cv::Vec3w>(cbcrLine, j).val[yuv_o[1]];
                // Duplicate Cr line
                out.at<cv::Vec3w>(cbcrLine + i, j).val[yuv_o[2]] = out.at<cv::Vec3w>(cbcrLine, j).val[yuv_o[2]];
            }
        }
    }

    return true;
}

bool VisionLive::Algorithms::convert_YUV_420_422_10bit_to_YUV444_CV_8UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool yvu, pxlFormat fmt)
{
    cv::Mat yuv444_10bit;
    if (!convert_YUV_420_422_10bit_to_YUV444_CV_16UC3(in, yuv444_10bit, width, height, yvu, fmt))
        return false;
   
    // Normalize to 8 bit
    out = cv::Mat(height, width, CV_8UC3);

    for(unsigned int j = 0; j < height; j++)
    {
        for(unsigned int i = 0; i < width; i++)
        {
            out.at<cv::Vec3b>(j, i).val[0] = static_cast<unsigned char>(yuv444_10bit.at<cv::Vec3w>(j, i).val[0] >> 2);
            out.at<cv::Vec3b>(j, i).val[1] = static_cast<unsigned char>(yuv444_10bit.at<cv::Vec3w>(j, i).val[1] >> 2);
            out.at<cv::Vec3b>(j, i).val[2] = static_cast<unsigned char>(yuv444_10bit.at<cv::Vec3w>(j, i).val[2] >> 2);
        }
    }

    return true;
}

cv::Mat VisionLive::Algorithms::convert_YUV_444_10bit_to_YUV444Mat(void *rawData, int width, int height, bool bYVU, pxlFormat fmt)
{
    cv::Mat ret(height, width, CV_16UC3);

    PIXELTYPE sYUVType;
    if (PixelTransformYUV(&sYUVType, fmt) != IMG_SUCCESS)
    {
        return ret;
    }

    IMG_UINT32 *data = (IMG_UINT32 *)rawData;

    IMG_UINT32 tmp, y, u, v;
    tmp = y = u = v = 0;

    int mask_u = 0xFFC00, shitf_u = 10;
    int mask_y = 0x3FF00000, shift_y = 20;
    if (bYVU)
    {
        mask_u = 0x3FF00000, shitf_u = 20;
        mask_y = 0xFFC00, shift_y = 10;
    }

    for (int line = 0; line < height; line++)
    {
        for (int pixel = 0; pixel < width; pixel++)
        {
            tmp = data[line*width + pixel];
            y = (tmp & mask_y) >> shift_y;
            u = (tmp & mask_u) >> shitf_u;
            v = (tmp & 0x3FF);
            ret.at<cv::Vec3w>(line, pixel).val[0] = (unsigned short)v;
            ret.at<cv::Vec3w>(line, pixel).val[1] = (unsigned short)y;
            ret.at<cv::Vec3w>(line, pixel).val[2] = (unsigned short)u;
        }
    }

    return ret;
}

cv::Mat VisionLive::Algorithms::convert_YUV444Mat_to_RGB888Mat(cv::Mat inputMat, int width, int height, bool bBGR, const double convMatrix[9], const double convInputOff[3])
{
	cv::Mat rgb(height, width, CV_8UC3);

    int rgb_o[3] = {0, 1, 2};

    if(bBGR)
    {
        rgb_o[1] = 2;
        rgb_o[2] = 1;
    }

	double y, u, v;

    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            y = inputMat.at<cv::Vec3w>(j, i).val[0]>>2; // Extra shift right by 2 to convert to 8bit value
            u = inputMat.at<cv::Vec3w>(j, i).val[1]>>2; // Extra shift right by 2 to convert to 8bit value
            v = inputMat.at<cv::Vec3w>(j, i).val[2]>>2; // Extra shift right by 2 to convert to 8bit value
           
            y += convInputOff[0];
            u += convInputOff[1];
            v += convInputOff[2];
                
            for(int k = 0; k < 3; k++)
            {
                double val;
                
                val = y*convMatrix[3*k+0]
                    + u*convMatrix[3*k+1]
                    + v*convMatrix[3*k+2];

                rgb.at<cv::Vec3b>(j, i).val[rgb_o[k]] = (unsigned char)std::max(std::min(255.0, floor(val)), (double)0.0f);
            }
        }
    }

    return rgb;
}

QMap<int, std::pair<int, QRgb> > VisionLive::Algorithms::generateVectorScope(QImage *image, bool full)
{
	QMap<int, std::pair<int, QRgb> > ret;

	double r, g, b;
	r = g = b = 0;

	double rP, gP, bP;
	rP = gP = bP = 0;

	double cMax, cMin;
	cMax = cMin = 0;

	double delta = 0;

	int h, s, v;
	h = s = v = 0;

	if(full)
	{
		for(int i = 0; i < image->height(); i++)
		{
			for(int j = 0; j < image->width(); j++)
			{
				QRgb pixelValue = image->pixel(j, i);
				QColor c(pixelValue);
				c.getHsv(&h, &s, &v);

				ret.insert((int)h, std::make_pair((int)s, pixelValue));
			}
		}
	}
	else
	{
		for(int i = 0; i < image->height()-2; i+=2)
		{
			for(int j = 0; j < image->width()-2; j+=2)
			{
				QRgb pixelValue = image->pixel(j, i);
				QColor c(pixelValue);
				c.getHsv(&h, &s, &v);

				// Show only pixels with saturation abowe 20 
				// Show only pixels one per HUE (with most saturation)
				if(s > 20) 
				{
					QMap<int, std::pair<int, QRgb> >::iterator it = ret.find((int)h);
					if(it == ret.end())
					{
						ret.insert((int)h, std::make_pair((int)s, pixelValue));
					}
					else
					{
						if(std::max(it.value().first, (int)s) == (int)s)
						{
							*it = std::make_pair((int)s, pixelValue);
						}
					}
				}
			}
		}
	}

	return ret;
}

QMap<QString, std::vector<std::vector<int> > > VisionLive::Algorithms::generateLineView(QImage *image, QPoint point, unsigned int step)
{
	QMap<QString, std::vector<std::vector<int> > > ret;

	if(point.x() > image->width() || point.x() < 0 ||
		point.y() > image->height() || point.y() < 0)
	{
		return ret;
	}

	// Removing that because I rather take r, g and b from QImage than real data
	/*int nChannels = (imgData.mosaic != MOSAIC_NONE)? 4 : 3;

	std::vector<cv::Mat> channelPlains;
	cv::split(imgData.data, channelPlains);

	switch(channelPlains[0].depth())
	{
	case CV_32F:
		for(int i = 0; i < imgData.data.rows; i+=3)
		{
			rVertLine.push_back((int)channelPlains[0].at<float>(i, point.x()));
			gVertLine.push_back((int)channelPlains[1].at<float>(i, point.x()));
			bVertLine.push_back((int)channelPlains[2].at<float>(i, point.x()));
		}
		for(int i = 0; i < imgData.data.cols; i+=3)
		{
			rHorizLine.push_back((int)channelPlains[0].at<float>(point.y(), i));
			gHorizLine.push_back((int)channelPlains[1].at<float>(point.y(), i));
			bHorizLine.push_back((int)channelPlains[2].at<float>(point.y(), i));
		}
		break;
	case CV_8U:
		for(int i = 0; i < imgData.data.rows; i+=3)
		{
			rVertLine.push_back((int)channelPlains[0].at<unsigned char>(i, point.x()));
			gVertLine.push_back((int)channelPlains[1].at<unsigned char>(i, point.x()));
			bVertLine.push_back((int)channelPlains[2].at<unsigned char>(i, point.x()));
		}
		for(int i = 0; i < imgData.data.cols; i+=3)
		{
			rHorizLine.push_back((int)channelPlains[0].at<unsigned char>(point.y(), i));
			gHorizLine.push_back((int)channelPlains[1].at<unsigned char>(point.y(), i));
			bHorizLine.push_back((int)channelPlains[2].at<unsigned char>(point.y(), i));
		}
		break;
	}*/

	QColor c;

	std::vector<int> rHorizLine;
	std::vector<int> gHorizLine;
	std::vector<int> bHorizLine;

	for(int i = 0; i < image->width(); i += step)
	{
		c = image->pixel(i, point.y());
		rHorizLine.push_back(c.red());
		gHorizLine.push_back(c.green());
		bHorizLine.push_back(c.blue());
	}

	std::vector<std::vector<int> > horizLine;
	horizLine.push_back(rHorizLine);
	horizLine.push_back(gHorizLine);
	horizLine.push_back(bHorizLine);

	ret.insert("Horizontal", horizLine);

	std::vector<int> rVertLine;
	std::vector<int> gVertLine;
	std::vector<int> bVertLine;

	for(int i = 0; i < image->height(); i += step)
	{
		c = image->pixel(point.x(), i);
		rVertLine.push_back(c.red());
		gVertLine.push_back(c.green());
		bVertLine.push_back(c.blue());
	}

	std::vector<std::vector<int> > vertLine;
	vertLine.push_back(rVertLine);
	vertLine.push_back(gVertLine);
	vertLine.push_back(bVertLine);

	ret.insert("Vertical", vertLine);

	return ret;
}

QMap<QString, std::vector<std::vector<int> > > VisionLive::Algorithms::generateLineView(cv::Mat imgData, MOSAICType mos, QPoint point, unsigned int step, double multFactor)
{
	QMap<QString, std::vector<std::vector<int> > > ret;

	if(point.x() > imgData.cols || point.x() < 0 ||
		point.y() > imgData.rows || point.y() < 0)
	{
		return ret;
	}

	int nChannels = (mos != MOSAIC_NONE)? 4 : 3;

	std::vector<cv::Mat> channelPlains;
	cv::split(imgData, channelPlains);

	int color1, color2, color3, color4;

	std::vector<int> rHorizLine;
	std::vector<int> gHorizLine;
	std::vector<int> g2HorizLine;
	std::vector<int> bHorizLine;

	if(nChannels == 3)
	{
		for(int i = 0; i < imgData.cols; i += step)
		{
			color1 = imgData.at<cv::Vec3b>(point.y(), i).val[0];
			color2 = imgData.at<cv::Vec3b>(point.y(), i).val[1];
			color3 = 0;
			color4 = imgData.at<cv::Vec3b>(point.y(), i).val[2];

			rHorizLine.push_back(color1);
			gHorizLine.push_back(color2);
			g2HorizLine.push_back(color3);
			bHorizLine.push_back(color4);
		}
	}
	else if(nChannels == 4)
	{
		for(int i = 0; i < imgData.cols; i += step)
		{
			color1 = imgData.at<cv::Vec4f>(point.y(), i).val[0]*multFactor;
			color2 = imgData.at<cv::Vec4f>(point.y(), i).val[1]*multFactor;
			color3 = imgData.at<cv::Vec4f>(point.y(), i).val[2]*multFactor;
			color4 = imgData.at<cv::Vec4f>(point.y(), i).val[3]*multFactor;

			rHorizLine.push_back(color1);
			gHorizLine.push_back(color2);
			g2HorizLine.push_back(color3);
			bHorizLine.push_back(color4);
		}
	}

	std::vector<std::vector<int> > horizLine;
	horizLine.push_back(rHorizLine);
	horizLine.push_back(gHorizLine);
	horizLine.push_back(g2HorizLine);
	horizLine.push_back(bHorizLine);

	ret.insert("Horizontal", horizLine);

	std::vector<int> rVertLine;
	std::vector<int> gVertLine;
	std::vector<int> g2VertLine;
	std::vector<int> bVertLine;

	if(nChannels == 3)
	{
		for(int i = 0; i < imgData.rows; i += step)
		{
			color1 = imgData.at<cv::Vec3b>(i, point.x()).val[0];
			color2 = imgData.at<cv::Vec3b>(i, point.x()).val[1];
			color3 = 0;
			color4 = imgData.at<cv::Vec3b>(i, point.x()).val[2];

			rVertLine.push_back(color1);
			gVertLine.push_back(color2);
			g2VertLine.push_back(color3);
			bVertLine.push_back(color4);
		}
	}
	else if(nChannels == 4)
	{
		for(int i = 0; i < imgData.rows; i += step)
		{
			color1 = imgData.at<cv::Vec4f>(i, point.x()).val[0]*multFactor;
			color2 = imgData.at<cv::Vec4f>(i, point.x()).val[1]*multFactor;
			color3 = imgData.at<cv::Vec4f>(i, point.x()).val[2]*multFactor;
			color4 = imgData.at<cv::Vec4f>(i, point.x()).val[3]*multFactor;

			rVertLine.push_back(color1);
			gVertLine.push_back(color2);
			g2VertLine.push_back(color3);
			bVertLine.push_back(color4);
		}
	}

	std::vector<std::vector<int> > vertLine;
	vertLine.push_back(rVertLine);
	vertLine.push_back(gVertLine);
	vertLine.push_back(g2VertLine);
	vertLine.push_back(bVertLine);

	ret.insert("Vertical", vertLine);

	return ret;
}

std::vector<int> VisionLive::Algorithms::generatePixelInfo(QImage *image, QPoint point)
{
	std::vector<int> ret;

	if(point.x() > image->width() || point.x() < 0 ||
		point.y() > image->height() || point.y() < 0)
	{
		return ret;
	}

	QColor c;

	c = image->pixel(point.x(), point.y());
	ret.push_back(c.red());
	ret.push_back(c.green());
	ret.push_back(c.blue());

	return ret;
}