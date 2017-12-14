/**
*******************************************************************************
@file lshconfig.cpp

@brief LSHConfig implementation

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
#include "lsh/config.hpp"

#include <QWidget>
#include <QGraphicsRectItem>
#include <string>
#include "lsh/compute.hpp"

#include "buffer.hpp"
#include "imageview.hpp"

#include "felix_hw_info.h"

/*
 * private
 */

void LSHConfig::init()
{
    Ui::LSHConfig::setupUi(this);

    outputMSE[0] = outputMSE_0;
    outputMSE[1] = outputMSE_1;
    outputMSE[2] = outputMSE_2;
    outputMSE[3] = outputMSE_3;

    outputMIN[0] = outputMin_0;
    outputMIN[1] = outputMin_1;
    outputMIN[2] = outputMin_2;
    outputMIN[3] = outputMin_3;

    outputMAX[0] = outputMax_0;
    outputMAX[1] = outputMax_1;
    outputMAX[2] = outputMax_2;
    outputMAX[3] = outputMax_3;

    // text filled in retranslate
    start_btn = buttonBox->addButton(QDialogButtonBox::Ok);  // accept role
    QPushButton *default_btn = buttonBox->button(
        QDialogButtonBox::RestoreDefaults);

    connect(inputFile, SIGNAL(fileChanged(QString)), this,
        SLOT(filesSelected(QString)));
    connect(outputFilename, SIGNAL(textChanged(QString)), this,
        SLOT(filesSelected(QString)));
    connect(start_btn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(default_btn, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(preview_btn, SIGNAL(clicked()), this, SLOT(previewInput()));
    connect(lshConfigAdv_btn, SIGNAL(clicked()), this, SLOT(openAdvSett()));

    start_btn->setEnabled(false);
    lshConfigAdv_btn->setEnabled(false);  // Button disabled until image is set

    inputFile->setFileMode(QFileDialog::ExistingFile);
    inputFile->setFileFilter(tr("*.flx", "input file filter for LSH"));

    outputFilename->setText("out.lsh");

    preview_btn->setEnabled(false);

    warning_icon->setPixmap(this->style()->standardIcon(
        QStyle::SP_MessageBoxWarning).pixmap(QSize(40, 40)));
    warning_lbl->setVisible(false);
    warning_icon->setVisible(false);

    // outputFilename->setValidator( &outputValidator );

    retranslate();
    // restoreDefaults();
    setReadOnly(false);
}

/*
 * public static
 */

bool LSHConfig::checkMargins(const CVBuffer &image, int *marginsH,
    int *marginsV)
{
    // Margin can't be bigger then half size of shorter side of the picture
    const int step = 1;
    bool changedMargins = false;

    for (int c = 0; c < 2; c++)
    {
        if (marginsH[c] >= image.data.cols / 2)
        {
            marginsH[c] = image.data.cols / 2 - step;
            changedMargins = true;
        }
        if (marginsV[c] >= image.data.rows / 2)
        {
            marginsH[c] = image.data.rows / 2 - step;
            changedMargins = true;
        }
    }

    return changedMargins;
}

/*
 * public
 */

LSHConfig::LSHConfig(QWidget *parent): QDialog(parent)
{
    init();
}

lsh_input LSHConfig::getConfig() const
{
    lsh_input retConfig;

    /*config.blackLevel[0] = sensorBlack0->value();
    config.blackLevel[1] = sensorBlack1->value();
    config.blackLevel[2] = sensorBlack2->value();
    config.blackLevel[3] = sensorBlack3->value();*/

    // These values we get from advance settings via config
    /*retConfig.margin = config.margin;
    retConfig.smoothing = config.smoothing;
    retConfig.maxMatrixValue = config.maxMatrixValue;
    retConfig.bUseMaxValue = config.bUseMaxValue;
    retConfig.maxLineSize = config.maxLineSize;
    retConfig.bUseMaxLineSize = config.bUseMaxLineSize;*/
    retConfig = config;

    // These values we get from widgets
    retConfig.tileSize = tilesize->itemData(tilesize->currentIndex()).toInt();
    retConfig.fitting = algorithm->currentIndex() == 1;
    retConfig.imageFilename =
        std::string(inputFile->absolutePath().toLatin1());

    return retConfig;
}

void LSHConfig::setConfig(const lsh_input &newConfig)
{
    config = newConfig;
    /*config.margin = newConfig.margin;
    config.smoothing = newConfig.smoothing;
    config.maxMatrixValue = newConfig.maxMatrixValue;
    config.bUseMaxValue = newConfig.bUseMaxValue;
    config.maxLineSize = newConfig.maxLineSize;*/
    // bits per diff and line size are output parameters

    /*sensorBlack0->setValue(config.blackLevel[0]);
    sensorBlack1->setValue(config.blackLevel[1]);
    sensorBlack2->setValue(config.blackLevel[2]);
    sensorBlack3->setValue(config.blackLevel[3]);*/

    // tilesize->setValue(config.tileSize);
    for ( int i = 0 ; i < tilesize->count() ; i++ )
    {
        if ( tilesize->itemData(i).toInt() == newConfig.tileSize )
        {
            tilesize->setCurrentIndex(i);
            break;
        }
    }

    if ( newConfig.fitting )
    {
        algorithm->setCurrentIndex(1);
    }
    else
    {
        algorithm->setCurrentIndex(0);
    }

    inputFile->forceFile(QString::fromStdString(newConfig.imageFilename));
    filesSelected(inputFile->absolutePath());
}

void LSHConfig::setConfig(const lsh_output *output)
{
    warning_icon->setVisible(false);
    warning_lbl->setVisible(false);
    if (output)
    {
        nBits->setValue(output->bitsPerDiff);
        // to ensure it is always displayed set the maximums
        lineSize->setMaximum(output->lineSize);
        lineSize->setValue(output->lineSize);
        matrixSize->setMaximum(output->lineSize*output->channels[0].rows);
        matrixSize->setValue(output->lineSize*output->channels[0].rows);
        if (output->rescaled != 1.0)
        {
            warning_lbl->setText(
                tr("The output grid has been rescaled by x")
                + QString::number(output->rescaled));
            warning_icon->setVisible(true);
            warning_lbl->setVisible(true);
        }

        bool ro = false;

        if (output->outputMSE[0].rows > 0 && output->outputMSE[0].cols > 0)
        {
            // double mean, std_dev;
            cv::Scalar mean, stddev;
            double acc = 0.0;
            double min_v, max_v;

            for (int c = 0; c < 4; c++)
            {
                cv::minMaxLoc(output->outputMSE[c], &(min_v), &(max_v));
                // cv::meanStdDev(output->targetMSE[c], mean, stddev);
                mean = cv::mean(output->outputMSE[c]);

                if (mean[0] > outputMSE[c]->maximum())
                {
                    outputMSE[c]->setPrefix(">");
                }
                else
                {
                    outputMSE[c]->setPrefix("");
                }
                outputMSE[c]->setValue(mean[0]);

                if (min_v > outputMIN[c]->maximum())
                {
                    outputMIN[c]->setPrefix(">");
                }
                else
                {
                    outputMIN[c]->setPrefix("");
                }
                outputMIN[c]->setValue(min_v);

                if (max_v > outputMAX[c]->maximum())
                {
                    outputMAX[c]->setPrefix(">");
                }
                else
                {
                    outputMAX[c]->setPrefix("");
                }
                outputMAX[c]->setValue(max_v);
            }

            ro = true;
        }
        outputMSE_lbl->setVisible(ro);
        outputMin_lbl->setVisible(ro);
        outputMax_lbl->setVisible(ro);
        for (int c = 0; c < 4; c++)
        {
            outputMSE[c]->setVisible(ro);
            outputMIN[c]->setVisible(ro);
            outputMAX[c]->setVisible(ro);
        }
    }
    else
    {
        nBits->setValue(0);
        lineSize->setValue(0);
        matrixSize->setValue(0);
    }
}

void LSHConfig::changeCSS(const QString &css)
{
    _css = css;

    setStyleSheet(css);
}

/*
 * public slots
 */

void LSHConfig::retranslate()
{
    Ui::LSHConfig::retranslateUi(this);

    int idx = algorithm->currentIndex();

    algorithm->clear();  // equivalent to lsh_input::fitting
    algorithm->insertItem(0, tr("Direct curve"));
    algorithm->insertItem(1, tr("Fitting curve"));

    if ( idx < 0 )
    {
        idx = 0;
    }
    algorithm->setCurrentIndex(idx);

    idx = tilesize->currentIndex();

    tilesize->clear();
    int i = 0;
    for (int n = LSH_TILE_MIN; n <= LSH_TILE_MAX; n *= 2)
    {
        tilesize->insertItem(i, tr("%n CFA", "", n), QVariant(n));
        i++;
    }

    if ( idx < 0 )
    {
        idx = 0;
    }
    tilesize->setCurrentIndex(idx);

    start_btn->setText(tr("Start"));
    /* need to be done every retranslate or UI message is there
     * instead of the icon */
    warning_icon->setPixmap(this->style()->standardIcon(WARN_ICON_TI)\
        .pixmap(QSize(WARN_ICON_W, WARN_ICON_H)));
}

void LSHConfig::filesSelected(QString newPath)
{
    start_btn->setEnabled(!inputFile->absolutePath().isEmpty()
        && !inputFile->absolutePath().isEmpty());
    /* If start_btn is enabled means file was selected and advance settings
     * are enabled */
    lshConfigAdv_btn->setEnabled(start_btn->isEnabled());

    preview_btn->setEnabled(!inputFile->absolutePath().isEmpty());
}

void LSHConfig::setReadOnly(bool ro)
{
    QAbstractSpinBox::ButtonSymbols btn = QAbstractSpinBox::UpDownArrows;
    if ( ro )
    {
        btn = QAbstractSpinBox::NoButtons;
    }

    inputFile->button()->setVisible(!ro);

    nBits_lbl->setVisible(ro);
    nBits->setVisible(ro);
    lineSize_lbl->setVisible(ro);
    lineSize->setVisible(ro);
    matrixSize_lbl->setVisible(ro);
    matrixSize->setVisible(ro);

    outputMSE_lbl->setVisible(ro);
    outputMin_lbl->setVisible(ro);
    outputMax_lbl->setVisible(ro);
    for (int c = 0; c < 4; c++)
    {
        outputMSE[c]->setVisible(ro);
        outputMIN[c]->setVisible(ro);
        outputMAX[c]->setVisible(ro);
    }

    // only way to make it read only for the moment...
    algorithm->setDisabled(ro);
    tilesize->setDisabled(ro);
    buttonBox->setVisible(!ro);
}

void LSHConfig::restoreDefaults()
{
    lsh_input defaults;
    setConfig(defaults);
}

int LSHConfig::previewInput()
{
    QString path = inputFile->absolutePath();
    if ( path.isEmpty() )
    {
        QtExtra::Exception ex(tr("Empty filename"));
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }
    CVBuffer sBuffer;

    try
    {
        sBuffer = CVBuffer::loadFile(path.toLatin1());
    }
    catch(QtExtra::Exception ex)
    {
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }

    preview_btn->setChecked(true);  // push the button to show it's loading...

    QDialog *pDiag = new QDialog(this);
    // pDiag->setModal(true);
    pDiag->setWindowTitle(tr("LSH input preview"));

    QGridLayout *pLay = new QGridLayout();

    ImageView *pView = new ImageView();
    pView->loadBuffer(sBuffer);
    pView->menubar->hide();  // Don't need menu bar here
    pView->showDPF_cb->hide();  // No DPF viewed here
    pLay->addWidget(pView);

    pDiag->setLayout(pLay);
    pDiag->resize(this->size());
    pDiag->exec();

    preview_btn->setChecked(false);  // unpush the button to show it's done

    return EXIT_SUCCESS;
}

// Creates LSH advance settings window
void LSHConfig::openAdvSett()
{
    advSett = new LSHConfigAdv();
    advSett->setStyleSheet(_css);

    connect(advSett->marginLeft_sb, SIGNAL(valueChanged(int)), this,
        SLOT(advSettPreview()));
    connect(advSett->marginRight_sb, SIGNAL(valueChanged(int)), this,
        SLOT(advSettPreview()));
    connect(advSett->marginTop_sb, SIGNAL(valueChanged(int)), this,
        SLOT(advSettPreview()));
    connect(advSett->marginBottom_sb, SIGNAL(valueChanged(int)), this,
        SLOT(advSettPreview()));
    connect(advSett->color_cb, SIGNAL(currentIndexChanged(int)), this,
        SLOT(advSettPreview()));

    advSett->setConfig(config);

    // To show the picture right away
    advSettPreview();

    if (advSett->exec() == QDialog::Accepted)
    {
        lsh_input res = advSett->getConfig();
        // Save changes (will be passed later to inputs in LSHTuning
        /*config.margin = advSett->margin_sb->value();
        config.smoothing = advSett->smoothing_sb->value();
        config.maxMatrixValue = advSett->maxMatrixValue_sb->value();
        config.bUseMaxValue = advSett->maxMatrixValueUsed_cb->isChecked();
        config.maxLineSize = advSett->maxLineSize_sb->value();
        if (!advSett->maxLineSize_cb->isChecked())
        {
            config.maxLineSize = 0;
        }*/
        config = advSett->getConfig();
    }
}

// Displays advance settings image preview
void LSHConfig::advSettPreview()
{
    // Load the image file
    QString path = inputFile->absolutePath();
    if ( path.isEmpty() )
    {
        QtExtra::Exception ex(tr("Empty filename"));
        ex.displayQMessageBox(this);
    }
    CVBuffer sBuffer;

    try
    {
        sBuffer = CVBuffer::loadFile(path.toLatin1());
    }
    catch(QtExtra::Exception ex)
    {
        ex.displayQMessageBox(this);
    }

    // Display image
    advSett->pImageView->clearButImage();
    advSett->pImageView->loadBuffer(sBuffer);

    int marginsH[2], marginsV[2];
    marginsH[0] = advSett->marginLeft_sb->value();
    marginsH[1] = advSett->marginRight_sb->value();
    marginsV[0] = advSett->marginTop_sb->value();
    marginsV[1] = advSett->marginBottom_sb->value();

    if (checkMargins(sBuffer, marginsH, marginsV))
    {
        advSett->marginLeft_sb->setValue(marginsH[0]);
        advSett->marginRight_sb->setValue(marginsH[1]);
        advSett->marginTop_sb->setValue(marginsV[0]);
        advSett->marginBottom_sb->setValue(marginsV[1]);
        return;
    }

    // Display margin rect
    QPen pen((Qt::GlobalColor)advSett->color_cb->itemData(
        advSett->color_cb->currentIndex()).toInt());
    QGraphicsRectItem *imgMargin = new QGraphicsRectItem(
        marginsH[0],
        marginsV[0],
        sBuffer.data.cols - (marginsH[0] + marginsH[1]),
        sBuffer.data.rows - (marginsV[0] + marginsV[1]));
    imgMargin->setPen(pen);
    advSett->pImageView->getScene()->addItem(imgMargin);
}

