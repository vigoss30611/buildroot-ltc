/******************************************************************************
@file lshconfigadv.cpp

@brief LSHConfigAdv implementation

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
#include "lsh/configadv.hpp"

LSHConfigAdv::LSHConfigAdv(enum HW_VERSION hwv, QWidget *parent)
    : hw_version(hwv)
{
    init();
}

void LSHConfigAdv::init()
{
    Ui::LSHConfigAdv::setupUi(this);
    lsh_input def_val(hw_version);

    // Add image view to advance settings for preview
    pImageView = new ImageView();
    pImageView->menubar->hide();  // Don't need menu bar here
    pImageView->setEnabled(false);
    pImageView->showStatistics(false);  // Don't need statistics here
    image_lay->addWidget(pImageView);

    double s = static_cast<double>(1 << def_val.vertex_frac);
    int p = ceil(log(s));
    maxMatrixValue_sb->setDecimals(p);
    // 10 time the step to be faster with +/-
    maxMatrixValue_sb->setSingleStep(10 * 1 / s);
#if 0
    // enfor HW maximum
    maxMatrixValue_sb->setMaximum(
        static_cast<double>(1 << (vertex_int + vertex_frac
        - vertex_signed)) / (1 << vertex_frac));
#else
    maxMatrixValue_sb->setMaximum(def_val.maxMatrixValue * 2.0);
    // to the GUI with other values
    maxMatrixValue_sb->setValue(def_val.maxMatrixValue);
#endif
    minMatrixValue_sb->setMaximum(maxMatrixValue_sb->maximum());
    minMatrixValue_sb->setValue(1.0);

    // fixed in 2.6
    maxLineSize_cb->setChecked(def_val.bUseMaxLineSize);

    maxBitDiff_sb->setValue(def_val.maxBitDiff);

    retranslate();
}

void LSHConfigAdv::retranslate()
{
    Ui::LSHConfigAdv::retranslateUi(this);

    // Adding margin color options
    color_cb->clear();
    color_cb->insertItem(color_cb->count(), tr("Green"),
        static_cast<int>(Qt::green));
    color_cb->insertItem(color_cb->count(), tr("Red"),
        static_cast<int>(Qt::red));
    color_cb->insertItem(color_cb->count(), tr("Blue"),
        static_cast<int>(Qt::blue));
    color_cb->insertItem(color_cb->count(), tr("White"),
        static_cast<int>(Qt::white));
}

//lsh_input LSHConfigAdv::getConfig() const
//{
//    lsh_input config(hw_version);
//
//    // move to normal input when gui is ready
//    //int temp = LSH_DEF_TEMP;  // no temperature yet
//
//    //config.addInput(temp, "",
//    //    marginLeft_sb->value(), marginTop_sb->value(),
//    //    marginRight_sb->value(), marginBottom_sb->value(),
//    //    smoothing_sb->value());
//    // end to move to normal input
//
//    config.maxMatrixValue = maxMatrixValue_sb->value();
//    config.bUseMaxValue = maxMatrixValueUsed_cb->isChecked();
//    config.minMatrixValue = minMatrixValue_sb->value();
//    config.bUseMinValue = minMatrixValueUsed_cb->isChecked();
//    config.maxLineSize = maxLineSize_sb->value();
//    config.bUseMaxLineSize = maxLineSize_cb->isChecked();
//    config.maxBitDiff = maxBitDiff_sb->value();
//    config.bUseMaxBitsPerDiff = maxBitDiff_cb->isChecked();
//
//    return config;
//}

//void LSHConfigAdv::setConfig(const lsh_input &config)
//{
//    // move to normal input when gui is ready
//	//if (config.inputs.size())
//	//{
//	//	int temp = LSH_DEF_TEMP;  // no temperature yet
//	//	lsh_input::const_iterator it = config.inputs.find(temp);
//	//	if (config.inputs.end() != it)
//	//	{
//	//		marginLeft_sb->setValue(it->second.margins[MARG_L]);
//	//		marginTop_sb->setValue(it->second.margins[MARG_T]);
//	//		marginRight_sb->setValue(it->second.margins[MARG_R]);
//	//		marginBottom_sb->setValue(it->second.margins[MARG_B]);
//
//	//		smoothing_sb->setValue(it->second.smoothing);
//	//	}
//	//}
//    // end to move to normal input
//
//    maxMatrixValue_sb->setValue(config.maxMatrixValue);
//    maxMatrixValueUsed_cb->setChecked(config.bUseMaxValue);
//    minMatrixValue_sb->setValue(config.minMatrixValue);
//    minMatrixValueUsed_cb->setChecked(config.bUseMinValue);
//    maxLineSize_sb->setValue(config.maxLineSize);
//    maxLineSize_cb->setChecked(config.bUseMaxLineSize);
//    maxBitDiff_sb->setValue(config.maxBitDiff);
//    maxBitDiff_cb->setChecked(config.bUseMaxBitsPerDiff);
//}

void LSHConfigAdv::setReadOnly(bool ro)
{
    QAbstractSpinBox::ButtonSymbols btn = QAbstractSpinBox::UpDownArrows;
    if (ro)
    {
        btn = QAbstractSpinBox::NoButtons;
    }
    marginLeft_sb->setReadOnly(ro);
    marginLeft_sb->setButtonSymbols(btn);

    marginRight_sb->setReadOnly(ro);
    marginRight_sb->setButtonSymbols(btn);

    marginTop_sb->setReadOnly(ro);
    marginTop_sb->setButtonSymbols(btn);

    marginBottom_sb->setReadOnly(ro);
    marginBottom_sb->setButtonSymbols(btn);

    smoothing_sb->setReadOnly(ro);
    smoothing_sb->setButtonSymbols(btn);

    maxMatrixValue_sb->setReadOnly(ro);
    maxMatrixValue_sb->setButtonSymbols(btn);

    minMatrixValue_sb->setReadOnly(ro);
    minMatrixValue_sb->setButtonSymbols(btn);

    maxMatrixValueUsed_cb->setEnabled(!ro);

    minMatrixValueUsed_cb->setEnabled(!ro);

    maxLineSize_sb->setReadOnly(ro);
    maxLineSize_sb->setButtonSymbols(btn);

    maxLineSize_cb->setEnabled(!ro);

    maxBitDiff_sb->setReadOnly(ro);
    maxBitDiff_sb->setButtonSymbols(btn);

    maxBitDiff_cb->setEnabled(!ro);
}
