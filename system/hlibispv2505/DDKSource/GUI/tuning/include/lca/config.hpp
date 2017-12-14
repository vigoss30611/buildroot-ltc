/**
******************************************************************************
@file lca/config.hpp

@brief Display parameters needed for LCA computing

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
#ifndef LCACONFIG_HPP
#define LCACONFIG_HPP

#include "ui/ui_lcaconfig.h"

#include "lca/tuning.hpp"
class QDialogButtonBox;

class QPushButton;
class GridPreview;

#include <QDialog>

#include "imageview.hpp"

/**
 * @ingroup VISION_TUNING
 */
class LCAConfig: public QDialog, public Ui::LCAConfig
{
    Q_OBJECT

private:
    QPushButton *start_btn;
    
	ImageView *pImageView;

    void init();
    
    void displayLCAGrid(const QPen &pen = QPen());// clears the scene before redisplaying the grid (includes the center)

	QString _css;

public:
    LCAConfig(QWidget *parent=0);

    std::vector<cv::Rect> gridCoordinates;

    lca_input getConfig() const;
    void setConfig(const lca_input &config);

	void changeCSS(const QString &css);

public slots:
    void retranslate();

    void inputSelected(QString newPath);

    void previewGrid(int x = 0);
    void updateCenter(qreal x, qreal y,Qt::MouseButton btn);

    void restoreDefaults();
};

#endif // LCACONFIG_HPP
