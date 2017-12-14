/********************************************************************************
@file lbctuning.cpp

@brief LBCTuning class implementation

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
#include "lbc/tuning.hpp"
#include <iostream>
#include <QFileDialog>
#include <QTabWidget>
#include <QPalette>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <ispc/ControlLBC.h>
#include "ui/ui_lbcwidget.h"
#include "lbc/config.hpp"
#include "ispc/ParameterFileParser.h"

LBCTuning::LBCTuning(GenConfig *genConfig, QWidget *parent): VisionModule(parent)
{
    init();
}

LBCTuning::~LBCTuning()
{
}

void LBCTuning::init()
{
    Ui::LBCTuning::setupUi(this);

    images_grp = new QTabWidget();

	scrollAreaLayout->setAlignment(Qt::AlignTop);

	QPalette Pal(palette());
	Pal.setColor(QPalette::Background, Qt::white);
	scrollArea->setPalette(Pal);
	scrollArea->setAutoFillBackground(true);
	
	spacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	scrollAreaLayout->addItem(spacer, 0, 0);

	saveAs_btn->setEnabled(false);

	connect(add_btn, SIGNAL(clicked()), this, SLOT(addEntry()));
	connect(saveAs_btn, SIGNAL(clicked()), this, SLOT(saveResult()));
}

QString LBCTuning::checkDependencies() const
{
	//There is no dependencies because there can be none LBC's

	QString mess;

	return mess;
}

int LBCTuning::saveResult(const QString &filename)
{
	QString newFilename;
        
    if ( filename.isEmpty() )
    {
        newFilename = QFileDialog::getSaveFileName(this, tr("Output FelixArgs file"), QString(), tr("*.txt"));
    }
    else
    {
        newFilename = filename;
    }

    if ( newFilename.isEmpty() )
    {
        return EXIT_FAILURE;
    }


	ISPC::ParameterList res;
    try
    {
		saveParameters(res);

		if(ISPC::ParameterFileParser::saveGrouped(res, std::string(newFilename.toLatin1())) != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save FelixParameter file with LBC parameters")));
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return EXIT_FAILURE;
    }
    
    QString mess = "LBC: saving FelixSetupArgs in " + newFilename + "\n";
    emit logMessage(mess);
	
    
    return EXIT_SUCCESS;
}

void LBCTuning::saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception)
{
	ISPC::ControlLBC lbc;

	for(unsigned int lbcN = 0; lbcN < lbcConfigVec.size(); lbcN++)
	{
		ISPC::LightCorrection lbcConfig;

		lbcConfig.lightLevel = lbcConfigVec[lbcN]->lightLevel_spin->value();
		lbcConfig.brightness = lbcConfigVec[lbcN]->brightness_spin->value();
		lbcConfig.contrast = lbcConfigVec[lbcN]->contrast_spin->value();
		lbcConfig.saturation = lbcConfigVec[lbcN]->saturation_spin->value();
		lbcConfig.sharpness = lbcConfigVec[lbcN]->sharpness_spin->value();

		lbc.addConfiguration(lbcConfig);
	}

	ISPC::Parameter p(lbc.LBC_CONFIGURATIONS.name, std::string(QString::number(lbcConfigVec.size()).toLatin1()));
	list.addParameter(p);

	lbc.save(list, ISPC::ModuleBase::SAVE_VAL);
}

void LBCTuning::retranslate()
{
	Ui::LBCTuning::retranslateUi(this);
}

void LBCTuning::addEntry()
{
	lbcConfigVec.push_back(new LBCConfig());

	int r = scrollAreaLayout->rowCount() - 1;
	scrollAreaLayout->removeItem(spacer);
	scrollAreaLayout->addWidget(lbcConfigVec[lbcConfigVec.size() - 1], r, 0);
	scrollAreaLayout->addItem(spacer, r + 1, 0);
	
	if(!lbcConfigVec.empty())
	{
		saveAs_btn->setEnabled(true);
	}

	connect(lbcConfigVec[lbcConfigVec.size() - 1], SIGNAL(removeSig(LBCConfig*)), this, SLOT(removeEntry(LBCConfig*)));
}

void LBCTuning::removeEntry(LBCConfig* pOb)
{
	
	for(unsigned int i = 0; i < lbcConfigVec.size(); i++)
	{
		if(lbcConfigVec[i] == pOb)
		{
			scrollAreaLayout->removeWidget(pOb);
			delete lbcConfigVec[i];
			lbcConfigVec.erase(lbcConfigVec.begin() + i);
		}
	}

	if(lbcConfigVec.empty())
	{
		saveAs_btn->setEnabled(false);
	}
}

