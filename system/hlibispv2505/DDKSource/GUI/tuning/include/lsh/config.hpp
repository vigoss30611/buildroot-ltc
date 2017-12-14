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

/**
 * @ingroup VISION_TUNING
 */
class LSHConfig: public QDialog, public Ui::LSHConfig
{
    Q_OBJECT

private:
    lsh_input config;
    QPushButton *start_btn;
    QPushButton *default_btn;
    LSHConfigAdv *advSett;

    /** populated at init time for ease of access - do not delete */
    QDoubleSpinBox *outputMSE[4];
    /** populated at init time for ease of access - do not delete */
    QDoubleSpinBox *outputMIN[4];
    /** populated at init time for ease of access - do not delete */
    QDoubleSpinBox *outputMAX[4];

    QString _css;
    enum HW_VERSION hw_version;
    enum INPUT_ENRTY
    {
        FILE,
        TEMP,
        CONFIG,
        PREVIEW,
        REMOVE
    };
    QString currentFileName;
    struct proxyInfo
    {
        LSH_TEMP temp;
        /** @brief input image */
        std::string imageFilename;
        /**
        * @brief margins to apply on the image in order: Left, Top, Right,
        * Bottom
        */
        int margins[4];
        int granularity;
        /** @brief to smooth noise out */
        float smoothing;

        proxyInfo()
        {
            temp = 0;
            margins[0] = 0;
            margins[1] = 0;
            margins[2] = 0;
            margins[3] = 0;
            smoothing = 0.0001f;
            granularity = 8;
        }
    };
    std::vector<proxyInfo> _proxyInput;

    void init();

public:
    explicit LSHConfig(enum HW_VERSION hwv, QWidget *parent = 0);
    virtual ~LSHConfig();

    static bool checkMargins(const CVBuffer &image, int *marginsH, int *marginsV);

    lsh_input getConfig() const;
    void setConfig(const lsh_input &config, bool reconfig = false);
    void setConfig(const lsh_output *output);

    void changeCSS(const QString &css);

public slots:
    void retranslate();

    void setReadOnly(bool ro);

    void restoreDefaults();

    void initInputFilesTable();
    void initOutputFilesTable();

    void openAdvSett(bool adv = true);
	void advSettPreview();

	void addInputFiles();
	void addInputTemps();
    void addInputEntry(int temp = 0, const QString &filename = QString());

    void removeInputFile();
    void clearInputFiles();

    void sortIputs();
	bool validateInputs(bool showError = false);
    void enableControls(bool opt = true);

	void configureInputFile();
    void previewInputFile();

	void fill_lsh_input();
	void finalizeSetup();
};

#endif /* LSHCONFIG_HPP */
