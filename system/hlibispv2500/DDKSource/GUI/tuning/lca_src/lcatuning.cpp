/**
******************************************************************************
@file lcatuning.cpp

@brief LCATuning implementation

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

#include "lca/tuning.hpp"
#include "genconfig.hpp"
#include "lca/compute.hpp"
#include "lca/config.hpp"
#include "lca/preview.hpp"
#include "ispc/ModuleLCA.h"
#include "ispc/ParameterFileParser.h"

/*
 * lca_input
 */

lca_input::lca_input()
{
    imageFilename = "";
    centerX = -1;
    centerY = -1;
}

/*
 * LCATuning
 */

/*
 * private
 */

void LCATuning::init()
{
    Ui::LCATuning::setupUi(this);

    resultView = new LCAPreview();
    resultView->setEnabled(false);

    connect(reset_btn, SIGNAL(clicked()), this, SLOT(reset()));
    connect(save_btn, SIGNAL(clicked()), this, SLOT(saveResult()));

    progress->setVisible(false);
    save_btn->setEnabled(false);

    calibration_grp->setEnabled(false);
    polynomial_grp->setEnabled(false);
    
    pOutput = 0;

	if(_isVLWidget)
    {
        integrate_btn->setEnabled(false);
        save_btn->hide();
    }
    else
    {
        integrate_btn->hide();
    }
}

/*
 * public
 */

LCATuning::LCATuning(GenConfig *genConfig, bool isVLWidget, QWidget *parent): VisionModule(parent)
{
	_isVLWidget = isVLWidget;

    //this->genConfig = genConfig;
    init();
}

LCATuning::~LCATuning()
{
    // genConfig is now owned by this object!
    if ( pOutput )
    {
        delete pOutput;
    }
}

QWidget *LCATuning::getResultWidget()
{
    return resultView;
}

void LCATuning::changeCSS(const QString &css)
{
	_css = css;

	setStyleSheet(css);
}

/*
 * public slots
 */

QString LCATuning::checkDependencies() const
{
    QString mess;

    if ( !pOutput )
    {
        mess += tr("LCA calibration was not run\n");
    }

    return mess;
}

int LCATuning::saveResult(const QString &_filename)
{
    QString filename;
    
    if ( _filename.isEmpty() )
    {
        filename = QFileDialog::getSaveFileName(this, tr("Output FelixArgs file"), QString(), tr("*.txt"));

        // on linux the filename may not have .txt - disabled because it does not warn if the file is already present
        /*int len = filename.count();
        if ( !(filename.at(len-4) == '.' && filename.at(len-3) == 't' && filename.at(len-2) == 'x' && filename.at(len-1) == 't')  )
        {
            filename += ".txt";
        }*/
    }
    else
    {
        filename = _filename;
    }

    if ( filename.isEmpty() )
    {
        return EXIT_FAILURE;
    }

	ISPC::ParameterList res;
    try
    {
        saveParameters(res);

        if(ISPC::ParameterFileParser::saveGrouped(res, std::string(filename.toLatin1())) != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save FelixParameter file with LCA parameters")));
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return EXIT_FAILURE;
    }
    
    QString mess = "LCA: saving parameters in " + filename + "\n";
    emit logMessage(mess);

    return EXIT_SUCCESS;
}

void LCATuning::saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception)
{
    const int R = 0;
    const int B = 3;
        
    if ( pOutput == 0 )
    {
        throw(QtExtra::Exception(tr("No results to save for LCA")));
    }

	ISPC::ModuleLCA lca;
    
    for ( int c = 0 ; c < 3 ; c++ )
		lca.aRedPoly_X[c] = pOutput->coeffsX[R].at<double>(c);

    for ( int c = 0 ; c < 3 ; c++ )
		lca.aRedPoly_Y[c] = pOutput->coeffsY[R].at<double>(c);

    for ( int c = 0 ; c < 3 ; c++ )
		lca.aBluePoly_X[c] = pOutput->coeffsX[B].at<double>(c);

    for ( int c = 0 ; c < 3 ; c++ )
		lca.aBluePoly_Y[c] = pOutput->coeffsY[B].at<double>(c);

	lca.aRedCenter[0] = inputs.centerX;
	lca.aRedCenter[1] = inputs.centerY;
	lca.aBlueCenter[0] = inputs.centerX;
	lca.aBlueCenter[1] = inputs.centerY;

	lca.aShift[0] = pOutput->shiftX;
	lca.aShift[1] = pOutput->shiftY;

	lca.save(list, ISPC::ModuleLCA::SAVE_VAL);

	list.removeParameter(ISPC::ModuleLCA::LCA_DEC.name);

	ISPC::Parameter inputFile("//LCA input image", inputs.imageFilename);
	list.addParameter(inputFile);
}

int LCATuning::tuning()
{
    if ( this->inputs.imageFilename.empty() )
    {
        return EXIT_FAILURE;
    }

    LCACompute *pCompute = new LCACompute();

    connect(this, SIGNAL(compute(const lca_input*)),
        pCompute, SLOT(compute(const lca_input*)));

    connect(pCompute, SIGNAL(finished(LCACompute *, lca_output *)), 
        this, SLOT(computationFinished(LCACompute*, lca_output *)));
    connect(pCompute, SIGNAL(currentStep(int)),
        progress, SLOT(setValue(int)));
    connect(pCompute, SIGNAL(logMessage(QString)),
        this, SIGNAL(logMessage(QString)));

    progress->setMaximum(LCA_N_STEPS);
    progress->setValue(0);
    progress->setVisible(true);

    save_btn->setEnabled(false);
    resultView->setEnabled(false);

    emit compute(&(this->inputs));
    emit changeCentral(getDisplayWidget());
    emit changeResult(getResultWidget());

	if(_isVLWidget)
    {
        gridLayout->addWidget(resultView, 0, gridLayout->columnCount(), gridLayout->rowCount(), 1);
    }

    return EXIT_SUCCESS;
}

int LCATuning::reset()
{
    if ( progress->isVisible() )
    {
        return EXIT_FAILURE;
    }

#if 1
    {
        LCAConfig sconf;

		if(_isVLWidget)
		{
			sconf.changeCSS(_css);
		}

        sconf.setConfig(this->inputs);

        if ( sconf.exec() == QDialog::Accepted )
        {
            this->inputs = sconf.getConfig();

            centerX->setValue(inputs.centerX);
            centerY->setValue(inputs.centerY);
            inputFile->setText(QString::fromStdString(inputs.imageFilename));

            calibration_grp->setEnabled(true);
            polynomial_grp->setEnabled(true);
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
#endif

    this->tuning();
    return EXIT_SUCCESS;
}

void LCATuning::computationFinished(LCACompute *toStop, lca_output *pResult)
{
    int RX = lca_output::RED_GREEN0, RY = lca_output::RED_GREEN0;
    int BX = lca_output::BLUE_GREEN0, BY = lca_output::BLUE_GREEN0;
    toStop->deleteLater();

    progress->setVisible(false);
    save_btn->setEnabled(true);

    if ( pOutput )
    {
        delete pOutput;
    }
    this->pOutput = pResult;
    //gridView->loadData(pResult);

    // choose the best result (should be the aligned RGGB element
    // (e.g. with RG GB R/G0 and B/G1, with BRBG R/G1 B/G0)
    if ( pResult->errorsX[lca_output::RED_GREEN0] > pResult->errorsX[lca_output::RED_GREEN1] )
    {
        RX = lca_output::RED_GREEN1;
    }
    if ( pResult->errorsY[lca_output::RED_GREEN0] > pResult->errorsY[lca_output::RED_GREEN1] )
    {
        RY = lca_output::RED_GREEN1;
    }
    if ( pResult->errorsX[lca_output::BLUE_GREEN0] > pResult->errorsX[lca_output::BLUE_GREEN1] )
    {
        BX = lca_output::BLUE_GREEN1;
    }
    if ( pResult->errorsY[lca_output::BLUE_GREEN0] > pResult->errorsY[lca_output::BLUE_GREEN1] )
    {
        BY = lca_output::BLUE_GREEN1;
    }
    
    redPolyX0->setValue(pResult->coeffsX[RX].at<double>(0));
    redPolyX1->setValue(pResult->coeffsX[RX].at<double>(1));
    redPolyX2->setValue(pResult->coeffsX[RX].at<double>(2));
    
    redPolyY0->setValue(pResult->coeffsY[RY].at<double>(0));
    redPolyY1->setValue(pResult->coeffsY[RY].at<double>(1));
    redPolyY2->setValue(pResult->coeffsY[RY].at<double>(2));

    bluePolyX0->setValue(pResult->coeffsX[BX].at<double>(0));
    bluePolyX1->setValue(pResult->coeffsX[BX].at<double>(1));
    bluePolyX2->setValue(pResult->coeffsX[BX].at<double>(2));

    bluePolyY0->setValue(pResult->coeffsY[BY].at<double>(0));
    bluePolyY1->setValue(pResult->coeffsY[BY].at<double>(1));
    bluePolyY2->setValue(pResult->coeffsY[BY].at<double>(2));

    resultView->setResults(*pResult, inputs);
    resultView->setEnabled(true);

	if(_isVLWidget)
    {
        integrate_btn->setEnabled(true);
    }
}

void LCATuning::retranslate()
{
    Ui::LCATuning::retranslateUi(this);
}
