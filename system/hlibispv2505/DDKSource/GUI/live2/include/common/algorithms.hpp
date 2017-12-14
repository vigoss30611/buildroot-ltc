#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "capturegalleryitem.hpp"
#include "capturepreview.hpp"

#include <qobject.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qaction.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qlabel.h>

#include "cv.h"
#include "felixcommon/pixel_format.h"

namespace VisionLive
{

enum ImageType
{
	DE,
	RGB,
	YUV,
	HDR,
	RAW2D
};

static QString ImageTypeString(VisionLive::ImageType type)
{
	QString ret;

	switch(type)
	{
	case DE: ret =  QStringLiteral("DE"); break;
	case RGB: ret = QStringLiteral("RGB"); break;
	case YUV: ret = QStringLiteral("YUV"); break;
	case HDR: ret = QStringLiteral("HDR"); break;
	case RAW2D: ret = QStringLiteral("RAW2D"); break;
	default: ret = QStringLiteral("");
	}

	return ret;
}

enum Type
{
	UNKNOWN,
	INT,
	DOUBLE,
	STRING,
	QSPINBOX,
	QDOUBLESPINBOX,
	QCOMBOBOX,
	QPUSHBUTTON,
	QRADIOBUTTON,
	QCHECKBOX,
	QLINEEDIT,
	QTEXTEDIT,
	QACTION,
	QLABEL,
	CAPTUREGALLERYITEM,
	CAPTUREPREVIEW
};

enum HandlerState
{
	DISCONNECTED,
	CONNECTING,
	CONNECTED,
};

class Algorithms: public QObject
{
public:
	static QMap<QString, std::pair<QObject *, QString> > getChildrenObjects(QObject *root, const QString &path = QString(), const QString &objectNamePrefix = QString(), bool recursive = true);
	static Type getObjectType(QObject *object);
	static const QString getTypeName(Type objectType);
	static bool isInt(const QString &val);
	static bool isDouble(const QString &val);
    static bool convert_RGB_to_CV_8UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool bgr, pxlFormat fmt);
    static bool convert_YUV_420_422_8bit_to_YUV444_CV_8UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool yvu, pxlFormat fmt);
    static bool convert_YUV_420_422_10bit_to_YUV444_CV_16UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool yvu, pxlFormat fmt);
    static bool convert_YUV_420_422_10bit_to_YUV444_CV_8UC3(void *in, cv::Mat &out, unsigned int width, unsigned int height, bool yvu, pxlFormat fmt);
    static cv::Mat convert_YUV_444_10bit_to_YUV444Mat(void *rawData, int width, int height, bool bYVU, pxlFormat fmt);
	static cv::Mat convert_YUV444Mat_to_RGB888Mat(cv::Mat, int width, int height, bool bBGR, const double convMatrix[9], const double convInputOff[3]);
	static QMap<int, std::pair<int, QRgb> > generateVectorScope(QImage *image, bool full);
	static QMap<QString, std::vector<std::vector<int> > > generateLineView(QImage *image, QPoint point, unsigned int step = 1); // Basis on QImage
	static QMap<QString, std::vector<std::vector<int> > > generateLineView(cv::Mat imageData, MOSAICType mos, QPoint point, unsigned int step = 1, double multFactor = 1.0); // Basis on real data
	static std::vector<int> generatePixelInfo(QImage *image, QPoint point);
};

} // namespace VisionLive

#endif // ALGORITHMS_H
