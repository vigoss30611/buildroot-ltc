/**
******************************************************************************
@file ccm/config.hpp

@brief Display the configuration needed for computing CCM in CCMCompute

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
#ifndef CCMCONFIG_HPP
#define CCMCONFIG_HPP

#include "ui/ui_ccmconfig.h"
#include "ui/ui_ccmconfigadv.h"
#include "ccm/tuning.hpp" // for MACBETCH_N

#include <QDialog>

#include "imageview.hpp"

struct ccm_input; // in ccm/tuning.hpp
class QDialogButtonBox;
class QDoubleSpinBox;
class QAbstractButton;

/**
 * @ingroup VISION_TUNING
 */
class CCMConfig: public QDialog, public Ui::CCMConfig
{
	Q_OBJECT
	
private:
    QDialogButtonBox *buttonBox;
    QPushButton *start_btn;
    QPushButton *advanced_btn;
    
    std::vector<cv::Rect> gridCoordinates;
    ccm_input advancedConfig;

    int image_width;
    int image_height;

    int grid_top;
    int grid_left;
    int grid_bottom;
    int grid_right;
    int grid_patchWidth;
    int grid_patchHeight;

	ImageView *pImageView;

	void init();

	QString _css;

public:
	CCMConfig(QWidget *parent = 0);
	
    ccm_input getConfig() const;
    void setConfig(const ccm_input &config);

	void changeCSS(const QString &css);

public slots:
	void retranslate();

    void inputSelected(QString newPath);

    int execAdvanced(); // show advanced settings

    void mouseClicked(qreal x, qreal y, Qt::MouseButton button);
    void mouseReleased(qreal x, qreal y, Qt::MouseButton button);

    void computeGrid(const QPen &pen = QPen(Qt::white));
    void previewGrid(const QPen &pen = QPen(Qt::white));
};

/**
 * @ingroup VISION_TUNING
 */
class CCMConfigAdv: public QDialog, public Ui::CCMConfigAdv
{
    Q_OBJECT
private:
    QLabel *apPatchLabels[MACBETCH_N];
    QDoubleSpinBox *apPatchValue[MACBETCH_N][3];

    QDoubleSpinBox *apPatchWeights[MACBETCH_N];
    
    QDoubleSpinBox *apMinMat[9];
    QDoubleSpinBox *apMinOff[3];

    QDoubleSpinBox *apMaxMat[9];
    QDoubleSpinBox *apMaxOff[3];

    void init();

public:
    CCMConfigAdv(QWidget *parent = 0);

    void setConfig(const ccm_input &config);
    ccm_input getConfig() const;

public slots:
    void restoreDefaults();
    void chooseAction(QAbstractButton *btn);
    void setReadOnly(bool ro=true);
};

#endif // CCMCONFIG_HPP