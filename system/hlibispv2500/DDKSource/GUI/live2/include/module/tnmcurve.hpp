#ifndef TNMCURVE_H
#define TNMCURVE_H

#include <QDialog>

#include <qwt_symbol.h>

#include "ui_tnmcurve.h"

#define TNM_CURVE_MAX 1.0f
#define TNM_CURVE_MIN 0.0f

class TNMCurve: public QWidget, public Ui::TNMCurve
{
    Q_OBJECT
private:
    void init();

protected:
    QVector<QPointF> _originalData;
    QPen correctCurve;
    QPen wrongCurve;
    QwtSymbol selectSymbol;
    bool editable;
    
    int tnmC; // tnm curve ID
    int idC; // identity curve ID

public:
    // expects data to have TNM_CURVE_N+2 elements
    TNMCurve(QWidget *parent = 0);

    const QVector<QPointF>& curveData() const;
    const QVector<QPointF>& originalData() const { return _originalData; } // can be used as a getter or setter
    int setData(const QVector<QPointF>& newData);
    bool curveEditable() const;

public slots:
    void setCurveEditable(bool);
    void replotCurve(); // replots using _originalData
    void curveToIdentity(); // does not change _originalData
    
    void pointSelected(int curveIdx, int point);
    void pointMoved(int curveIdx, int point, QPointF prevPosition);
    bool verify(); // does the replot (and change colours if valid or not)
};

#endif // TNMCURVE_H
