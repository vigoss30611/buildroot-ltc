#ifndef CCMPATCH_H
#define CCMPATCH_H

#include <qwidget.h>
#include "ui_ccmpatch.h"

namespace VisionLive
{

class CCMPatch : public QWidget, public Ui::CCMPatch
{
	Q_OBJECT

public:
    CCMPatch(int index, QWidget *parent = 0); // Class constructor
    ~CCMPatch(); // Class destructor
	void retranslate(); // Retranslates GUI

	// More convinient storage for actual widgets
	QDoubleSpinBox *CCM_Gains[4];
	QDoubleSpinBox *CCM_Clips[4];
	QDoubleSpinBox *CCM_Matrix[9];
	QDoubleSpinBox *CCM_Offsets[3];

private:
	int _index; // Index of this patch in WBC _CCMPaches vector
	void initPatch(); // Initializes CCMPatch

signals:
	void requestRemove(CCMPatch *ob); // Requests WBC to remove this patch
	void requestDefault(CCMPatch *ob); // Requests WBC to set this patch as default
	void requestPromote(); // Requests WBC to promote this patch to multi CCM

protected slots:
	void promote(); // Emits requestRemove() signal
	void setDefault(); // Emits requestDefault() signal
	void remove(); // Emits requestPromote() signal
	void identity(); // Sets all values to ISPC defaults
};

} // namespace VisionLive

#endif // MODULEBLC_H
