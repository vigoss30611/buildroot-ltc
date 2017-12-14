#ifndef LBCCONFIG_H
#define LBCCONFIG_H

#include "ui_lbcconfig.h"

namespace VisionLive
{

class LBCConfig: public QWidget, public Ui::LBCConfig
{
	Q_OBJECT
	
public:
	LBCConfig(QWidget *parent = 0);
	~LBCConfig();

signals:
	void remove(LBCConfig *config);

public slots:
	void remove();

};

} // namespace VisionLive

#endif //LBCCONFIG_H
