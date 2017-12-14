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
#include "lsh/tuning.hpp"
#include "lca/tuning.hpp"
#include "lbc/tuning.hpp"
#include "gma/tuning.hpp"

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
	pLSH = new LSHTuning(gen);
	addModule(pLSH);
	pLCA = new LCATuning(gen);
	addModule(pLCA);

	//addModule(new LBCTuning(gen)); // LBC tab moved to VisionLive
	//addModule(new GMATuning()); // GMA tab moved to VisionLive

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
    QString paramFilename;

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

		if (ISPC::ParameterFileParser::saveGrouped(res, std::string(paramFilename.toLatin1())) != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save parameter file")));
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return;
    }

    mess = "Global: saving parameters in " + paramFilename + "\n";
    emit logMessage(mess);
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


