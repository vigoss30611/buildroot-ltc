/**
******************************************************************************
@file genconfig.cpp

@brief Generic configuration display implementation

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

#include "genconfig.hpp"
#include "ispc/ParameterFileParser.h"
#include "ispc/ModuleBLC.h"

/*
 * blc_config
 */

blc_config::blc_config()
{
	for ( int i = 0 ; i < 4 ; i++ )
	{
		sensorBlack[i] = 0.0;
	}
	systemBlack = 64.0;
}

/*
 * GenConfig
 */

 /*
  * private
  */
  
void GenConfig::init()
{
	Ui::GenConfig::setupUi(this);
	
	displayWidget = new QWidget();

    sensorBlack[0] = sensorBlack0;
    sensorBlack[1] = sensorBlack1;
    sensorBlack[2] = sensorBlack2;
    sensorBlack[3] = sensorBlack3;

    for ( int i = 0 ; i < 4 ; i++ )
	{
		sensorBlack[i]->setMinimum(-127.0);
        sensorBlack[i]->setMaximum(127.0);
	}
    systemBlack->setMinimum(0);
    systemBlack->setMaximum(1023);
    
}

/*
 * public
 */

GenConfig::GenConfig(QWidget *parent): VisionModule(parent)
{
	init();

    resetDefaults();
}

blc_config GenConfig::getBLC() const
{
	blc_config result;

    for ( int i = 0 ; i < 4 ; i++ )
    {
        result.sensorBlack[i] = sensorBlack[i]->value();
    }
    result.systemBlack = systemBlack->value();

    return result;
}

void GenConfig::setBLC(const blc_config &config)
{
    for ( int i = 0 ; i < 4 ; i++ )
    {
        sensorBlack[i]->setValue(config.sensorBlack[i]);
    }
    systemBlack->setValue(config.systemBlack);
}

/*
 * public slots
 */

int GenConfig::saveResult(const QString &_filename)
{
    QString filename = _filename;
    if ( filename.isEmpty() )
    {
        // could add a messagebox to ask for a file but there is no button to save that tab yet
        return EXIT_FAILURE;
    }

	ISPC::ParameterList res;
    try
    {
        saveParameters(res);
        if (ISPC::ParameterFileParser::saveGrouped(res, std::string(filename.toLatin1())) != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save FelixParameter file with BLC parameters")));
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return EXIT_FAILURE;
    }

    QString mess = "BLC: save to " + filename;
    emit logMessage(mess);

    return EXIT_SUCCESS;
}

void GenConfig::saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception)
{
    blc_config blc = getBLC();

    ISPC::ModuleBLC blcParam;

    for ( int i = 0 ; i < 4 ; i++ )
    {
		blcParam.aSensorBlack[i] = blc.sensorBlack[i];
    }
	blcParam.ui32SystemBlack = blc.systemBlack;

	blcParam.save(list, ISPC::ModuleBLC::SAVE_VAL);
}

void GenConfig::retranslate()
{
	Ui::GenConfig::retranslateUi(this);
}

void GenConfig::resetDefaults()
{
	blc_config config;
	
	setBLC(config);
}