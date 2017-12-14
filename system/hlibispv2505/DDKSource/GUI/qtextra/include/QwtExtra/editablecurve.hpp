/**
******************************************************************************
 @file editable_curve.hpp

 @brief BasicScaler class definition

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K.

******************************************************************************/

#ifndef EDITABLE_CURVE
#define EDITABLE_CURVE

#include <QWidget>
#include <QtCore/QObject>
#include <QtCore/QEvent>

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>

namespace QwtExtra {

	class EditableCurve: public QwtPlot
	{
		Q_OBJECT

	public:
		EditableCurve(QWidget *parent = 0);
		~EditableCurve();

		int nCurves() const;
		
		int addCurve(const QVector<QPointF> &data, const QString &name = "");
		void removeCurve(int curve);

		QwtPlotCurve* curve(int curve); // access one curve
		QVector<QPointF>* data(int curve); // access data
		bool editable(int curve, int axis); // 0 for horiz, 1 for vert
		void setEditable(int curve, int axis, bool value);
		void setEditable(int curve, bool horiz, bool vert);

		void cancelSelection();

		void setPrecision(double precision);
		double precision() const;
		
	protected:
		int selectedPoint;
		int selectedCurve;
		double _precision; // precision of the mouse click in pixels (distance)

		struct Curve
		{
			QwtPlotCurve curve;
			QVector<QPointF> data;
			bool editable[2]; // editable: horiz, vert

			Curve(const QVector<QPointF>data, bool editHoriz = true, bool editVert = true);

			void move(int point, const QPointF &neo);
			bool isEditable() const { return editable[0]||editable[1]; }
		};
		
		QVector< Curve* > curves;
		QwtPlotPicker tracker; // use to convert the position of the mouse

		void mouseMoveEvent(QMouseEvent *event);
		

	public slots:
		// similar to mousePressEvent but with graph's position
		void mouseClicked(const QPoint& pos);

	signals:
		void pointSelected(int curve, int point);
		void pointMoved(int curve, int point, QPointF prevPosition);
	};
	
} // namespace QtExtra

#endif // EDITABLE_CURVE
