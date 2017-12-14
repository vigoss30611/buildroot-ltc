#include "plot.h"
#include "component.h"
#include "area.h"
#include "curvearea.h"
#include "configwidget.h"
#include "curve.h"
#include "title.h"
#include "axis.h"
#include "legend.h"
#include "grid.h"

#include <QResizeEvent>
#include <QGraphicsItemGroup>

EasyPlot::Plot::Plot(QWidget *parent): QGraphicsView(parent)
{
    _pScene = NULL;
    _pPlotArea = NULL;
    _pMainTitle = NULL;
    _pAxisXTitle = NULL;
    _pAxisYTitle = NULL;
    _pAxisX = NULL;
    _pAxisY = NULL;
    _pLegend = NULL;
    _pSpacer1 = NULL;
    _pSpacer2 = NULL;
    _pSpacer3 = NULL;
    _pSpacer4 = NULL;
    _pSpacer5 = NULL;
    _pSpacer6 = NULL;
    _pCurvesArea = NULL;
    _pConfigWidget = NULL;

	_minX = 0.0f;
	_maxX = 0.0f;
	_minY = 0.0f;
	_maxY = 0.0f;

    init();
}

EasyPlot::Plot::~Plot()
{
    _pScene->clear();

    if(_pScene)
    {
        delete _pScene;
    }
}

void EasyPlot::Plot::addCurve(const QString &name, QVector<QPointF> data)
{
    if((!_curves.isEmpty() && _curves.find(name) != _curves.end()) ||
       (!_curvesData.isEmpty() && _curvesData.find(name) != _curvesData.end()) ||
            data.size() < 2 || !_pCurvesArea || !_pConfigWidget)
        return;

    CurveData newCurveData;
    newCurveData.data = data;
    newCurveData.name = name;
    _curvesData.insert(name, newCurveData);

    Curve *newCurve = new Curve(500, 500);
    newCurve->setZValue(100);
    _curves.insert(name, newCurve);
    _pCurvesArea->addToGroup(newCurve);

    QObject::connect(newCurve, SIGNAL(pointChanged(QString,uint,QPointF,QPointF)), this, SIGNAL(pointChanged(QString,uint,QPointF,QPointF)));

	updateStatistics();

    _legendData.curveData.insert(name, newCurveData);
    _pLegend->setData(_legendData);

    QStringList curveOptions;
    curveOptions.push_back("All");
    QMap<QString, Curve *>::iterator it;
    for(it = _curves.begin(); it != _curves.end(); it++)
        curveOptions.push_back(it.key());
    _pConfigWidget->setCurveSelectionOptions(curveOptions);
}

void EasyPlot::Plot::updateCurve(const QString &name, QVector<QPointF> data)
{
    if(_curves.isEmpty() || _curves.find(name) == _curves.end() ||
       _curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
            data.size() < 2)
        return;

    _curvesData[name].data = data;

	updateStatistics();

    if(_pPlotArea)
        _pPlotArea->update();
}

QVector<QPointF> EasyPlot::Plot::data(const QString &name)
{
    QVector<QPointF> ret;

    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end())
        return ret;

    return _curvesData[name].data;
}

void EasyPlot::Plot::setCurvePen(const QString &name, const QPen &pen)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].pen = pen;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].pen = pen;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setCurveType(const QString &name, CurveType type)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].type = type;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].type = type;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setCurveEditable(const QString &name, bool option)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].editable = option;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].editable = option;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setCurveZValue(const QString &name, qreal z)
{
    if(_curves.isEmpty() || _curves.find(name) == _curves.end())
        return;

    _curves[name]->setZValue(z);

    _pPlotArea->update();
}

void EasyPlot::Plot::setMarkerType(const QString &name, MarkerType type)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].markerData.type = type;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].markerData.type = type;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setMarkerSize(const QString &name, QSize size)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].markerData.size = size;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].markerData.size = size;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setMarkerPen(const QString &name, QPen pen)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].markerData.pen = pen;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].markerData.pen = pen;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setMarkerEditPen(const QString &name, QPen pen)
{
    if(_curvesData.isEmpty() || _curvesData.find(name) == _curvesData.end() ||
       _curves.isEmpty() || _curves.find(name) == _curves.end() ||
       !_pLegend || !_pPlotArea)
        return;

    _curvesData[name].markerData.editPen = pen;
    _curves[name]->setData(_curvesData[name]);

    _legendData.curveData[name].markerData.editPen = pen;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setPositionInfoColor(const QColor &color)
{
    if(!_pLegend || !_pPlotArea)
        return;

    QMap<QString, CurveData>::iterator curveDataIt;
    for(curveDataIt = _curvesData.begin(); curveDataIt != _curvesData.end(); curveDataIt++)
    {
        curveDataIt->positionInfoColor = color;
        _curves[curveDataIt->name]->setData(*curveDataIt);
    }

    QMap<QString, CurveData>::iterator legendDataIt;
    for(legendDataIt = _legendData.curveData.begin(); legendDataIt != _legendData.curveData.end(); legendDataIt++)
        legendDataIt->positionInfoColor = color;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setTitleText(const QString &text)
{
    if(!_pMainTitle || !_pPlotArea)
        return;

    _mainTitleData.text = text;
    _pMainTitle->setData(_mainTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setTitleFont(const QFont &font)
{
    if(!_pMainTitle || !_pPlotArea)
        return;

    _mainTitleData.font = font;
    _pMainTitle->setData(_mainTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setTitleColor(const QColor &color)
{
    if(!_pMainTitle || !_pPlotArea)
        return;

    _mainTitleData.color = color;
    _pMainTitle->setData(_mainTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setTitleVisible(bool visible)
{
    if(!_pMainTitle || !_pPlotArea)
        return;

    _pMainTitle->setVisible(visible);

    _pPlotArea->update();

	repositionComponents();
}

void EasyPlot::Plot::setAxisXTitleText(const QString &text)
{
    if(!_pAxisXTitle || !_pPlotArea)
        return;

    _axisXTitleData.text = text;
    _pAxisXTitle->setData(_axisXTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisXTitleFont(const QFont &font)
{
    if(!_pAxisXTitle || !_pPlotArea)
        return;

    _axisXTitleData.font = font;
    _pAxisXTitle->setData(_axisXTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisXTitleColor(const QColor &color)
{
    if(!_pAxisXTitle || !_pPlotArea)
        return;

    _axisXTitleData.color = color;
    _pAxisXTitle->setData(_axisXTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisXTitleVisible(bool visible)
{
    if(!_pAxisXTitle || !_pPlotArea)
        return;

    _pAxisXTitle->setVisible(visible);

    _pPlotArea->update();

	repositionComponents();
}

void EasyPlot::Plot::setAxisXPen(const QPen &pen)
{
    if(!_pAxisX || !_pPlotArea)
        return;

    _axisXData.pen = pen;
    _pAxisX->setData(_axisXData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisXVisible(bool visible)
{
    if(!_pAxisX || !_pPlotArea)
        return;

    _pAxisX->setVisible(visible);

    _pPlotArea->update();

	repositionComponents();
}

void EasyPlot::Plot::setAxisXMinRange(float min, float max)
{
	_minX = min;
	_maxX = max;

	updateStatistics();
}

void EasyPlot::Plot::setAxisXLinesVisible(bool visible)
{
	_gridData.axisXData.drawGrid = visible;

	if (_pGrid) _pGrid->setData(_gridData);
}

void EasyPlot::Plot::setAxisXLinesPen(QPen pen)
{
	_gridData.axisXData.gridPen = pen;

	if (_pGrid) _pGrid->setData(_gridData);
}

void EasyPlot::Plot::setAxisYTitleText(const QString &text)
{
    if(!_pAxisYTitle || !_pPlotArea)
        return;

    _axisYTitleData.text = text;
    _pAxisYTitle->setData(_axisYTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisYTitleFont(const QFont &font)
{
    if(!_pAxisYTitle || !_pPlotArea)
        return;

    _axisYTitleData.font = font;
    _pAxisYTitle->setData(_axisYTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisYTitleColor(const QColor &color)
{
    if(!_pAxisYTitle || !_pPlotArea)
        return;

    _axisYTitleData.color = color;
    _pAxisYTitle->setData(_axisYTitleData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisYTitleVisible(bool visible)
{
    if(!_pAxisYTitle || !_pPlotArea)
        return;

    _pAxisYTitle->setVisible(visible);

    _pPlotArea->update();

	repositionComponents();
}

void EasyPlot::Plot::setAxisYPen(const QPen &pen)
{
    if(!_pAxisY || !_pPlotArea)
        return;

    _axisYData.pen = pen;
    _pAxisY->setData(_axisYData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setAxisYVisible(bool visible)
{
    if(!_pAxisY || !_pPlotArea)
        return;

    _pAxisY->setVisible(visible);

    _pPlotArea->update();

	repositionComponents();
}

void EasyPlot::Plot::setAxisYMinRange(float min, float max)
{
	_minY = min;
	_maxY = max;

	updateStatistics();
}

void EasyPlot::Plot::setAxisYLinesVisible(bool visible)
{
	_gridData.axisYData.drawGrid = visible;

	if (_pGrid) _pGrid->setData(_gridData);
}

void EasyPlot::Plot::setAxisYLinesPen(QPen pen)
{
	_gridData.axisYData.gridPen = pen;

	if (_pGrid) _pGrid->setData(_gridData);
}

void EasyPlot::Plot::setLegendPen(const QPen &pen)
{
    if(!_pLegend || !_pPlotArea)
        return;

    _legendData.curveNamePen = pen;
    _pLegend->setData(_legendData);

    _pPlotArea->update();
}

void EasyPlot::Plot::setLegendVisible(bool visible)
{
    if(!_pLegend || !_pPlotArea)
        return;

    _pLegend->setVisible(visible);

    _pPlotArea->update();

	repositionComponents();
}

void EasyPlot::Plot::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    repositionComponents();
}

void EasyPlot::Plot::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}

QSize EasyPlot::Plot::cSize(Component *component)
{
    if(!component)
        return QSize(0, 0);

    return ((component->isVisible())? QSize(component->boundingRect().width(), component->boundingRect().height()) : QSize(0, 0));
}

EasyPlot::DataStats EasyPlot::Plot::calculateStats()
{
    DataStats stats;

    QMap<QString, CurveData>::iterator it;
    for(it = _curvesData.begin(); it != _curvesData.end(); it++)
    {
        for(int i = 0; i < it.value().data.size(); i++)
        {
            (it.value().data[i].x() < stats.minX)?
                        stats.minX = it.value().data[i].x() : stats.minX;
            (it.value().data[i].x() > stats.maxX)?
                        stats.maxX = it.value().data[i].x() : stats.maxX;
            (it.value().data[i].y() < stats.minY)?
                        stats.minY = it.value().data[i].y() : stats.minY;
            (it.value().data[i].y() > stats.maxY)?
                        stats.maxY = it.value().data[i].y() : stats.maxY;
        }
    }

	if (stats.maxX < _maxX) stats.maxX = _maxX;
	if (stats.minX > _minX) stats.minX = _minX;
	if (stats.maxY < _maxY) stats.maxY = _maxY;
	if (stats.minY > _minY) stats.minY = _minY;

    return stats;
}

void EasyPlot::Plot::updateStatistics()
{
	DataStats stats = calculateStats();

	QMap<QString, CurveData>::iterator curveDataIt;
	for (curveDataIt = _curvesData.begin(); curveDataIt != _curvesData.end(); curveDataIt++)
		curveDataIt.value().stats = stats;

	QMap<QString, Curve *>::iterator curveIt;
	for (curveIt = _curves.begin(); curveIt != _curves.end(); curveIt++)
		curveIt.value()->setData(_curvesData[curveIt.key()]);

	_axisXData.stats = stats;
	_axisYData.stats = stats;

	_pAxisX->setData(_axisXData);
	_pAxisY->setData(_axisYData);
}

void EasyPlot::Plot::init()
{
    _curvesHidden = false;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumSize(300, 150);

    initScene();
    initComponents();
}

void EasyPlot::Plot::initScene()
{
    if(_pScene)
        delete _pScene;

    _pScene = new QGraphicsScene(this);
    setScene(_pScene);
}

void EasyPlot::Plot::initComponents()
{
    if(!_pScene)
        return;

    _pPlotArea = new Area(800, 670);
    _pScene->addItem(_pPlotArea);

	_pMainTitle = new Title(800, 50, Horizontal);
    _pMainTitle->setZValue(101);
    _pMainTitle->setPen(QPen(Qt::red));
    _pMainTitle->setData(_mainTitleData);
    _pPlotArea->addToGroup(_pMainTitle);
    QObject::connect(_pMainTitle, SIGNAL(reposition()), this, SLOT(repositionComponents()));

    _pAxisXTitle = new Title(500, 50, Horizontal);
    _pAxisXTitle->setZValue(101);
    _pAxisXTitle->setPen(QPen(Qt::green));
    _pAxisXTitle->setData(_axisXTitleData);
    _pPlotArea->addToGroup(_pAxisXTitle);
    QObject::connect(_pAxisXTitle, SIGNAL(reposition()), this, SLOT(repositionComponents()));

    _pAxisX = new Axis(500, 50, Horizontal);
    _pAxisX->setZValue(101);
    _pAxisX->setPen(QPen(Qt::blue));
    _pPlotArea->addToGroup(_pAxisX);
    QObject::connect(_pAxisX, SIGNAL(reposition()), this, SLOT(repositionComponents()));

    _pAxisYTitle = new Title(50, 500, Vertical);
    _pAxisYTitle->setZValue(101);
    _pAxisYTitle->setPen(QPen(Qt::yellow));
    _pAxisYTitle->setData(_axisYTitleData);
    _pPlotArea->addToGroup(_pAxisYTitle);
    QObject::connect(_pAxisYTitle, SIGNAL(reposition()), this, SLOT(repositionComponents()));

    _pAxisY = new Axis(50, 500, Vertical);
    _pAxisY->setZValue(101);
    _pAxisY->setPen(QPen(Qt::cyan));
    _pPlotArea->addToGroup(_pAxisY);
    QObject::connect(_pAxisY, SIGNAL(reposition()), this, SLOT(repositionComponents()));

    _pLegend = new Legend(200, 500);
    _pLegend->setZValue(101);
    _pLegend->setPen(QPen(Qt::magenta));
    _pPlotArea->addToGroup(_pLegend);
    QObject::connect(_pLegend, SIGNAL(reposition()), this, SLOT(repositionComponents()));

    _pSpacer1 = new Component(800, 10);
    _pSpacer1->setZValue(101);
    _pSpacer1->setPen(QPen(Qt::gray));
    _pPlotArea->addToGroup(_pSpacer1);

    _pSpacer2 = new Component(10, 500);
    _pSpacer2->setZValue(101);
    _pSpacer2->setPen(QPen(Qt::darkBlue));
    _pPlotArea->addToGroup(_pSpacer2);

    _pSpacer3 = new Component(10, 500);
    _pSpacer3->setZValue(101);
    _pSpacer3->setPen(QPen(Qt::darkRed));
    _pPlotArea->addToGroup(_pSpacer3);

    _pSpacer4 = new Component(800, 10);
    _pSpacer4->setZValue(101);
    _pSpacer4->setPen(QPen(Qt::darkGreen));
    _pPlotArea->addToGroup(_pSpacer4);

    _pSpacer5 = new Component(100, 100);
    _pSpacer5->setZValue(101);
    _pSpacer5->setPen(QPen(Qt::darkCyan));
    _pPlotArea->addToGroup(_pSpacer5);

    _pSpacer6 = new Component(200, 100);
    _pSpacer6->setZValue(101);
    _pSpacer6->setPen(QPen(Qt::darkMagenta));
    _pPlotArea->addToGroup(_pSpacer6);

    _pConfigWidget = new ConfigWidget();
	QObject::connect(_pConfigWidget, SIGNAL(curveSelectionChanged(QString)), this, SLOT(showSelectedCurve(QString)));

    _pCurvesArea = new CurveArea(500, 500, _pConfigWidget);
    _pCurvesArea->setZValue(101);
    _pPlotArea->addToGroup(_pCurvesArea);
    QObject::connect(_pCurvesArea, SIGNAL(rotate(qreal)), this, SLOT(rotate(qreal)));

	_pGrid = new Grid(500, 500);
	_pCurvesArea->setZValue(101);
	_pCurvesArea->addToGroup(_pGrid);
}

void EasyPlot::Plot::repositionComponents()
{
    if(_pPlotArea && _pMainTitle && _pAxisXTitle && _pAxisX && _pAxisYTitle && _pAxisY && _pLegend &&
		_pSpacer1 && _pSpacer2 && _pSpacer3 && _pSpacer4 && _pSpacer5 && _pSpacer6 && _pCurvesArea && 
		_pGrid)
    {
        _pPlotArea->resize(width(), height());

        _pMainTitle->resize(width(), -1);
        _pMainTitle->setPos(0, 0);

        _pSpacer1->resize(width(), -1);
        _pSpacer1->setPos(0, cSize(_pMainTitle).height());

        _pAxisYTitle->resize(-1, height()-cSize(_pMainTitle).height()-cSize(_pSpacer1).height()-cSize(_pSpacer4).height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());
        _pAxisYTitle->setPos(0, cSize(_pMainTitle).height()+cSize(_pSpacer1).height());

        _pAxisY->resize(-1,  height()-cSize(_pMainTitle).height()-cSize(_pSpacer1).height()-cSize(_pSpacer4).height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());
        _pAxisY->setPos(cSize(_pAxisYTitle).width(), cSize(_pMainTitle).height()+cSize(_pSpacer1).height());

        _pSpacer2->resize(-1, height()-cSize(_pMainTitle).height()-cSize(_pSpacer1).height()-cSize(_pSpacer4).height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());
        _pSpacer2->setPos(cSize(_pAxisYTitle).width()+cSize(_pAxisY).width(), cSize(_pMainTitle).height()+cSize(_pSpacer1).height());

		_pCurvesArea->resize(width() - cSize(_pAxisYTitle).width() - cSize(_pAxisY).width() - cSize(_pSpacer2).width() - cSize(_pSpacer3).width() - cSize(_pLegend).width(),
			height() - cSize(_pMainTitle).height() - cSize(_pSpacer1).height() - cSize(_pSpacer4).height() - cSize(_pAxisX).height() - cSize(_pAxisXTitle).height());
        _pCurvesArea->setPos(cSize(_pAxisYTitle).width()+cSize(_pAxisY).width()+cSize(_pSpacer2).width(),
                           +cSize(_pMainTitle).height()+cSize(_pSpacer1).height());

        _pConfigWidget->resize(_pCurvesArea->boundingRect().width(), _pCurvesArea->boundingRect().height());

		_pGrid->resize(_pCurvesArea->boundingRect().width(), _pCurvesArea->boundingRect().height());

        QMap<QString, Curve *>::iterator it;
        for(it = _curves.begin(); it != _curves.end(); it++)
            it.value()->resize(_pCurvesArea->boundingRect().width(), _pCurvesArea->boundingRect().height());

        _pSpacer3->resize(-1, height()-cSize(_pMainTitle).height()-cSize(_pSpacer1).height()-cSize(_pSpacer4).height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());
        _pSpacer3->setPos(width()-cSize(_pLegend).width()-cSize(_pSpacer3).width(), cSize(_pMainTitle).height()+cSize(_pSpacer1).height());

        _pLegend->resize(-1, height()-cSize(_pMainTitle).height()-cSize(_pSpacer1).height()-cSize(_pSpacer4).height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());
        _pLegend->setPos(width()-cSize(_pLegend).width(), cSize(_pMainTitle).height()+cSize(_pSpacer1).height());

        _pSpacer4->resize(width(), -1);
        _pSpacer4->setPos(0, height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height()-cSize(_pSpacer4).height());

        _pSpacer5->resize(cSize(_pAxisYTitle).width()+cSize(_pAxisY).width()+cSize(_pSpacer2).width(),
                          cSize(_pAxisXTitle).height()+cSize(_pAxisX).height());
        _pSpacer5->setPos(0, height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());

        _pAxisX->resize(width()-cSize(_pAxisYTitle).width()-cSize(_pAxisY).width()-cSize(_pSpacer2).width()-cSize(_pSpacer3).width()-cSize(_pLegend).width(), -1);
        _pAxisX->setPos(cSize(_pAxisYTitle).width()+cSize(_pAxisY).width()+cSize(_pSpacer2).width(),
                        height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());

        _pAxisXTitle->resize(width()-cSize(_pAxisYTitle).width()-cSize(_pAxisY).width()-cSize(_pSpacer2).width()-cSize(_pSpacer3).width()-cSize(_pLegend).width(), -1);
        _pAxisXTitle->setPos(cSize(_pAxisYTitle).width()+cSize(_pAxisY).width()+cSize(_pSpacer2).width(),
                             height()-cSize(_pAxisXTitle).height());

        _pSpacer6->resize(cSize(_pSpacer3).width()+cSize(_pLegend).width(), cSize(_pAxisXTitle).height()+cSize(_pAxisX).height());
        _pSpacer6->setPos(width()-cSize(_pSpacer3).width()-cSize(_pLegend).width(), height()-cSize(_pAxisXTitle).height()-cSize(_pAxisX).height());

		if(_pScene)
			_pScene->setSceneRect(0, 0, width(), height());
    }
}

void EasyPlot::Plot::showSelectedCurve(QString option)
{
    QMap<QString, Curve *>::iterator it;
    for(it = _curves.begin(); it != _curves.end(); it++)
        it.value()->setShow(option == it.key() || option == "All");
}

void EasyPlot::Plot::rotate(qreal pos)
{
    if(!_pCurvesArea || !_pConfigWidget || !_pGrid)
        return;

    int angle = (pos < 0.5)? static_cast<int>(180*pos) : static_cast<int>(180*pos) + 180;

    QTransform t;
    t.translate(_pCurvesArea->boundingRect().width()/2, _pCurvesArea->boundingRect().height()/2);
    t.rotate(angle, Qt::YAxis);
    t.translate(-_pCurvesArea->boundingRect().width()/2, -_pCurvesArea->boundingRect().height()/2);
    _pCurvesArea->setTransform(t, false);

    if(pos > 0.5 && !_curvesHidden)
    {
        QMap<QString, Curve *>::iterator it;
        for(it = _curves.begin(); it != _curves.end(); it++)
            it.value()->setVisible(!it.value()->isVisible());

		_pGrid->setVisible(!_pGrid->isVisible());

        _pConfigWidget->setVisible(!_pConfigWidget->isVisible());

        _curvesHidden = !_curvesHidden;
    }
    else if(pos == 1.0)
        _curvesHidden = !_curvesHidden;
}


