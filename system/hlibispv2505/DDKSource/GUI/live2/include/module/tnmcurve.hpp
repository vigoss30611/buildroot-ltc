#ifndef TNMCURVE_H
#define TNMCURVE_H

#include "ui_tnmcurve.h"

#include <plot.h>

namespace VisionLive
{

class TNMCurve: public QWidget, public Ui::TNMCurve
{
    Q_OBJECT

public:
    TNMCurve(QWidget *parent = 0);
	~TNMCurve();
    void setData(const QVector<QPointF> newData);
	const QVector<QPointF> data() const;
    bool curveEditable() const;

private:
	EasyPlot::Plot *_pPlot;
    bool _editable;

    void init();

public slots:
    void setCurveEditable(bool editable);
    void curveToIdentity(); // does not change _originalData
    void pointMoved(QString name, unsigned index, QPointF prevPos, QPointF currPos);
};

} // namespace VisionLive

#endif // TNMCURVE_H
