#include "gmawidget.hpp"

#include <QInputDialog>
#include "QtExtra/textparser.hpp"
#include <QMessageBox>

#include <felix_hw_info.h>

#define GMA_REG_MAX_VAL 4096.0f
#define GMA_PRE_MAX_VAL 255.0f
#define GMA_MIN_VAL 0.0f

#define GMA_LUT_INT 8
#define GMA_LUT_FRAC 4
#define GMA_LUT_SIGNRED 0

#define GMA_CURVE_RED_NAME "Red"
#define GMA_CURVE_GREEN_NAME "Green"
#define GMA_CURVE_BLUE_NAME "Blue"

/*
 * EditPoints
 */

//
// Public Functions
//

VisionLive::EditPoints::EditPoints(EasyPlot::Plot *plot, QString curveName): _pPlot(plot), _curveName(curveName)
{
	setWindowTitle(curveName);

	_mainLayout = new QGridLayout();
	setLayout(_mainLayout);

	_import = new QPushButton("Import values");
	_mainLayout->addWidget(_import, 0, 0, 1, 2);

	//_data = _curve->data(_id);
	_data = _pPlot->data(curveName);

	int j = 0;
	int k = 0;
	for(int i = 0; i < _data.size(); i++)
	{
		if(i > 0 && i%10 == 0) 
		{
			j++;
			k = 0;
		}
		_values.push_back(new QDoubleSpinBox(this));
		_mainLayout->addWidget(_values[i], j+1, k);
		_values[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
		_values[i]->setMaximum(GMA_REG_MAX_VAL);
		_values[i]->setValue(_data[i].y());
		connect(_values[i], SIGNAL(valueChanged(double)), this, SLOT(valueChanged()));
		k++;
	}

	connect(_import, SIGNAL(clicked()), this, SLOT(importValues()));
}

VisionLive::EditPoints::~EditPoints()
{
}

//
// Public Slots
//

void VisionLive::EditPoints::valueChanged()
{
	for(int i = 0; i < _data.size(); i++)
	{
		_data[i] = QPointF(_data[i].x(), _values[i]->value());
	}
	//_curve->curve(_id)->setSamples(*_data);
	//_curve->replot();
	_pPlot->updateCurve(_curveName, _data);
}

void VisionLive::EditPoints::importValues()
{
	QtExtra::TextParser parser(63, 1);
	
	if(parser.exec() == QDialog::Accepted)
    {
		QList<double> list;
		parser.results(list);

		if(list.size() != _values.size())
		{
			// Create ConnectionClosed popup
			QMessageBox::warning(this, tr("Warning"), tr("Incorrect imported values count!"));
			return;
		}

		for(int i = 0; i < _values.size(); i++)
		{
			_values[i]->setValue(list[i]);
		}
	}
}

/*
 * GMAWidget
 */

//
// Public Functions
//

VisionLive::GMAWidget::GMAWidget(QVector<QPointF> dataChannelR, QVector<QPointF> dataChannelG, QVector<QPointF> dataChannelB, bool editable, QWidget *parent):
	_dataChannelR(dataChannelR), _dataChannelG(dataChannelG), _dataChannelB(dataChannelB), _editable(editable)
{
	Ui::GMAWidget::setupUi(this);

	_pPlotRed = NULL;
	_pPlotGreen = NULL;
	_pPlotBlue = NULL;
	_pEdit = NULL;

    init();
}

VisionLive::GMAWidget::~GMAWidget()
{
	if(_pPlotRed)
	{
		delete _pPlotRed;
	}
	if(_pPlotGreen)
	{
		delete _pPlotGreen;
	}
	if(_pPlotBlue)
	{
		delete _pPlotBlue;
	}
	if(_pEdit)
	{
		delete _pEdit;
	}
}

const QVector<QPointF> VisionLive::GMAWidget::data(int channel) const
{
	switch(channel)
	{
	case 0: 
		return _pPlotRed->data(QStringLiteral(GMA_CURVE_RED_NAME));
	case 1: 
		return _pPlotGreen->data(QStringLiteral(GMA_CURVE_GREEN_NAME));
	case 2: 
		return _pPlotBlue->data(QStringLiteral(GMA_CURVE_BLUE_NAME));
	}

	return QVector<QPointF>();
}

//
// Private Functions
//

void VisionLive::GMAWidget::init()
{
	if(_pPlotRed || _pPlotGreen || _pPlotBlue)
	{
		return;
	}

	_pPlotRed = new EasyPlot::Plot();
	redTabLayout->addWidget(_pPlotRed);
	_pPlotRed->setAxisYMinRange(0, GMA_REG_MAX_VAL);
	_pPlotRed->addCurve(QStringLiteral(GMA_CURVE_RED_NAME), _dataChannelR);
	_pPlotRed->setCurveEditable(QStringLiteral(GMA_CURVE_RED_NAME), _editable);
	_pPlotRed->setCurveZValue(QStringLiteral(GMA_CURVE_RED_NAME), 1);
	_pPlotRed->setCurvePen(QStringLiteral(GMA_CURVE_RED_NAME), QPen(QBrush(Qt::red), 3, Qt::SolidLine));
    _pPlotRed->setCurveType(QStringLiteral(GMA_CURVE_RED_NAME), EasyPlot::Line);
    _pPlotRed->setMarkerPen(QStringLiteral(GMA_CURVE_RED_NAME), QPen(QBrush(Qt::red), 5));
    _pPlotRed->setMarkerEditPen(QStringLiteral(GMA_CURVE_RED_NAME), QPen(QBrush(qRgb(0, 207, 207)), 7));
    _pPlotRed->setMarkerType(QStringLiteral(GMA_CURVE_RED_NAME), EasyPlot::Ellipse);
	_pPlotRed->setTitleVisible(false);
	_pPlotRed->setLegendVisible(false);
	_pPlotRed->setAxisXTitleVisible(false);
    _pPlotRed->setAxisXPen(QPen(QBrush(Qt::white), 1));
    _pPlotRed->setAxisYTitleVisible(false);
    _pPlotRed->setAxisYPen(QPen(QBrush(Qt::white), 1));
    _pPlotRed->setLegendPen(QPen(QBrush(Qt::white), 1));
	_pPlotRed->setPositionInfoColor(Qt::white);

	_pPlotGreen = new EasyPlot::Plot();
	greenTabLayout->addWidget(_pPlotGreen);
	_pPlotGreen->setAxisYMinRange(0, GMA_REG_MAX_VAL);
	_pPlotGreen->addCurve(QStringLiteral(GMA_CURVE_GREEN_NAME), _dataChannelG);
	_pPlotGreen->setCurveEditable(QStringLiteral(GMA_CURVE_GREEN_NAME), _editable);
	_pPlotGreen->setCurveZValue(QStringLiteral(GMA_CURVE_GREEN_NAME), 1);
	_pPlotGreen->setCurvePen(QStringLiteral(GMA_CURVE_GREEN_NAME), QPen(QBrush(Qt::green), 3, Qt::SolidLine));
    _pPlotGreen->setCurveType(QStringLiteral(GMA_CURVE_GREEN_NAME), EasyPlot::Line);
    _pPlotGreen->setMarkerPen(QStringLiteral(GMA_CURVE_GREEN_NAME), QPen(QBrush(Qt::green), 5));
    _pPlotGreen->setMarkerEditPen(QStringLiteral(GMA_CURVE_GREEN_NAME), QPen(QBrush(qRgb(0, 207, 207)), 7));
    _pPlotGreen->setMarkerType(QStringLiteral(GMA_CURVE_GREEN_NAME), EasyPlot::Ellipse);
	_pPlotGreen->setTitleVisible(false);
	_pPlotGreen->setLegendVisible(false);
	_pPlotGreen->setAxisXTitleVisible(false);
    _pPlotGreen->setAxisXPen(QPen(QBrush(Qt::white), 1));
    _pPlotGreen->setAxisYTitleVisible(false);
    _pPlotGreen->setAxisYPen(QPen(QBrush(Qt::white), 1));
    _pPlotGreen->setLegendPen(QPen(QBrush(Qt::white), 1));
	_pPlotGreen->setPositionInfoColor(Qt::white);

	_pPlotBlue = new EasyPlot::Plot();
	blueTabLayout->addWidget(_pPlotBlue);
	_pPlotBlue->setAxisYMinRange(0, GMA_REG_MAX_VAL);
	_pPlotBlue->addCurve(QStringLiteral(GMA_CURVE_BLUE_NAME), _dataChannelB);
	_pPlotBlue->setCurveEditable(QStringLiteral(GMA_CURVE_BLUE_NAME), _editable);
	_pPlotBlue->setCurveZValue(QStringLiteral(GMA_CURVE_BLUE_NAME), 1);
	_pPlotBlue->setCurvePen(QStringLiteral(GMA_CURVE_BLUE_NAME), QPen(QBrush(Qt::blue), 3, Qt::SolidLine));
    _pPlotBlue->setCurveType(QStringLiteral(GMA_CURVE_BLUE_NAME), EasyPlot::Line);
    _pPlotBlue->setMarkerPen(QStringLiteral(GMA_CURVE_BLUE_NAME), QPen(QBrush(Qt::blue), 5));
    _pPlotBlue->setMarkerEditPen(QStringLiteral(GMA_CURVE_BLUE_NAME), QPen(QBrush(qRgb(0, 207, 207)), 7));
    _pPlotBlue->setMarkerType(QStringLiteral(GMA_CURVE_BLUE_NAME), EasyPlot::Ellipse);
	_pPlotBlue->setTitleVisible(false);
	_pPlotBlue->setLegendVisible(false);
	_pPlotBlue->setAxisXTitleVisible(false);
    _pPlotBlue->setAxisXPen(QPen(QBrush(Qt::white), 1));
    _pPlotBlue->setAxisYTitleVisible(false);
    _pPlotBlue->setAxisYPen(QPen(QBrush(Qt::white), 1));
    _pPlotBlue->setLegendPen(QPen(QBrush(Qt::white), 1));
	_pPlotBlue->setPositionInfoColor(Qt::white);

	connect(_pPlotRed, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)), this, SLOT(pointMovedChannelR(QString, unsigned, QPointF, QPointF)));
	connect(_pPlotGreen, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)), this, SLOT(pointMovedChannelG(QString, unsigned, QPointF, QPointF)));
	connect(_pPlotBlue, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)), this, SLOT(pointMovedChannelB(QString, unsigned, QPointF, QPointF)));
	connect(new_btn, SIGNAL(clicked()), this, SLOT(exportToNew()));
	connect(editRed_btn, SIGNAL(clicked()), this, SLOT(editRed()));
	connect(editGreen_btn, SIGNAL(clicked()), this, SLOT(editGreen()));
	connect(editBlue_btn, SIGNAL(clicked()), this, SLOT(editBlue()));
	connect(remove_btn, SIGNAL(clicked()), this, SLOT(removeGMA()));
	connect(range_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRange(int)));
}

//
// public slots:
//

void VisionLive::GMAWidget::retranslate()
{
	Ui::GMAWidget::retranslateUi(this);
}

void VisionLive::GMAWidget::pointMovedChannelR(QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	if(_pPlotRed)
	{
		pointMoved(_pPlotRed, name, index, prevPos, currPos);;
	}
}

void VisionLive::GMAWidget::pointMovedChannelG(QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	if(_pPlotGreen)
	{
		pointMoved(_pPlotGreen, name, index, prevPos, currPos);;
	}
}

void VisionLive::GMAWidget::pointMovedChannelB(QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	if(_pPlotBlue)
	{
		pointMoved(_pPlotBlue, name, index, prevPos, currPos);;
	}
}

void VisionLive::GMAWidget::pointMoved(EasyPlot::Plot *plot, QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	if(!plot)
	{
		return;
	}

	double max = (range_lb->currentIndex() == 0)? GMA_REG_MAX_VAL : GMA_PRE_MAX_VAL;
	double min = 0.01f;

	if(currPos.x() >= min && currPos.x() <= max &&
		currPos.y() >= min && currPos.y() <= max)
	{
		interpolatePlot(plot, name, index, prevPos, currPos);
	}
}

void VisionLive::GMAWidget::interpolatePlot(EasyPlot::Plot *plot, QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	if(!plot)
	{
		return;
	}

	QVector<QPointF> data = plot->data(name);
	data[index] = QPointF(prevPos.x(), currPos.y());

	int max = (range_lb->currentIndex() == 0)? GMA_REG_MAX_VAL : GMA_PRE_MAX_VAL;
    double currH = data.at(index).y();
    double oldH = prevPos.y();
    double delta = currH - oldH;

    for(int i = 0; i < data.size(); i++)
    {
        double ph = data[i].y();

        if(i == index)
		{
			continue;
		}

        QPointF neo(data[i]);

        if(ph > oldH)
		{
            ph = (ph-oldH)/(max-oldH) * (max-currH) + currH;
		}
        else
		{
            ph = currH - (oldH-ph)/(oldH) * (currH);
		}

        neo.setY(ph);
        if(ph < max && ph > GMA_MIN_VAL)
        {
			data[i] = neo;
        }
    }

	plot->updateCurve(name, data);
}

void VisionLive::GMAWidget::exportToNew()
{
	if(!_pPlotRed || !_pPlotGreen || !_pPlotBlue)
	{
		return;
	}

	QString name = QString();

	bool ok;
	name = QInputDialog::getText(this, tr("Export"), tr("Name:"), QLineEdit::Normal, QString(), &ok);
	if(ok)
	{
		if(name.isEmpty())
		{
			QMessageBox::warning(this, tr("Warning"), tr("Gamma LUT name empty!"));
			return;
		}

		int prevIndex = range_lb->currentIndex();
		if(prevIndex != 0)
		{
			range_lb->setCurrentIndex(0);
		}

		range_lb->setCurrentIndex(prevIndex);

		emit requestExport(_pPlotRed->data(QStringLiteral(GMA_CURVE_RED_NAME)), 
			_pPlotGreen->data(QStringLiteral(GMA_CURVE_GREEN_NAME)), 
			_pPlotBlue->data(QStringLiteral(GMA_CURVE_BLUE_NAME)), true, name);
	}
}

void VisionLive::GMAWidget::removeGMA()
{
	emit requestRemove(this);
}

void VisionLive::GMAWidget::editRed()
{
	if(!_pPlotRed)
	{
		return;
	}

	if(_pEdit)
	{
		_pEdit->deleteLater();
		delete _pEdit;
		_pEdit = NULL;
	}

	_pEdit = new EditPoints(_pPlotRed, QStringLiteral(GMA_CURVE_RED_NAME));
	_pEdit->show();
}

void VisionLive::GMAWidget::editGreen()
{
	if(!_pPlotGreen)
	{
		return;
	}

	if(_pEdit)
	{
		_pEdit->deleteLater();
		delete _pEdit;
		_pEdit = NULL;
	}

	_pEdit = new EditPoints(_pPlotGreen, QStringLiteral(GMA_CURVE_GREEN_NAME));
	_pEdit->show();
}

void VisionLive::GMAWidget::editBlue()
{
	if(!_pPlotBlue)
	{
		return;
	}

	if(_pEdit)
	{
		_pEdit->deleteLater();
		delete _pEdit;
		_pEdit = NULL;
	}

	_pEdit = new EditPoints(_pPlotBlue, QStringLiteral(GMA_CURVE_BLUE_NAME));
	_pEdit->show();
}

void VisionLive::GMAWidget::changeRange(int index)
{
	QVector<QPointF> dataRed = _pPlotRed->data(QStringLiteral(GMA_CURVE_RED_NAME));
	QVector<QPointF> dataGreen = _pPlotGreen->data(QStringLiteral(GMA_CURVE_GREEN_NAME));
	QVector<QPointF> dataBlue = _pPlotBlue->data(QStringLiteral(GMA_CURVE_BLUE_NAME));

	for(int i = 0; i < dataRed.size(); i++)
	{
		if(index == 0)
		{
			dataRed[i] = QPointF(dataRed[i].x()*(1<<GMA_LUT_FRAC), dataRed[i].y()*(1<<GMA_LUT_FRAC));
			dataGreen[i] = QPointF(dataGreen[i].x()*(1<<GMA_LUT_FRAC), dataGreen[i].y()*(1<<GMA_LUT_FRAC));
			dataBlue[i] = QPointF(dataBlue[i].x()*(1<<GMA_LUT_FRAC), dataBlue[i].y()*(1<<GMA_LUT_FRAC));
		}
		else if(index = 1)
		{
			dataRed[i] = QPointF(dataRed[i].x()/(1<<GMA_LUT_FRAC), dataRed[i].y()/(1<<GMA_LUT_FRAC));
			dataGreen[i] = QPointF(dataGreen[i].x()/(1<<GMA_LUT_FRAC), dataGreen[i].y()/(1<<GMA_LUT_FRAC));
			dataBlue[i] = QPointF(dataBlue[i].x()/(1<<GMA_LUT_FRAC), dataBlue[i].y()/(1<<GMA_LUT_FRAC));
		}
	}

	if (index == 0)
	{
		_pPlotRed->setAxisYMinRange(0, GMA_REG_MAX_VAL);
		_pPlotGreen->setAxisYMinRange(0, GMA_REG_MAX_VAL);
		_pPlotBlue->setAxisYMinRange(0, GMA_REG_MAX_VAL);
	}
	else if (index = 1)
	{
		_pPlotRed->setAxisYMinRange(0, GMA_PRE_MAX_VAL);
		_pPlotGreen->setAxisYMinRange(0, GMA_PRE_MAX_VAL);
		_pPlotBlue->setAxisYMinRange(0, GMA_PRE_MAX_VAL);
	}

	_pPlotRed->updateCurve(QStringLiteral(GMA_CURVE_RED_NAME), dataRed);
	_pPlotGreen->updateCurve(QStringLiteral(GMA_CURVE_GREEN_NAME), dataGreen);
	_pPlotBlue->updateCurve(QStringLiteral(GMA_CURVE_BLUE_NAME), dataBlue);
}