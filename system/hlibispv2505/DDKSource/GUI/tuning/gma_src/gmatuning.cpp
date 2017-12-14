/********************************************************************************
@file gmatuning.cpp

@brief GMATuning class implementation

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
#include "gma/tuning.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

GMATuning::GMATuning(QWidget *parent): VisionModule(parent)
{
    init();
}

void GMATuning::init()
{
    Ui::GMATuning::setupUi(this);

	// BT.709 according to reasearch computations (std curve + tweaking to minimise error)
    // uses the same values for R/G/B channels
    // uiCurve == 0
    const static IMG_UINT16 BT709_curve[63] = 
        {   0,   72,  144,  180,  213,  243,  270,  320,  365,  406,
          444,  512,  573,  629,  680,  728,  773,  816,  857,  896,
          933,  969, 1003, 1069, 1131, 1189, 1245, 1298, 1349, 1398,
         1445, 1490, 1535, 1577, 1619, 1659, 1699, 1737, 1775, 1811,
         1847, 1882, 1917, 1950, 1984, 2016, 2048, 2079, 2110, 2141,
         2171, 2200, 2229, 2258, 2286, 2314, 2341, 2368, 2395, 2421,
         2447, 2473, 2499};

    // sRGB according to research computaions (std curve + tweaking to minimise error)
    // uses the same values for R/G/B channels
    // uiCurve == 1
    const static IMG_UINT16 sRGB_curve[63] = 
        {   0,  173,  269,  307,  340,  370,  397,  447,  491,  531,
          568,  634,  693,  747,  796,  841,  884,  925,  963,  999,
         1034, 1068, 1100, 1161, 1218, 1272, 1323, 1372, 1418, 1463,
         1506, 1547, 1587, 1626, 1664, 1700, 1736, 1770, 1804, 1837,
         1869, 1900, 1931, 1961, 1991, 2020, 2048, 2076, 2103, 2130,
         2157, 2183, 2208, 2234, 2259, 2283, 2307, 2331, 2355, 2378,
         2401, 2423, 2446};

	const static IMG_UINT16 kneePoints[63] = {0, 16, 32, 40, 48};

	QVector<QPointF> dataBT709;

	int x = 0;
	int z = 0;
	for(int i = 0; i < 63; i++)
	{
		dataBT709.push_back(QPointF(x, BT709_curve[i]));
		if(z < 32) x += 16;
		else if(z < 64) x += 8;
		else if(z < 128) x += 16;
		else if(z < 256) x += 32;
		else if(z < 512) x += 32;
		else if(z < 1024) x += 64;
		else x += 64;
		z += 64;
	}

	x = 0;
	z = 0;

	QVector<QPointF> datasRGB;
	for(int i = 0; i < 63; i++)
	{
		datasRGB.push_back(QPointF(x, sRGB_curve[i]));
		if(z < 32) x += 16;
		else if(z < 64) x += 8;
		else if(z < 128) x += 16;
		else if(z < 256) x += 32;
		else if(z < 512) x += 32;
		else if(z < 1024) x += 64;
		else x += 64;
		z += 64;
	}

	addGammaCurve(dataBT709, dataBT709, dataBT709, false, "BT709");
	addGammaCurve(datasRGB, datasRGB, datasRGB, false, "sRGB");

	connect(generate_btn, SIGNAL(clicked()), this, SLOT(generatePseudoCode()));
}

void GMATuning::retranslate()
{
	Ui::GMATuning::retranslateUi(this);
}

void GMATuning::addGammaCurve(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name)
{
	_gammaCurves.push_back(new GMAWidget(dataRed, dataGreen, dataBlue, editable));
	curveTabs->addTab(_gammaCurves[_gammaCurves.size()-1], name);
	
	// Disabling removing first two standards
	if(_gammaCurves.size() <= 2)
	{
		_gammaCurves[_gammaCurves.size()-1]->remove_btn->setVisible(false);
		_gammaCurves[_gammaCurves.size()-1]->editRed_btn->setVisible(false);
		_gammaCurves[_gammaCurves.size()-1]->editGreen_btn->setVisible(false);
		_gammaCurves[_gammaCurves.size()-1]->editBlue_btn->setVisible(false);
	}

	connect(_gammaCurves[_gammaCurves.size()-1], SIGNAL(requestExport(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>, bool, QString)), 
		this, SLOT(handleExportRequest(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>, bool, QString)));
	connect(_gammaCurves[_gammaCurves.size()-1], SIGNAL(requestRemove(GMAWidget *)), 
		this, SLOT(handleRemoveRequest(GMAWidget *)));
}

void GMATuning::removeGammaCurve(GMAWidget *gma)
{
	curveTabs->removeTab(curveTabs->indexOf(gma));
	for(unsigned int i = 0; i < _gammaCurves.size(); i++)
	{
		if(_gammaCurves[i] == gma)
		{
			delete _gammaCurves[i];
			_gammaCurves[i] = NULL;
			_gammaCurves.erase(_gammaCurves.begin()+i);
		}
	}
}

void GMATuning::handleExportRequest(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name)
{
	addGammaCurve(dataRed, dataGreen, dataBlue, editable, name);
}

void GMATuning::handleRemoveRequest(GMAWidget *gma)
{
	removeGammaCurve(gma);
}

void GMATuning::generatePseudoCode()
{
	QString filename;

    if(filename.isEmpty())
    {
		filename = QFileDialog::getSaveFileName(this, tr("Generate pseudo code"), ".", tr("File name (*.txt)"));
        if(filename.isEmpty()) return;
    }

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Message"), tr("Error openning file!"));
		return;
	}

	QTextStream out(&file);
	for(unsigned int i = 0; i < _gammaCurves.size(); i++)
	{
		QVector<QPointF> dataRed = _gammaCurves[i]->getData(0);
		QVector<QPointF> dataGreen = _gammaCurves[i]->getData(1);
		QVector<QPointF> dataBlue = _gammaCurves[i]->getData(2);

		out << "IMG_UINT16 " << curveTabs->tabText(i) << "_Red_curve[GMA_N_POINTS] = \n";
		out << "{";
		for(int j = 0; j < dataRed.size(); j++)
		{
			out << (IMG_UINT16)dataRed[j].y();
			if(j<dataRed.size()-1) out << ", ";
			if(j%10 == 0 && j > 0) out << "\n";
		}
		out << "}\n\n";

		out << "IMG_UINT16 " << curveTabs->tabText(i) << "_Green_curve[GMA_N_POINTS] = \n";
		out << "{";
		for(int j = 0; j < dataGreen.size(); j++)
		{
			out << (IMG_UINT16)dataGreen[j].y();
			if(j<dataGreen.size()-1) out << ", ";
			if(j%10 == 0 && j > 0) out << "\n";
		}
		out << "}\n\n";

		out << "IMG_UINT16 " << curveTabs->tabText(i) << "_Blue_curve[GMA_N_POINTS] = \n";
		out << "{";
		for(int j = 0; j < dataBlue.size(); j++)
		{
			out << (IMG_UINT16)dataBlue[j].y();
			if(j<dataBlue.size()-1) out << ", ";
			if(j%10 == 0 && j > 0) out << "\n";
		}
		out << "}\n\n";
	}

	file.close();
}