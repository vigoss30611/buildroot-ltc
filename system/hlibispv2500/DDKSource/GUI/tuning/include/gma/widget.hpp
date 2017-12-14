/********************************************************************************
@file gma/widget.hpp

@brief Displayable inputs for GMAWidget

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

#ifndef GMA_WIDGET
#define GMA_WIDGET

#include "ui/ui_gmawidget.h"
#include <QwtExtra/editablecurve.hpp>
#include <QPen>
#include <QDoubleSpinBox>

#define GMA_REG_MAX_VAL 4096.0f
#define GMA_PRE_MAX_VAL 255.0f

class EditPoints: public QWidget
{
	Q_OBJECT

private:
	QGridLayout *_mainLayout;
	QPushButton *_import;
	QVector<QDoubleSpinBox *> _values;
	QVector<QPointF> *_data;
	QwtExtra::EditableCurve *_curve;
	int _id;
public:
	EditPoints(QwtExtra::EditableCurve *curve, int id, QString name);
	~EditPoints();

public slots:
	void valueChanged();
	void importValues();
};

class GMAWidget: public QWidget, public Ui::GMAWidget
{
    Q_OBJECT
private:  
	QwtExtra::EditableCurve *_pCurveChannelR, *_pCurveChannelG, *_pCurveChannelB;
	QVector<QPointF> _dataChannelR, _dataChannelG, _dataChannelB;
	int _idR, _idG, _idB;
	QPen _penR, _penG, _penB;
    QwtSymbol *_symbolR, *_symbolG, *_symbolB;

	bool _editable;

	EditPoints *_edit;

    void init();
    
public:
    GMAWidget(QVector<QPointF> dataChannelR, QVector<QPointF> dataChannelG, QVector<QPointF> dataChannelB, bool editable = false, QWidget *parent = 0);
	~GMAWidget();

	QVector<QPointF> getData(int channel);
    
signals:
	void requestExport(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name);
	void requestRemove(GMAWidget *gma);

public slots:
    void retranslate();

	void pointMovedChannelR(int, int, QPointF);
	void pointMovedChannelG(int, int, QPointF);
	void pointMovedChannelB(int, int, QPointF);
	void interpolatePlot(int intcurveIdx, int point, QPointF prevPosition, QwtExtra::EditableCurve *curve, int id);

	void exportToNew();
	void removeGMA();

	void editRed();
	void editGreen();
	void editBlue();

	void changeRange(int index);
};

#endif //GMA_WIDGET