#include "histogram.hpp"

#include <qlayout.h>

#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_canvas.h>

//
// Public Functions
//

VisionLive::Histogram::Histogram(QWidget *parent): QWidget(parent)
{
	retranslate();

	_pLayout = NULL;
	_pPlotR = NULL;
	_pPlotG = NULL;
	_pPlotG2 = NULL;
	_pPlotB = NULL;
	_pHistR = NULL;
	_pHistG = NULL;
	_pHistG2 = NULL;
	_pHistB = NULL;
	_pSymbolR = NULL;
	_pSymbolG = NULL;
	_pSymbolG2 = NULL;
	_pSymbolB = NULL;
	
	initHistogram();
}

VisionLive::Histogram::~Histogram()
{
}

//
// Private Functions
//

void VisionLive::Histogram::initHistogram()
{
	_pLayout = new QVBoxLayout();
	setLayout(_pLayout);

	_pPlotR = new QwtPlot();
	_pPlotG = new QwtPlot();
	_pPlotG2 = new QwtPlot();
	_pPlotB = new QwtPlot();

	_pHistR = new QwtPlotHistogram(tr("Red"));
	_pHistG = new QwtPlotHistogram(tr("Green"));
	_pHistG2 = new QwtPlotHistogram(tr("Green"));
	_pHistB = new QwtPlotHistogram(tr("Blue"));
		
	_pSymbolR = new QwtColumnSymbol(QwtColumnSymbol::Box);
	_pSymbolR->setPalette(QPalette(Qt::red));
    _pHistR->setSymbol(_pSymbolR);
	_pSymbolG = new QwtColumnSymbol(QwtColumnSymbol::Box);
	_pSymbolG->setPalette(QPalette(Qt::green));
    _pHistG->setSymbol(_pSymbolG);
	_pSymbolG2 = new QwtColumnSymbol(QwtColumnSymbol::Box);
	_pSymbolG2->setPalette(QPalette(Qt::green));
    _pHistG2->setSymbol(_pSymbolG2);
	_pSymbolB = new QwtColumnSymbol(QwtColumnSymbol::Box);
	_pSymbolB->setPalette(QPalette(Qt::blue));
    _pHistB->setSymbol(_pSymbolB);

	_pPlotR->enableAxis(QwtPlot::yLeft, false);
	_pPlotG->enableAxis(QwtPlot::yLeft, false);
	_pPlotG2->enableAxis(QwtPlot::yLeft, false);
	_pPlotB->enableAxis(QwtPlot::yLeft, false);

	_pHistR->attach(_pPlotR);
	_pHistG->attach(_pPlotG);
	_pHistG2->attach(_pPlotG2);
	_pHistB->attach(_pPlotB);

	_pLayout->addWidget(_pPlotR);
	_pLayout->addWidget(_pPlotG);
	_pLayout->addWidget(_pPlotG2);
	_pLayout->addWidget(_pPlotB);
}

void VisionLive::Histogram::retranslate()
{
}

//
// Public Slots
//

void VisionLive::Histogram::histogramReceived(std::vector<cv::Mat> histData, int nChannels, int nElem)
{
	if(_pPlotR && _pPlotG && _pPlotG2 && _pPlotB)
	{
		QVector<QwtIntervalSample> data;
		for(int i = 0; i < histData[0].rows; i++)
		{
			data.push_back(QwtIntervalSample(histData[0].at<float>(i), (i-nElem), (i-nElem+1)));
		}
		_pHistR->setSamples(data);
		_pPlotR->setAxisScale(QwtPlot::xBottom, 0, (double)nElem, (double)nElem);
		_pPlotR->replot();

		data.clear();
		for(int i = 0; i < histData[1].rows; i++)
		{
			data.push_back(QwtIntervalSample(histData[1].at<float>(i), (i-nElem), (i-nElem+1)));
		}
		_pHistG->setSamples(data);
		_pPlotG->setAxisScale(QwtPlot::xBottom, 0, (double)nElem, (double)nElem);
		_pPlotG->replot();

		if(nChannels == 4)
		{
			data.clear();
			for(int i = 0; i < histData[3].rows; i++)
			{
				data.push_back(QwtIntervalSample(histData[3].at<float>(i), (i-nElem), (i-nElem+1)));
			}
			_pHistG2->setSamples(data);
			_pPlotG2->setAxisScale(QwtPlot::xBottom, 0, (double)nElem, (double)nElem);
			_pPlotG2->replot();
		}
		else
		{
			_pPlotG2->hide();
		}

		data.clear();
		for(int i = 0; i < histData[2].rows; i++)
		{
			data.push_back(QwtIntervalSample(histData[2].at<float>(i), (i-nElem), (i-nElem+1)));
		}
		_pHistB->setSamples(data);
		_pPlotB->setAxisScale(QwtPlot::xBottom, 0, (double)nElem, (double)nElem);
		_pPlotB->replot();
	}
}