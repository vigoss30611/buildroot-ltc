#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QSlider>
#include <QWidget>

namespace VisionLive
{

class DoubleSlider: public QSlider
{
	Q_OBJECT

public:
	DoubleSlider(QWidget *parent = 0); // Class condtructor
	~DoubleSlider(); // Class destructor

	void setMinimumD(double value); // Double version of setMinimum()
	void setMaximumD(double value); // Double version of setMaximum()

	double getValueD(); // Double version of value()

private:
	double _precision; // How many ticks per range

	void init(); // Initializes widget

signals:
	void valueChanged(double value); // Double version of signal valueChanged(int value)

public slots:
	void setValueD(double value); // Double version of setValue()

private slots:
	void reemitValueChanged(); // Called when value changed by valueChanged(int value) to emit valueChanged(double value)

};

} // namespace VisionLive

#endif // DOUBLESLIDER_H
