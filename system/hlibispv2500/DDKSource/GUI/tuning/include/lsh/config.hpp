/**
*******************************************************************************
@file ccm/config.hpp

@brief Display parameters needed to compute LSH grid

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
#ifndef LSHCONFIG_HPP
#define LSHCONFIG_HPP

#include "ui/ui_lshconfig.h"
#include "lsh/tuning.hpp"
#include "lsh/configadv.hpp"

struct lsh_output;

class LSHConfig: public QDialog, public Ui::LSHConfig
{
    Q_OBJECT

private:
    lsh_input config;
    QPushButton *start_btn;
    LSHConfigAdv *advSett;

    /** populated at init time for ease of access - do not delete */
    QDoubleSpinBox *outputMSE[4];
    /** populated at init time for ease of access - do not delete */
    QDoubleSpinBox *outputMIN[4];
    /** populated at init time for ease of access - do not delete */
    QDoubleSpinBox *outputMAX[4];

    QString _css;

    void init();

public:
    /** returns if margins were modified */
    static bool checkMargins(const CVBuffer &image, int *marginsH,
        int *marginsV);

    explicit LSHConfig(QWidget *parent = 0);

    lsh_input getConfig() const;
    void setConfig(const lsh_input &config);
    /** if given output is NULL resets to 0 */
    void setConfig(const lsh_output *output);

    void changeCSS(const QString &css);

public slots:
    void retranslate();

    void filesSelected(QString newPath);
    void setReadOnly(bool ro);

    void restoreDefaults();
    int previewInput();

    void openAdvSett();
    void advSettPreview();
};

#endif /* LSHCONFIG_HPP */
