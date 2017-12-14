#include "configwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

EasyPlot::ConfigWidget::ConfigWidget(QWidget *parent) : QWidget(parent)
{
    _pMainLayout = NULL;
    _pShowCurveLabel = NULL;
    _pShowCurve = NULL;

    init();
}

void EasyPlot::ConfigWidget::setCurveSelectionOptions(QStringList options)
{
    if(!_pShowCurve)
        return;

    _pShowCurve->clear();
    _pShowCurve->addItems(options);
}

void EasyPlot::ConfigWidget::init()
{
    _pMainLayout = new QGridLayout(this);
    setLayout(_pMainLayout);

    _pShowCurveLabel = new QLabel("Show Curve:", this);
    _pMainLayout->addWidget(_pShowCurveLabel, 0, 0, 1, 1, Qt::AlignRight);

    _pShowCurve = new QComboBox(this);
    _pMainLayout->addWidget(_pShowCurve, 0, 1, 1, 1, Qt::AlignLeft);

    QObject::connect(_pShowCurve, SIGNAL(currentTextChanged(QString)),
                     this, SIGNAL(curveSelectionChanged(QString)));

    setObjectName("ConfigWidget");
    setStyleSheet(QStringLiteral("QWidget#ConfigWidget { background-color: transparent }") +
                  QStringLiteral("QLabel { color: white; }"));
}
