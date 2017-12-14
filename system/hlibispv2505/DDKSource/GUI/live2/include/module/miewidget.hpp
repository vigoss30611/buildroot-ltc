#ifndef MODULEMIEWIDGET_H
#define MODULEMIEWIDGET_H

#include <qwidget.h>
#include "ui_miewidget.h"

// #include "felix_hw_info.h"
#define MIE_NUM_SLICES 4

class QGraphicsView;
class QGraphicsScene;
class QGraphicsItem;
class QGraphicsRectItem;
class QGraphicsEllipseItem;
class QGraphicsPolygonItem;

namespace VisionLive
{

class CustomGraphicsView;

class MIEWidget : public QWidget, public Ui::MIEWidget
{
	Q_OBJECT

public:
    MIEWidget(QWidget *parent = 0); // Class constructor
    ~MIEWidget(); // Class destructor
	void retranslate(); // Retranslates GUI
	void updatePixelInfo(QPoint pixel, std::vector<int> pixelInfo); // Called from MIE to update pixel info

protected:
	void initMIEWidget(); // Initializes MIEWidget

	CustomGraphicsView *_pMIEWidget_lumaView_gv;
	QGraphicsScene *_pLumaScene; // Holds LumaView scene
	std::vector<QGraphicsRectItem *> _lumaSliceItems; // Holds all luma slice rectangle position items
	double _lumaSlice[MIE_NUM_SLICES];
	void initLumaView(); // Initializes LumaView

	std::vector<CustomGraphicsView *> _centerInputColourView;
    std::vector<QGraphicsScene *> _centerInputScene;
    std::vector<QGraphicsRectItem *> _centerInputColour;
	void initCenterInputColorView();

    std::vector<CustomGraphicsView *> _centerOutputColourView;
    std::vector<QGraphicsScene *> _centerOutputScene;
    std::vector<QGraphicsRectItem *> _centerOutputColour;
	void initCenterOutputColorView();

	CustomGraphicsView *_pMIEWidget_chromaView_gv;
	QGraphicsScene *_pChromaScene;
    QGraphicsEllipseItem *_pChromaCenter;
	QGraphicsPolygonItem *_chromaDiamond[MIE_NUM_SLICES];
    QGraphicsItem *_pLineToCenter;
	void initChromaView();

private:
	std::vector<int> _pixelInfo; // Holds current pixel info

	std::vector<QRadioButton *> _sliceBtns; // Holds all slice selection radio buttons
	std::vector<QDoubleSpinBox *> _chromaExtents;

	int currentSlice() const; // Returns index of current slice
	unsigned char clip(double v); // Helper function. Clips double value to range <0, 255> and returns unsigned char conversion of that value
	void modifyYCbCr(unsigned char &y, unsigned char &cb, unsigned char &cr);


private slots:
	void sliceChanged(); // Called when one of slice selection radio buttons been clicked
	void updateSlices(double unused = 0.0f); // Updates _lumaSliceItems
	void updateCenter(double unused = 0.0f); // Updates _pChromaCenter
	void updateCenterColour();
	void mousePressed(QPointF pos, Qt::MouseButton button);
	void importValues();

};

} // namespace VisionLive

#endif // MODULEMIEWIDGET_H
