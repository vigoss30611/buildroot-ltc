#ifndef HWINFO_H
#define HWINFO_H

#include <qwidget.h>

#include "ui_hwinfo.h"
#include "ci/ci_api_structs.h"

class QSpinBox;

namespace VisionLive
{

class HWInfo: public QWidget, public Ui::HWINFO
{
	Q_OBJECT

public:
	HWInfo(QWidget *parent = 0); // Class constructor
	~HWInfo(); // Class destructor
	
private:
	QSpacerItem *_pSpacer; // Pushes all entrys to the top
	QList<QSpinBox *> params; // Holds all entrys values
	void initHWInfo(); // Initializes HWInfo
	void addEntry(QString description, QString value); // Adds entry

	void retranslate(); // Retranslates GUI

public slots:
	void HWInfoReceived(CI_HWINFO hwInfo); // Called when HWInfo received by CommandHandler

};

} // namespace VisionLive

#endif // HWINFO_H
