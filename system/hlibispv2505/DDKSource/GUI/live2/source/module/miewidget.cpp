#include "miewidget.hpp"
#include "customgraphicsview.hpp"
#include "doubleslider.hpp"

#include "felixcommon/pixel_format.h"

#ifdef USE_MATH_NEON
#include <mneon.h>
#endif
#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include <qgraphicsitem.h>
#include <QGraphicsRectItem>

#define LUMA_SCENE_W 100.0
#define LUMA_SCENE_H 100.0
#define LUMA_SCENE_BAR 0.5
#define LUMA_BAR_COL Qt::green // default colour
#define LUMA_BAR_SEL Qt::red // selected colour

#define CHROMA_CENTER_W 5.0

static const double CHROMA_SCENE_MIN_CBCR = -0.25; // in same range than -0.5..0.5
static const double CHROMA_SCENE_MAX_CBCR = 0.25; // in same range than -0.5..0.5
static const unsigned int CHROMA_SCENE_IMIN_CBCR = ((CHROMA_SCENE_MIN_CBCR+0.5)*256);
static const unsigned int CHROMA_SCENE_IMAX_CBCR = ((CHROMA_SCENE_MAX_CBCR+0.5)*256);
static const unsigned int CHROMA_SCENE_W = (CHROMA_SCENE_IMAX_CBCR-CHROMA_SCENE_IMIN_CBCR);
#define CHROMA_SCENE_H CHROMA_SCENE_W

static const double CHROMA_SCENE_COORD_MULT = ((0.5-(-0.5))/(CHROMA_SCENE_MAX_CBCR-CHROMA_SCENE_MIN_CBCR));

#ifdef WIN32
#define _USE_MATH_DEFINES // to define M_PI
#endif
#include <math.h>
#include <mc/module_config.h>

//
// Public Functions
//

VisionLive::MIEWidget::MIEWidget(QWidget *parent): QWidget(parent)
{
	Ui::MIEWidget::setupUi(this);

	retranslate();

	_pMIEWidget_lumaView_gv = NULL;
	_pLumaScene = NULL;
	_pChromaScene = NULL;
    _pChromaCenter = NULL;
    _pLineToCenter = NULL;
	_pMIEWidget_chromaView_gv = NULL;
	for(int i = 0; i < MIE_NUM_SLICES; i++)
	{
		_chromaDiamond[i] = 0;
	}

	initMIEWidget();

	initLumaView();

	initCenterInputColorView();

	initCenterOutputColorView();

	initChromaView();

	updateSlices();

	QObject::connect(lumaMin_sb, SIGNAL(valueChanged(double)), this, SLOT(updateSlices(double)));
    QObject::connect(lumaMax_sb, SIGNAL(valueChanged(double)), this, SLOT(updateSlices(double)));

	QObject::connect(cbCenter_sl, SIGNAL(valueChanged(double)), cbCenter_sb, SLOT(setValue(double)));
    QObject::connect(crCenter_sl, SIGNAL(valueChanged(double)), crCenter_sb, SLOT(setValue(double)));
    QObject::connect(aspect_sl, SIGNAL(valueChanged(double)), aspect_sb, SLOT(setValue(double)));
	QObject::connect(rotation_sl, SIGNAL(valueChanged(double)), rotation_sb, SLOT(setValue(double)));

	QObject::connect(cbCenter_sb, SIGNAL(valueChanged(double)), cbCenter_sl, SLOT(setValueD(double)));
    QObject::connect(crCenter_sb, SIGNAL(valueChanged(double)), crCenter_sl, SLOT(setValueD(double)));
    QObject::connect(aspect_sb, SIGNAL(valueChanged(double)), aspect_sl, SLOT(setValueD(double)));
	QObject::connect(rotation_sb, SIGNAL(valueChanged(double)), rotation_sl, SLOT(setValueD(double)));

    QObject::connect(cbCenter_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
    QObject::connect(crCenter_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
    QObject::connect(aspect_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
	QObject::connect(rotation_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));

	QObject::connect(gain0_sl, SIGNAL(valueChanged(double)), gain0_sb, SLOT(setValue(double)));
	QObject::connect(gain1_sl, SIGNAL(valueChanged(double)), gain1_sb, SLOT(setValue(double)));
	QObject::connect(gain2_sl, SIGNAL(valueChanged(double)), gain2_sb, SLOT(setValue(double)));
	QObject::connect(gain3_sl, SIGNAL(valueChanged(double)), gain3_sb, SLOT(setValue(double)));

	QObject::connect(gain0_sb, SIGNAL(valueChanged(double)), gain0_sl, SLOT(setValueD(double)));
	QObject::connect(gain1_sb, SIGNAL(valueChanged(double)), gain1_sl, SLOT(setValueD(double)));
	QObject::connect(gain2_sb, SIGNAL(valueChanged(double)), gain2_sl, SLOT(setValueD(double)));
	QObject::connect(gain3_sb, SIGNAL(valueChanged(double)), gain3_sl, SLOT(setValueD(double)));

	QObject::connect(chromaExtent0_sl, SIGNAL(valueChanged(double)), chromaExtent0_sb, SLOT(setValue(double)));
	QObject::connect(chromaExtent1_sl, SIGNAL(valueChanged(double)), chromaExtent1_sb, SLOT(setValue(double)));
	QObject::connect(chromaExtent2_sl, SIGNAL(valueChanged(double)), chromaExtent2_sb, SLOT(setValue(double)));
	QObject::connect(chromaExtent3_sl, SIGNAL(valueChanged(double)), chromaExtent3_sb, SLOT(setValue(double)));

	QObject::connect(chromaExtent0_sb, SIGNAL(valueChanged(double)), chromaExtent0_sl, SLOT(setValueD(double)));
	QObject::connect(chromaExtent1_sb, SIGNAL(valueChanged(double)), chromaExtent1_sl, SLOT(setValueD(double)));
	QObject::connect(chromaExtent2_sb, SIGNAL(valueChanged(double)), chromaExtent2_sl, SLOT(setValueD(double)));
	QObject::connect(chromaExtent3_sb, SIGNAL(valueChanged(double)), chromaExtent3_sl, SLOT(setValueD(double)));

    QObject::connect(chromaExtent0_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
	QObject::connect(chromaExtent1_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
	QObject::connect(chromaExtent2_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
	QObject::connect(chromaExtent3_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));

	QObject::connect(brightness_sl, SIGNAL(valueChanged(double)), brightness_sb, SLOT(setValue(double)));
    QObject::connect(contrast_sl, SIGNAL(valueChanged(double)), contrast_sb, SLOT(setValue(double)));
    QObject::connect(contrastCenter_sl, SIGNAL(valueChanged(double)), contrastCenter_sb, SLOT(setValue(double)));
    QObject::connect(saturation_sl, SIGNAL(valueChanged(double)), saturation_sb, SLOT(setValue(double)));
    QObject::connect(hue_sl, SIGNAL(valueChanged(double)), hue_sb, SLOT(setValue(double)));

	QObject::connect(brightness_sb, SIGNAL(valueChanged(double)), brightness_sl, SLOT(setValueD(double)));
    QObject::connect(contrast_sb, SIGNAL(valueChanged(double)), contrast_sl, SLOT(setValueD(double)));
    QObject::connect(contrastCenter_sb, SIGNAL(valueChanged(double)), contrastCenter_sl, SLOT(setValueD(double)));
    QObject::connect(saturation_sb, SIGNAL(valueChanged(double)), saturation_sl, SLOT(setValueD(double)));
    QObject::connect(hue_sb, SIGNAL(valueChanged(double)), hue_sl, SLOT(setValueD(double)));

    QObject::connect(brightness_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
    QObject::connect(contrast_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
    QObject::connect(contrastCenter_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
    QObject::connect(saturation_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));
    QObject::connect(hue_sb, SIGNAL(valueChanged(double)), this, SLOT(updateCenter(double)));

    QObject::connect(_pMIEWidget_chromaView_gv, SIGNAL(mousePressed(QPointF, Qt::MouseButton)), this, SLOT(mousePressed(QPointF, Qt::MouseButton)));
}

VisionLive::MIEWidget::~MIEWidget()
{
}

void VisionLive::MIEWidget::retranslate()
{
	Ui::MIEWidget::retranslateUi(this);
}

void VisionLive::MIEWidget::updatePixelInfo(QPoint pixel, std::vector<int> pixelInfo)
{
	_pixelInfo = pixelInfo;
}


//
// Protected Functions
//

void VisionLive::MIEWidget::initMIEWidget()
{
	_sliceBtns.push_back(slice0_rb);
	_sliceBtns.push_back(slice1_rb);
	_sliceBtns.push_back(slice2_rb);
	_sliceBtns.push_back(slice3_rb);

	_chromaExtents.push_back(chromaExtent0_sb);
	_chromaExtents.push_back(chromaExtent1_sb);
	_chromaExtents.push_back(chromaExtent2_sb);
	_chromaExtents.push_back(chromaExtent3_sb);

	// Hide ContrastCenter parameter
	label_10->setVisible(false);
    contrastCenter_sb->setVisible(false);
	contrastCenter_sl->setVisible(false);

	QObject::connect(import_btn, SIGNAL(clicked()), this, SLOT(importValues()));
}

void VisionLive::MIEWidget::initLumaView()
{
	_pMIEWidget_lumaView_gv = new CustomGraphicsView();
	_pMIEWidget_lumaView_gv->setZoomingEnabled(false);
	_pMIEWidget_lumaView_gv->setMaximumHeight(40);
	lumaLayout->addWidget(_pMIEWidget_lumaView_gv, 0, 0);

	_pLumaScene = new QGraphicsScene(0.0, 0.0, LUMA_SCENE_W, LUMA_SCENE_H);

    QLinearGradient lumaGrad(0.0, 0.0, LUMA_SCENE_W, 0.0);
	lumaGrad.setSpread(QGradient::RepeatSpread);

	QBrush grad(lumaGrad);

    QGraphicsRectItem *pRectGrad = new QGraphicsRectItem(0, 0, LUMA_SCENE_W, LUMA_SCENE_H);
    pRectGrad->setBrush(grad);
    pRectGrad->setPen(Qt::NoPen);

    _pLumaScene->addItem(pRectGrad);

	for(int i = 0; i < MIE_NUM_SLICES; i++)
    {
		_lumaSliceItems.push_back(new QGraphicsRectItem(0, 0, LUMA_SCENE_BAR, LUMA_SCENE_H));
        _lumaSliceItems[i]->setPen(QPen(LUMA_BAR_COL));
        _lumaSliceItems[i]->setBrush(QBrush(LUMA_BAR_COL, Qt::SolidPattern));
        _pLumaScene->addItem(_lumaSliceItems[i]);

        QObject::connect(_sliceBtns[i], SIGNAL(clicked()), this, SLOT(sliceChanged()));
    }

	_sliceBtns[0]->setChecked(true);

    _pMIEWidget_lumaView_gv->setAspectRatio(Qt::IgnoreAspectRatio);
    _pMIEWidget_lumaView_gv->setScene(_pLumaScene);
}

void VisionLive::MIEWidget::initCenterInputColorView()
{
	for(int i = 0; i < MIE_NUM_SLICES; i++)
    {
		_centerInputColourView.push_back(new CustomGraphicsView());
		_centerInputColourView[i]->setZoomingEnabled(false);
        _centerInputColourView[i]->setMaximumSize(40, 40);
        _centerInputColourView[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _centerInputColourView[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		_centerInputColour.push_back(new QGraphicsRectItem(0, 0, 40, 40));
		_centerInputScene.push_back(new QGraphicsScene(0, 0, 40, 40));
        _centerInputScene[i]->addItem(_centerInputColour[i]);
        _centerInputColourView[i]->setScene(_centerInputScene[i]);
        _centerInputColourView[i]->setSceneRect(10, 10, 20, 20);

        centerInputColourLayout->addWidget(_centerInputColourView[i], i);
	}
}

void VisionLive::MIEWidget::initCenterOutputColorView()
{
	for(int i = 0; i < MIE_NUM_SLICES; i++)
    {
		_centerOutputColourView.push_back(new CustomGraphicsView());
		_centerOutputColourView[i]->setZoomingEnabled(false);
        _centerOutputColourView[i]->setMaximumSize(40, 40);
        _centerOutputColourView[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _centerOutputColourView[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		_centerOutputColour.push_back(new QGraphicsRectItem(0, 0, 40, 40));
		_centerOutputScene.push_back(new QGraphicsScene(0, 0, 40, 40));
        _centerOutputScene[i]->addItem(_centerOutputColour[i]);
        _centerOutputColourView[i]->setScene(_centerOutputScene[i]);
        _centerOutputColourView[i]->setSceneRect(10, 10, 20, 20);

        centerOutputColourLayout->addWidget(_centerOutputColourView[i], i);
	}
}

void VisionLive::MIEWidget::initChromaView()
{
    unsigned char y = 128;

    unsigned char *rgbBuff = (unsigned char*)malloc(CHROMA_SCENE_W*CHROMA_SCENE_H*3*sizeof(char)); // RGB8 24b
    unsigned c = 0;

    for(unsigned int b = CHROMA_SCENE_IMAX_CBCR-1; b >= CHROMA_SCENE_IMIN_CBCR; b--)
    {
        for(unsigned int r = CHROMA_SCENE_IMIN_CBCR; r < CHROMA_SCENE_IMAX_CBCR; r++)
        {
            for(int k = 0; k < 3; k++)
            {
                double v = (y+BT709_Inputoff[0])*BT709_coeff[3*k+0] + (b+BT709_Inputoff[1])*BT709_coeff[3*k+1] + (r+BT709_Inputoff[2])*BT709_coeff[3*k+2];
                Q_ASSERT(c < CHROMA_SCENE_W*CHROMA_SCENE_H*3);
                rgbBuff[c++] = clip(v);
            }
        }
    }

    QImage ori(rgbBuff, CHROMA_SCENE_W, CHROMA_SCENE_H, QImage::Format_RGB888);

	_pMIEWidget_chromaView_gv = new CustomGraphicsView();
	chromaViewLayout->addWidget(_pMIEWidget_chromaView_gv, 0, 0);
    _pChromaScene = new QGraphicsScene(0.0, 0.0, CHROMA_SCENE_W, CHROMA_SCENE_H);
    _pChromaScene->addPixmap(QPixmap::fromImage(ori.rgbSwapped()));
    _pMIEWidget_chromaView_gv->setScene(_pChromaScene);

    free(rgbBuff);

	_pChromaCenter = new QGraphicsEllipseItem(0, 0, CHROMA_CENTER_W, CHROMA_CENTER_W);
	_pChromaCenter->setPen(QPen(Qt::white));
    _pChromaScene->addItem(_pChromaCenter);

    _pChromaScene->addLine(
        0, CHROMA_SCENE_H/2.0,
        CHROMA_SCENE_W, CHROMA_SCENE_H/2.0,
        QPen(QBrush(Qt::white), 1, Qt::DashLine)
        );
    _pChromaScene->addLine(
        CHROMA_SCENE_W/2.0, 0,
        CHROMA_SCENE_W/2.0, CHROMA_SCENE_H,
        QPen(QBrush(Qt::white), 1, Qt::DashLine)
        );

	updateCenter();

    sliceChanged();
}

//
// Private Functions
//

int VisionLive::MIEWidget::currentSlice() const
{
    for(int i = 0; i < MIE_NUM_SLICES; i++)
    {
        if(_sliceBtns[i]->isChecked())
        {
            return i;
        }
    }

	return 0;
}

unsigned char VisionLive::MIEWidget::clip(double v)
{
    if(v > 255)
	{ 
		v = 255;
	}
    else if(v < 0)
	{
		v = 0;
	}
    return (unsigned char)v;
}

void VisionLive::MIEWidget::modifyYCbCr(unsigned char &y, unsigned char &cb, unsigned char &cr)
{
    // from Keith Jack, "Video Demystified" 3rd ed. Digital Enhancement page 192
    int iY = y-16, iCb = cb, iCr = cr;

    iY = (int)floor(iY*contrast_sb->value());
    iY += (int)floor(brightness_sb->value()*128.0);
    // contrast center (from setup function in CSIM)
	//iY += (int)floor(contrast_center->value()*128.0*(1.0 - contrast->value())); // which is strange because contrast is 0..2

#ifdef USE_MATH_NEON
    iCb = (cb-128)*(double)cosf_neon((float)(hue_sb->value()*M_PI))+(cr-128)*(double)sinf_neon((float)(hue_sb->value()*M_PI));
    iCr = (cr-128)*(double)cosf_neon((float)(hue_sb->value()*M_PI))-(cb-128)*(double)sinf_neon((float)(hue_sb->value()*M_PI));
#else
    iCb = (cb-128)*cos(hue_sb->value()*M_PI)+(cr-128)*sin(hue_sb->value()*M_PI);
    iCr = (cr-128)*cos(hue_sb->value()*M_PI)-(cb-128)*sin(hue_sb->value()*M_PI);
#endif
    iCb = (int)floor(iCb*saturation_sb->value());
    iCr = (int)floor(iCr*saturation_sb->value());

    y = IMG_clip(iY+16, 8, 0, "GUI_MIE_Y");
    cb = IMG_clip(iCb+128, 8, 0, "GUI_MIE_Cb");
    cr = IMG_clip(iCr+128, 8, 0, "GUI_MIE_Cr");
}

//
// Private Slots
//

void VisionLive::MIEWidget::sliceChanged()
{
    int slice = currentSlice();

    for(int i = 0; i < MIE_NUM_SLICES; i++)
    {
        if(i != slice)
        {
            _lumaSliceItems[i]->setPen(QPen(LUMA_BAR_COL));
            _lumaSliceItems[i]->setBrush(QBrush(LUMA_BAR_COL, Qt::SolidPattern));
        }
        else
        {
            _lumaSliceItems[i]->setPen(QPen(LUMA_BAR_SEL));
            _lumaSliceItems[i]->setBrush(QBrush(LUMA_BAR_SEL, Qt::SolidPattern));
        }
    }

    updateCenter();
}

void VisionLive::MIEWidget::updateSlices(double unused)
{
    double Ymin = lumaMin_sb->value(), Ymax = lumaMax_sb->value();

    if( Ymax < Ymin)
    {
        Ymin = Ymax;
        Ymax = lumaMin_sb->value();
    }

    double step = (Ymax - Ymin)/3.0f;

    for(int i = 0; i < MIE_NUM_SLICES; i++)
    {
        _lumaSlice[i] = (Ymin+i*step);
        double x = _lumaSlice[i]*LUMA_SCENE_W;
        if(x == LUMA_SCENE_W)
        {
            x -= LUMA_SCENE_BAR;
        }
        _lumaSliceItems[i]->setRect(x, 0.0, LUMA_SCENE_BAR, LUMA_SCENE_H);
    }

    updateCenterColour();

    Q_UNUSED(unused);
}

void VisionLive::MIEWidget::updateCenter(double unused)
{
    int slice = currentSlice();
    QPolygonF diamond[MIE_NUM_SLICES];

    // converts Cb and Cr centers into view coordinates (-0.5 .. 0.5) to (0 .. CHROMA_SCENE_W)
    // but only -0.25..0.25 are displayed
    {
        double x = (cbCenter_sb->value()*CHROMA_SCENE_COORD_MULT)*CHROMA_SCENE_W + (CHROMA_SCENE_W/2.0);
        double y = CHROMA_SCENE_H - ((crCenter_sb->value()*CHROMA_SCENE_COORD_MULT)*CHROMA_SCENE_H + (CHROMA_SCENE_H/2.0));
        unsigned char iy = 0.5, cb = (cbCenter_sb->value()+0.5)*256, cr = (crCenter_sb->value()+0.5)*256;

        _pChromaCenter->setRect(x-CHROMA_CENTER_W/2.0, y-CHROMA_CENTER_W/2.0, CHROMA_CENTER_W, CHROMA_CENTER_W);

        modifyYCbCr(iy, cb, cr);

        if(_pLineToCenter)
        {
            _pMIEWidget_chromaView_gv->scene()->removeItem(_pLineToCenter);
            delete _pLineToCenter;
        }
        // cb/cr are 0..255
        double x2, y2;

        x2 = ((cb/256.0 -0.5)*CHROMA_SCENE_COORD_MULT)*CHROMA_SCENE_W + (CHROMA_SCENE_W/2.0);
        y2 = CHROMA_SCENE_H - (((cr/256.0 -0.5)*CHROMA_SCENE_COORD_MULT)*CHROMA_SCENE_H + (CHROMA_SCENE_H/2.0));

        _pLineToCenter = _pMIEWidget_chromaView_gv->scene()->addLine(x, y, x2, y2);
    }

    // compute the diamond
    for(int s = 0; s < MIE_NUM_SLICES; s++)
    {
        // compute the position of the 4 points for the extent
        double L0 = 0.5; // desired iso-value for the diamond
        double norm_angle = rotation_sb->value();
        double cb0 = cbCenter_sb->value(), cr0 = crCenter_sb->value();
#ifdef USE_MATH_NEON
        double r0 = (double)cosf_neon((float)norm_angle*M_PI);
        double r1 = (double)sinf_neon((float)norm_angle*M_PI);
#else
        double r0 = cos(norm_angle*M_PI);
        double r1 = sin(norm_angle*M_PI);
#endif
        double sigma_b, sigma_r, sqrt_base; // variances on rotation axis, from extent and aspect -
        double cb_p0, cb_p1, cb_p2, cb_p3, cr_p0, cr_p1, cr_p2, cr_p3;

        // compute sigma values

#ifdef USE_MATH_NEON
        sqrt_base = ((double)sqrtf_neon((float)powf_neon(2.0f, (float)aspect_sb->value())));
#else
        sqrt_base = (sqrt(pow(2.0, aspect_sb->value())));
#endif
        sigma_b = _chromaExtents[s]->value() * sqrt_base; //chromaExtent * (std::sqrt(pow(2.0, aspect)));
        sigma_r = _chromaExtents[s]->value() / sqrt_base; ////chromaExtent / (std::sqrt(pow(2.0, aspect)));

        // compute the coordinates

#ifdef USE_MATH_NEON
        sqrt_base = (double)sqrtf_neon(-1.0f*(float)logf_neon((float)L0));
#else
        sqrt_base = sqrt(-1*log(L0));
#endif

        cb_p0 = (cb0 + r0*sigma_b*sqrt_base/2.5);//(cb0 + r0*sigma_b*std::sqrt(-1*std::log(L0))/2.5);
        cb_p1 = (cb0 - r0*sigma_b*sqrt_base/2.5);//(cb0 - r0*sigma_b*std::sqrt(-1*std::log(L0))/2.5);

        cr_p0 = (cr0 - r1*sigma_b*sqrt_base/2.5);//(cr0 - r1*sigma_b*std::sqrt(-1*std::log(L0))/2.5);
        cr_p1 = (cr0 + r1*sigma_b*sqrt_base/2.5);//(cr0 + r1*sigma_b*std::sqrt(-1*std::log(L0))/2.5);

        cb_p2 = (cb0 + r1*sigma_r*sqrt_base/2.5);//(cb0 + r1*sigma_r*std::sqrt(-1*std::log(L0))/2.5);
        cb_p3 = (cb0 - r1*sigma_r*sqrt_base/2.5);//(cb0 - r1*sigma_r*std::sqrt(-1*std::log(L0))/2.5);

        cr_p2 = (cr0 + r0*sigma_r*sqrt_base/2.5);//(cr0 + r0*sigma_r*std::sqrt(-1*std::log(L0))/2.5);
        cr_p3 = (cr0 - r0*sigma_r*sqrt_base/2.5);//(cr0 - r0*sigma_r*std::sqrt(-1*std::log(L0))/2.5);

        // convert the coordinates into scene coordinates

        QPointF p;

        cb_p0 *= CHROMA_SCENE_COORD_MULT;
        cr_p0 *= CHROMA_SCENE_COORD_MULT;
        p.setX(cb_p0*CHROMA_SCENE_W + (CHROMA_SCENE_W/2));
        p.setY(CHROMA_SCENE_H - (cr_p0*CHROMA_SCENE_H + (CHROMA_SCENE_H/2)));
        diamond[s] << p;

        cb_p3 *= CHROMA_SCENE_COORD_MULT;
        cr_p3 *= CHROMA_SCENE_COORD_MULT;
        p.setX(cb_p3*CHROMA_SCENE_W + (CHROMA_SCENE_W/2));
        p.setY(CHROMA_SCENE_H - (cr_p3*CHROMA_SCENE_H + (CHROMA_SCENE_H/2)));
        diamond[s] << p;

        cb_p1 *= CHROMA_SCENE_COORD_MULT;
        cr_p1 *= CHROMA_SCENE_COORD_MULT;
        p.setX(cb_p1*CHROMA_SCENE_W + (CHROMA_SCENE_W/2));
        p.setY(CHROMA_SCENE_H - (cr_p1*CHROMA_SCENE_H + (CHROMA_SCENE_H/2)));
        diamond[s] << p;

        cb_p2 *= CHROMA_SCENE_COORD_MULT;
        cr_p2 *= CHROMA_SCENE_COORD_MULT;
        p.setX(cb_p2*CHROMA_SCENE_W + (CHROMA_SCENE_W/2));
        p.setY(CHROMA_SCENE_H - (cr_p2*CHROMA_SCENE_H + (CHROMA_SCENE_H/2)));
        diamond[s] << p;
    }

    for(int s = 0; s < MIE_NUM_SLICES; s++)
    {
        if(_chromaDiamond[s])
        {
            _pMIEWidget_chromaView_gv->scene()->removeItem(_chromaDiamond[s]);
            delete _chromaDiamond[s];
        }
        if(s != slice)
        {
			_chromaDiamond[s] = _pMIEWidget_chromaView_gv->scene()->addPolygon(diamond[s], QPen(Qt::white));
        }
    }
    // add the current diamond last to ensure it is on top of the view even if they are all the same
    _chromaDiamond[slice] = _pMIEWidget_chromaView_gv->scene()->addPolygon(diamond[slice]);
    _chromaDiamond[slice]->setPen(QPen(Qt::red));

    // center colour
    updateCenterColour();

    Q_UNUSED(unused);
}

void VisionLive::MIEWidget::updateCenterColour()
{
    for(int slice = 0; slice < MIE_NUM_SLICES; slice++)
    {
        unsigned char y = clip(_lumaSlice[slice]*255.0), cb = clip((cbCenter_sb->value()+0.5f)*255.0), cr = clip((crCenter_sb->value()+0.5f)*255.0);

        unsigned char rgb[3] = {(uchar) 0.5, (uchar) cbCenter_sb->value(), (uchar) crCenter_sb->value()};

        for(int k = 0; k < 3; k++)
        {
            double v = (y+BT709_Inputoff[0])*BT709_coeff[3*k+0] + (cb+BT709_Inputoff[1])*BT709_coeff[3*k+1] + (cr+BT709_Inputoff[2])*BT709_coeff[3*k+2];
            rgb[k] = clip(v);
        }

        QBrush bI(QColor(rgb[0], rgb[1], rgb[2]), Qt::SolidPattern);
        _centerInputColourView[slice]->setToolTip(tr("YCbCr %n", "Y", y) + tr(" %n", "Cb", cb) + tr(" %n", "Cr", cr));

        // modify colour
        modifyYCbCr(y, cb, cr);

        for(int k = 0; k < 3; k++)
        {
            double v = (y+BT709_Inputoff[0])*BT709_coeff[3*k+0] + (cb+BT709_Inputoff[1])*BT709_coeff[3*k+1] + (cr+BT709_Inputoff[2])*BT709_coeff[3*k+2];
            rgb[k] = clip(v);
        }

        QBrush bO(QColor(rgb[0], rgb[1], rgb[2]), Qt::SolidPattern);
        _centerOutputColourView[slice]->setToolTip(tr("YCbCr %n", "Y", y) + tr(" %n", "Cb", cb) + tr(" %n", "Cr", cr));

        _centerInputColour[slice]->setBrush(bI);
        _centerInputColourView[slice]->update();

        _centerOutputColour[slice]->setBrush(bO);
        _centerOutputColourView[slice]->update();
    }
}

void VisionLive::MIEWidget::mousePressed(QPointF pos, Qt::MouseButton button)
{
    QPointF toSc = _pMIEWidget_chromaView_gv->mapToScene(pos.x(), pos.y());

    if(button == Qt::LeftButton)
    {
        double x = (toSc.x()/CHROMA_SCENE_W) *(CHROMA_SCENE_MAX_CBCR-CHROMA_SCENE_MIN_CBCR) +CHROMA_SCENE_MIN_CBCR;
        double y = (CHROMA_SCENE_H - toSc.y())/CHROMA_SCENE_H *(CHROMA_SCENE_MAX_CBCR-CHROMA_SCENE_MIN_CBCR)+CHROMA_SCENE_MIN_CBCR;

        if(x >= cbCenter_sb->minimum() && x <= cbCenter_sb->maximum() &&
           y >= crCenter_sb->minimum() && y <= crCenter_sb->maximum())
        {
            cbCenter_sb->setValue(x);
            crCenter_sb->setValue(y);

            updateCenter();
        }
    }
    else if(button == Qt::RightButton)
    {
        _pMIEWidget_chromaView_gv->centerOn(toSc);
    }
}

void VisionLive::MIEWidget::importValues()
{
	if(_pixelInfo.size() == 3)
	{
		lumaMin_sb->setValue((double)_pixelInfo[0]/255.0f - 0.15);
		lumaMax_sb->setValue((double)_pixelInfo[0]/255.0f + 0.15);
		cbCenter_sb->setValue((double)_pixelInfo[1]/255.0f - 0.5f);
		crCenter_sb->setValue((double)_pixelInfo[2]/255.0f - 0.5f);
	}
}
