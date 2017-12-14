#include "tnmcurve.hpp"

// #include "felix_hw_info.h"
#define TNM_CURVE_NPOINTS 63


#define TNM_CURVE_MAX 1.0f
#define TNM_CURVE_MIN 0.0f
#define TNM_CURVE_NAME "TNM Curve"
#define IDENTITY_CURVE_NAME "Identity"

//
// Public Functions
//
	
VisionLive::TNMCurve::TNMCurve(QWidget *parent): QWidget(parent)
{
	Ui::TNMCurve::setupUi(this);

	_pPlot = NULL;

	_editable = false;

    init();
}

VisionLive::TNMCurve::~TNMCurve()
{
	if(_pPlot)
	{
		_pPlot->deleteLater();
	}
}

void VisionLive::TNMCurve::setData(const QVector<QPointF> newData)
{
	if(!_pPlot || newData.size() != TNM_CURVE_NPOINTS+2)
	{
        return;
    }

	_pPlot->updateCurve(QStringLiteral(TNM_CURVE_NAME), newData);
}

const QVector<QPointF> VisionLive::TNMCurve::data() const
{
	return (_pPlot)? _pPlot->data(QStringLiteral(TNM_CURVE_NAME)) : QVector<QPointF>();
}

bool VisionLive::TNMCurve::curveEditable() const
{
    return _editable;
}

//
// Private Functions
//

void VisionLive::TNMCurve::init()
{
	if(_pPlot)
	{
		return;
	}

	_pPlot = new EasyPlot::Plot();
	gridLayout->addWidget(_pPlot, 1, 0);

    QVector<QPointF> iden;
    iden.push_back(QPointF(TNM_CURVE_MIN, TNM_CURVE_MIN));
    iden.push_back(QPointF(TNM_CURVE_NPOINTS+2.0f, TNM_CURVE_MAX));
	_pPlot->addCurve(QStringLiteral(IDENTITY_CURVE_NAME), iden);
	_pPlot->setCurveZValue(QStringLiteral(IDENTITY_CURVE_NAME), 1);
	_pPlot->setCurvePen(QStringLiteral(IDENTITY_CURVE_NAME), QPen(QBrush(Qt::black), 1, Qt::DashLine));

	QVector<QPointF> tnmData;
    double v = TNM_CURVE_MIN, add = TNM_CURVE_MAX/(TNM_CURVE_NPOINTS+2);
    tnmData.push_back(QPointF(TNM_CURVE_MIN, TNM_CURVE_MIN));
    for(int i = 0; i < TNM_CURVE_NPOINTS; i++)
    {
		v += add;
        tnmData.push_back(QPointF(i+1, v));
    }
    tnmData.push_back(QPointF(TNM_CURVE_NPOINTS+2, TNM_CURVE_MAX));
	_pPlot->addCurve(QStringLiteral(TNM_CURVE_NAME), iden);
	_pPlot->setCurveZValue(QStringLiteral(TNM_CURVE_NAME), 2);
	_pPlot->setCurvePen(QStringLiteral(TNM_CURVE_NAME), QPen(QBrush(Qt::green), 3, Qt::SolidLine));
    _pPlot->setCurveType(QStringLiteral(TNM_CURVE_NAME), EasyPlot::Line);
    _pPlot->setCurveEditable(QStringLiteral(TNM_CURVE_NAME), true);
    _pPlot->setMarkerPen(QStringLiteral(TNM_CURVE_NAME), QPen(QBrush(Qt::green), 5));
    _pPlot->setMarkerEditPen(QStringLiteral(TNM_CURVE_NAME), QPen(QBrush(qRgb(255, 0, 0)), 7));
    _pPlot->setMarkerType(QStringLiteral(TNM_CURVE_NAME), EasyPlot::Ellipse);

	_pPlot->setAxisXPen(QPen(QBrush(Qt::white), 1));

    _pPlot->setAxisYPen(QPen(QBrush(Qt::white), 1));

	_pPlot->setTitleVisible(false);

	_pPlot->setLegendVisible(false);

	_pPlot->setAxisXTitleVisible(false);

	_pPlot->setAxisYTitleVisible(false);

	_pPlot->setPositionInfoColor(Qt::white);

	connect(_pPlot, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)), this, SLOT(pointMoved(QString, unsigned, QPointF, QPointF)));
    connect(toIdentity_btn, SIGNAL(clicked()), this, SLOT(curveToIdentity()));
    connect(curveControl_grp, SIGNAL(toggled(bool)), this, SLOT(setCurveEditable(bool)));
}

//
// Public Slots
//
	
void VisionLive::TNMCurve::setCurveEditable(bool editable)
{
	if(!_pPlot)
	{
		curveControl_grp->setChecked(false);
		return;
	}

    _editable = editable;

	_pPlot->setCurveEditable(QStringLiteral(TNM_CURVE_NAME), _editable);
}

void VisionLive::TNMCurve::pointMoved(QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	QVector<QPointF> data = _pPlot->data(QStringLiteral(TNM_CURVE_NAME));
	
	double currH = currPos.y();
    
	if((currH >= TNM_CURVE_MAX || currH <= TNM_CURVE_MIN) || (index == 0 || index == data.size()-1))
    {
        return;
    }

	data[index] = QPointF(prevPos.x(), currPos.y());

    if(interpolateFwd->isChecked() || interpolateBwd->isChecked())
    {
        double oldH, delta;
        int start, end;

        currH = data.at(index).y();
        oldH = prevPos.y();
        delta = currH - oldH;

        if(interpolateFwd->isChecked())
        {
            start = index+1;
            end = data.size()-1; // size -1 because last element is not modificable
        }
        if(interpolateBwd->isChecked())
        {
            start = 1; // 0 is not editable
            if(!interpolateFwd->isChecked())
			{
                end = index;
			}
        }

        for(int i = start; i < end; i++)
        {
            double ph = data.at(i).y();

            if(i == index)
			{
				continue;
			}

            QPointF neo(data.at(i));

            if(ph > oldH)
			{
                ph = (ph-oldH)/(1.0f-oldH) * (1.0f-currH) + currH;
			}
            else
			{
                ph = currH - (oldH-ph)/(oldH) * (currH);
			}

            neo.setY(ph);
            if(ph < TNM_CURVE_MAX && ph > TNM_CURVE_MIN) // not going higher than one
            {
				data[i] = neo;
            }
        }
    }
	_pPlot->updateCurve(QStringLiteral(TNM_CURVE_NAME), data);
}

void VisionLive::TNMCurve::curveToIdentity()
{
    double v = TNM_CURVE_MIN, add = TNM_CURVE_MAX/(TNM_CURVE_NPOINTS+1);

	QVector<QPointF> data;
    data.push_back(QPointF(TNM_CURVE_MIN, TNM_CURVE_MIN));
    for(int i = 0; i < TNM_CURVE_NPOINTS; i++)
    {
		v += add;
        data.push_back(QPointF(i+1, v));
    }
    data.push_back(QPointF(TNM_CURVE_NPOINTS+1, TNM_CURVE_MAX));

	_pPlot->updateCurve(QStringLiteral(TNM_CURVE_NAME), data);
}

