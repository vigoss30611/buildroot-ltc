/********************************************************************************
@file gma/tuning.hpp

@brief Displayable inputs for GMATuning

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

#ifndef GMA_TUNING
#define GMA_TUNING

#include "ui/ui_gmatuning.h"
#include "visiontuning.hpp"
#include "gma/widget.hpp"

class GMATuning: public VisionModule, public Ui::GMATuning
{
    Q_OBJECT
private:  
	std::vector<GMAWidget *> _gammaCurves;

    void init();
    
public:
    GMATuning(QWidget *parent = 0);

	virtual QWidget *getDisplayWidget() { return this; }
    virtual QWidget *getResultWidget() { return NULL; }
    
public slots:
	virtual QString checkDependencies() const { return QString(); }
	virtual int saveResult(const QString &filename="") { return IMG_SUCCESS; }
	virtual void saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception) {}
    void retranslate();

	void addGammaCurve(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name);
	void removeGammaCurve(GMAWidget *gma);
	void handleExportRequest(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name);
	void handleRemoveRequest(GMAWidget *gma);

	void generatePseudoCode();
};

#endif //GMA_TUNING