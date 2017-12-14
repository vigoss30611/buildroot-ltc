#ifndef EXTPLOT_H
#define EXTPLOT_H

#include <QWidget>

#include "common.h"

class QGridLayout;
class QComboBox;
class QSpacerItem;

namespace EasyPlot {

	enum ViewType{
		JoinedView,
		SplitView,
	};

	enum SplitViewTpye{
		HorizontalView,
		VerticalView,
		GridView
	};

	class Plot;

	class ExtPlot : public QWidget
	{
		Q_OBJECT
	public:
		ExtPlot(QWidget *parent = 0);
		~ExtPlot();
		void addCurve(const QString &name, QVector<QPointF> data);
		void updateCurve(const QString &name, QVector<QPointF> data);
		QVector<QPointF> data(const QString &name);
		void setCurvePen(const QString &name, const QPen &pen);
		void setCurveType(const QString &name, CurveType type);
		void setCurveEditable(const QString &name, bool option);
		void setCurveZValue(const QString &name, qreal z);
		void setCurveVisible(const QString &name, bool visible = true);
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
		void setViewType(ViewType type);
		void setSplitViewType(SplitViewTpye type);

	private:

		QGridLayout *_pMainLayout;
		QGridLayout *_pControlLayout;
		QGridLayout *_pPlotLayout;
		QComboBox *_pViewOption;
		QComboBox *_pViewLayoutOption;
		QSpacerItem *_pSpacer;

		Plot *_pJoinPlot;
		QMap<QString, Plot *> _SplitPlot;

		QVector<QPointF> _zeroData;

		void init();
		void resetLayouts();

	signals:
		void pointChanged(QString name, unsigned index, QPointF prevPos, QPointF currPos);

		public slots:
		void changeView();

	};

} // namespace EasyPlot

#endif // EXTPLOT_H
