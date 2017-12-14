#ifndef CAPTUREGALLERYITEM_H
#define CAPTUREGALLERYITEM_H

#include <qwidget.h>

class QLabel;
class QImage;
class QGridLayout;
class QMenu;
class QAction;

namespace VisionLive
{

struct RecData;

class CaptureGalleryItem: public QWidget
{
		Q_OBJECT

public:
	CaptureGalleryItem(RecData *recData, QWidget *parent = 0); // Class constructor
	~CaptureGalleryItem(); // Class destructor

	const QString &getName() const { return _name; } // Returns showing name
	const QImage *getImage() const { return _pImage; } // Returns poiter to showing image
	const RecData *getRecData() const { return _pRecData; } // Returns pointer to RecData

protected:
	void contextMenuEvent(QContextMenuEvent *event); // Pops up context menu event
	void mouseDoubleClickEvent(QMouseEvent *event); // Calls open() an double clisk

private:
	QGridLayout *_pMainLayout; // CaptureGalleryItem main layaut
	QLabel *_pLiveFeedView; // Displays _pImage
	QLabel *_pName; // Displays _name
	QString _name; // Holds showing name
	QImage *_pImage; // Holds showing image
	RecData *_pRecData; // Holds RecData
	void initCaptureGalleryItem(); // Initializes CaptureGalleryItem

	QMenu *_pContextMenu; // Context menu
	QAction *_pOpen; // Open action
	QAction *_pSaveAsJPEG; // Save as JPEG action
	QAction *_pSaveAsFLX; // Save as FLX action
	QAction *_pSaveConfiguration; // Save configuration
	QAction *_pRemove; // Remove action
	void initContextMenu(); // Initializes ContextMenu

signals:
	void open(CaptureGalleryItem *item); // Requests to CaptureGallery opening in CapturePreview window
	void saveAsJPEG(CaptureGalleryItem *item); // Requests to CaptureGallery saving as JPEG
	void saveAsFLX(CaptureGalleryItem *item); // Requests to CaptureGallery saving as FLX
	void saveConfiguration(CaptureGalleryItem *item); // Requests to CaptureGallery saving as FLX
	void remove(CaptureGalleryItem *item); // Requests to CaptureGallery removing item

public slots:
	void open(); // Emits open() signal
	void saveAsJPEG(); // Emits saveAsJPEG() signal
	void saveAsFLX(); // Emits saveAsFLX() signal
	void saveConfiguration(); // Emits saveConfiguration() signal
	void remove(); // Emits remove() signal
};

} // namespace VisionLive

#endif // CAPTUREGALLERYITEM_H
