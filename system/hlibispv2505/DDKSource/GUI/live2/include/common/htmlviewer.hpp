#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

#include <qwidget.h>

class QGridLayout;
class QWebView;

namespace VisionLive
{

class HTMLViewer: public QWidget
{
	Q_OBJECT

public:
	HTMLViewer(QWidget *parent = 0); // Class condtructor
	~HTMLViewer(); // Class destructor
	void loadFile(const QString &path); // Loads HTML file
	void loadWebPage(const QString &www); // Loads Web page

private:
	enum LoadType {File, WWW};
	QGridLayout *_pMainLayout; // Main layout
	QWebView *_pWebView; // QWebViewer widget
	QString _reqFile; // Holds last requested file path
	QString _reqWWW; // Holds last requested www
	LoadType _reqType; // Holds last requested type

	void init(); // Initializes HTMLViewer

signals:
	void loadingStarted(); // Emited when load started
	void loadingFailed(); // Emited when load failed

private slots:
	void loadStarted(); // Called when page loading started
	void loadFinished(bool success); // Called when page loading finished (successfully or not)

};

} // namespace VisionLive

#endif // HTMLVIEWER_H
