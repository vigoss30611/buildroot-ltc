#ifndef ALGORITHMS_H
#define ALGORITHMS_H

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

class QStringList;
class QRadioButton;
class QCheckbox;
class QSpinBox;
class QComboBox;
class QAction;
class QPushButton;
class QTextEdit;
class QLineEdit;
class QLabel;

namespace VisionLive
{

enum ImageType
{
	DE,
	RGB,
	YUV
};

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
	QLABEL
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
	static cv::Mat convert_YUV_420_422_10bit_to_YUV444Mat(void *rawData, int width, int height, bool bYVU, pxlFormat fmt);
	static cv::Mat convert_YUV444Mat_to_RGB888Mat(cv::Mat, int width, int height, bool bBGR, const double convMatrix[9], const double convInputOff[3]);
	static QMap<int, std::pair<int, QRgb> > generateVectorScope(QImage *image, bool full);
	static QMap<QString, std::vector<std::vector<int> > > generateLineView(QImage *image, QPoint point, unsigned int step = 1);
	static std::vector<int> generatePixelInfo(QImage *image, QPoint point);
};

} // namespace VisionLive

#endif // ALGORITHMS_H
