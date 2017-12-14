/**
******************************************************************************
@file lcaconfig.cpp

@brief LCAConfig implementation

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

#include "lca/config.hpp"

#include "lca/compute.hpp"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QGraphicsEllipseItem>

#define CIRCLE_DIAMETER 10

/*
 * private
 */

void LCAConfig::init()
{
    Ui::LCAConfig::setupUi(this);

    inputFile->setFileMode(QFileDialog::ExistingFile);
    inputFile->setFileFilter(tr("*.flx", "input file filter for LCA"));

    // text filled in retranslate
    start_btn = buttonBox->addButton(QDialogButtonBox::Ok); // accept role


    QPushButton *reset_btn = buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(reset_btn, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    
    lca_input defaults;
    setConfig(defaults);

    pImageView = new ImageView();
	pImageView->menubar->hide();
    pImageView->setEnabled(false);
    pImageView->showStatistics(false);
	imgview_lay->addWidget(pImageView);

	connect(inputFile, SIGNAL(fileChanged(QString)), this, SLOT(inputSelected(QString)));
    connect(start_btn, SIGNAL(clicked()), this, SLOT(accept()));
    
    connect(centerX, SIGNAL(valueChanged(int)), this, SLOT(previewGrid(int)));
    connect(centerY, SIGNAL(valueChanged(int)), this, SLOT(previewGrid(int)));
    connect(curveColour, SIGNAL(currentIndexChanged(int)), this, SLOT(previewGrid(int)));
    connect(lcaPreview_btn, SIGNAL(clicked()), this, SLOT(previewGrid()));
	connect(pImageView, SIGNAL(mousePressed(qreal,qreal,Qt::MouseButton)), this, SLOT(updateCenter(qreal,qreal,Qt::MouseButton)));

    start_btn->setEnabled(false);
    lcaPreview_btn->setEnabled(false);
    featureWarning_lbl->setVisible(false); // only if less than LCA_MIN_FEATURES are found
    
    retranslate();
}

void LCAConfig::displayLCAGrid(const QPen &pen)
{
    pImageView->clearButImage();
    
    pImageView->addGrid(gridCoordinates, pen);

    QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(centerX->value()-(CIRCLE_DIAMETER/2), centerY->value()-(CIRCLE_DIAMETER/2), CIRCLE_DIAMETER, CIRCLE_DIAMETER);
    circle->setPen(pen);

    pImageView->getScene()->addItem(circle);
}

/*
 * public
 */

LCAConfig::LCAConfig(QWidget *parent): QDialog(parent)
{
    init();
}

lca_input LCAConfig::getConfig() const
{
    lca_input config;

    config.imageFilename = std::string(inputFile->absolutePath().toLatin1());
    config.centerX = centerX->value();
    config.centerY = centerY->value();
    config.roiVector = gridCoordinates;

    return config;
}

void LCAConfig::setConfig(const lca_input &config)
{
    if ( !config.imageFilename.empty() )
    {
        QString f = QString::fromStdString(config.imageFilename);
        inputFile->forceFile(f);
        inputSelected(f);
    }
    centerX->setValue(config.centerX);
    centerY->setValue(config.centerY);

    if ( config.roiVector.size() > 0 )
    {
        gridCoordinates = config.roiVector;

        previewGrid();
    }
}

void LCAConfig::changeCSS(const QString &css)
{
	_css = css;

	setStyleSheet(css);
}

/*
 * public slots
 */

void LCAConfig::retranslate()
{
    Ui::LCAConfig::retranslateUi(this);

    start_btn->setText(tr("Start"));

    int c = curveColour->currentIndex();
    curveColour->clear();
    curveColour->insertItem(curveColour->count(), tr("Green"), (int)Qt::green);
    curveColour->insertItem(curveColour->count(), tr("Red"), (int)Qt::red);
    curveColour->insertItem(curveColour->count(), tr("Blue"), (int)Qt::blue);
    curveColour->insertItem(curveColour->count(), tr("White"), (int)Qt::white);
    if ( c < 0 )
    {
        c = 0;
    }
    curveColour->setCurrentIndex(c);

    featureWarning_lbl->setText(tr("Need at least %n features to compute LCA.", "", LCA_MIN_FEATURES));
}

void LCAConfig::inputSelected(QString newPath)
{
    int r = EXIT_FAILURE;
    
    if ( !newPath.isEmpty() )
    {
        CVBuffer sBuffer;

        try
        {
            sBuffer = CVBuffer::loadFile(newPath.toLatin1());
            r = EXIT_SUCCESS;

            pImageView->loadBuffer(sBuffer);

            restoreDefaults();
        }
        catch(QtExtra::Exception ex)
        {
            ex.displayQMessageBox(this);
        }
    }

    pImageView->setEnabled(r==EXIT_SUCCESS);
    lcaPreview_btn->setEnabled(r==EXIT_SUCCESS);
    start_btn->setEnabled(false);

    previewGrid();
}

void LCAConfig::previewGrid(int x)
{
    if ( pImageView->isEnabled() )
    {
        const CVBuffer &buff = pImageView->getBuffer();
        int minFeat = -1;
        int maxFeat = -1;

        if ( minFeature->isChecked() )
        {
            minFeat = minFeatureSize->value();
        }
        if ( maxFeature->isChecked() )
        {
            maxFeat = maxFeatureSize->value();
        }

        gridCoordinates = LCACompute::locateFeatures(minFeat, maxFeat, buff.data);

        features->setValue( gridCoordinates.size() );

        featureWarning_lbl->setVisible(gridCoordinates.size() < LCA_MIN_FEATURES);
        start_btn->setEnabled(gridCoordinates.size() >= LCA_MIN_FEATURES);

        QPen pen((Qt::GlobalColor)curveColour->itemData(curveColour->currentIndex()).toInt());
        pen.setWidth(2);
        displayLCAGrid(pen);
    }
}

void LCAConfig::updateCenter(qreal x, qreal y, Qt::MouseButton btn)
{
    if ( pImageView->isEnabled() )
    {
        int _x = (int)floor(x), _y = (int)floor(y);

        centerX->setValue(_x);
        centerY->setValue(_y);
    }
}

void LCAConfig::restoreDefaults()
{
    const CVBuffer &sBuffer = pImageView->getBuffer();
    if ( sBuffer.data.cols > 0 )
    {
        centerX->setValue(sBuffer.data.cols/2);
        centerX->setMaximum(sBuffer.data.cols);
    }
    if ( sBuffer.data.rows > 0 )
    {
        centerY->setValue(sBuffer.data.rows/2);
        centerY->setMaximum(sBuffer.data.rows);
    }
    
    minFeature->setChecked(false);
    maxFeature->setChecked(false);
}
