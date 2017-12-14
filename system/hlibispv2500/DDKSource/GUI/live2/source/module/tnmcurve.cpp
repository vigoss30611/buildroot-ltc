#include "tnmcurve.hpp"

#include <QDialog>
#include <QMessageBox>

#include <QwtExtra/editablecurve.hpp>
#include <qwt_symbol.h>

#include "felix_hw_info.h"
#include <assert.h>

/*
 * private
 */

void TNMCurve::init()
{
    Ui::TNMCurve::setupUi(this);

    QVector<QPointF> iden;
    QwtPlotCurve *toneMapper;

    correctCurve.setColor(Qt::green);
    correctCurve.setWidth(3);

    wrongCurve.setColor(Qt::red);
    wrongCurve.setWidth(3);

    selectSymbol.setBrush(Qt::yellow);
    selectSymbol.setSize(5, 5);
    selectSymbol.setStyle(QwtSymbol::Ellipse);

    iden.push_back(QPointF(0.0f, 0.0f));
    iden.push_back(QPointF(TNM_CURVE_NPOINTS+2.0f, 1.0f));

    double v = 0.0f, add = 1.0f/(TNM_CURVE_NPOINTS+2);
    v = add;
    _originalData.push_back(QPointF(0.0f, 0.0f));
    for ( int i = 0 ; i < TNM_CURVE_NPOINTS ; i++ )
    {
        _originalData.push_back(QPointF(i+1, v));
        v += add;
    }
    _originalData.push_back(QPointF(TNM_CURVE_NPOINTS+2, 1.0f));

    idC = curve->addCurve(iden, tr("Identity"));
    tnmC = curve->addCurve(_originalData, tr("Tone Mapper Curve"));
    curve->setEditable(tnmC, false, curveControl_grp->isChecked());

	curve->setEditable(idC, false, false);

    editable = false;

    toneMapper = curve->curve(tnmC);
        
    curve->curve(idC)->setPen(QPen(Qt::DashLine));

    curve->setAxisScale(QwtPlot::xBottom, 0.0f, TNM_CURVE_NPOINTS+2.0f, 10.0f);
    curve->setAxisMaxMinor(QwtPlot::xBottom, 5);

    setCurveEditable( curveControl_grp->isChecked() ); // calls verify that calls replot
    
    verify(); // does a replot

    connect(curve, SIGNAL(pointSelected(int, int)), this, SLOT(pointSelected(int, int)));
    connect(curve, SIGNAL(pointMoved(int, int, QPointF)), this, SLOT(pointMoved(int, int, QPointF)));

    connect(toIdentity_btn, SIGNAL(clicked()), this, SLOT(curveToIdentity()));
    connect(reset_btn, SIGNAL(clicked()), this, SLOT(replotCurve()));
    connect(curveControl_grp, SIGNAL(toggled(bool)), this, SLOT(setCurveEditable(bool)));
}

/*
 * public
 */
	
TNMCurve::TNMCurve(QWidget *parent): QWidget(parent)
{
    init();
}

const QVector<QPointF>& TNMCurve::curveData() const
{
    return *curve->data(tnmC);
}

int TNMCurve::setData(const QVector<QPointF>& newData)
{
    if(newData.size() != TNM_CURVE_NPOINTS+2 )
    {
		printf("TNM Curve has incorrect size! Size: %d\n", newData.size());
        return EXIT_FAILURE;
    }

    _originalData.clear();
    _originalData = newData;
    replotCurve();

    return EXIT_SUCCESS;
}

bool TNMCurve::curveEditable() const
{
    return editable;
}

/*
 * public slots
 */
	
void TNMCurve::setCurveEditable(bool _editable)
{
    this->editable = _editable;
    curve->setEditable(tnmC, false, editable);

    if (!editable)
    {
        curve->curve(tnmC)->setSymbol(NULL);
    }
    else
    {
        QwtSymbol *neo = new QwtSymbol( selectSymbol.style(), selectSymbol.brush(), curve->curve(1)->pen(), selectSymbol.size() );
        curve->curve(tnmC)->setSymbol( neo );
    }
    //curve->replot();
    verify(); // does a replot
}

void TNMCurve::pointSelected(int curveIdx, int point)
{
    if ( curveIdx == tnmC && (point == 0 || point == curve->data(tnmC)->size()-1) )
        curve->cancelSelection(); // do not select the first and last point
}

void TNMCurve::pointMoved(int curveIdx, int point, QPointF prevPosition)
{
    QVector<QPointF> *data = curve->data(tnmC);
    double currH = data->at(point).y();

    if(currH >= TNM_CURVE_MAX || currH <= TNM_CURVE_MIN)
    {
        data->replace(point, prevPosition);
        curve->curve(tnmC)->setSamples(*data);
        verify();
        return;
    }

    if(interpolateFwd->isChecked() || interpolateBwd->isChecked())
    {
        double oldH, delta;
        int start, end;

        currH = data->at(point).y();
        oldH = prevPosition.y();
        delta = currH - oldH;

        if(interpolateFwd->isChecked())
        {
            start = point+1;
            end = data->size()-1; // size -1 because last element is not modificable
        }
        if(interpolateBwd->isChecked())
        {
            start = 1; // 0 is not editable
            if(!interpolateFwd->isChecked())
			{
                end = point;
			}
        }

        for(int i = start; i < end; i++)
        {
            double ph = data->at(i).y();

            if(i == point)
			{
				continue;
			}

            QPointF neo(data->at(i));

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
                data->replace(i, neo);
            }
        }

        curve->curve(tnmC)->setSamples(*data);
    }

    verify(); // does a replot
}

void TNMCurve::replotCurve()
{
    curve->curve(tnmC)->setSamples(_originalData);
    //*(curve->data(tnmC)) = _originalData; // copy back origianl data

    //curve->replot();
    verify(); // does a replot
}

void TNMCurve::curveToIdentity()
{
    QVector<QPointF> *data = curve->data(tnmC);
    double v = 0.0f, add = 1.0f/(TNM_CURVE_NPOINTS+1);

    Q_ASSERT( data->size() == TNM_CURVE_NPOINTS+2 );
    
    data->clear();
    data->push_back(QPointF(0, 0.0f));
    v = add;

    for ( int i = 0 ; i < TNM_CURVE_NPOINTS ; i++ )
    {
        data->push_back(QPointF(i+1, v));
        v += add;
    }
    data->push_back(QPointF(TNM_CURVE_NPOINTS+1, 1.0f));

    curve->curve(tnmC)->setSamples(*data);
    //curve->replot();
    verify(); // does a replot
}

bool TNMCurve::verify()
{
    const QVector<QPointF> *data = curve->data(tnmC);
    bool correct = true;
    int i;

    Q_ASSERT( data->size() == TNM_CURVE_NPOINTS+2 );

    for ( i = 1 ; i < TNM_CURVE_NPOINTS+1 && correct ; i++ )
    {
        if ( data->at(i-1).y() > data->at(i).y() ) correct = false;
    }

    if ( correct )
    {
        curve->curve(tnmC)->setPen(correctCurve);
    }
    else
    {
        curve->curve(tnmC)->setPen(wrongCurve);
		//fprintf(stderr, "Wrong at element %d\n", i);
    }

    if ( curve->curve(tnmC)->symbol() )
    {
        curve->curve(tnmC)->setSymbol(
            new QwtSymbol( selectSymbol.style(), selectSymbol.brush(), curve->curve(tnmC)->pen(), selectSymbol.size() )
        );
    }

    curve->replot();

    return correct;
}
