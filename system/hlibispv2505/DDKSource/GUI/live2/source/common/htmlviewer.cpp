#include "htmlviewer.hpp"

#include <qgridlayout.h>
#include <QtWebKitWidgets/qwebview.h>

//
// Public Functions
//

VisionLive::HTMLViewer::HTMLViewer(QWidget *parent): QWidget(parent)
{
	_pWebView = NULL;

	_reqType = File;

	init();
}

VisionLive::HTMLViewer::~HTMLViewer()
{
}

void VisionLive::HTMLViewer::loadFile(const QString &path)
{
	if(!_pWebView)
	{
		return;
	}

	_reqFile = path;
	_reqType  = File;

	_pWebView->load(QUrl("file:///" + path));
}

void VisionLive::HTMLViewer::loadWebPage(const QString &www)
{
	if(!_pWebView)
	{
		return;
	}

	_reqWWW = www;
	_reqType  = WWW;

	_pWebView->load(QUrl("http://" + www));
}

//
// Private Functions
//

void VisionLive::HTMLViewer::init()
{
	_pMainLayout = new QGridLayout(this);
	setLayout(_pMainLayout);

	_pWebView = new QWebView(this);
    _pWebView->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
	QObject::connect(_pWebView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    QObject::connect(_pWebView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

	_pMainLayout->addWidget(_pWebView);

	resize(800, 800);
}

//
// Private Slots
//

void VisionLive::HTMLViewer::loadStarted()
{
	emit loadingStarted();
}

void VisionLive::HTMLViewer::loadFinished(bool success)
{
	if(!success)
	{
		emit loadingFailed();

		if(_pWebView)
		{
			switch(_reqType)
			{
			case File: 
				_pWebView->setHtml(QStringLiteral("<!DOCTYPE html><html><body>Failed to load file: ") + _reqFile + QStringLiteral("</body></html>"));
				break;
			case WWW: 
				_pWebView->setHtml(QStringLiteral("<!DOCTYPE html><html><body>Failed to web page: ") + _reqFile + QStringLiteral("</body></html>"));
				break;
			}
		}
	}
}

