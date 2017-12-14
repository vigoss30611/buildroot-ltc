/********************************************************************************
@file lbc/tuning.hpp

@brief Displayable inputs for LBCTuning

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

#ifndef LBC_TUNING
#define LBC_TUNING

#include <QtCore/QRect>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QPointF>
#include "ui/ui_lbctuning.h"
#include "visiontuning.hpp"

class GenConfig;
class LBCConfig;

class LBCTuning: public VisionModule, public Ui::LBCTuning
{
    Q_OBJECT
private:
    GenConfig *genConfig;
    QTabWidget *images_grp;
	QSpacerItem *spacer; // used in ScrollArea to push elements up

	std::vector<LBCConfig*> lbcConfigVec;
    
    void init();
    
public:
    LBCTuning(GenConfig *genConfig, QWidget *parent = 0);
    virtual ~LBCTuning();

	virtual QWidget *getDisplayWidget() { return this; }
    virtual QWidget *getResultWidget() { return images_grp; }
    
signals:

public slots:
    virtual QString checkDependencies() const;
    virtual int saveResult(const QString &filename="");
	virtual void saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception);

	void addEntry();
	void removeEntry(LBCConfig*);

    void retranslate();
};

#endif //LBC_TUNING