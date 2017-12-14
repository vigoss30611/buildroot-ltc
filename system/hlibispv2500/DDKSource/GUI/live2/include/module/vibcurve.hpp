#ifndef VIBCURVE_H
#define VIBCURVE_H

#include <QDialog>
#include <QPen>
#include <qwt_symbol.h>
#include "ui_vibcurve.h"

class VIBCurve: public QWidget, public Ui::VIBCurve
{
    Q_OBJECT

private:
    void init();

protected:
    QPen settPen;
    QwtSymbol* settSymbol;
    
    int idC; // identity curve ID
	int settC; // points settings curve ID

	void setupDoublePoints(double in1, double out1, double in2, double out2, double *satMult);

public:
    VIBCurve(QWidget *parent = 0);

	QVector<QPointF> getGains();

signals:
	void updateSpins(double, double, double, double);

public slots:
    void receivePoints(double, double, double, double);
	void pointMoved(int curveIdx, int point, QPointF prevPosition);
};

#endif // VIBCURVE_H
