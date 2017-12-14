#ifndef GMA_WIDGET
#define GMA_WIDGET

#include "ui_gmawidget.h"
#include <plot.h>
#include <QDoubleSpinBox>

namespace VisionLive
{

class EditPoints: public QWidget
{
	Q_OBJECT

public:
	EditPoints(EasyPlot::Plot *plot, QString curveName);
	~EditPoints();

private:
	QGridLayout *_mainLayout;
	QPushButton *_import;
	QVector<QDoubleSpinBox *> _values;
	QVector<QPointF> _data;
	EasyPlot::Plot *_pPlot;
	QString _curveName;

public slots:
	void valueChanged();
	void importValues();
};

class GMAWidget: public QWidget, public Ui::GMAWidget
{
    Q_OBJECT

public:
    GMAWidget(QVector<QPointF> dataChannelR, QVector<QPointF> dataChannelG, QVector<QPointF> dataChannelB, bool editable = false, QWidget *parent = 0);
	~GMAWidget();
	const QVector<QPointF> data(int channel) const;

private:  
	QVector<QPointF> _dataChannelR, _dataChannelG, _dataChannelB;

	EasyPlot::Plot *_pPlotRed;
	EasyPlot::Plot *_pPlotGreen;
	EasyPlot::Plot *_pPlotBlue;

	bool _editable;

	EditPoints *_pEdit;

    void init();
    
signals:
	void requestExport(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name);
	void requestRemove(GMAWidget *gma);

public slots:
    void retranslate();

	void pointMovedChannelR(QString name, unsigned index, QPointF prevPos, QPointF currPos);
	void pointMovedChannelG(QString name, unsigned index, QPointF prevPos, QPointF currPos);
	void pointMovedChannelB(QString name, unsigned index, QPointF prevPos, QPointF currPos);
	void pointMoved(EasyPlot::Plot *plot, QString name, unsigned index, QPointF prevPos, QPointF currPos);

	void interpolatePlot(EasyPlot::Plot *plot, QString name, unsigned index, QPointF prevPos, QPointF currPos);

	void exportToNew();
	void removeGMA();

	void editRed();
	void editGreen();
	void editBlue();

	void changeRange(int index);
};

} // namespace VisionLive

#endif //GMA_WIDGET