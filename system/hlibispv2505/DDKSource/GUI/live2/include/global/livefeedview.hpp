#ifndef LIVEFEEDVIEW_H
#define LIVEFEEDVIEW_H

#include "algorithms.hpp"

#include <qgraphicsview.h>
#include <qwidget.h>

class QMenu;

namespace VisionLive
{

class LiveFeedView: public QGraphicsView
{
	Q_OBJECT

public:
	LiveFeedView(QWidget *parent = 0); // Class constructor
	~LiveFeedView(); // Class destructor
	void setLiveViewOption(int opt);

protected:
	void resizeEvent(QResizeEvent *event); // Called on resize event
	void contextMenuEvent(QContextMenuEvent *event); // Called on right click event
	void wheelEvent(QWheelEvent *event); // Called on mouse wheel move event
	void mousePressEvent(QMouseEvent *event); // Called on mouse pressed event
	void mouseMoveEvent(QMouseEvent *event); // Called on mouse move event
	void mouseReleaseEvent(QMouseEvent *event); // Called on mouse released event

private:
	enum LineViewOption {None, Horizontal, Vertical};
	LineViewOption _lineViewOption;
	QGraphicsLineItem *_pLineViewLineItem;

	bool _selecting;
	bool _moving;
	bool _leftMouseButtonPressed;
	bool _rightMouseButtonPressed;
	QPoint _selectionStartPoint;
	QPoint _selectionEndPoint;
	QPoint _imageMoveStartPoint;
	QGraphicsRectItem *_pSelectionRect;

	// Image params
	int _type;
	int _format;
	int _mosaic;
	int _width;
	int _height;
	bool _firstFrameReceived;

	// FPS params
	double _fps;

	// Zoom params
	bool _enableZoom; // Enabled zooming
    double _currentZoom; // Current zoom value
    double _minZoom; // Minimum possible zoom
    double _maxZoom; // Maximum possible zoom
    double _stepZoom; // Zoom step when a wheel event is captured

	QGraphicsScene *_pScene; // Scene object
	void initScene(); // Initiates Scene

	QGraphicsPixmapItem *_pImageItem; // Image item
	void initImageItem(); // Initiates ImageItem

	QGraphicsTextItem *_pImageImfoItem; // ImageImfo item
	void initImageImfoItem(); // Initiates ImageImfoItem

	QGraphicsTextItem *_pFPSInfoItem; // FPS info text item
	void initFPSInfoItem(); // Initiates FPSInfoItem

	QMenu *_pContextMenu; // Context menu
	void initContextMenu(); // Initiates context menu

	QPointF _marker; // Position of selected pixel (selected by left click)
	QGraphicsRectItem *_pMarkerItem; // Marker item (of posittion of _marker)
	void initMarkerItem(); // Initiates MarkerItem

	QGraphicsTextItem *_pPixelInfoItem; // Marked pixel info text item
	void initPixelInfoItem(); // Initiates PixelInfoItem

	QGraphicsTextItem *_pAvgItem; // Selected area average pixel value text item
	QGraphicsTextItem *_pSNRItem; // Selected area signal to noise ratio text item
	void initAreaStatisticsItems(); // Initiates AreaStatisticsItem

	QGraphicsTextItem *_pAWBItem; // AWB Debug Mode indicator
	void initAWBItems(); // Initiates AWBItem

signals:
	void logError(const QString &error, const QString &src); // Called to log an error to LogView
	void logWarning(const QString &warning, const QString &src); // Called to log an warning to LogView
	void logMessage(const QString &message, const QString &src); // Called to log an info to LogView
	void logAction(const QString &action); // Called to log an action to LogView

	void CS_saveAsJPEG(const QString &filename); // Called to request ImageHandler to save image as JPEG
	void CS_saveAsFLX(const QString &filename, ImageType type); // Called to request ImageHandler to save image as FLX
	void CS_setMarker(QPointF point); // Called to update ImageHandler marker position
	void CS_setSelection(QRect area, bool genStats); // Called to update ImageHandler selection area and enable/disable statistics generation for that area

public slots:
	void imageReceived(QImage *pImage, int type, int format, int mosaic, int width, int height, bool awbDebugMode); // Called when ImageHandler finished receiving new frame
	void pixelInfoReceived(QPoint pixel, std::vector<int> pixelInfo); // Called when ImageHandler finished calculating pixel info
	void areaStatisticsReceived(std::vector<double> areaStats); // Called when ImageHandler finished generating area statistics
	void fpsInfoReceived(double fps); // Called when ImageHandler finished calculating fps info
};

} // namespace VisionLive

#endif // LIVEFEEDVIEW_H
