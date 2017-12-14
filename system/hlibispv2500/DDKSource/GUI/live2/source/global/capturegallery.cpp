#include "capturegallery.hpp"

#include <qgridlayout.h>
#include <qlayout.h>
#include <qscrollarea.h>
#include <qfiledialog.h>

#include "capturegalleryitem.hpp"
#include "capturepreview.hpp"
#include "proxyhandler.hpp"
#include "imagehandler.hpp"

//
// Public Functions
//

VisionLive::CaptureGallery::CaptureGallery(QWidget *parent): QWidget(parent)
{
	Ui::CaptureGalleryWidget::setupUi(this);

	retranslate();

	_pProxyHandler = NULL;
	_pSpacer = NULL;

	initCaptureGallery();
}

VisionLive::CaptureGallery::~CaptureGallery()
{
	for(unsigned int i = 0; i < _previews.size(); i++)
	{
		delete _previews[i];
		_previews.clear();
	}
}

void VisionLive::CaptureGallery::addItem(CaptureGalleryItem *item)
{
	if(DisplayAreaLayout && _pSpacer && item)
	{
		_items.push_back(item);

		switch(_css)
		{
		case VisionLive::CUSTOM_CSS:
			item->setStyleSheet(CSS_CUSTOM_QLABEL);
			break;
		case VisionLive::DEFAULT_CSS:
			item->setStyleSheet(CSS_DEFAULT_QLABEL);
			break;
		}

		DisplayAreaLayout->removeItem(_pSpacer);
		DisplayAreaLayout->addWidget(item);
		DisplayAreaLayout->addItem(_pSpacer);

		QObject::connect(item, SIGNAL(open(CaptureGalleryItem *)), this, SLOT(open(CaptureGalleryItem *)));
		QObject::connect(item, SIGNAL(saveAsJPEG(CaptureGalleryItem *)), this, SLOT(saveAsJPEG(CaptureGalleryItem *)));
		QObject::connect(item, SIGNAL(saveAsFLX(CaptureGalleryItem *)), this, SLOT(saveAsFLX(CaptureGalleryItem *)));
		QObject::connect(item, SIGNAL(saveConfiguration(CaptureGalleryItem *)), this, SLOT(saveConfiguration(CaptureGalleryItem *)));
		QObject::connect(item, SIGNAL(remove(CaptureGalleryItem *)), this, SLOT(remove(CaptureGalleryItem *)));
	}
}

void VisionLive::CaptureGallery::setProxyHandler(ProxyHandler *commandSender)
{
	_pProxyHandler = commandSender;
}

void VisionLive::CaptureGallery::setCSS(VisionLive::CSS css)
{
	_css = css;

	for(unsigned int i = 0; i < _previews.size(); i++)
	{
		switch(_css)
		{
		case VisionLive::CUSTOM_CSS:
			_previews[i]->setStyleSheet(CSS_CUSTOM_STYLESHEET);
			break;
		case VisionLive::DEFAULT_CSS:
			_previews[i]->setStyleSheet(CSS_DEFAULT_STYLESHEET);
			break;
		}
	}
}

//
// Private Functions
//

void VisionLive::CaptureGallery::initCaptureGallery()
{
	DisplayArea->setAlignment(Qt::AlignLeft);

	_pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	DisplayAreaLayout->addItem(_pSpacer);

	setFixedHeight(250);
}

void VisionLive::CaptureGallery::saveDPFMap(IMG_UINT8 *map, int nDefects, const QString &filename)
{
	QString path = filename;

    if ( path.isEmpty() )
    {
        path = QFileDialog::getSaveFileName(this, tr("Save DPF Map"), "DPF_OutMap.dpf", tr("DPF Map (*.dpf)"));
    }

    if ( !path.isEmpty() )
    {
		QFile file(path);
		if (!file.open(QIODevice::WriteOnly))
			return;
		file.write((char *)map, nDefects*sizeof(IMG_UINT64));
		file.close();
    }
}

void VisionLive::CaptureGallery::saveParameters(const ISPC::ParameterList &config, const QString &filename)
{
    QString path = filename;

    if(path.isEmpty())
    {
        path = QFileDialog::getSaveFileName(this, tr("Save Felix Setup Parameters"), "FelixSetupArgs.txt", tr("Felix Setup Args (*.txt)"));
    }

    if(!path.isEmpty())
    {
        std::string tosave = std::string(path.toLatin1());
		ISPC::ParameterFileParser::save(config, tosave);
    }
}

void VisionLive::CaptureGallery::retranslate()
{
	retranslateUi(this);
}

//
// Public Slots
//

void VisionLive::CaptureGallery::open(CaptureGalleryItem *item)
{
	CapturePreview *preview = new CapturePreview(item);
	_previews.push_back(preview);

	switch(_css)
	{
	case VisionLive::CUSTOM_CSS:
		preview->setStyleSheet(CSS_CUSTOM_STYLESHEET);
		break;
	case VisionLive::DEFAULT_CSS:
		preview->setStyleSheet(CSS_DEFAULT_STYLESHEET);
		break;
	}

	QObject::connect(preview, SIGNAL(previewClosed(CapturePreview *)), this, SLOT(previewClosed(CapturePreview *)));
	QObject::connect(preview, SIGNAL(saveAsJPEG(CaptureGalleryItem *)), this, SLOT(saveAsJPEG(CaptureGalleryItem *)));
	QObject::connect(preview, SIGNAL(saveAsFLX(CaptureGalleryItem *)), this, SLOT(saveAsFLX(CaptureGalleryItem *)));
	QObject::connect(preview, SIGNAL(saveDPFMap(CaptureGalleryItem *, int)), this, SLOT(saveDPFMap(CaptureGalleryItem *, int)));
	QObject::connect(preview, SIGNAL(saveConfiguration(CaptureGalleryItem *)), this, SLOT(saveConfiguration(CaptureGalleryItem *)));
	preview->show();
}

void VisionLive::CaptureGallery::saveAsJPEG(CaptureGalleryItem *item)
{
	if(_pProxyHandler)
	{
		for(unsigned int i = 0; i < _items.size(); i++)
		{
			if(item == _items[i])
			{
				QString filename;
				QString proposedName = item->getRecData()->name;

				if(filename.isEmpty())
				{
					ImageType type = item->getRecData()->type;

					QString title = tr("Save");
					QString filter =  tr("JPEG (*.jpg *.jpeg)");
					proposedName += ".jpg";

					filename = QFileDialog::getSaveFileName(this, title, proposedName, filter);
				}

				if(!filename.isEmpty())
				{
					_pProxyHandler->saveRecAsJPEG(i, filename);
				}
				break;
			}
		}
	}
}

void VisionLive::CaptureGallery::saveAsFLX(CaptureGalleryItem *item)
{
	if(_pProxyHandler)
	{
		for(unsigned int i = 0; i < _items.size(); i++)
		{
			if(item == _items[i])
			{
				QString filename;
				QString proposedName = item->getRecData()->name;

				if(filename.isEmpty())
				{
					ImageType type = item->getRecData()->type;

					QString title = tr("Save FLX file");
					QString filter = tr("FLX images (*.flx)");
					if(type == YUV)
					{
						title = tr("Save YUV file");
						filter = tr("YUV images (*.yuv)");

						PIXELTYPE sYUVType;
						if(PixelTransformYUV(&sYUVType, item->getRecData()->frameData[0].origData.pxlFormat) != IMG_SUCCESS)
						{
							return;
						}

						proposedName = "encoder0_"+QString::number(item->getRecData()->frameData[0].origData.width)+"x"+
							QString::number(item->getRecData()->frameData[0].origData.height)+"_"+
							QString(FormatString(item->getRecData()->frameData[0].origData.pxlFormat))+"_align"+
							QString::number(sYUVType.ui8PackedStride)+".yuv";
					}
					else if(type == RGB)
					{
						title = tr("Save RGB file");
						filter = tr("RGB images (*.flx)");
						proposedName += "_RGB.flx";
					}
					else if(type == DE)
					{
						title = tr("Save DE file");
						filter = tr("DE images (*.flx)");
						proposedName += "_DE.flx";
					}

					filename = QFileDialog::getSaveFileName(this, title, proposedName, filter);
				}

				if(!filename.isEmpty())
				{
					_pProxyHandler->saveRecAsFLX(i, filename);
				}
				break;
			}
		}
	}
}

void VisionLive::CaptureGallery::saveDPFMap(CaptureGalleryItem *item, int index)
{
	saveDPFMap(item->getRecData()->DPFMaps[index], item->getRecData()->nDefects[index]);
}
	
void VisionLive::CaptureGallery::saveConfiguration(CaptureGalleryItem *item)
{
	saveParameters(item->getRecData()->configuration);
}

void VisionLive::CaptureGallery::remove(CaptureGalleryItem *item)
{
	if(_pProxyHandler)
	{
		for(unsigned int i = 0; i < _items.size(); i++)
		{
			if(item == _items[i])
			{
				DisplayAreaLayout->removeItem(_pSpacer);
				DisplayAreaLayout->removeWidget(item);
				DisplayAreaLayout->addItem(_pSpacer);
				item->deleteLater();
				_items.erase(_items.begin() + i);
				_pProxyHandler->removeRecord(i);
				break;
			}
		}
	}
}

void VisionLive::CaptureGallery::previewClosed(CapturePreview *preview)
{
	for(unsigned int i = 0; i < _previews.size(); i++)
	{
		if(_previews[i] == preview)
		{
			delete _previews[i];
			_previews.erase(_previews.begin() + i);
			break;
		}
	}
}