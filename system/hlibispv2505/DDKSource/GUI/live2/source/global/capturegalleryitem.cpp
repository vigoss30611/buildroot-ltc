#include "capturegalleryitem.hpp"
#include "imagehandler.hpp"

#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qgridlayout.h>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <qmenu.h>
#include <qaction.h>

//
// Public Functions
//

VisionLive::CaptureGalleryItem::CaptureGalleryItem(RecData *recData, QWidget *parent): QWidget(parent)
{
	_pMainLayout = NULL;
	_pLiveFeedView = NULL;
	_pName = NULL;
	_pImage = NULL;
	_pRecData = recData;
	_pContextMenu = NULL;
	_pOpen = NULL;
	_pSaveAsJPEG = NULL;
	_pSaveAsFLX = NULL;
	_pSaveConfiguration = NULL;
	_pRemove = NULL;

	initCaptureGalleryItem();

	initContextMenu();
}

VisionLive::CaptureGalleryItem::~CaptureGalleryItem()
{
	if(_pImage)
	{
		delete _pImage;
	}
	if(_pMainLayout)
	{
		_pMainLayout->deleteLater();
	}
	if(_pLiveFeedView)
	{
		_pLiveFeedView->deleteLater();
	}
	if(_pName)
	{
		 _pName->deleteLater();
	}
	if(_pContextMenu)
	{
		 _pContextMenu->deleteLater();
	}
}

//
// Protected Functions
//

void VisionLive::CaptureGalleryItem::contextMenuEvent(QContextMenuEvent *event)
{
	if(_pContextMenu)
	{
		_pContextMenu->popup(event->globalPos());
	}

	event->accept();
}

void VisionLive::CaptureGalleryItem::mouseDoubleClickEvent(QMouseEvent *event)
{
	open();

	event->accept();
}

//
// Private Functions
//

void VisionLive::CaptureGalleryItem::initCaptureGalleryItem()
{
	_name = ((_pRecData->nFrames > 1)? "MOV: " : "PIC: ") + _pRecData->name;

	_pMainLayout = new QGridLayout();
	setLayout(_pMainLayout);

	_pLiveFeedView = new QLabel();
	_pLiveFeedView->setFixedSize(150, 150);

	_pImage = _pRecData->frameData[0].convertToQImage();
	_pLiveFeedView->setPixmap(QPixmap::fromImage(*_pImage).scaled(_pLiveFeedView->width(), _pLiveFeedView->height(), Qt::KeepAspectRatio));

	_pName = new QLabel(_name);
	_pName->setAlignment(Qt::AlignHCenter);

	_pMainLayout->addWidget(_pLiveFeedView, 0, 0);
	_pMainLayout->addWidget(_pName, 1, 0);

	setFixedSize(200, 200);
}

void VisionLive::CaptureGalleryItem::initContextMenu()
{
	_pContextMenu = new QMenu();
	_pOpen = new QAction(tr("Open"), _pContextMenu);
	_pSaveAsJPEG = new QAction(tr("Save as JPEG"), _pContextMenu);
	_pSaveAsFLX = new QAction(((_pRecData->type == VisionLive::YUV)? tr("Save as YUV") : tr("Save as FLX")), _pContextMenu);
	_pSaveConfiguration = new QAction(tr("Save Configuration"), _pContextMenu);
	_pRemove = new QAction(tr("Remove"), _pContextMenu);

	_pContextMenu->addAction(_pOpen);
	_pContextMenu->addSeparator();
	_pContextMenu->addAction(_pSaveAsJPEG);
	_pContextMenu->addAction(_pSaveAsFLX);
	_pContextMenu->addAction(_pSaveConfiguration);
	_pContextMenu->addSeparator();
	_pContextMenu->addAction(_pRemove);

	QObject::connect(_pOpen, SIGNAL(triggered()), this, SLOT(open()));
	QObject::connect(_pSaveAsJPEG, SIGNAL(triggered()), this, SLOT(saveAsJPEG()));
	QObject::connect(_pSaveAsFLX, SIGNAL(triggered()), this, SLOT(saveAsFLX()));
	QObject::connect(_pSaveConfiguration, SIGNAL(triggered()), this, SLOT(saveConfiguration()));
	QObject::connect(_pRemove, SIGNAL(triggered()), this, SLOT(remove()));
}

//
// Public Slots
//

void VisionLive::CaptureGalleryItem::open()
{
	emit open(this);
}

void VisionLive::CaptureGalleryItem::saveAsJPEG()
{
	emit saveAsJPEG(this);
}

void VisionLive::CaptureGalleryItem::saveAsFLX()
{
	emit saveAsFLX(this);
}

void VisionLive::CaptureGalleryItem::saveConfiguration()
{
	emit saveConfiguration(this);
}

void VisionLive::CaptureGalleryItem::remove()
{
	emit remove(this);
}
