#ifndef CAPTUREGALLERY_H
#define CAPTUREGALLERY_H

#include <qwidget.h>
#include "ui_capturegallerywidget.h"

#include "img_systypes.h"
#include "css.hpp"
#include "ispc/ParameterFileParser.h"

class QSpacerItem;
class QScrollArea;

namespace VisionLive
{

struct RecData;
class ObjectRegistry;
class ProxyHandler;
class CaptureGalleryItem;
class CapturePreview;

class CaptureGallery: public QWidget, public Ui::CaptureGalleryWidget
{
	Q_OBJECT

public:
	CaptureGallery(QWidget *parent = 0); // Class contructor
	~CaptureGallery(); // Class destructor
	void addItem(CaptureGalleryItem *item); // Adds new CaptureGalleryItem to gallery
	QStringList getItemsList(); // Returns list of CaptureGalleryItems names
	void setObjectRegistry(ObjectRegistry *objReg); // Sets ObjectRegistry object
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
	void setCSS(VisionLive::CSS css); // Sets CSS to CaptureGallery and all CaptureGalleryItems
	
private:
	CSS _css; // Holds css type
	ObjectRegistry *_pObjReg; // Holds ObjectRegistry object
	ProxyHandler *_pProxyHandler; // ProxyHandler object
	std::vector<CapturePreview *> _previews; // Holds all open CapturePreview windows
	std::vector<CaptureGalleryItem *> _items; // Holds all CaptureGalleryItem objects in gallery
	QSpacerItem *_pSpacer; // Used to push all CaptureGalleryItem objects to left side of _pDisplayArea
	void initCaptureGallery(); // Initializes CaptureGallery
	void saveDPFMap(IMG_UINT8 *map, int nDefects, const QString &filename = QString());
	void saveParameters(const ISPC::ParameterList &config, const QString &filename = QString());
	void retranslate(); // Retranslates GUI

signals:
	void logError(const QString &error, const QString &src); // Rquests Log to log error
	void logWarning(const QString &warning, const QString &src); // Rquests Log to log warning
	void logMessage(const QString &message, const QString &src); // Rquests Log to log message
	void logAction(const QString &action); // Rquests Log to log action

public slots:
	void open(CaptureGalleryItem *item); // Called to open CaptureGalleryItem in CapturePreview window
	void saveAsJPEG(CaptureGalleryItem *item); // Called to save CaptureGalleryItem as JPEG
	void saveAsFLX(CaptureGalleryItem *item); // Called to save CaptureGalleryItem as FLX
	void saveDPFMap(CaptureGalleryItem *item, int index); // Called to save CaptureGalleryItem's DPF map of index index
	void saveConfiguration(CaptureGalleryItem *item); // Called to save CaptureGalleryItem's configuration
	void remove(CaptureGalleryItem *item); // Called to remove CaptureGalleryItem

	void previewClosed(CapturePreview *preview); // Called when closing CapturePreview window to delete it from _previews
};

} // namespace VisionLive

#endif // HWINFO_H
