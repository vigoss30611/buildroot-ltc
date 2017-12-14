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

#include <felix_hw_info.h>  // LSH_VERTEX size info

LSHConfigAdv::LSHConfigAdv(QWidget *parent)
{
    init();
}

void LSHConfigAdv::init()
{
    Ui::LSHConfigAdv::setupUi(this);

    // Add image view to advance settings for preview
    pImageView = new ImageView();
    pImageView->menubar->hide();  // Don't need menu bar here
    pImageView->setEnabled(false);
    pImageView->showStatistics(false);  // Don't need statistics here
    image_lay->addWidget(pImageView);

    double s = static_cast<double>(1 << LSH_VERTEX_FRAC);
    int p = ceil(log(s));
    maxMatrixValue_sb->setDecimals(p);
    // 10 time the step to be faster with +/-
    maxMatrixValue_sb->setSingleStep(10*1/s);
#if 0
    // enfor HW maximum
    maxMatrixValue_sb->setMaximum(
        static_cast<double>(1 << (LSH_VERTEX_INT + LSH_VERTEX_FRAC
        - LSH_VERTEX_SIGNED)) / (1 << LSH_VERTEX_FRAC) );  // not -1
#else
    maxMatrixValue_sb->setMaximum(8.0);  // to the GUI with other values
    maxMatrixValue_sb->setValue(
        static_cast<double>(1 << (LSH_VERTEX_INT + LSH_VERTEX_FRAC
        - LSH_VERTEX_SIGNED)) / (1 << LSH_VERTEX_FRAC));  // not -1
#endif

    retranslate();

    setReadOnly(false);
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

lsh_input LSHConfigAdv::getConfig() const
{
    lsh_input config;

    config.margins[MARG_L] = marginLeft_sb->value();
    config.margins[MARG_T] = marginTop_sb->value();
    config.margins[MARG_R] = marginRight_sb->value();
    config.margins[MARG_B] = marginBottom_sb->value();

    config.smoothing = smoothing_sb->value();
    config.maxMatrixValue = maxMatrixValue_sb->value();
    config.bUseMaxValue = maxMatrixValueUsed_cb->isChecked();
    config.maxLineSize = maxLineSize_sb->value();
    config.bUseMaxLineSize = maxLineSize_cb->isChecked();
    config.maxBitDiff = maxBitDiff_sb->value();
    config.bUseMaxBitsPerDiff = maxBitDiff_cb->isChecked();

    return config;
}

void LSHConfigAdv::setConfig(const lsh_input &config)
{
    marginLeft_sb->setValue(config.margins[MARG_L]);
    marginTop_sb->setValue(config.margins[MARG_T]);
    marginRight_sb->setValue(config.margins[MARG_R]);
    marginBottom_sb->setValue(config.margins[MARG_B]);

    smoothing_sb->setValue(config.smoothing);
    maxMatrixValue_sb->setValue(config.maxMatrixValue);
    maxMatrixValueUsed_cb->setChecked(config.bUseMaxValue);
    maxLineSize_sb->setValue(config.maxLineSize);
    maxLineSize_cb->setChecked(config.bUseMaxLineSize);
    maxBitDiff_sb->setValue(config.maxBitDiff);
    maxBitDiff_cb->setChecked(config.bUseMaxBitsPerDiff);
}

void LSHConfigAdv::setReadOnly(bool ro)
{
    marginLeft_sb->setEnabled(!ro);
    marginRight_sb->setEnabled(!ro);
    marginTop_sb->setEnabled(!ro);
    marginBottom_sb->setEnabled(!ro);
    smoothing_sb->setEnabled(!ro);
    maxMatrixValue_sb->setEnabled(!ro);
    maxMatrixValueUsed_cb->setEnabled(!ro);
    maxLineSize_sb->setEnabled(!ro);
    maxLineSize_cb->setEnabled(!ro);
    maxBitDiff_sb->setEnabled(!ro);
    maxBitDiff_cb->setEnabled(!ro);
}
