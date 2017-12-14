/**
******************************************************************************
@file lca/preview.hpp

@brief Display LCA coeffs

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
#ifndef LCAPREVIEW_HPP
#define LCAPREVIEW_HPP

#include "ui/ui_lcapreview.h"

#include "lca/tuning.hpp" // lca_input
#include "lca/compute.hpp" // lca_output
class ImageView;

/**
 * @ingroup VISION_TUNING
 */
class LCAPreview: public QWidget, public Ui::LCAPreview
{
	Q_OBJECT
	
private:
	void init();
	
    lca_input parameters;
	lca_output results;
    ImageView *pImageView;
	
public:
	LCAPreview(QWidget *parent=0);
	
	//virtual ~LCAPreview();
	void setResults(const lca_output &output, const lca_input &param);
	
public slots:
	void channelChanged(int channel);
	
	void displayVectors(double amplifier=-1.0); // if amplifier <= 0 then dispAmplifier->value() is used

    void retranslate();
};

#endif // LCAPREVIEW_HPP
