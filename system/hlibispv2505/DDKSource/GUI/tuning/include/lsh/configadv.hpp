/******************************************************************************
@file lsh/configadv.hpp

@brief Advanced parameters setting for LSH

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
#ifndef CONFIGADV_HPP
#define CONFIGADV_HPP

#include "ui/ui_lshconfigadv.h"
#include "lsh/tuning.hpp"
#include "imageview.hpp"

/**
 * @ingroup VISION_TUNING
 */
class LSHConfigAdv: public QDialog, public Ui::LSHConfigAdv
{
    Q_OBJECT

private:
    enum HW_VERSION hw_version;
    void init();
        
public:
    LSHConfigAdv(enum HW_VERSION hwv, QWidget *parent = 0);

    ImageView *pImageView;
    /*lsh_input getConfig() const;
    void setConfig(const lsh_input &config);*/
    void setReadOnly(bool ro);
    
public slots:
    void retranslate();

private slots:
    void onOKButtonClicked(){ this->setResult(QDialog::Accepted); }
    void onCancelButtonClicked(){ this->setResult(QDialog::Rejected); }

};

#endif // CONFIGADV_HPP
