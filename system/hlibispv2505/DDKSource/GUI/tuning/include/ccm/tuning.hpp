/**
******************************************************************************
@file ccm/tuning.hpp

@brief Displayable results and inputs for CCM computation

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
#ifndef CCM_TUNING
#define CCM_TUNING

class QGraphicsView;
class QGraphicsScene;
#include <QtCore/QRect>
#include <QtCore/QObject>
class FLXLoader;
class QThread;

#define MACBETCH_W 6
#define MACBETCH_H 4
#define MACBETCH_N (MACBETCH_W*MACBETCH_H)

#include "ui/ui_ccmtuning.h"
#include "buffer.hpp"
#include "imageview.hpp"
#include "visiontuning.hpp"

#include <QtCore/QVector>
#include <QtCore/QPointF>
class QTabWidget;

class CCMCompute;

namespace QwtExtra {
class EditableCurve;
}

class GenConfig; // contains BLC info

/**
 * @ingroup VISION_TUNING
 */
struct ccm_input
{
    double expectedRGB[MACBETCH_N][3]; // macbetch colours without gamma applied normalized to 0-255 to match - not expected to change
    cv::Rect checkerCoords[MACBETCH_N];
    int whitePatchI; // index of white patch in expectedRGB - not expected to change
    int blackPatchI; // index of black patch in expectedRGB - not expected to change

    double sensorBlack[4];
    double systemBlack; // value used as system black level

    // advanced parameters of the algorithm
    int niterations; // total nb of iterations
    int nwarmiterations; // nb of iterations where the matrix is random (1st step)
    int sendinfo; // nb of iterations between two updates of the current information (when finding best it is sent anyway)
    double temperatureStrength; // strength of the temperature - change to modify the strength of changes
    int changeProb; // probability of changing an element of the best matrix between 1 and 100
    double patchWeight[MACBETCH_N];

    double matrixMax[9];
    double matrixMin[9];
    double offsetMax[3];
    double offsetMin[3];

    double luminanceCorrection[3]; // rgb gamma correction (used only if TUNE_CCM_WBC_TARGET_AVG is defined at compilation time)

    bool bCheckMaxWBCGain; // should we limit the WBC gains? (used only if TUNE_CCM_WBC_TARGET_AVG is not defined at compilation time)
    double maxWBCGain; // maximum accepted WBC gain (used only if TUNE_CCM_WBC_TARGET_AVG is not defined at compilation time)

    bool bCIE94; // use CIE94 as error metric - cannot be gamma corrected!
    bool bgammadisplay; // apply gamma correction on the pictures to display
    bool bnormalise; // normalise the matrix so that it has columns adding up to 1.0
    bool bverbose;
    bool berrorGraph;

    // files
    QString inputFilename; // input FLX in RGGB

    ccm_input();
};

/**
 * create the object
 *
 * change the inputs if needed
 * run the tuning function
 *
 * @ingroup VISION_TUNING
 */
class CCMTuning: public VisionModule, public Ui::CCMTuning
{
    Q_OBJECT
private:
    GenConfig *genConfig; // from mainwindow
    double currBest;

    // image tabs
    QTabWidget *images_grp;
    ImageView *blc_tab;
    ImageView *wbc_tab;
    ImageView *best_tab;

    // result tabs
    QWidget *finalError;

public:
	ISPC::ParameterList *pResult;
private:
    void init();
    void addGrid(QGraphicsScene *pScene, QRect *checkerCoords); // checkerCoords has to be populated

    bool _isVLWidget;
	QString _css;

public:
    enum BufferDisplay
    {
        ORIG_BUFFER = 0,
        WBC_BUFFER,
        BEST_BUFFER,
    };

    ccm_input inputs;

    virtual QWidget *getDisplayWidget() { return this; }
    virtual QWidget *getResultWidget() { return images_grp; }
    
    CCMTuning(GenConfig *genConfig, bool isVLWidget = false, QWidget *parent = 0);

	bool savable() const { return default_rbtn->isChecked(); }

    virtual ~CCMTuning();

	void changeCSS(const QString &css);
    
signals:
    void compute(const ccm_input *inputs);
	void reqRemove(CCMTuning*);
	void reqDefault(CCMTuning*);

public slots:
    void displayBuffer(int b, CVBuffer *pBuffer); // will delete pBuffer
    void addError(int ie, double error, int ib, double best);
	void computationFinished(CCMCompute *toStop, ISPC::ParameterList *result, double *patchErrors);
    void displayErrors(double *patchError);
	void displayResult(const ISPC::ParameterList &result);

    virtual QString checkDependencies() const;
    virtual int saveResult(const QString &filename="");
	virtual void saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception);
	void saveParameters(ISPC::ParameterList &list, int ccmN) const throw(QtExtra::Exception);

    int tuning();
    int reset();

	void removeSlot();
	void setDefaultSlot();
	void resetDefaultSlot();

	void advSettLookUp();

	void logMessage(const QString &message);
	void showLog();
	void showPreview();

    void retranslate();
};

#endif // CCM_TUNING
