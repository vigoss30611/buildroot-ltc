/**
******************************************************************************
@file lca/tuning.hpp

@brief Display results and parameters for LCA computing

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
#ifndef LCATUNING_HPP
#define LCATUNING_HPP

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

#include "ui/ui_lcatuning.h"
#include "visiontuning.hpp"

class GenConfig;
class LCACompute;
class LCAPreview;
struct lca_output;

struct lca_input
{
    std::string imageFilename;
    int centerX; // lens center
    int centerY; // lens center

    std::vector<cv::Rect> roiVector; // region of interest for the computation
    
    lca_input();
};

class LCATuning: public VisionModule, public Ui::LCATuning
{
    Q_OBJECT

private:
    lca_input inputs;
    lca_output *pOutput;

    LCAPreview *resultView;
    
    void init();

	bool _isVLWidget;
	QString _css;

public:
    LCATuning(GenConfig *genConfig, bool isVLWidget = false, QWidget *parent=0);
    virtual ~LCATuning();

    virtual QWidget *getDisplayWidget() { return this; }
    virtual QWidget *getResultWidget(); // implemented in cpp because lca/preview.hpp is not included to avoid circular dependencies as it needs lca_input
    
	void changeCSS(const QString &css);

signals:
    void compute(const lca_input *inputs);

public slots:
    virtual QString checkDependencies() const;
    virtual int saveResult(const QString &filename="");

	virtual void saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception);
        
    int tuning();
    int reset();
    void computationFinished(LCACompute *toStop, lca_output *pOutput);

    void retranslate();
};

#endif // LCATUNING_HPP
