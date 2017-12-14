/**
*******************************************************************************
@file lsh/tuning.hpp

@brief Display results and parameters for LSH computing

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
#ifndef LSHTUNING_HPP
#define LSHTUNING_HPP
#include "ui/ui_lshtuning.h"

#include <QWidget>
#include <string>
#include "visiontuning.hpp"

class GenConfig;
class LSHConfig;  // "lsh/config.hpp"
class LSHCompute;  // "lsh/compute.hpp"
struct lsh_output;  // "lsh/compute.hpp"
#include "lsh/gridview.hpp"

class LSHConfigAdv;

#define MARG_L 0  // left index in margins
#define MARG_T 1  // top index in margins
#define MARG_R 2  // right index in margins
#define MARG_B 3  // bottom index in margins

struct lsh_input
{
    /** @brief input image */
    std::string imageFilename;
    /**
     * @brief margins to apply on the image in order: Left, Top, Right, Bottom
     */
    int margins[4];
    /** @brief output tile size */
    int tileSize;
    int granularity;
    /** @brief to smooth noise out */
    float smoothing;
    /** @brief black level to substract per channel */
    int blackLevel[4];
    /** @brief algorithm choice */
    bool fitting;
    /** @brief max output matrix value (see @ref bUseMaxValue) */
    double maxMatrixValue;
    /** @brief use the max output value or not */
    bool bUseMaxValue;
    /** @brief maximum line size in Bytes (see @ref bUseMaxLineSize) */
    unsigned int maxLineSize;
    /** @brief use the max line size value or not */
    bool bUseMaxLineSize;
    /** @brief maximum number of bits for differencial encoding */
    unsigned int maxBitDiff;
    /** @brief enfore the max bits per difference in HW format */
    bool bUseMaxBitsPerDiff;

    lsh_input();
};

std::ostream& operator<<(std::ostream &os, const lsh_input &in);

class LSHTuning: public VisionModule, public Ui::LSHTuning
{
    Q_OBJECT

private:
    GenConfig *genConfig;  // from mainwindow
    lsh_input inputs;
    lsh_output *pOutput;
    LSHConfig *params;
    LSHGridView *gridView;
    LSHConfigAdv *advSett;

    ISPC::ParameterList *pResult;

    void init();

    bool _isVLWidget;
    QString _css;

public:
    QString outFile;
    explicit LSHTuning(GenConfig *genConfig, bool isVLWidget = false,
        QWidget *parent = 0);
    virtual ~LSHTuning();

    QString getOutputFilename();

    virtual QWidget *getDisplayWidget();
    virtual QWidget *getResultWidget();

    void changeCSS(const QString &css);

signals:
    void compute(const lsh_input *inputs);

public slots:
    virtual QString checkDependencies() const;
    int saveMatrix(const QString &dirPath);
    int saveTargetMSE(const QString &filename = "");
    virtual int saveResult(const QString &filename = "");

    virtual void saveParameters(ISPC::ParameterList &list) const
        throw(QtExtra::Exception);

    int tuning();
    int reset();
    void computationFinished(LSHCompute *toStop, lsh_output *pOutput);

    void advSettLookUp();
    void advSettLookUpPreview();

    void retranslate();
};

#endif /* LSHTUNING_HPP */
