/**
*******************************************************************************
@file lsh/gridview.hpp

@brief Display LSHCompute results

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
#ifndef LSHGRIDVIEW_HPP
#define LSHGRIDVIEW_HPP

#include "ui/ui_lshgridview.h"
#include "imageview.hpp"

struct lsh_output;
struct lsh_input;
class QwtPlot;
class QwtPlotCurve;

class LSHGridView: public QWidget, public Ui::LSHGridView
{
    Q_OBJECT

private:
    lsh_output *data;
    lsh_input *inputs;

    int sliceLine;

    QwtPlot *inputSlice;
    QwtPlotCurve *inputSliceCurve;

    QwtPlot *gridSlice;
    QwtPlotCurve *gridSliceCurve;
    QwtPlotCurve *gridTargetCurve;
    QwtPlotCurve *gridTargetMSECurve;

    ImageView *pGridView;

    void init();

public:
    explicit LSHGridView(QWidget *parent = 0);
    virtual ~LSHGridView();

public slots:
    void channelChanged(int c);
    void lineChanged(int l);
    void loadData(const lsh_output *pOutput, const lsh_input *pInput);
    void retranslate();

    void mousePressed(qreal x, qreal y, Qt::MouseButton button);
};

#endif /* LSHGRIDVIEW_HPP */
