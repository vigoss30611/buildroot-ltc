/**
******************************************************************************
@file ccmconfig.cpp

@brief CCMConfig class implementation

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
#include "ccm/config.hpp"
#include "ccm/tuning.hpp"

#include "ui/ui_ccmconfig.h"
#include "ui/ui_ccmconfigadv.h"

#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QMessageBox>

#include <ctx_reg_precisions.h> // CCM register min/max

//
// CCM Config
//

// private

void CCMConfig::init()
{
	Ui::CCMConfig::setupUi(this);
	
    inputFile->setFileMode(QFileDialog::ExistingFile);
    inputFile->setFileFilter(tr("*.flx", "input file filter for CCM"));
    
    // text filled in retranslate
    buttonBox = new QDialogButtonBox();
    start_btn = buttonBox->addButton(QDialogButtonBox::Ok); // accept role
    advanced_btn = buttonBox->addButton(QDialogButtonBox::Help);

    ccmconfig_lay->addWidget(buttonBox, ccmconfig_lay->rowCount(), 0, 1, -1);
    
    connect(inputFile, SIGNAL(fileChanged(QString)), this, SLOT(inputSelected(QString)));
    connect(start_btn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(advanced_btn, SIGNAL(clicked()), this, SLOT(execAdvanced()));
    
    ccm_input defaults;
    setConfig(defaults);

	pImageView = new ImageView();
	pImageView->menubar->hide();    
	pImageView->showDPF_cb->hide();
    pImageView->setEnabled(false);
	imgview_lay->addWidget(pImageView);

    pImageView->setContextMenuPolicy(Qt::PreventContextMenu); // disable context menu
    start_btn->setEnabled(false);

    image_height = 0;
    image_width = 0;

    grid_top = 0;
    grid_left = 0;
    grid_bottom = 0;
    grid_right = 0;
    
    update_btn->setEnabled(false); // becomes enabled when file is loaded
    connect(update_btn, SIGNAL(clicked()), this, SLOT(computeGrid()));
    connect(pImageView, SIGNAL(mousePressed(qreal, qreal, Qt::MouseButton)), this, SLOT(mouseClicked(qreal, qreal, Qt::MouseButton)));
    connect(pImageView, SIGNAL(mouseReleased(qreal, qreal, Qt::MouseButton)), this, SLOT(mouseReleased(qreal, qreal, Qt::MouseButton)));

    retranslate();
}

// public

CCMConfig::CCMConfig(QWidget *parent): QDialog(parent)
{
	init();
}

ccm_input CCMConfig::getConfig() const
{
    ccm_input config = this->advancedConfig;
        
    if ( start_btn->isEnabled() )
    {
        for ( int p = 0 ; p < MACBETCH_N ; p++ )
        {
            config.checkerCoords[p] = gridCoordinates[p];
        }
    }

    // BLC info comes from its own tab
    /*config.systemBlack = systemBlack->value();
    config.sensorBlack[0] = sensorBlack0->value();
    config.sensorBlack[1] = sensorBlack1->value();
    config.sensorBlack[2] = sensorBlack2->value();
    config.sensorBlack[3] = sensorBlack3->value();*/
    
    config.inputFilename = inputFile->absolutePath();
    
    return config;
}

void CCMConfig::setConfig(const ccm_input &config)
{
    // could also copy the macbech info

    grid_bottom = 0;
    grid_right = 0;

    if ( !config.inputFilename.isEmpty() )
    {
        inputFile->forceFile(config.inputFilename);
        grid_top = 10000; // big values as we look for the minimum
        grid_left = 10000;
        grid_patchHeight = 10000;
        grid_patchWidth = 10000;
        
        for ( int i = 0 ; i < MACBETCH_N ; i++ )
        {
            grid_top = std::min(config.checkerCoords[i].y, grid_top);
            grid_left = std::min(config.checkerCoords[i].x, grid_left);
            grid_bottom = std::max(config.checkerCoords[i].y + config.checkerCoords[i].height, grid_bottom);
            grid_right = std::max(config.checkerCoords[i].x + config.checkerCoords[i].width, grid_right);

            grid_patchWidth = std::min(grid_patchWidth, config.checkerCoords[i].width);
            grid_patchHeight = std::min(grid_patchHeight, config.checkerCoords[0].height);
        }

        //computeGrid();
        inputSelected(config.inputFilename);
    }
    this->advancedConfig = config;
}

void CCMConfig::changeCSS(const QString &css)
{
	_css = css;

	setStyleSheet(css);
}

// public slots

void CCMConfig::retranslate()
{
	Ui::CCMConfig::retranslateUi(this);

    start_btn->setText(tr("Start"));
    advanced_btn->setText(tr("Advanced Settings"));
}


void CCMConfig::inputSelected(QString newPath)
{
    int r = EXIT_FAILURE;
    start_btn->setEnabled(false);

    if ( !newPath.isEmpty() )
    {
        //r = pPreview->loadFile(newPath.toLatin1());
        CVBuffer sBuffer;

        try
        {
            sBuffer = CVBuffer::loadFile(newPath.toLatin1());
            r = EXIT_SUCCESS;

            pImageView->loadBuffer(sBuffer);

            image_width = sBuffer.data.cols;
            image_height = sBuffer.data.rows;

            if ( grid_right > 0 && grid_right < image_width && grid_bottom > 0 && grid_bottom < image_height )
            {
                patchWidth->setValue(grid_patchWidth);
                patchHeight->setValue(grid_patchHeight);

                top->setValue(grid_top);
                left->setValue(grid_left);
                
                bottom->setValue(grid_bottom);
                right->setValue(grid_right);
            }
            else
            {
                patchWidth->setValue( (image_width*8/10)/(MACBETCH_W*2) );
                patchHeight->setValue( (image_height*8/10)/(MACBETCH_H*2) );

                top->setValue(image_height/10);
                left->setValue(image_width/10);

                bottom->setValue(image_height - top->value());
                right->setValue(image_width - left->value());
            }

            update_btn->setEnabled(true);
            computeGrid();
        }
        catch(QtExtra::Exception ex)
        {
            ex.displayQMessageBox(this);
        }
    }

    pImageView->setEnabled(r==EXIT_SUCCESS);
    start_btn->setEnabled(r==EXIT_SUCCESS);
}

int CCMConfig::execAdvanced()
{
    int ret;
    CCMConfigAdv advConfig;

	advConfig.setStyleSheet(_css);

    advConfig.setConfig(this->advancedConfig);

    ret = advConfig.exec();

    if ( ret == QDialog::Accepted )
    {
        this->advancedConfig = advConfig.getConfig();
    }
    return ret;
}

void CCMConfig::mouseClicked(qreal x, qreal y, Qt::MouseButton button)
{
    bool preview = topleftMouse->isChecked();

    // computation for CCM grid
    if ( preview || sizeMouse->isChecked() )
    {
        left->setValue( (int)floor(x) );
        top->setValue( (int)floor(y) );
    }
    else if ( bottomRightMouse->isChecked() )
    {
        right->setValue( (int)floor(x) );
        bottom->setValue( (int)floor(y) );

        preview = true;
    }

    if ( preview )
    {
        computeGrid();
    }
}

void CCMConfig::mouseReleased(qreal x, qreal y, Qt::MouseButton button)
{
    // computation for CCM
    if ( sizeMouse->isChecked() )
    {
        int w = x - left->value();
        int h = y - top->value();

        if ( w < 0 )
        {
            w = left->value() - x;
            if ( w < 1 ) w = 1;
        }
        if ( h < 0 )
        {
            h = top->value() - y;
            if ( h < 1 ) h = 1;
        }
        
        patchWidth->setValue(w);
        patchHeight->setValue(h);

        computeGrid();
    }
}

void CCMConfig::computeGrid(const QPen &pen)
{
    int coord_left = left->value();
    int coord_top = top->value();

    hMargin->setValue( ((right->value()-left->value()) - patchWidth->value()*MACBETCH_W)/(MACBETCH_W-1) );
    vMargin->setValue( ((bottom->value()-top->value()) - patchHeight->value()*MACBETCH_H)/(MACBETCH_H-1) );

    int coord_width = patchWidth->value();
    int coord_height = patchHeight->value();
    int coord_hormarg = hMargin->value();
    int coord_vertmarg = vMargin->value();
    int CFA_W = 1, CFA_H = 1;

    if ( coord_width == 0 || coord_height == 0 )//|| coord_top == coord_bottom || coord_right == coord_left )
    {
        QMessageBox::warning(this, tr("Cancelled"), tr("Some given sizes are 0 or the rectangle is too small."));
        
        return;
    }

    int right = coord_left + MACBETCH_W*coord_width + (MACBETCH_W-1)*coord_hormarg;
    int bottom = coord_top + MACBETCH_H*coord_height + (MACBETCH_H-1)*coord_vertmarg;
    if ( right > image_width || bottom > image_height )
    {
        QMessageBox::warning(this, tr("Cancelled"), tr("The patches are getting out of the input image."));
        
        return;
    }

    gridCoordinates.clear();

    for ( int j = 0 ; j < MACBETCH_H ; j++ )
    {
        for ( int i = 0 ; i < MACBETCH_W ; i++ )
        {
            int p = j*MACBETCH_W+i;
            cv::Rect coord(
                (coord_left+i*(coord_width+coord_hormarg))/CFA_W, // x
                (coord_top+j*(coord_height+coord_vertmarg))/CFA_W, // y
                coord_width,
                coord_height);

            gridCoordinates.push_back(coord);
        }
    }

    previewGrid(pen);
}

void CCMConfig::previewGrid(const QPen &pen)
{    
    pImageView->clearButImage();
        
    pImageView->addGrid(gridCoordinates, pen);
}

//
// CCM Config Advanced
//

// private

void CCMConfigAdv::init()
{
    Ui::CCMConfigAdv::setupUi(this);

    QGridLayout *pGrid = new QGridLayout();
    unsigned int m;
    double maxCCM, maxOff, maxWBG;
    double minCCM, minOff, minWBG ;
    double stepCCM, /*stepOff, */stepWBG;
    int decimalCCM, decimalOff, decimalWBG;
    
    m = (1<<(CC_COEFF_0_INT+CC_COEFF_0_FRAC-CC_COEFF_0_SIGNED))-1; // max value for register
    maxCCM = double(m)/(1<<CC_COEFF_0_FRAC);
    if ( CC_COEFF_0_SIGNED )
    {
        minCCM = -1.0*double(m+1)/(1<<CC_COEFF_0_FRAC);
    }
    else
    {
        minCCM = 0.0;
    }
    decimalCCM = ceil(log(double(1<<CC_COEFF_0_FRAC)));
    stepCCM = 1/double(1<<CC_COEFF_0_FRAC);

    m = (1<<(CC_OFFSET_0_INT+CC_OFFSET_0_FRAC-CC_OFFSET_0_SIGNED))-1; // max value for register
    maxOff = double(m)/(1<<CC_OFFSET_0_FRAC);
    if ( CC_OFFSET_0_SIGNED )
    {
        minOff = -1.0*double(m+1)/(1<<CC_OFFSET_0_FRAC);
    }
    else
    {
        minOff = 0.0;
    }
    decimalOff = ceil(log(double(1<<CC_OFFSET_0_FRAC)));
    stepCCM = 1/double(1<<CC_OFFSET_0_FRAC);

    m = (1<<(WHITE_BALANCE_GAIN_0_INT+WHITE_BALANCE_GAIN_0_FRAC-WHITE_BALANCE_GAIN_0_SIGNED))-1;
    maxWBG = double(m)/(1<<WHITE_BALANCE_GAIN_0_FRAC);
    if ( WHITE_BALANCE_GAIN_0_SIGNED )
    {
        minWBG = -1.0*double(m+1)/(1<<WHITE_BALANCE_GAIN_0_FRAC);
    }
    else
    {
        minWBG = 0.0;
    }
    decimalWBG = ceil(log(double(1<<WHITE_BALANCE_GAIN_0_FRAC)));
    stepWBG = 1/double(1<<WHITE_BALANCE_GAIN_0_FRAC);
    
    for ( int y = 0 ; y < MACBETCH_H ; y++ )
    {
        for ( int x = 0 ; x < MACBETCH_W ; x++ )
        {
            int p = y*MACBETCH_W+x;
            apPatchWeights[p] = new QDoubleSpinBox();
            apPatchWeights[p]->setMinimum(1.0);
            apPatchWeights[p]->setMaximum(100.0);

            pGrid->addWidget(apPatchWeights[p], y, x);


            apPatchLabels[p] = new QLabel(QString::number(p));
            target_layout->addWidget(apPatchLabels[p], p, 0);

            for ( int c = 0 ; c < 3 ; c++ )
            {
                apPatchValue[p][c] = new QDoubleSpinBox();
                apPatchValue[p][c]->setMinimum(0.0);
                apPatchValue[p][c]->setMaximum(255.0);
                
                target_layout->addWidget(apPatchValue[p][c], p, c+1);
            }
        }

    }
    patchWeigths_grp->setLayout(pGrid);

    apMinMat[0] = min_mat0;
    apMinMat[1] = min_mat1;
    apMinMat[2] = min_mat2;
    apMinMat[3] = min_mat3;
    apMinMat[4] = min_mat4;
    apMinMat[5] = min_mat5;
    apMinMat[6] = min_mat6;
    apMinMat[7] = min_mat7;
    apMinMat[8] = min_mat8;

    apMinOff[0] = min_off0;
    apMinOff[1] = min_off1;
    apMinOff[2] = min_off2;

    apMaxMat[0] = max_mat0;
    apMaxMat[1] = max_mat1;
    apMaxMat[2] = max_mat2;
    apMaxMat[3] = max_mat3;
    apMaxMat[4] = max_mat4;
    apMaxMat[5] = max_mat5;
    apMaxMat[6] = max_mat6;
    apMaxMat[7] = max_mat7;
    apMaxMat[8] = max_mat8;

    apMaxOff[0] = max_off0;
    apMaxOff[1] = max_off1;
    apMaxOff[2] = max_off2;

    maxWBCGain->setMinimum(minWBG);
    maxWBCGain->setMaximum(maxWBG);
    maxWBCGain->setSingleStep(stepWBG);
    maxWBCGain->setDecimals(decimalWBG);

    whitePatch->setMinimum(0);
    whitePatch->setMaximum(MACBETCH_N-1);
    blackPatch->setMinimum(0);
    blackPatch->setMaximum(MACBETCH_N-1);

    for ( int i = 0 ; i < 3 ; i++ )
    {
        for ( int j = 0 ; j < 3 ; j++ )
        {
            apMinMat[3*i+j]->setMinimum(minCCM);
            apMinMat[3*i+j]->setMaximum(maxCCM);
            apMinMat[3*i+j]->setDecimals(decimalCCM);
            apMinMat[3*i+j]->setSingleStep(stepCCM);

            apMaxMat[3*i+j]->setMinimum(minCCM);
            apMaxMat[3*i+j]->setMaximum(maxCCM);
            apMaxMat[3*i+j]->setDecimals(decimalCCM);
            apMaxMat[3*i+j]->setSingleStep(stepCCM);
        }
        apMinOff[i]->setMinimum(minOff);
        apMinOff[i]->setMaximum(maxOff);
        apMinOff[i]->setDecimals(decimalCCM);
        apMinOff[i]->setSingleStep(stepCCM);

        apMaxOff[i]->setMinimum(minOff);
        apMaxOff[i]->setMaximum(maxOff);
        apMaxOff[i]->setDecimals(decimalCCM);
        apMaxOff[i]->setSingleStep(stepCCM);
    }

    patchWeigths_grp->setDisabled(CIE94->isChecked());

#ifndef TUNE_CCM_WBC_TARGET_AVG
    lumcorr_grp->setVisible(false);
#else
    useMaxWBC_grp->setVisible(false);
#endif

    restoreDefaults();

    connect(buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(chooseAction(QAbstractButton *)));
}

// public

CCMConfigAdv::CCMConfigAdv(QWidget *parent): QDialog(parent)
{
    init();
}

void CCMConfigAdv::setConfig(const ccm_input &config)
{
    /// @ load patches colour target
    nIterations->setValue(config.niterations);
    nWarm->setValue(config.nwarmiterations);
    normalise->setChecked(config.bnormalise);
    verboseLog->setChecked(config.bverbose);
    errorCurve->setChecked(config.berrorGraph);
    gammaDisplay->setChecked(config.bgammadisplay);
    CIE94->setChecked(config.bCIE94);
    temperatureStr->setValue(config.temperatureStrength);
    changeProb->setValue(config.changeProb);
    whitePatch->setValue(config.whitePatchI);
    blackPatch->setValue(config.blackPatchI);
    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        apPatchWeights[p]->setValue(config.patchWeight[p]);

        apPatchValue[p][0]->setValue(config.expectedRGB[p][0]);
        apPatchValue[p][1]->setValue(config.expectedRGB[p][1]);
        apPatchValue[p][2]->setValue(config.expectedRGB[p][2]);
    }
    for ( int i = 0 ; i < 9 ; i++ )
    {
        apMinMat[i]->setValue(config.matrixMin[i]);
        apMaxMat[i]->setValue(config.matrixMax[i]);
    }
    for ( int i = 0 ; i < 3 ; i++ )
    {
        apMinOff[i]->setValue(config.offsetMin[i]);
        apMaxOff[i]->setValue(config.offsetMax[i]);
    }

    // used only if TUNE_CCM_WBC_TARGET_AVG is defined at compilation time
    luminanceR->setValue(config.luminanceCorrection[0]);
    luminanceG->setValue(config.luminanceCorrection[1]);
    luminanceB->setValue(config.luminanceCorrection[2]);
    // otherwise uses
    useMaxWBC_grp->setChecked(config.bCheckMaxWBCGain);
    maxWBCGain->setValue(config.maxWBCGain);
}

ccm_input CCMConfigAdv::getConfig() const
{
    ccm_input config;

    config.niterations = nIterations->value();
    config.nwarmiterations = nWarm->value();
    if ( config.niterations > 100 )
    {
        config.sendinfo = config.niterations/100;
    }
    else
    {
        config.sendinfo = 100;
    }
    config.bnormalise = normalise->isChecked();
    config.bverbose = verboseLog->isChecked();
    config.berrorGraph = errorCurve->isChecked();
    config.bgammadisplay = gammaDisplay->isChecked();
    config.bCIE94 = CIE94->isChecked();
    config.temperatureStrength = temperatureStr->value();
    config.changeProb = changeProb->value();
    config.whitePatchI = whitePatch->value();
    config.blackPatchI = blackPatch->value();
    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        config.patchWeight[p] = apPatchWeights[p]->value();

        config.expectedRGB[p][0] = apPatchValue[p][0]->value();
        config.expectedRGB[p][1] = apPatchValue[p][1]->value();
        config.expectedRGB[p][2] = apPatchValue[p][2]->value();
    }
    for ( int i = 0 ; i < 9 ; i++ )
    {
        config.matrixMin[i] = apMinMat[i]->value();
        config.matrixMax[i] = apMaxMat[i]->value();
    }
    for ( int i = 0 ; i < 3 ; i++ )
    {
        config.offsetMin[i] = apMinOff[i]->value();
        config.offsetMax[i] = apMaxOff[i]->value();
    }

    // used only if TUNE_CCM_WBC_TARGET_AVG is defined at compilation time
    config.luminanceCorrection[0] = luminanceR->value();
    config.luminanceCorrection[1] = luminanceG->value();
    config.luminanceCorrection[2] = luminanceB->value();
    // otherwise uses
    config.bCheckMaxWBCGain = useMaxWBC_grp->isChecked();
    config.maxWBCGain = maxWBCGain->value();

    return config;
}

// public slots

void CCMConfigAdv::restoreDefaults()
{
    ccm_input defaults;
    setConfig(defaults);
}

void CCMConfigAdv::chooseAction(QAbstractButton *btn)
{
    switch(buttonBox->buttonRole(btn))
    {
    case QDialogButtonBox::AcceptRole:
        accept();
        break;
    case QDialogButtonBox::RejectRole:
        reject();
        break;
    case QDialogButtonBox::ResetRole:
        restoreDefaults();
        break;
    }
}

void CCMConfigAdv::setReadOnly(bool ro)
{
    nIterations->setReadOnly(ro);
    if ( ro ) nIterations->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else nIterations->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    
    nWarm->setReadOnly(ro);
    if ( ro ) nWarm->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else nWarm->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    normalise->setCheckable(!ro);
    verboseLog->setCheckable(!ro);
    errorCurve->setCheckable(!ro);
    gammaDisplay->setCheckable(!ro);
    CIE94->setCheckable(!ro);

    temperatureStr->setReadOnly(ro);
    if ( ro ) temperatureStr->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else temperatureStr->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    changeProb->setReadOnly(ro);
    if ( ro ) changeProb->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else changeProb->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    whitePatch->setReadOnly(ro);
    if ( ro ) whitePatch->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else whitePatch->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    blackPatch->setReadOnly(ro);
    if ( ro ) blackPatch->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else blackPatch->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        apPatchWeights[p]->setReadOnly(ro);
        if ( ro ) apPatchWeights[p]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        else apPatchWeights[p]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        for ( int c = 0 ; c < 3 ; c++ )
        {
            apPatchValue[p][c]->setReadOnly(ro);
            if ( ro ) apPatchValue[p][c]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            else apPatchValue[p][c]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        }
    }
    for ( int i = 0 ; i < 9 ; i++ )
    {
        apMinMat[i]->setReadOnly(ro);
        if ( ro ) apMinMat[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        else apMinMat[i]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        apMaxMat[i]->setReadOnly(ro);
        if ( ro ) apMaxMat[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        else apMaxMat[i]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    }
    for ( int i = 0 ; i < 3 ; i++ )
    {
        apMinOff[i]->setReadOnly(ro);
        if ( ro ) apMinOff[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        else apMinOff[i]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        apMaxOff[i]->setReadOnly(ro);
        if ( ro ) apMaxOff[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        else apMaxOff[i]->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    }

    // used only if TUNE_CCM_WBC_TARGET_AVG is defined at compilation time
    luminanceR->setReadOnly(ro);
    if ( ro ) luminanceR->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else luminanceR->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    luminanceG->setReadOnly(ro);
    if ( ro ) luminanceG->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else luminanceG->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    luminanceB->setReadOnly(ro);
    if ( ro ) luminanceB->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else luminanceB->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    // otherwise uses
    useMaxWBC_grp->setCheckable(!ro);
    maxWBCGain->setReadOnly(ro);
    if ( ro ) maxWBCGain->setButtonSymbols(QAbstractSpinBox::NoButtons);
    else maxWBCGain->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
}
