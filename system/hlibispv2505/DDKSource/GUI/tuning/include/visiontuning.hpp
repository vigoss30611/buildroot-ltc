/**
*******************************************************************************
@file visiontuning.hpp

@brief Base class for tuning modules (e.g. CCMTuning, LSHTuning) and main
 window class

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
#ifndef VISIONTUNING_HPP
#define VISIONTUNING_HPP

#include "ui/ui_visiontuning.h"
#include <QtExtra/exception.hpp>
#include "ispc/ParameterList.h"

#include "ccm/locuscompute.hpp"

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QCloseEvent>

/**
 * @defgroup VISION_TUNING Vision Tuning library
 * @{
 */

// used in other classes when creating warning icon
#define WARN_ICON_W 40
#define WARN_ICON_H 40
// warning icon
#define WARN_ICON_TW QStyle::SP_MessageBoxWarning
// information icon
#define WARN_ICON_TI QStyle::SP_MessageBoxInformation

#ifdef WIN32
/* disable C4290: C++ exception specification ignored except to indicate a
* function is not __declspec(nothrow) */
#pragma warning( disable : 4290 )
#endif

/** @brief To know which HW is present */
enum HW_VERSION {
    /** @brief unkown HW */
    HW_UNKNOWN = 0,
    HW_2_X,
    HW_2_4,
    HW_2_6,
	HW_2_7,
    /** @brief Not supported yet */
	HW_3_X,
	MAX_HW_VERSION = HW_3_X
};

/**
 * Get the HW version based on the currently compiled-in version
 */
enum HW_VERSION getDefaultVersion();

class VisionModule: public QWidget
{
    Q_OBJECT

public:
    VisionModule(QWidget *parent=0): QWidget(parent){}
    virtual QWidget *getDisplayWidget() =0;
    virtual QWidget *getResultWidget() =0;
    /**
     * @brief returns true if the results should be saved when saveAll is done
     */
    virtual bool savable() const { return true; }
    virtual QString getTitle() const { return windowTitle(); }

public slots:
    virtual int saveResult(const QString &filename="")=0;
    virtual void saveParameters(ISPC::ParameterList &list) const
        throw(QtExtra::Exception)=0;
    /**
     * Should check if dependencies changed since result was acquired.
     *
     * Should display warning message in the UI when appropriate as well as
     * returning the message for potential pop-up message
     */
    virtual QString checkDependencies() const = 0;

signals:
    void changeCentral(QWidget *);
    void changeResult(QWidget *);
    void logMessage(QString);
};

class GenConfig;
class CCMTuning;
class LSHTuning;
class LCATuning;
class LocusCompute;

class VisionTuning: public QMainWindow, public Ui::VisionTuning
{
    Q_OBJECT

private:
    void init();
    GenConfig *gen;
    LSHTuning *pLSH;
    LCATuning *pLCA;
    LocusCompute locusCompute;

    QString paramFilename;

    std::vector<VisionModule*> modules;

    std::vector<CCMTuning*> CCM_tabs;
    void addModule(VisionModule *module);

public:
    VisionTuning(bool startedFromVL = false);
    GenConfig *getGenConfig() { return gen; }
    LSHTuning *getLSH() { return pLSH; }
    LCATuning *getLCA() { return pLCA; }
    CCMTuning *getCCM(int n) { return CCM_tabs[n]; }
    int getCCMCount() const { return CCM_tabs.size(); }

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed();

    // used to pass signal to LocusCompute
    void postprocessParams();

public slots:
    void saveAll();
    void retranslate();

    void centralTabChanged(int idx);
    void changeCentral(QWidget *pWidget);
    void changeResult(QWidget *pWidget);

    void logMessage(QString mess);
    void aboutGUI();

    void addCCM();
    void removeCCM(CCMTuning*);
    void setDefaultCCM(CCMTuning*);

    /**
     * @brief slot called when postprocessParams() has finished
     */
    void paramsReady(LocusCompute* src);
};

/**
 * @}
 */
/* end of vision tuning group */

#endif /* VISIONTUNING_HPP */
