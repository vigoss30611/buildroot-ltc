#ifndef EASYPLOT_PLOT_H
#define EASYPLOT_PLOT_H

#include <QGraphicsView>
#include <QWidget>
#include <QMap>

#include "common.h"

class QGraphicsItemGroup;

namespace EasyPlot {

class Component;
class Area;
class CurveArea;
class ConfigWidget;
class Curve;
class Title;
class Axis;
class Legend;
class Grid;

class Plot: public QGraphicsView
{
    Q_OBJECT

public:
    Plot(QWidget *parent = 0);
    ~Plot();
    void addCurve(const QString &name, QVector<QPointF> data);
    void updateCurve(const QString &name, QVector<QPointF> data);
    QVector<QPointF> data(const QString &name);

    void setCurvePen(const QString &name, const QPen &pen);
    void setCurveType(const QString &name, CurveType type);
    void setCurveEditable(const QString &name, bool option);
    void setCurveZValue(const QString &name, qreal z);
    void setMarkerType(const QString &name, MarkerType type);
    void setMarkerSize(const QString &name, QSize size);
    void setMarkerPen(const QString &name, QPen pen);
    void setMarkerEditPen(const QString &name, QPen pen);
    void setPositionInfoColor(const QColor &color);
    void setTitleText(const QString &text);
    void setTitleFont(const QFont &font);
    void setTitleColor(const QColor &color);
    void setTitleVisible(bool visible = true);
    void setAxisXTitleText(const QString &text);
    void setAxisXTitleFont(const QFont &font);
    void setAxisXTitleColor(const QColor &color);
    void setAxisXTitleVisible(bool visible = true);
    void setAxisXPen(const QPen &pen);
    void setAxisXVisible(bool visible = true);
	void setAxisXMinRange(float min, float max);
	void setAxisXLinesVisible(bool visible = true);
	void setAxisXLinesPen(QPen pen);
    void setAxisYTitleText(const QString &text);
    void setAxisYTitleFont(const QFont &font);
    void setAxisYTitleColor(const QColor &color);
    void setAxisYTitleVisible(bool visible = true);
    void setAxisYPen(const QPen &pen);
    void setAxisYVisible(bool visible = true);
	void setAxisYMinRange(float min, float max);
	void setAxisYLinesVisible(bool visible = true);
	void setAxisYLinesPen(QPen pen);
    void setLegendPen(const QPen &pen);
    void setLegendVisible(bool visible = true);

protected:
    QGraphicsScene *_pScene;

    Area *_pPlotArea;
	Title *_pMainTitle;
	Title *_pAxisXTitle;
	Title *_pAxisYTitle;
    Axis *_pAxisX;
    Axis *_pAxisY;
    Legend *_pLegend;
    Component *_pSpacer1;
    Component *_pSpacer2;
    Component *_pSpacer3;
    Component *_pSpacer4;
    Component *_pSpacer5;
    Component *_pSpacer6;
    CurveArea *_pCurvesArea;
    ConfigWidget *_pConfigWidget;
    QMap<QString, Curve *> _curves;
	Grid *_pGrid;

    TitleData _mainTitleData;
    TitleData _axisXTitleData;
    TitleData _axisYTitleData;
    AxisData _axisXData;
    AxisData _axisYData;
    QMap<QString, CurveData> _curvesData;
    LegendData _legendData;
	GridData _gridData;

	float _minX;
	float _maxX;
	float _minY;
	float _maxY;

    bool _curvesHidden;

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    QSize cSize(Component *component);
    DataStats calculateStats();
	void updateStatistics();

private:
    void init();
    void initScene();
    void initComponents();

signals:
    void pointChanged(QString name, unsigned index, QPointF prevPos, QPointF currPos);

public slots:
    void repositionComponents();
    void showSelectedCurve(QString option);
    void rotate(qreal pos);

};

} // namesapce EasyPlot

#endif // EASYPLOT_PLOT_H
