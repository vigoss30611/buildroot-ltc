#include "capturepreview.hpp"
#include "capturegalleryitem.hpp"
#include "imagehandler.hpp"
#include "customgraphicsview.hpp"
#include "vectorscope.hpp"
#include "histogram.hpp"
#include "lineview.hpp"

#include <QCloseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qaction.h>

//
// Public Functions
//

VisionLive::CapturePreview::CapturePreview(CaptureGalleryItem *item, QMainWindow *parent): QMainWindow(parent)
{
	Ui::CapturePreviewWidget::setupUi(this);

	retranslate();

	_pImageView = NULL;
	_pImageScene = NULL;
	_pImageItem = NULL;
	_pItem = item;
	_pVectorScope = NULL;
	_pShowViewControl = NULL;
	_pLineViewAction = NULL;
	_pLineViewOption = NULL;
	_pDPFAction = NULL;
	_pLineView = NULL;
	_pMarkerItem = NULL;
	_pHistogramAction = NULL;
	_pVectorScopeAction = NULL;
	_pLineViewLineItem = NULL;
	_pPixelInfoItem = NULL;

	_nFrames = 0;
	_currentFrame = 0;
	_name = QString();

	_DPFDisplayed = false;

	_marker = QPoint(0, 0);

	_lineViewOption = None;

	initCapturePreview();

	initHistograms();

	initDPFMaps();

	initConfiguration();

	initVectorScope();

	initShowViewControl();

	initMarkerItem();

	initLineView();

	initPixelInfoItem();

	tabWidget->setCurrentIndex(0);
}

VisionLive::CapturePreview::~CapturePreview()
{
	if(_pImageView)
	{
		_pImageView->deleteLater();
	}

	if(_pImageScene)
	{
		_pImageScene->deleteLater();
	}

	if(_pImageItem)
	{
		delete _pImageItem;
	}

	for(unsigned int i = 0; i < _DPFPoints.size(); i++)
	{
		for(unsigned int j = 0; j < _DPFPoints[i].size(); j++)
		{
			if(_DPFPoints[i][j])
			{
				delete _DPFPoints[i][j];
			}
		}
	}

	if(_pVectorScope)
	{
		_pVectorScope->deleteLater();
	}

	for(unsigned int i = 0; i < _Histograms.size(); i++)
	{
		if(_Histograms[i]) 
		{
			_Histograms[i]->deleteLater();
		}
	}

	if(_pLineView)
	{
		_pLineView->deleteLater();
	}
}

//
// Protected Functions
//

void VisionLive::CapturePreview::closeEvent(QCloseEvent *event)
{
	emit previewClosed(this);

	event->accept();
}

//
// Private Functions
//

void VisionLive::CapturePreview::initCapturePreview()
{
	actionSave_as_FLX->setText((_pItem->getRecData()->type == VisionLive::YUV)? tr("Save as YUV") : tr("Save as FLX"));

	_pImageView = new CustomGraphicsView();
	imageViewLayout->addWidget(_pImageView);

	QObject::connect(_pImageView, SIGNAL(mousePressed(QPointF, Qt::MouseButton)), this, SLOT(markerPointChanged(QPointF, Qt::MouseButton)));

	_pImageScene = new QGraphicsScene();
	_pImageView->setScene(_pImageScene);

	_pImageItem = new QGraphicsPixmapItem();
	_pImageItem->setTransformationMode(Qt::SmoothTransformation);
	_pImageScene->addItem(_pImageItem);

	_nFrames = _pItem->getRecData()->nFrames;
	_name = _pItem->getName();

	captureName_l->setText(_name);

	_pImageItem->setPixmap(QPixmap::fromImage(*_pItem->getRecData()->frameData[_currentFrame].convertToQImage()));
	_pImageView->fitInView(0, 0, _pImageScene->width(), _pImageScene->height(), Qt::KeepAspectRatio);

	progress_pb->hide();

	if(_nFrames < 2)
	{
		previousFrame_btn->hide();
		nextFrame_btn->hide();
		frame_l->hide();
	}
	else
	{
		frame_l->setText(QString::number(1) + "/" + QString::number(_nFrames));
	}

	QObject::connect(previousFrame_btn, SIGNAL(clicked()), this, SLOT(setPreviousFrame()));
	QObject::connect(nextFrame_btn, SIGNAL(clicked()), this, SLOT(setNextFrame()));

	QObject::connect(actionSave_as_JPEG, SIGNAL(triggered()), this, SLOT(saveAsJPEG()));
	QObject::connect(actionSave_as_FLX, SIGNAL(triggered()), this, SLOT(saveAsFLX()));
	QObject::connect(actionSave_Configuration, SIGNAL(triggered()), this, SLOT(saveConfiguration()));
}

void VisionLive::CapturePreview::initHistograms()
{
	for(unsigned int i = 0; i < _pItem->getRecData()->HistDatas.size(); i++)
	{
		Histogram *pHistogram = new Histogram();

		if(_pItem->getRecData()->type == VisionLive::YUV)
		{
			pHistogram->setPlotColors(Qt::black, Qt::yellow, Qt::darkYellow, Qt::blue);
		}
		else
		{
			pHistogram->setPlotColors(Qt::red, Qt::green, Qt::darkGreen, Qt::blue);
		}

		pHistogram->histogramReceived(_pItem->getRecData()->HistDatas[i].histograms);

		imageViewLayout->addWidget(pHistogram);
		pHistogram->hide();

		_Histograms.push_back(pHistogram);
	}
}

void VisionLive::CapturePreview::showHistogram(bool option)
{
	if(_Histograms[_currentFrame] && _pHistogramAction)
	{
		if(option)
		{
			for(int i = 0; i < _nFrames; i++)
			{
				if(!_Histograms[i]->isHidden())
				{
					_Histograms[i]->hide();
				}
			}
			_Histograms[_currentFrame]->show();
		}
		else 
		{
			_Histograms[_currentFrame]->hide();
		}

		_pHistogramAction->setChecked(option);
	}
}

void VisionLive::CapturePreview::initDPFMaps()
{
	for(unsigned int frame = 0; frame < _pItem->getRecData()->DPFMaps.size(); frame++)
	{
		QTableWidget *mapWidget = new QTableWidget();
		mapWidget->setEditTriggers(false);
		mapWidget->setColumnCount(5);
		QStringList labels;
		labels << "X Coordinate" << "Y Coordinate" << "Pixel Value" << "Confidence Index" << "S";
		mapWidget->setHorizontalHeaderLabels(labels);

		std::vector<QGraphicsRectItem *> points;

		for(unsigned int i = 0; i < _pItem->getRecData()->nDefects[frame]*8; i+=8)
		{
			IMG_UINT16 xCoord = (_pItem->getRecData()->DPFMaps[frame][i+1]<<8)|_pItem->getRecData()->DPFMaps[frame][i+0];
			IMG_UINT16 yCoord = (_pItem->getRecData()->DPFMaps[frame][i+3]<<8)|_pItem->getRecData()->DPFMaps[frame][i+2];
			IMG_UINT16 value = (_pItem->getRecData()->DPFMaps[frame][i+5]<<8)|_pItem->getRecData()->DPFMaps[frame][i+4];
			IMG_UINT16 conf = ((_pItem->getRecData()->DPFMaps[frame][i+7]<<8)|_pItem->getRecData()->DPFMaps[frame][i+6])&0x0FFF;
			IMG_UINT16 s = (_pItem->getRecData()->DPFMaps[frame][i+7])>>7;

			mapWidget->insertRow(mapWidget->rowCount());
			mapWidget->setItem(i/8, 0, new QTableWidgetItem(QString::number(xCoord)));
			mapWidget->setItem(i/8, 1, new QTableWidgetItem(QString::number(yCoord)));
			mapWidget->setItem(i/8, 2, new QTableWidgetItem(QString::number(value)));
			mapWidget->setItem(i/8, 3, new QTableWidgetItem(QString::number(conf)));
			mapWidget->setItem(i/8, 4, new QTableWidgetItem(QString::number(s)));

			xCoord = xCoord/_pItem->getRecData()->xScaler;
			yCoord = yCoord/_pItem->getRecData()->yScaler;

			QGraphicsRectItem *point = new QGraphicsRectItem(xCoord-1, yCoord-1, 1+2, 1+2);
			point->setPen(QPen(Qt::red));
			point->setBrush(Qt::red);
			points.push_back(point);
		}

		DPFMaps->addTab(mapWidget, "Map " + QString::number(frame));

		_DPFPoints.push_back(points);

		QAction *saveDPFAction = new QAction("Save Map " + QString::number(frame), menuDPF_Map);
		menuDPF_Map->addAction(saveDPFAction);
		QObject::connect(saveDPFAction, SIGNAL(triggered()), this, SLOT(saveDPFMap()));
	}

	DPFMaps->setCurrentIndex(0);
}

void VisionLive::CapturePreview::showDPF(bool option)
{
	_DPFDisplayed = option;

	_pDPFAction->setChecked(option);

	showDPFPoints(option);
}

void VisionLive::CapturePreview::initConfiguration()
{
	QTableWidget *configWidget = new QTableWidget();
	configWidget->setEditTriggers(false);
	configWidget->setColumnCount(2);
	QStringList labels;
	labels << "Name" << "Value(s)";
	configWidget->setHorizontalHeaderLabels(labels);

	for(std::map<std::string, ISPC::Parameter>::const_iterator it = _pItem->getRecData()->configuration.begin(); 
		it !=  _pItem->getRecData()->configuration.end(); it++)
	{
		QString name = it->first.c_str();
		QString values;
		for(unsigned int n = 0; n < it->second.size(); n++)
		{
			values += it->second.getString(n).c_str() + QString(" ");
		}

		configWidget->insertRow(configWidget->rowCount());
		configWidget->setItem(configWidget->rowCount()-1, 0, new QTableWidgetItem(name));
		configWidget->setItem(configWidget->rowCount()-1, 1, new QTableWidgetItem(values));
	}

	configWidget->resizeColumnsToContents();

	tabWidget->addTab(configWidget, "Configuration");
}

void VisionLive::CapturePreview::initVectorScope()
{
	_pVectorScope = new VectorScope();
	imageViewLayout->addWidget(_pVectorScope);
	_pVectorScope->hide();
}

void VisionLive::CapturePreview::showVectorScope(bool option)
{
	if(_pVectorScope && _pVectorScopeAction)
	{
		if(option)
		{
			QImage *image = _pItem->getRecData()->frameData[_currentFrame].convertToQImage();

			_pVectorScope->vectorscopeReceived(VisionLive::Algorithms::generateVectorScope(image, false));
			_pVectorScope->show();

			delete image;
		}
		else 
		{
			_pVectorScope->hide();
		}

		_pVectorScopeAction->setChecked(option);
	}
}

void VisionLive::CapturePreview::initShowViewControl()
{
	_pShowViewControl = new QToolBar(this);
	_pShowViewControl->setObjectName("ShowViewControl");
	_pShowViewControl->setMovable(true);
	_pShowViewControl->setFloatable(true);

	addToolBar(Qt::TopToolBarArea, _pShowViewControl);

	_pLineViewOption = new QComboBox();
	_pLineViewOption->addItem("Horizontal");
	_pLineViewOption->addItem("Vertical");
	QObject::connect(_pLineViewOption, SIGNAL(currentIndexChanged(int)), this, SLOT(lineViewOptionChanged()));

	_pHistogramAction = new QAction(QIcon(":/files/histogram_icon.png"), tr("Histogram"), _pShowViewControl);
	_pHistogramAction->setObjectName("HistogramAction");
	_pHistogramAction->setCheckable(true);
	_pVectorScopeAction = new QAction(QIcon(":/files/histogram_icon.png"), tr("Vectorscope"), _pShowViewControl);
	_pVectorScopeAction->setObjectName("VectorscopeAction");
	_pVectorScopeAction->setCheckable(true);
	_pLineViewAction = new QAction(QIcon(":/files/histogram_icon.png"), tr("Line View"), _pShowViewControl);
	_pLineViewAction->setObjectName("LineViewAction");
	_pLineViewAction->setCheckable(true);
	_pDPFAction = new QAction(QIcon(":/files/DPF_icon.png"), tr("Defective Pixels"), _pShowViewControl);
	_pDPFAction->setObjectName("DefectivePixelsAction");
	_pDPFAction->setCheckable(true);

	_pShowViewControl->addAction(_pHistogramAction);
	_pShowViewControl->addAction(_pVectorScopeAction);
	_pShowViewControl->addAction(_pLineViewAction);
	_pShowViewControl->addWidget(_pLineViewOption);
	_pShowViewControl->addAction(_pDPFAction);

	QObject::connect(_pHistogramAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pVectorScopeAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pLineViewAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pDPFAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
}

void VisionLive::CapturePreview::initMarkerItem()
{
	_pMarkerItem = new QGraphicsRectItem(-2, -2, 4, 4);
	_pMarkerItem->setPen(QPen(Qt::white));
	_pMarkerItem->setBrush(Qt::white);
	if(_pImageScene)
	{
		_pImageScene->addItem(_pMarkerItem);
	}
}

void VisionLive::CapturePreview::initPixelInfoItem()
{
	_pPixelInfoItem = new QGraphicsTextItem();
	_pPixelInfoItem->setPos(0, 0);
	_pPixelInfoItem->setDefaultTextColor(Qt::white);
	if(_pImageScene)
	{
		_pImageScene->addItem(_pPixelInfoItem);
	}
}

void VisionLive::CapturePreview::updatePixelInfo()
{
	QImage *image = _pItem->getRecData()->frameData[_currentFrame].convertToQImage();
	std::vector<int> pixelInfo = VisionLive::Algorithms::generatePixelInfo(image, _marker.toPoint());
	if(pixelInfo.size() == 3)
	{
		_pPixelInfoItem->setPlainText(QString::number(_marker.toPoint().x()) + "x" + QString::number(_marker.toPoint().y()) + " " +
			QString::number(pixelInfo[0]) + " " + QString::number(pixelInfo[1]) + " " + QString::number(pixelInfo[2]));
	}

	int x = _marker.toPoint().x();
	int y = _marker.toPoint().y() - 20;
	int width = _pItem->getRecData()->frameData[_currentFrame].data.cols;

	if(x + _pPixelInfoItem->boundingRect().width() > width)
	{
		x += width - ceil(x + _pPixelInfoItem->boundingRect().width());
	}

	if(y < 0)
	{
		y += 0 - y;
	}

	_pPixelInfoItem->setPos(x, y);
	delete image;
}

void VisionLive::CapturePreview::setLineViewOption(int opt)
{
	if(opt == 1)
	{
		_lineViewOption = Horizontal;

		if(imageViewLayout)
		{
			imageViewLayout->removeWidget(_pLineView);
			imageViewLayout->addWidget(_pLineView, 1, 0);
		}

		if(!_pLineViewLineItem && _pImageScene)
		{
			_pLineViewLineItem = new QGraphicsLineItem(0, _marker.y(), _pItem->getRecData()->frameData[_currentFrame].data.cols, _marker.y());
			_pLineViewLineItem->setPen(QPen(QBrush(Qt::white), 1));
			_pImageScene->addItem(_pLineViewLineItem);
		}
		else if(_pLineViewLineItem && _pImageItem)
		{
			_pLineViewLineItem->setLine(0, _marker.y(), _pImageItem->boundingRect().width(), _marker.y());
		}
	}
	else if(opt == 2) 
	{
		_lineViewOption = Vertical;

		if(imageViewLayout)
		{
			imageViewLayout->removeWidget(_pLineView);
			imageViewLayout->addWidget(_pLineView, 0, 1);
		}

		if(!_pLineViewLineItem && _pImageScene)
		{
			_pLineViewLineItem = new QGraphicsLineItem(_marker.x(), 0, _marker.x(), _pItem->getRecData()->frameData[_currentFrame].data.rows);
			_pLineViewLineItem->setPen(QPen(QBrush(Qt::white), 1));
			_pImageScene->addItem(_pLineViewLineItem);
		}
		else if(_pLineViewLineItem && _pImageItem)
		{
			_pLineViewLineItem->setLine(_marker.x(), 0, _marker.x(), _pImageItem->boundingRect().height());
		}
	}
	else 
	{
		_lineViewOption = None;
		if(_pLineViewLineItem && _pImageScene)
		{
			_pImageScene->removeItem(_pLineViewLineItem);
			_pLineViewLineItem = NULL;
		}
	}
}

void VisionLive::CapturePreview::initLineView()
{
	_pLineView = new LineView();
	imageViewLayout->addWidget(_pLineView);
	if(_pItem->getRecData()->type == VisionLive::YUV)
	{
		_pLineView->setPlotColors(Qt::black, Qt::yellow, Qt::darkYellow, Qt::blue);
	}
	else
	{
		_pLineView->setPlotColors(Qt::red, Qt::green, Qt::darkGreen, Qt::blue);
	}
	_pLineView->hide();

	QObject::connect(_pLineView, SIGNAL(lineViewUpdateProgress(int)), this, SLOT(showProgress(int)));
}

void VisionLive::CapturePreview::showLineView(bool option)
{
	if(_pLineView && _pLineViewAction)
	{
		if(option)
		{
			_pLineView->setLineOption(_pLineViewOption->currentIndex());
			_pLineView->show();
			
			setLineViewOption(_pLineViewOption->currentIndex()+1);

			updateLineView();
		}
		else 
		{
			_pLineView->hide();
			
			setLineViewOption(0);
		}

		_pLineViewAction->setChecked(option);
	}
}

void VisionLive::CapturePreview::updateLineView()
{
	if (!_pLineView)
	{
		return;
	}

	QMap<QString, std::vector<std::vector<int> > > lineViewData;

	VisionLive::ImageType imgType = _pItem->getRecData()->type;
	VisionLive::ImageData imgData = _pItem->getRecData()->frameData[_currentFrame];
	if(imgType == DE)
	{
		lineViewData = VisionLive::Algorithms::generateLineView(imgData.data, imgData.mosaic, _marker.toPoint(), 3, 0.50);
	}
	else if(imgType == RAW2D)
	{
		lineViewData = VisionLive::Algorithms::generateLineView(imgData.data, imgData.mosaic, _marker.toPoint(), 3, 0.25);
	}
	else
	{
		lineViewData = VisionLive::Algorithms::generateLineView(imgData.data, imgData.mosaic, _marker.toPoint(), 3);
	}

	_pLineView->lineViewReceived(_marker.toPoint(), lineViewData, 3);
}

void VisionLive::CapturePreview::retranslate()
{
	Ui::CapturePreviewWidget::retranslateUi(this);
}

//
// Private Slots
//

void VisionLive::CapturePreview::setPreviousFrame()
{
	if(_currentFrame-1 >= 0)
	{
		changeFrame(_currentFrame-1);
	}
}

void VisionLive::CapturePreview::setNextFrame()
{
	if(_currentFrame+1 <= _nFrames-1)
	{
		changeFrame(_currentFrame+1);
	}
}

void VisionLive::CapturePreview::changeFrame(int frameIndex)
{
	_currentFrame = frameIndex;

	_pImageItem->setPixmap(QPixmap::fromImage(*_pItem->getRecData()->frameData[frameIndex].convertToQImage()));
	_pImageView->fitInView(0, 0, _pImageScene->width(), _pImageScene->height(), Qt::KeepAspectRatio);

	frame_l->setText(QString::number(frameIndex+1) + "/" + QString::number(_nFrames));

	showHistogram(_pHistogramAction->isChecked());
	showVectorScope(_pVectorScopeAction->isChecked());
	showLineView(_pLineViewAction->isChecked());
	showDPF(_pDPFAction->isChecked());
	updatePixelInfo();
}

void VisionLive::CapturePreview::showDPFPoints(bool option)
{
	if(option)
	{
		// Remove DPF points of previous frame
		QList<QGraphicsItem *> items = _pImageScene->items();
		for(int i = items.size()-1; i >= 0; i--)
		{
			if(items.at(i) != _pImageItem && items.at(i) != _pMarkerItem && items.at(i) != _pLineViewLineItem && items.at(i) != _pPixelInfoItem)
			{
				_pImageScene->removeItem(items.at(i));
			}
		}

		// Show DPF points for current frame
		for(unsigned int i = 0; i < _DPFPoints[_currentFrame].size(); i++)
		{
			_pImageScene->addItem(_DPFPoints[_currentFrame][i]);
		}
	}
	else
	{
		// Remove DPF points of current frame
		QList<QGraphicsItem *> items = _pImageScene->items();
		for(int i = items.size()-1; i >= 0; i--)
		{
			if(items.at(i) != _pImageItem && items.at(i) != _pMarkerItem && items.at(i) != _pLineViewLineItem && items.at(i) != _pPixelInfoItem)
			{
				_pImageScene->removeItem(items.at(i));
			}
		}
	}
}

void VisionLive::CapturePreview::changeShowViewWidget()
{
	QAction *showAction = ((QAction *)sender());
	QString showActionName = ((QAction *)sender())->text();

	// Show Histogram
	if(showAction == _pHistogramAction)
	{
		showHistogram(_Histograms[_currentFrame]->isHidden());
	}
	else
	{
		showHistogram(false);
	}

	// Show Vectorscope
	if(showAction == _pVectorScopeAction)
	{
		showVectorScope(_pVectorScope->isHidden());
	}
	else
	{
		showVectorScope(false);
	}

	// Show LineView
	if(showAction == _pLineViewAction)
	{
		showLineView(_pLineView->isHidden());
	}
	else
	{
		showLineView(false);
	}

	// Show DPF
	if(showAction == _pDPFAction)
	{
		showDPF(!_DPFDisplayed);
	}
	else
	{
		showDPF(false);
	}
}

void VisionLive::CapturePreview::markerPointChanged(QPointF pos, Qt::MouseButton button)
{
	if(button == Qt::LeftButton)
	{
		if(_pItem && _pMarkerItem)
		{
			QPointF point =  _pImageView->mapToScene(pos.toPoint());

			if(!(point.x() < 0 || point.x() > _pItem->getRecData()->frameData[_currentFrame].data.cols ||
				point.y() < 0 || point.y() > _pItem->getRecData()->frameData[_currentFrame].data.rows))
			{
				_marker = _pImageView->mapToScene(pos.toPoint());

				_pMarkerItem->setPos(_marker.x(), _marker.y());

				updatePixelInfo();

				if(!_pLineView->isHidden())
				{
					setLineViewOption(_pLineViewOption->currentIndex()+1);

					updateLineView();
				}
			}
		}
	}
}

void VisionLive::CapturePreview::lineViewOptionChanged()
{
	if(_pLineView)
	{
		_pLineView->setLineOption(_pLineViewOption->currentIndex());

		if(!_pLineView->isHidden())
		{
			setLineViewOption(_pLineViewOption->currentIndex()+1);
		}

		updateLineView();
	}
}

void VisionLive::CapturePreview::showProgress(int percent)
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

void VisionLive::CapturePreview::saveAsJPEG()
{
	emit saveAsJPEG(_pItem);
}

void VisionLive::CapturePreview::saveAsFLX()
{
	emit saveAsFLX(_pItem);
}

void VisionLive::CapturePreview::saveDPFMap()
{
	QStringList list = ((QAction *)sender())->text().split(" ");
	emit saveDPFMap(_pItem, list[list.size() - 1].toInt());
}

void VisionLive::CapturePreview::saveConfiguration()
{
	emit saveConfiguration(_pItem);
}