#include "dpf.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleDPF.h"
#include <felix_hw_info.h>  // to access present BRN

#include <qfiledialog.h>
#include <qmessagebox.h>

//
// Public Functions
//

VisionLive::ModuleDPF::ModuleDPF(QWidget *parent): ModuleBase(parent)
{
	Ui::DPF::setupUi(this);

	retranslate();

	_inputMapApplyed = false;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleDPF::~ModuleDPF()
{
}

void VisionLive::ModuleDPF::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		HWDetect_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		threshold_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		weight_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		outputMap_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		inputMap_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		break;

	case DEFAULT_CSS:
		HWDetect_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		threshold_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		weight_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		outputMap_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		inputMap_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		break;
	}
}

void VisionLive::ModuleDPF::retranslate()
{
	Ui::DPF::retranslateUi(this);
}

void VisionLive::ModuleDPF::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleDPF dpf;
	dpf.load(config);

	//
	// Set minimum
	//

	threshold_sb->setMinimum(ISPC::ModuleDPF::DPF_THRESHOLD.min);
	weight_sb->setMinimum(ISPC::ModuleDPF::DPF_WEIGHT.min);

	//
	// Set maximum
	//
	
	threshold_sb->setMaximum(ISPC::ModuleDPF::DPF_THRESHOLD.max);
	weight_sb->setMaximum(ISPC::ModuleDPF::DPF_WEIGHT.max);

	//
	// Set precision
	//

	DEFINE_PREC(weight_sb, 1.0f);

	//
	// Set value
	//

	HWDetect_cb->setChecked(dpf.bDetect);
	threshold_sb->setValue(dpf.ui32Threshold);
	weight_sb->setValue(dpf.fWeight);
	outputMap_cb->setChecked(dpf.bWrite);
}

void VisionLive::ModuleDPF::save(ISPC::ParameterList &config)
{
	ISPC::ModuleDPF dpf;
	dpf.load(config);

	dpf.bDetect = HWDetect_cb->isChecked();
	dpf.ui32Threshold = threshold_sb->value();
	dpf.fWeight = weight_sb->value();
	dpf.bWrite = outputMap_cb->isChecked();
	dpf.bRead = inputMap_cb->isChecked();

	dpf.save(config, ISPC::ModuleDPF::SAVE_VAL);
}

void VisionLive::ModuleDPF::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(SENSOR_received(QMap<QString, QString>)), this, SLOT(SENSOR_received(QMap<QString, QString>)));
	QObject::connect(_pProxyHandler, SIGNAL(DPF_received(QMap<QString, QString>)), this, SLOT(DPF_received(QMap<QString, QString>)));
} 

//
// Protected Functions
//

void VisionLive::ModuleDPF::initModule()
{
	_name = windowTitle();

	showInputMap(false);

	outputMap_cb->setEnabled(false);
	inputMap_cb->setEnabled(false);

	QObject::connect(HWDetect_cb, SIGNAL(stateChanged(int)), this, SLOT(detectChanged()));

	QObject::connect(load_btn, SIGNAL(clicked()), this, SLOT(DPF_loadMap()));
	QObject::connect(apply_btn, SIGNAL(clicked()), this, SLOT(DPF_applyMap()));
}

//
// Private Functions
//

void VisionLive::ModuleDPF::showInputMap(bool show)
{
	if(show)
	{
		label_3->show();
		minConfidence_sb->show();
		maxConfidence_sb->show();
		label_4->show();
		s_lb->show();
		inputMap_tbl->show();
		apply_btn->show();
		inputMapinfo_lbl->show();
	}
	else
	{
		label_3->hide();
		minConfidence_sb->hide();
		maxConfidence_sb->hide();
		label_4->hide();
		s_lb->hide();
		progress_pb->hide();
		inputMap_tbl->hide();
		apply_btn->hide();
		inputMapinfo_lbl->hide();
	}
}

void VisionLive::ModuleDPF::showProgress(int percent)
{
	if(percent < 100)
	{
		progress_pb->show();
		progress_pb->setValue(percent);
	}
	else
	{
		progress_pb->hide();
	}
}

void VisionLive::ModuleDPF::inputMapInfo(const IMG_UINT16* pDPFOutput, int nDefects, IMG_UINT height, IMG_UINT paralelism)
{
    int currWin, nWindow = 0;
    int d;
    int y;
    int maxDefects = 0;
    int *nDefectPerWindow = NULL;
    int linesPerWin = DPF_WINDOW_HEIGHT(paralelism);
    const int nD = 4;

    nWindow = height/linesPerWin +1;

    nDefectPerWindow = (int*)calloc(nWindow, sizeof(int));
    if ( nDefectPerWindow == NULL ) return;

	for ( d = 0 ; d < nDefects ; d++)
    {
		y = inputMap_tbl->item(d, 1)->text().toInt();
        currWin = y/linesPerWin;

        nDefectPerWindow[currWin]++;
    }

	maxDefects = 0; 
    currWin = 0;
    for ( y = 0 ; y < nWindow ; y++ )
    {
        maxDefects = IMG_MAX_INT(maxDefects, nDefectPerWindow[y]);
        currWin += nDefectPerWindow[y];
    }

    // may need to check selected HW instead
#ifdef BRN49116
	// BRN49116: DPF read map loading in HW has trouble if a single line uses the whole setup buffer to avoid that we add an extra element in the buffer size
    // (we could also verify that no line is matching that case but it is unlikely that a single window will use the whole available buffer)
    {
        maxDefects++;
    }
#endif /* BRN49116 */

	_readMapSize = maxDefects*DPF_MAP_INPUT_SIZE;

	inputMapinfo_lbl->setText(tr("Number of defects: ") + QString::number(currWin) + 
		tr(" Min. Conf.: ") + QString::number(_minConf) + 
		tr(" Max. Conf.: ") + QString::number(_maxConf) + 
		tr(" Avg. Conf.: ") + QString::number(_avgConf) + 
		tr(" HW Internal buffer[bytes]: ") + QString::number(_readMapSize) + "/" + QString::number(_DPFInternalSize));

    free(nDefectPerWindow);
}

//
// Public Slots
//

void  VisionLive::ModuleDPF::DPF_received(QMap<QString, QString> params)
{
	fixedPixels_sb->setSpecialValueText(params.find("FixedPixels").value());
	nOutCorrections_sb->setSpecialValueText(params.find("NOutCorrection").value());
	ndroppedCorrections_sb->setSpecialValueText(params.find("DroppedMapModifications").value());

	_paralelism = params.find("Parallelism").value().toInt();
	_DPFInternalSize = params.find("DPFInternalSize").value().toInt();
}

void  VisionLive::ModuleDPF::SENSOR_received(QMap<QString, QString> params)
{
	_sensorHeight = params.find("Height").value().toInt();;
}

//
// Protected Slots
//

void VisionLive::ModuleDPF::detectChanged()
{
	if(HWDetect_cb->isChecked())
	{
		outputMap_cb->setEnabled(true);

		if(_inputMapApplyed)
		{
			inputMap_cb->setEnabled(true);
		}
	}
	else
	{
		outputMap_cb->setChecked(false);
		outputMap_cb->setEnabled(false);

		inputMap_cb->setChecked(false);
		inputMap_cb->setEnabled(false);
	}
}

void VisionLive::ModuleDPF::DPF_loadMap(const QString &filename)
{
	emit logAction("CALL loadDPFOutMap() " + filename);

	QString path = filename;

    if(path.isEmpty())
    {
		path = QFileDialog::getOpenFileName(this, tr("Load DPF Map"), "DPF_InMap.dpf", tr("DPF Map (*.dpf)"));
    }

    if(!path.isEmpty())
    {
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly))
		{
			return;
		}
		_inputMapData = file.readAll();
		file.close();

		QObject::disconnect(minConfidence_sb, SIGNAL(valueChanged(int)), this, SLOT(filterMap()));
		QObject::disconnect(maxConfidence_sb, SIGNAL(valueChanged(int)), this, SLOT(filterMap()));
		QObject::disconnect(s_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(filterMap()));

		showInputMap(true);

		gridLayout->removeItem(verticalSpacer);

		IMG_UINT8 *map = (IMG_UINT8 *)_inputMapData.data();

		inputMap_tbl->clear();
		inputMap_tbl->setColumnCount(5);
		inputMap_tbl->setRowCount(0);
		QStringList labels;
		labels << "X Coordinate" << "Y Coordinate" << "Pixel Value" << "Confidence Index" << "S";
		inputMap_tbl->setHorizontalHeaderLabels(labels);

		int maxConf = 0;
		int minConf = 9999999;
		double averageConf = 0;
		int sumConf = 0;

		for(int i = 0; i < _inputMapData.size(); i+=8)
		{
			IMG_UINT16 xCoord = (map[i+1]<<8)|map[i+0];
			IMG_UINT16 yCoord = (map[i+3]<<8)|map[i+2];
			IMG_UINT16 value = (map[i+5]<<8)|map[i+4];
			IMG_UINT16 conf = ((map[i+7]<<8)|map[i+6])&0x0FFF;
			IMG_UINT16 s = (map[i+7])>>7;

			if(conf > maxConf)
			{
				maxConf = conf;
			}
			if(conf < minConf) 
			{
				minConf = conf;
			}
			sumConf += conf;

			inputMap_tbl->insertRow(inputMap_tbl->rowCount());
			inputMap_tbl->setItem(i/8, 0, new QTableWidgetItem(QString::number(xCoord)));
			inputMap_tbl->setItem(i/8, 1, new QTableWidgetItem(QString::number(yCoord)));
			inputMap_tbl->setItem(i/8, 2, new QTableWidgetItem(QString::number(value)));
			inputMap_tbl->setItem(i/8, 3, new QTableWidgetItem(QString::number(conf)));
			inputMap_tbl->setItem(i/8, 4, new QTableWidgetItem(QString::number(s)));

			showProgress(i*100/(_inputMapData.size()-8));
		}

		averageConf = (double)sumConf/(double)inputMap_tbl->rowCount();

		minConfidence_sb->setRange(minConf, maxConf);
		maxConfidence_sb->setRange(minConf, maxConf);
		maxConfidence_sb->setValue(maxConf);

		_maxConf = maxConf;
		_minConf = minConf;
		_avgConf = averageConf;

		IMG_UINT16 *pDPFOutput = (IMG_UINT16 *)_inputMapData.data();
		inputMapInfo(pDPFOutput, inputMap_tbl->rowCount(), _sensorHeight, _paralelism);
    }

	QObject::connect(minConfidence_sb, SIGNAL(valueChanged(int)), this, SLOT(filterMap()));
	QObject::connect(maxConfidence_sb, SIGNAL(valueChanged(int)), this, SLOT(filterMap()));
	QObject::connect(s_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(filterMap()));
}

void VisionLive::ModuleDPF::filterMap()
{
	inputMap_tbl->clear();
	inputMap_tbl->setColumnCount(5);
	inputMap_tbl->setRowCount(0);
	QStringList labels;
	labels << "X Coordinate" << "Y Coordinate" << "Pixel Value" << "Confidence Index" << "S";
	inputMap_tbl->setHorizontalHeaderLabels(labels);

	IMG_UINT8 *map = (IMG_UINT8 *)_inputMapData.data();
	
	int j = 0;
	for(int i = 0; i < _inputMapData.size(); i+=8)
	{
		showProgress(i*100/(_inputMapData.size()-8));

		IMG_UINT16 xCoord = (map[i+1]<<8)|map[i+0];
		IMG_UINT16 yCoord = (map[i+3]<<8)|map[i+2];
		IMG_UINT16 value = (map[i+5]<<8)|map[i+4];
		IMG_UINT16 conf = ((map[i+7]<<8)|map[i+6])&0x0FFF;
		IMG_UINT16 s = (map[i+7])>>7;

		if(conf > maxConfidence_sb->value() || conf < minConfidence_sb->value()) continue;

		if(s_lb->currentText() != "All")
		{
			if(s != s_lb->currentText().toInt()) continue;
		}

		inputMap_tbl->insertRow(inputMap_tbl->rowCount());
		inputMap_tbl->setItem(j, 0, new QTableWidgetItem(QString::number(xCoord)));
		inputMap_tbl->setItem(j, 1, new QTableWidgetItem(QString::number(yCoord)));
		inputMap_tbl->setItem(j, 2, new QTableWidgetItem(QString::number(value)));
		inputMap_tbl->setItem(j, 3, new QTableWidgetItem(QString::number(conf)));
		inputMap_tbl->setItem(j, 4, new QTableWidgetItem(QString::number(s)));

		j++;
	}

	IMG_UINT16 *pDPFOutput = (IMG_UINT16 *)_inputMapData.data();
	inputMapInfo(pDPFOutput, inputMap_tbl->rowCount(), _sensorHeight, _paralelism);
}

void VisionLive::ModuleDPF::DPF_applyMap()
{
	emit logAction("CALL applyDPFinputMap()");

	if(_pProxyHandler)
	{
		// ReadMap size is too big
		if(_readMapSize > _DPFInternalSize)
		{
			QMessageBox::warning(this, tr("Warning"), tr("ReadMap size is too big: ") + QString::number(_readMapSize) + "/" + QString::number(_DPFInternalSize) + " !");
			return;
		}

		// Create InMap from loaded Map (sorted)
		IMG_UINT8 *map2 = (IMG_UINT8 *)_inputMapData.data();
		QByteArray inMapData;
		for(int i = 0; i < _inputMapData.size(); i+=sizeof(IMG_UINT64))
		{
			IMG_UINT16 conf = ((map2[i+7]<<8)|map2[i+6])&0x0FFF;
			IMG_UINT16 s = (map2[i+7])>>7;

			if(conf > maxConfidence_sb->value() || conf < minConfidence_sb->value()) continue;

			if(s_lb->currentText() != "All")
			{
				if(s != s_lb->currentText().toInt()) continue;
			}

			inMapData.append(_inputMapData[i]);
			inMapData.append(_inputMapData[i+1]);
			inMapData.append(_inputMapData[i+2]);
			inMapData.append(_inputMapData[i+3]);
		}

		//printf("\nInMapSize - %d\n\n", inMapData.size()/sizeof(IMG_UINT32));

		IMG_UINT8 *map = (IMG_UINT8 *)inMapData.data();
		
		// Sorting if map size > 1
		if(inMapData.size()/sizeof(IMG_UINT32) > 1)
		{
			int ok = 0;
			while(ok == inMapData.size()/sizeof(IMG_UINT32)-1)
			{
				ok = 0;
				for(int i = 0; i < inMapData.size()-8; i+=4)
				{
					IMG_UINT16 p1XCoord = (map[i+1]<<8)|map[i+0];
					IMG_UINT16 p1YCoord = (map[i+3]<<8)|map[i+2];
					IMG_UINT16 p2XCoord = (map[i+5]<<8)|map[i+4];
					IMG_UINT16 p2YCoord = (map[i+7]<<8)|map[i+6];

					if(p2YCoord>p1YCoord)
					{
						memcpy(&(map[i+0]), &(p2XCoord), sizeof(IMG_UINT16));
						memcpy(&(map[i+2]), &(p2YCoord), sizeof(IMG_UINT16));
						memcpy(&(map[i+4]), &(p1XCoord), sizeof(IMG_UINT16));
						memcpy(&(map[i+6]), &(p1YCoord), sizeof(IMG_UINT16));
					}
					else if((p2YCoord==p1YCoord) && (p2XCoord<p1XCoord))
					{
						memcpy(&(map[i+0]), &(p2XCoord), sizeof(IMG_UINT16));
						memcpy(&(map[i+2]), &(p2YCoord), sizeof(IMG_UINT16));
						memcpy(&(map[i+4]), &(p1XCoord), sizeof(IMG_UINT16));
						memcpy(&(map[i+6]), &(p1YCoord), sizeof(IMG_UINT16));
					}
					else
					{
						ok++;
					}
				}
			}
		}
		
		// Send InMap to Fpga
		_pProxyHandler->DPF_set(map, inMapData.size()/sizeof(IMG_UINT32));

		_inputMapApplyed = true;
		inputMap_cb->setEnabled(true);
	}
}