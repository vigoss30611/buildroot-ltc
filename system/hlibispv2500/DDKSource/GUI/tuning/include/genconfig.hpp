/**
******************************************************************************
@file genconfig.hpp

@brief Generic configuration display (e.g. BLC)

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
#ifndef GENCONFIG_HPP
#define GENCONFIG_HPP

#include "visiontuning.hpp"
#include "ui/ui_genconfig.h"

class QSpinBox;

struct blc_config
{
	int sensorBlack[4]; ///< @brief sensor black on each channel (as in FelixSetupArgs)
    int systemBlack; ///< @brief value used as system black level (as in FelixSetupArgs)
	
	blc_config();
};

class GenConfig: public VisionModule, public Ui::GenConfig
{
	Q_OBJECT
private:
	QWidget *displayWidget;

    QSpinBox *sensorBlack[4];
	
	void init();
	
public:
	GenConfig(QWidget *parent = 0);
	
	virtual QWidget *getDisplayWidget() { return this; }
    virtual QWidget *getResultWidget() { return displayWidget; }
    virtual QString getTitle() const { return windowTitle(); }
    	
	blc_config getBLC() const;
	void setBLC(const blc_config &config);
	
public slots:
    virtual QString checkDependencies() const { return QString(); }
    virtual int saveResult(const QString &filename="");
	virtual void saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception);

	void retranslate();
	void resetDefaults();

signals:
    void valueChanged();
};

#endif // GENCONFIG_HPP
