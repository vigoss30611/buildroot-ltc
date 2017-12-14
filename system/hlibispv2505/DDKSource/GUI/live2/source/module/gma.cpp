#include "gma.hpp"
#include "gmawidget.hpp"
#include "proxyhandler.hpp"
#include "objectregistry.hpp"

#include "ispc/ModuleGMA.h"

#include <QFileDialog>
#include <QMessageBox>

#define GMA_CURR_GAMMA_NAME "Current_Gamma_LUT"

//
// Public Functions
//

VisionLive::ModuleGMA::ModuleGMA(QWidget *parent): ModuleBase(parent)
{
	Ui::GMA::setupUi(this);

	retranslate();

	_firstLoad = true;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleGMA::~ModuleGMA()
{
	for(unsigned int i = 0; i < _gammaCurves.size(); i++)
	{
		if(_gammaCurves[i])
		{
			_gammaCurves[i]->deleteLater();
		}
	}
}

void VisionLive::ModuleGMA::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:

		break;

	case DEFAULT_CSS:

		break;
	}
}

void VisionLive::ModuleGMA::retranslate()
{
	Ui::GMA::retranslateUi(this);
}

void VisionLive::ModuleGMA::load(const ISPC::ParameterList &config)
{
	if(_firstLoad)
	{
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

		_firstLoad = false;
	}

	//
	// Set minimum
	//

	//
	// Set maximum
	//

	//
	// Set precision
	//

	//
	// Set value
	//
}

void VisionLive::ModuleGMA::save(ISPC::ParameterList &config)
{

}

void VisionLive::ModuleGMA::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(GMA_received(CI_MODULE_GMA_LUT)), this, SLOT(GMA_received(CI_MODULE_GMA_LUT)));

	GMA_get();
} 

void VisionLive::ModuleGMA::resetConnectionDependantControls()
{
	GMA_set(GMA_changeGLUT_lb->currentIndex());
}

//
// Protected Functions
//

void VisionLive::ModuleGMA::initModule()
{
	_name = windowTitle();

	connect(GMA_changeGLUT_lb, SIGNAL(activated(int)), this, SLOT(GMA_set(int)));
	connect(GMA_genPseudoCode_btn, SIGNAL(clicked()), this, SLOT(generatePseudoCode()));
}

//
// Private Slots
//

void VisionLive::ModuleGMA::addGammaCurve(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name)
{
	GMAWidget *newGMA = new GMAWidget(dataRed, dataGreen, dataBlue, editable);
	newGMA->setObjectName("GMA_" + name);

	_gammaCurves.push_back(newGMA);
	GMA_curveTabs_tw->addTab(newGMA, name);

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(newGMA, "Modules/GMA/", newGMA->objectName());
	}
	else
	{
		emit logError("Couldn't register GMA LUT!", "ModuleGMA::addGammaCurve()");
	}

	// Disabling removing first two standards
	if(_gammaCurves.size() <= 2)
	{
		newGMA->remove_btn->setVisible(false);
		newGMA->editRed_btn->setVisible(false);
		newGMA->editGreen_btn->setVisible(false);
		newGMA->editBlue_btn->setVisible(false);
	}

	QObject::connect(newGMA, SIGNAL(requestExport(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>, bool, QString)), 
		this, SLOT(addGammaCurve(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>, bool, QString)));
	QObject::connect(newGMA, SIGNAL(requestRemove(GMAWidget *)), 
		this, SLOT(removeGammaCurve(GMAWidget *)));

	GMA_changeGLUT_lb->addItem(name);
}

void VisionLive::ModuleGMA::removeGammaCurve(GMAWidget *gma)
{
	GMA_changeGLUT_lb->removeItem(GMA_curveTabs_tw->indexOf(gma));
	GMA_curveTabs_tw->removeTab(GMA_curveTabs_tw->indexOf(gma));
	for(unsigned int i = 0; i < _gammaCurves.size(); i++)
	{
		if(_gammaCurves[i] == gma)
		{
			if(_pObjReg)
			{
				_pObjReg->deregisterObject(_gammaCurves[i]->objectName());
			}

			delete _gammaCurves[i];
			_gammaCurves[i] = NULL;
			_gammaCurves.erase(_gammaCurves.begin()+i);
		}
	}
}

void VisionLive::ModuleGMA::GMA_get()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->GMA_get();
	}
}

void VisionLive::ModuleGMA::GMA_set(int index)
{
	if(_pProxyHandler && GMA_changeGLUT_lb->itemText(index) != GMA_CURR_GAMMA_NAME)
	{
		CI_MODULE_GMA_LUT data;

		QVector<QPointF> redData = ((GMAWidget *)GMA_curveTabs_tw->widget(index))->data(0);
		QVector<QPointF> greenData = ((GMAWidget *)GMA_curveTabs_tw->widget(index))->data(1);
		QVector<QPointF> blueData = ((GMAWidget *)GMA_curveTabs_tw->widget(index))->data(2);

		for(int i = 0; i < redData.size(); i++)
		{
			data.aRedPoints[i] = (IMG_UINT16)redData[i].y();
			data.aGreenPoints[i] = (IMG_UINT16)greenData[i].y();
			data.aBluePoints[i] = (IMG_UINT16)blueData[i].y();
		}

		_pProxyHandler->GMA_set(data);
	}
}

void VisionLive::ModuleGMA::GMA_received(CI_MODULE_GMA_LUT data)
{
	QVector<QPointF> dataRed;
	QVector<QPointF> dataGreen;
	QVector<QPointF> dataBlue;

	int x = 0;
	int z = 0;
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		dataRed.push_back(QPointF(x, data.aRedPoints[i]));
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
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		dataGreen.push_back(QPointF(x, data.aGreenPoints[i]));
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
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		dataBlue.push_back(QPointF(x, data.aBluePoints[i]));
		if(z < 32) x += 16;
		else if(z < 64) x += 8;
		else if(z < 128) x += 16;
		else if(z < 256) x += 32;
		else if(z < 512) x += 32;
		else if(z < 1024) x += 64;
		else x += 64;
		z += 64;
	}

	// Remove previous initial gamma curve
	int index = -1;
	for(int i = 0; i < GMA_curveTabs_tw->count(); i++)
	{
		if(GMA_curveTabs_tw->tabText(i) == GMA_CURR_GAMMA_NAME) 
		{
			index = i;
			break;
		}
	}

	if(index != -1) removeGammaCurve((GMAWidget *)GMA_curveTabs_tw->widget(index));

	_gammaCurves.insert(_gammaCurves.begin(), new GMAWidget(dataRed, dataGreen, dataBlue, false));
	GMA_curveTabs_tw->insertTab(0, _gammaCurves[0], GMA_CURR_GAMMA_NAME);
	//curveTabs->setCurrentIndex(0);
	
	// Disabling removing first two standards
	_gammaCurves[0]->remove_btn->setVisible(false);
	_gammaCurves[0]->editRed_btn->setVisible(false);
	_gammaCurves[0]->editGreen_btn->setVisible(false);
	_gammaCurves[0]->editBlue_btn->setVisible(false);

	QObject::connect(_gammaCurves[0], SIGNAL(requestExport(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>, bool, QString)), 
		this, SLOT(addGammaCurve(QVector<QPointF>, QVector<QPointF>, QVector<QPointF>, bool, QString)));
	QObject::connect(_gammaCurves[0], SIGNAL(requestRemove(GMAWidget *)), 
		this, SLOT(removeGammaCurve(GMAWidget *)));

	GMA_changeGLUT_lb->insertItem(0, GMA_CURR_GAMMA_NAME);
}

void VisionLive::ModuleGMA::generatePseudoCode(const QString &filename)
{
	QString fileToUse = filename;

    if(fileToUse.isEmpty())
    {
		fileToUse = QFileDialog::getSaveFileName(this, tr("Generate pseudo code"), ".", tr("File name (*.txt)"));
        if(fileToUse.isEmpty()) return;
    }

	emit logAction("CALL GMA_generatePseudoCode() " + fileToUse);

	QFile file(fileToUse);
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Message"), tr("Error openning file!"));
		return;
	}

	QTextStream out(&file);
	for(unsigned int i = 0; i < _gammaCurves.size(); i++)
	{
		QVector<QPointF> dataRed = _gammaCurves[i]->data(0);
		QVector<QPointF> dataGreen = _gammaCurves[i]->data(1);
		QVector<QPointF> dataBlue = _gammaCurves[i]->data(2);

		out << "IMG_UINT16 " << GMA_curveTabs_tw->tabText(i) << "_Red_curve[GMA_N_POINTS] = \n";
		out << "{";
		for(int j = 0; j < dataRed.size(); j++)
		{
			out << (IMG_UINT16)dataRed[j].y();
			if(j<dataRed.size()-1) out << ", ";
			if(j%10 == 0 && j > 0) out << "\n";
		}
		out << "}\n\n";

		out << "IMG_UINT16 " << GMA_curveTabs_tw->tabText(i) << "_Green_curve[GMA_N_POINTS] = \n";
		out << "{";
		for(int j = 0; j < dataGreen.size(); j++)
		{
			out << (IMG_UINT16)dataGreen[j].y();
			if(j<dataGreen.size()-1) out << ", ";
			if(j%10 == 0 && j > 0) out << "\n";
		}
		out << "}\n\n";

		out << "IMG_UINT16 " << GMA_curveTabs_tw->tabText(i) << "_Blue_curve[GMA_N_POINTS] = \n";
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