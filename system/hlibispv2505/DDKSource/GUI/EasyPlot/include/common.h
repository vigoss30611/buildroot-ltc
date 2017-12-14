#ifndef EASYPLOT_COMMON
#define EASYPLOT_COMMON

#include <QPen>
#include <QFont>

#include <QDebug>

#define EASYPLOT_FORMAT_CHANGE_VALUE 10000.0f

namespace EasyPlot {

enum Orientation{
    Horizontal,
    Vertical
};

enum ScaleType{
    Linear,
    Logaritmic
};

enum CurveType{
    Line,
    Point,
    Bar
};

enum MarkerType{
    Rectangle,
    Ellipse
};

struct DataStats {
    float minX;
    float maxX;
    float minY;
    float maxY;
    DataStats() {
        minX = 0.0;
        maxX = 0.0;
        minY = 0.0;
        maxY = 0.0;
    }
};

struct TitleData {
    QString text;
    QFont font;
    QColor color;
    TitleData() {
        text = QString();
        font = QFont();
        color = Qt::black;
    }
};

struct AxisData {
    DataStats stats;
    int numMarks;
    int minMarkSize;
    int midMarkSize;
    int maxMarkSize;
    QPen pen;
    ScaleType scaleType;
	bool drawGrid;
	QPen gridPen;
    AxisData() {
        stats = DataStats();
        numMarks = 20;
        minMarkSize = 5;
        midMarkSize = 7;
        maxMarkSize = 10;
        pen = QPen();
        scaleType = Linear;
		drawGrid = false;
		gridPen = QPen();
    }
};

struct MarkerData {
    MarkerType type;
    QPen pen;
    QPen editPen;
    QSize size;
    MarkerData() {
        type = Rectangle;
        pen = QPen();
        editPen = QPen();
        size = QSize(5, 5);
    }
};

struct CurveData {
    QVector<QPointF> data;
    DataStats stats;
    QPen pen;
    CurveType type;
    QString name;
    bool editable;
    ScaleType xScaleType;
    ScaleType yScaleType;
    MarkerData markerData;
    QColor positionInfoColor;
    CurveData() {
        stats = DataStats();
        pen = QPen();
        type = Line;
        name = QString();
        editable = false;
        xScaleType = Linear;
        yScaleType = Logaritmic;
        positionInfoColor = QColor();
    }
};

struct GridData {
	AxisData axisXData;
	AxisData axisYData;
};

struct LegendData {
    int spacing;
    int padding;
    QPen curveNamePen;
    QMap<QString, CurveData> curveData;
    LegendData() {
        spacing = 20;
        padding = 10;
        curveNamePen = QPen();
    }
};

} // namespace EasyPlot

#endif // EASYPLOT_COMMON



