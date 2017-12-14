#ifndef CAPTUREPREVIEW_H
#define CAPTUREPREVIEW_H

#include "ui_capturepreview.h"

class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QGraphicsLineItem;
class QGraphicsTextItem;
class QGraphicsScene;
class QComboBox;

namespace VisionLive
{

struct RecData;
class CaptureGalleryItem;
class CustomGraphicsView;
class VectorScope;
class Histogram;
class LineView;

class CapturePreview: public QMainWindow, public Ui::CapturePreviewWidget
{
	Q_OBJECT

public:
	CapturePreview(CaptureGalleryItem *item, QMainWindow *parent = 0); // Class constructor
	~CapturePreview(); // Class destructor

protected:
	void closeEvent(QCloseEvent *event); // Called on closing CapturePreview window
	
private:
	CustomGraphicsView *_pImageView; // Holds GraphicsView object displaying image
	QGraphicsScene *_pImageScene; // Scene displaying QGraphicsPixmapItem image
	QGraphicsPixmapItem *_pImageItem; // Holds pixmap of QGraphicsPixmapItem image
	void initCapturePreview(); // Initializes CapturePreview

	CaptureGalleryItem *_pItem; // Holds pointer to QGraphicsPixmapItem

	int _nFrames; // Holds number of frames taken from QGraphicsPixmapItem RecData
	int _currentFrame; // Holds index of currently displayed frame
	QString _name; // Holds name taken from QGraphicsPixmapItem RecData

	std::vector<Histogram *> _Histograms; // Holds all Histogram widgets
	void initHistograms(); // Displays Histograms widget in Histogram tab
	void showHistogram(bool option); // Shows/Hides Histogram widget

	std::vector<std::vector<QGraphicsRectItem *> > _DPFPoints; // Holds all DPF points items
	bool _DPFDisplayed; // Indicates if DPF points are being displayed
	void initDPFMaps(); // Displays DPF maps for all frames into DPF map tab
	void showDPF(bool option); // Shows/Hides DPF points

	void initConfiguration(); // Displays Configurations for all frames info Configuration tab

	VectorScope * _pVectorScope; // VectorScope widget
	void initVectorScope(); // Displays VectorScope widget in Vectorscope tab
	void showVectorScope(bool option); // Shows/Hides VectorScope widget

	// ShowViewControl menu and actions
	QToolBar *_pShowViewControl;
	QAction *_pHistogramAction;
	QAction *_pVectorScopeAction;
	QAction *_pLineViewAction;
	QComboBox *_pLineViewOption;
	QAction *_pDPFAction;
	void initShowViewControl(); // Initiates ShowViewControl widget

	QPointF _marker; // Position of selected pixel (selected by left click)
	QGraphicsRectItem *_pMarkerItem; // Marker item (of posittion of _marker)
	void initMarkerItem(); // Initiates MarkerItem

	QGraphicsTextItem *_pPixelInfoItem; // Marked pixel info text item
	void initPixelInfoItem(); // Initiates PixelInfoItem
	void updatePixelInfo(); // Updates pixel info when _marker coordinates change

	enum LineViewOption {None, Horizontal, Vertical}; // LineView widget line option
	LineViewOption _lineViewOption; // Inasance of LineViewOption
	QGraphicsLineItem *_pLineViewLineItem; // Shows current LineView option
	void setLineViewOption(int opt); // Switches between LineViewOption

	LineView *_pLineView; // LineView widget
	void initLineView(); // Initializes LineView widget
	void showLineView(bool option); // Shows/Hides LineView widget
	void updateLineView(); // Updates data od LineView widget

	void retranslate(); // Retranslates GUI

signals:
	void previewClosed(CapturePreview *preview); // Requests from CaptureGallery to delete this preview
	void saveAsJPEG(CaptureGalleryItem *item); // Requests CaptureGallery to save CaptureGalleryItem as JPEG
	void saveAsFLX(CaptureGalleryItem *item); // Requests CaptureGallery to save CaptureGalleryItem as FLX
	void saveDPFMap(CaptureGalleryItem *item, int index); // Requests CaptureGallery to save CaptureGalleryItem's DPF map of frame index
	void saveConfiguration(CaptureGalleryItem *item); // Requests CaptureGallery to save CaptureGalleryItem's configuration

private slots:
	void setPreviousFrame(); // Sets previous frame
	void setNextFrame(); // Setx next frame
	void changeFrame(int frameIndex); // Changes showing frame of CaptureGalleryItem
	void showDPFPoints(bool option); // Shows DPF points on displayed frame image
	void changeShowViewWidget(); // Switches between displayd widgets
	void markerPointChanged(QPointF pos, Qt::MouseButton button); // Called when _pImageView been clicked
	void lineViewOptionChanged(); // Called when _pLineViewOption curent index changed
	void showProgress(int percent); // Called from updateLineView() to indicate update progress
	void saveAsJPEG(); // Called to save CaptureGalleryItem as JPEG
	void saveAsFLX(); // Called to save CaptureGalleryItem as FLX
	void saveDPFMap(); // Called to save DPF out map
	void saveConfiguration(); // Called to save configuration

};

} // namespace VisionLive

#endif // CAPTUREPREVIEW_H
