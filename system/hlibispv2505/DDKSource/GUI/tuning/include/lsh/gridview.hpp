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

#include <deque>

#include "ui/ui_lshgridview.h"
#include "imageview.hpp"

#include "lsh/compute.hpp"  // to have struct lsh_output
#include "lsh/tuning.hpp"  // to have struct lsh_input
class QwtPlot;
class QwtPlotCurve;

/**
 * @ingroup VISION_TUNING
 */
class LSHGridView: public QWidget, public Ui::LSHGridView
{
    Q_OBJECT

private:
    lsh_input inputs;

    typedef std::deque<lsh_output> output_list;

    /** @brief elements are copied from LSHTuning::output */
    output_list data;

    /**
     * @brief line in the input or smoothed view  displayed in inputSliceCurve
     *
     * If output is interpolated this cannot be used
     */
    int sliceLine;
    /**
     * @brief matrix line in the gridSliceCurve and gridTargetCurve
     */
    int matLine;

    QwtPlot *inputSlice;
    QwtPlotCurve *inputSliceCurve;

    QwtPlot *gridSlice;
    QwtPlotCurve *gridSliceCurve;
    QwtPlotCurve *gridTargetCurve;
    QwtPlotCurve *gridTargetMSECurve;

    ImageView *pGridView;

    void init();

    /* using sliceLine and matLine */
    void updateDisplay();

public:
    explicit LSHGridView(QWidget *parent = 0);
    virtual ~LSHGridView();

    void changeInput(const lsh_input &input);
    void loadData(const lsh_output *pOutput);
    /** only empty the list - owner is still LSHTuning */
    void clearData();

    void connectButtons(bool conn = true);

public slots:
    // update from the temperature
    void channelChanged(int c);
    void lineChanged(int l);
    void retranslate();

    void mousePressed(qreal x, qreal y, Qt::MouseButton button);
};

#endif /* LSHGRIDVIEW_HPP */
