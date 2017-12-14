/**
******************************************************************************
@file visiontuning.cpp

@brief Implementation of VisionTuning

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
#include "visiontuning.hpp"

#include "vision_about.hpp"
#include "genconfig.hpp"
#include "ccm/tuning.hpp"
#include "ccm/compute.hpp"
#include "lsh/tuning.hpp"
#include "lca/tuning.hpp"
#include "lbc/tuning.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QtCore/QFileInfo>

#include <QtExtra/ostream.hpp>
#include <iostream>

#include <ispc/TemperatureCorrection.h>
#include "ispc/ParameterFileParser.h"

/*
 * VisionTuning
 */

enum HW_VERSION getDefaultVersion()
{
    enum HW_VERSION hwV = HW_UNKNOWN;
#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 4
    hwV = HW_2_X;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN == 4
    hwV = HW_2_4;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN == 6
    hwV = HW_2_6;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN == 7
    hwV = HW_2_7;
#elif FELIX_VERSION_MAJ == 3
    hwV = HW_3_X;
#else
#error unkown HW versions at compilation time
#endif
    return hwV;
}

/*
 * private
 */
void VisionTuning::addModule(VisionModule *module)
{
    //connect(module, SIGNAL(changeResult(QWidget*)), this, SLOT(changeResult(QWidget*)));
    connect(module, SIGNAL(changeCentral(QWidget*)), this, SLOT(changeCentral(QWidget*)));
    connect(module, SIGNAL(logMessage(QString)), this, SLOT(logMessage(QString)));

    moduleTabs->addTab(module->getDisplayWidget(), module->getTitle());
    //resultTabs->addTab(module->getResultWidget(), module->getTitle());
    modules.push_back(module);
}

void VisionTuning::init()
{
    Ui::VisionTuning::setupUi(this);

    //Adding General Parameter tab
    gen = new GenConfig();
    addModule(gen);

    //Adding all other tabs
    pLSH = new LSHTuning(gen, HW_UNKNOWN);
    addModule(pLSH);
    pLCA = new LCATuning(gen);
    addModule(pLCA);

    //addModule(new LBCTuning(gen)); // LBC tab moved to VisionLive

    //Adding CCM tab
    addCCM(); // calls addModule

    changeCentral(gen->getDisplayWidget());
    changeResult(gen->getResultWidget());

    connect(moduleTabs, SIGNAL(currentChanged(int)), this, SLOT(centralTabChanged(int)));

    connect(logDock, SIGNAL(visibilityChanged(bool)), view_log_action, SLOT(setChecked(bool)));
    connect(view_log_action, SIGNAL(toggled(bool)), logDock, SLOT(setVisible(bool)));

    connect(resultDock, SIGNAL(visibilityChanged(bool)), view_result_action, SLOT(setChecked(bool)));
    connect(view_result_action, SIGNAL(toggled(bool)), resultDock, SLOT(setVisible(bool)));

    connect(file_save_action, SIGNAL(triggered()), this, SLOT(saveAll()));

    connect(actionAdd_CCM_Tab, SIGNAL(triggered()), this, SLOT(addCCM()));

    connect(help_about_action, SIGNAL(triggered()), this, SLOT(aboutGUI()));

    // connect compute signal
    connect(this, SIGNAL(postprocessParams()),
            &locusCompute, SLOT(compute()));
    // connect signal finished processing
    connect(&locusCompute, SIGNAL(finished(LocusCompute*)),
            this, SLOT(paramsReady(LocusCompute*)));
    // provide log facility for LocusCompute
    connect(&locusCompute, SIGNAL(logMessage(QString)), this, SLOT(logMessage(QString)));

    logDock->setVisible(false);
    view_log_action->setChecked(false);
}

/*
 * public
 */

VisionTuning::VisionTuning(bool startedFromVL): QMainWindow()
{
    init();

    if(!startedFromVL) menuIntegrate_to_VisionLive->setEnabled(false);
}

/*
 * protected
 */

void VisionTuning::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

/*
 * public slot
 */

void VisionTuning::saveAll()
{
    ISPC::ParameterList res;
    QString dirPath;

    paramFilename = QFileDialog::getSaveFileName(this, tr("Global output FelixArgs file"), QString(), tr("*.txt"));

    if ( paramFilename.isEmpty() )
    {
        return;
    }

    // on linux the filename may not have .txt - disabled because it does not warn if the file is already present
    /*int len = paramFilename.count();
    if ( !(paramFilename.at(len-4) == '.' && paramFilename.at(len-3) == 't' && paramFilename.at(len-2) == 'x' && paramFilename.at(len-1) == 't')  )
    {
        paramFilename += ".txt";
    }*/

    dirPath = QFileInfo(paramFilename).dir().absolutePath();

    QString mess;

    for (unsigned int i = 0; i < modules.size(); i++)
    {
        mess += modules[i]->checkDependencies();
    }

    if ( !mess.isEmpty() )
    {
        QMessageBox* box = new QMessageBox(QMessageBox::Warning,
            tr("Warning"),
            tr("Some modules report warnings. Do you want to continue saving?"),
            QMessageBox::NoButton, this);
        box->addButton(QMessageBox::Save);
        box->addButton(QMessageBox::Cancel);
        box->setDefaultButton(QMessageBox::Cancel);

        box->setDetailedText(mess);

        if ( box->exec() == QMessageBox::Cancel )
        {
            return;
        }
    }

    try
    {
        unsigned int i;
        for ( i = 0 ; i < modules.size() ; i++ )
        {
            if ( modules[i]->savable() )
            {
                modules[i]->saveParameters(res);
            }
        }

        if ( pLSH ) // check in case LSH tab was not created
        {
            pLSH->saveMatrix(dirPath);
        }

        ISPC::Parameter corrections(ISPC::TemperatureCorrection::WB_CORRECTIONS.name, std::string(QString::number(CCM_tabs.size()).toLatin1()));
        res.addParameter(corrections);

        int ccmN = 0;
        for ( i = 0 ; i < CCM_tabs.size() ; i++ )
        {
            ccmN = i;
            CCM_tabs[i]->saveParameters(res, ccmN);
        }
        // generate AWS_CURVE_* parameters
        {
            locusCompute.setParams(res);
            // set `Save All` button greyed out
            file_save_action->setDisabled(true);
            // start computation
            emit postprocessParams();
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return;
    }
    return;
}

void VisionTuning::paramsReady(
        LocusCompute* ptr)
{
    QString mess;

    try
    {
        if(!ptr)
        {
            throw(QtExtra::Exception(tr("Runtime error")));
        }
        if (paramFilename.isEmpty() ||
            ISPC::ParameterFileParser::saveGrouped(
                ptr->getParams(),
                std::string(paramFilename.toLatin1())) != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save parameter file")));
        }
        mess = "Global: saving parameters in " + paramFilename + "\n";
        // save parameters even if compute has failed
        if(ptr->hasFailed())
        {
            // if failed, display warning
            throw(QtExtra::Exception(QString::fromStdString(ptr->status()), "", true));
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
    }

    if(!mess.isEmpty())
    {
        emit logMessage(mess);
    }
    // cleanup
    emit file_save_action->setEnabled(true);
}

void VisionTuning::retranslate()
{
    Ui::VisionTuning::retranslateUi(this);

    for (unsigned int i = 0 ; i < modules.size() ; i++ )
    {
        if ( modules[i] )
        {
            moduleTabs->setTabText(i, modules[i]->getTitle());
        }
    }
}

void VisionTuning::centralTabChanged(int idx)
{
    VisionModule *mod = dynamic_cast<VisionModule*>( moduleTabs->widget(idx) ); // could check idx < modules.size()

    changeResult(mod->getResultWidget());
    mod->checkDependencies();
}

void VisionTuning::changeCentral(QWidget *pWidget)
{
    moduleTabs->setCurrentWidget(pWidget);
}

void VisionTuning::changeResult(QWidget *pWidget)
{
    //resultTabs->setCurrentWidget(pWidget);
    resultDock->setWidget(pWidget);
}

void VisionTuning::logMessage(QString mess)
{
    // could also use appendPlainText() that moves to the last line automatically but it splits the text in paragraph for each append
    // usefull for debugging messages individually
#if 1
    logTabText->insertPlainText(mess);
    logTabText->moveCursor(QTextCursor::End);
#else
    logTabText->appendPlainText(mess);
#endif

    // also print on console for easy logging with stream redirection
    std::cout << mess;
}

void VisionTuning::aboutGUI()
{
    QMessageBox::about(this, TITLE_STR, VERSION_STR + "\n" + LICENCE_STR + "\n\n" + MESSAGE_STR);
}

void VisionTuning::addCCM()
{
    CCM_tabs.push_back(new CCMTuning(gen));
    addModule(CCM_tabs[CCM_tabs.size() - 1]);
    connect(CCM_tabs[CCM_tabs.size() - 1], SIGNAL(reqRemove(CCMTuning*)), this, SLOT(removeCCM(CCMTuning*)));
    connect(CCM_tabs[CCM_tabs.size() - 1], SIGNAL(reqDefault(CCMTuning*)), this, SLOT(setDefaultCCM(CCMTuning*)));

    // Set first CCM tab as default
    if(CCM_tabs.size() == 1)
    {
        CCM_tabs[0]->default_rbtn->setChecked(true);
        CCM_tabs[0]->remove_btn->setEnabled(false);
    }
    else if(CCM_tabs.size() > 1)
    {
        CCM_tabs[0]->remove_btn->setEnabled(true);
    }
}

void VisionTuning::removeCCM(CCMTuning* ob)
{
    if(CCM_tabs.size() > 1)
    {
        int tabIndex = moduleTabs->indexOf(ob);
        moduleTabs->removeTab(tabIndex);

        for(std::vector<CCMTuning*>::iterator it = CCM_tabs.begin(); it != CCM_tabs.end(); it++)
        {
            if(*it == ob)
            {
                CCM_tabs.erase(it);
                break;
            }
        }

        if(ob->default_rbtn->isChecked())
        {
            CCM_tabs[0]->default_rbtn->setChecked(true);
        }

        for (std::vector<VisionModule*>::iterator it = modules.begin() ; it != modules.end() ; it++ )
        {
            if(*it == ob)
            {
                modules.erase(it);
                break;
            }
        }
        ob->deleteLater();

        if(CCM_tabs.size() == 1)
        {
            CCM_tabs[0]->remove_btn->setEnabled(false);
        }
    }
}

void VisionTuning::setDefaultCCM(CCMTuning* ob)
{
    for(unsigned int i = 0; i < CCM_tabs.size(); i++)
    {
        if(CCM_tabs[i] != ob)
        {
            CCM_tabs[i]->resetDefaultSlot();
        }
    }
}


