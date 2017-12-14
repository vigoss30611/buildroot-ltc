#include "algorithms.hpp"
#include "img_errors.h"

//
// Public Functions
//

QMap<QString, std::pair<QObject *, QString> > VisionLive::Algorithms::getChildrenObjects(QObject *root, const QString &path, const QString &objectNamePrefix, bool recursive)
{
	QMap<QString, std::pair<QObject *, QString> > objList;

	std::string o = std::string(root->objectName().toLatin1()).c_str();
	std::string p = std::string(objectNamePrefix.toLatin1()).c_str();
	int i = root->objectName().compare(objectNamePrefix);

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
				objList.insert(it2.key(), it2.value());
			}
		}
	}
	else
	{
		for(QObjectList::iterator it = childrenObjects.begin(); it != childrenObjects.end(); it++)
		{
			objList.insert((*it)->objectName(), std::make_pair(*it, path));
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

cv::Mat VisionLive::Algorithms::convert_YUV_420_422_10bit_to_YUV444Mat(void *rawData, int width, int height, bool bYVU, pxlFormat fmt)
{
	PIXELTYPE sYUVType;
	if(PixelTransformYUV(&sYUVType, fmt) != IMG_SUCCESS)
	{
		return cv::Mat(height, width, CV_16UC3);
	}

	int yuv_o[3] = {0, 1, 2};

    if(bYVU)
    {
        yuv_o[1] = 2;
        yuv_o[2] = 1;
    }

	// Packs per line
	unsigned ppl = width/sYUVType.ui8PackedElements;
	if(width%sYUVType.ui8PackedElements) ppl++;
	// Bytes per line
	unsigned bpl = ppl*sYUVType.ui8PackedStride;
	// Where CbCr planes starts
	unsigned cbcrOffset = bpl*height/sizeof(IMG_UINT32);

	cv::Mat ret = cv::Mat(height, bpl, CV_16UC3);

	IMG_UINT32 *data = (IMG_UINT32 *)rawData;

	IMG_UINT16 y1, y2, y3, y4, y5, y6, cb1, cr1, cb2, cr2, cb3, cr3;
	y1 =y2 = y3 = y4 = y5 = y6 = cb1 = cr1 = cb2 = cr2 = cb3 = cr3 = 0;
	IMG_UINT32 tmp = 0;

	int cbcrLine = 0;

	for(int line = 0; line < height; line++)
	{
		for(unsigned int pack = 0; pack < ppl; pack++)
		{
			tmp = data[(pack+line*ppl)*2] & 0x3FF00000;
			tmp = tmp >> 20;
			//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
			y1 = tmp;
			tmp = data[(pack+line*ppl)*2] & 0xFFC00;
			tmp = tmp >> 10;
			//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
			y2 = tmp;
			tmp = data[(pack+line*ppl)*2] & 0x3FF;
			//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
			y3 = tmp;
			tmp = data[(pack+line*ppl)*2+1] & 0x3FF00000;
			tmp = tmp >> 20;
			//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
			y4 = tmp;
			tmp = data[(pack+line*ppl)*2+1] & 0xFFC00;
			tmp = tmp >> 10;
			//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
			y5 = tmp;
			tmp = data[(pack+line*ppl)*2+1] & 0x3FF;
			//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
			y6 = tmp;

			// Populate Y
			ret.at<cv::Vec3w>(line, pack*sYUVType.ui8PackedElements+0).val[yuv_o[0]] = y3;
			ret.at<cv::Vec3w>(line, pack*sYUVType.ui8PackedElements+1).val[yuv_o[0]] = y2;
			ret.at<cv::Vec3w>(line, pack*sYUVType.ui8PackedElements+2).val[yuv_o[0]] = y1;
			ret.at<cv::Vec3w>(line, pack*sYUVType.ui8PackedElements+3).val[yuv_o[0]] = y6;
			ret.at<cv::Vec3w>(line, pack*sYUVType.ui8PackedElements+4).val[yuv_o[0]] = y5;
			ret.at<cv::Vec3w>(line, pack*sYUVType.ui8PackedElements+5).val[yuv_o[0]] = y4;


			if(line < height/sYUVType.ui8VSubsampling)
			{
				tmp = data[(pack+line*ppl)*2+cbcrOffset] & 0x3FF00000;
				tmp = tmp >> 20;
				//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
				cb2 = tmp;
				tmp = data[(pack+line*ppl)*2+cbcrOffset] & 0xFFC00;
				tmp = tmp >> 10;
				//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
				cr1 = tmp;
				tmp = data[(pack+line*ppl)*2+cbcrOffset] & 0x3FF;
				//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
				cb1 = tmp;
				tmp = data[(pack+line*ppl)*2+1+cbcrOffset] & 0x3FF00000;
				tmp = tmp >> 20;
				//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
				cr3 = tmp;
				tmp = data[(pack+line*ppl)*2+1+cbcrOffset] & 0xFFC00;
				tmp = tmp >> 10;
				//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
				cb3 = tmp;
				tmp = data[(pack+line*ppl)*2+1+cbcrOffset] & 0x3FF;
				//tmp = tmp >> 2; // Extra shift right by 2 to convert to 8bit value
				cr2 = tmp;

				// Populate U
				for(int i = 0; i < sYUVType.ui8HSubsampling; i++)
				{
					ret.at<cv::Vec3w>(cbcrLine, pack*3*sYUVType.ui8HSubsampling+0*sYUVType.ui8HSubsampling+i).val[yuv_o[1]] = cb1;
				}
				for(int i = 0; i < sYUVType.ui8HSubsampling; i++)
				{
					ret.at<cv::Vec3w>(cbcrLine, pack*3*sYUVType.ui8HSubsampling+1*sYUVType.ui8HSubsampling+i).val[yuv_o[1]] = cb2;
				}
				for(int i = 0; i < sYUVType.ui8HSubsampling; i++)
				{
					ret.at<cv::Vec3w>(cbcrLine, pack*3*sYUVType.ui8HSubsampling+2*sYUVType.ui8HSubsampling+i).val[yuv_o[1]] = cb3;
				}

				// Populate V
				for(int i = 0; i < sYUVType.ui8HSubsampling; i++)
				{
					ret.at<cv::Vec3w>(cbcrLine, pack*3*sYUVType.ui8HSubsampling+0*sYUVType.ui8HSubsampling+i).val[yuv_o[2]] = cr1;
				}
				for(int i = 0; i < sYUVType.ui8HSubsampling; i++)
				{
					ret.at<cv::Vec3w>(cbcrLine, pack*3*sYUVType.ui8HSubsampling+1*sYUVType.ui8HSubsampling+i).val[yuv_o[2]] = cr2;
				}
				for(int i = 0; i < sYUVType.ui8HSubsampling; i++)
				{
					ret.at<cv::Vec3w>(cbcrLine, pack*3*sYUVType.ui8HSubsampling+2*sYUVType.ui8HSubsampling+i).val[yuv_o[2]] = cr3;
				}
			}
		}
		if(line < height/sYUVType.ui8VSubsampling)
		{
			// Duplicate U line
			for(int i = 1; i < sYUVType.ui8VSubsampling; i++)
			{
				for(unsigned int j = 0; j < ppl*3*sYUVType.ui8HSubsampling; j++)
				{
					ret.at<cv::Vec3w>(cbcrLine+i, j).val[yuv_o[1]] = ret.at<cv::Vec3w>(cbcrLine, j).val[yuv_o[1]];
				}
			}

			// Duplicate V line
			for(int i = 1; i < sYUVType.ui8VSubsampling; i++)
			{
				for(unsigned int j = 0; j < ppl*3*sYUVType.ui8HSubsampling; j++)
				{
					ret.at<cv::Vec3w>(cbcrLine+i, j).val[yuv_o[2]] = ret.at<cv::Vec3w>(cbcrLine, j).val[yuv_o[2]];
				}
			}

			cbcrLine += sYUVType.ui8VSubsampling;
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