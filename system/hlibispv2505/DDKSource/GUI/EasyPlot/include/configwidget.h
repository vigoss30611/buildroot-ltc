#ifndef EASYPLOT_CONFIGWIDGET_H
#define EASYPLOT_CONFIGWIDGET_H

#include <QWidget>

class QGridLayout;
class QLabel;
class QComboBox;

namespace EasyPlot {

class ConfigWidget : public QWidget
{
    Q_OBJECT

public:
    ConfigWidget(QWidget *parent = 0);
    void setCurveSelectionOptions(QStringList options);

protected:
    QGridLayout *_pMainLayout;
    QLabel *_pShowCurveLabel;
    QComboBox *_pShowCurve;

private:
    void init();

signals:
    void curveSelectionChanged(QString selection);

public slots:

};

} // namespace EasyPlot

#endif // CONFIGWIDGET_H
