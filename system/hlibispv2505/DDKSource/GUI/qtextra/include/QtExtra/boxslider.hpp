/**
******************************************************************************
 @file boxslider.hpp

 @brief BoxSlider and DoubleBoxSlider class definitions

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K.

******************************************************************************/
#ifndef BOXSLIDER_HPP
#define BOXSLIDER_HPP

#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QLayout>

#include "QtExtra/doubleslider.hpp"

namespace QtExtra {

/**
 * @brief Widget that combines a QSlider and a QSpinBox as a unique object.
 */
class BoxSlider: public QWidget
{
    Q_OBJECT

private:
    //QSpinBox *spinBox; ///< @brief the spin box
    QSlider *slider; ///< @brief the slider
    QLayout *layout; ///< @brief The layout is stored so that the one can change the widget's orientation
    bool imported; ///< @brief If the slider element are imported (not created by this widget) do not create a layout

	/**
	 * @brief Initialise the components and connects signals and slots
	 *
	 * Connects:
	 * @li spinBox::valueChanged(int) -> slider::setValue(int)
	 * @li slider::valueChanged(int) -> spinBox::setValue(int)
	 * @li spinBox::valueChanged(int) -> this::valueChanged(int) [both are signals]
	 */
	void init(Qt::Orientation orientation = Qt::Horizontal);

public:
	QSpinBox *spinBox; ///< @brief the spin box

	/**
	 * @brief Classic constructor used in the Designer - inits the widget as default init value
	 *
	 * @see Delegates to init()
	 */
	BoxSlider(QWidget* parent = 0);

	/**
	 * @param orientation to use at create
	 * @param parent 
	 *
	 * @see Delegates to init()
	 */
    BoxSlider(Qt::Orientation orientation, QWidget* parent = 0);

    /**
     * @brief import the spinbox and slider that will compose the widget. Layout and orientation are not created.
     *
     * @param pSpin not 0
     * @param pSlide not 0
     * @param parent
     *
     * @see Delegates to init()
     */
    BoxSlider(QSpinBox *pSpin, QSlider *pSlide, QWidget *parent = 0);

	/**
	 * Similar to QSpinBox::value() or QSlider::value()
	 * @see setValue()
	 */
    int value() const;

	/**
	 * Similar to QSpinBox::minimum() or QSlider::minimum()
	 * @see setMinimum() maximum() setMaximum()
	 */
    int minimum(void) const;

	/**
	 * Similar to QSpinBox::maximum() or QSlider::maximum()
	 * @see setMaximum() minimum() setMinimum()
	 */
    int maximum(void) const;

	/**
	 * Similar to QSpinBox::setMinimum() or QSlider::setMinimum()
	 * @see minimum() maximum() setMaximum()
	 */
    void setMinimum(int newMin);

	/**
	 * Similar to QSpinBox::setMaximum() or QSlider::setMaximum()
	 * @see maximum() minimum() setMinimum()
	 */
    void setMaximum(int newMax);

	/**
	 * Similar to QSlider::orientation()
	 * @see setOrientation()
	 */
	Qt::Orientation orientation(void) const;

signals:
	/**
	 * @brief Emitted when spinBox's value changed
	 * @see init()
	 */
    void valueChanged(int);

public slots:
	/**
	 * Similar to QSpinBox::setValue() or QSlider::setValue().
	 * @note Only the spinBox's value is changed (slider updates with signals).
	 */
    void setValue(int v);

	/**
	 * @brief Changes the orientation of the couple spinBox/slider.
	 *
	 * Horizontally the spin box is first (on the left) and the slider second.
	 *
	 * Vertically the slider is first (on the top) and the spin box second.
	 *
	 * Similar to QSlider::setOrientation()
     *
     * @note if the elements were imported the orientation cannot be changed
	 * @see orientation()
	 */
    void setOrientation(Qt::Orientation orientation);

    bool wasImported(void) const { return imported; }
};

/**
 * @brief Widget that combines a DoubleSlider and a QDoubleSpinBox as a unique object.
 */
class DoubleBoxSlider: public QWidget
{
    Q_OBJECT

private:
    //QDoubleSpinBox *spinBox; ///< @brief the spin box
    DoubleSlider *slider; ///< @brief the slider
    QLayout *layout; ///< @brief The layout is stored so that the one can change the widget's orientation
    bool imported; /// < @brief If the slider element are imported (not created by this widget) do not create a layout

	/**
	 * @brief Initialise the components and connects signals and slots
	 *
	 * Connects:
	 * @li spinBox::valueChanged(int) -> slider::setValue(int)
	 * @li slider::valueChanged(int) -> spinBox::setValue(int)
	 * @li spinBox::valueChanged(int) -> this::valueChanged(int) [both are signals]
	 */
	void init(Qt::Orientation orientation = Qt::Horizontal);

public:
	QDoubleSpinBox *spinBox; ///< @brief the spin box
	/**
	 * @brief Classic constructor for the Designer - initialise the orientation using default value
	 *
	 * @see Delegates to init()
	 */
	DoubleBoxSlider(QWidget* parent = 0);

	/**
	 * @param orientation to use at creation time
	 * @param parent 
	 *
	 * @see Delegates to init()
	 */
    DoubleBoxSlider(Qt::Orientation orientation, QWidget* parent = 0);
    
    /**
     * @brief import the spinbox and slider that will compose the widget. Layout and orientation are not created.
     *
     * @param pSpin not 0
     * @param pSlide not 0
     * @param parent
     *
     * @see Delegates to init()
     */
    DoubleBoxSlider(QDoubleSpinBox *pSpin, DoubleSlider *pSlide, QWidget *parent = 0);

	/**
	 * Similar to QDoubleSpinBox::value() or QSlider::value()
	 * @see setValue()
	 */
    double value() const;

	/**
	 * Similar to QDoubleSpinBox::minimum() or DoubleSlider::minimumD()
	 * @see setMinimum() maximum() setMaximum()
	 */
    double minimum(void) const;

	/**
	 * Similar to QDoubleSpinBox::maximum() or DoubleSlider::maximumD()
	 * @see setMaximum() minimum() setMinimum()
	 */
    double maximum(void) const;

	/**
	 * Similar to QDoubleSpinBox::setMinimum() or Slider::setMinimumD()
	 * @see minimum() maximum() setMaximum()
	 */
    void setMinimum(double newMin);

	/**
	 * Similar to QDoubleSpinBox::setMaximum() or Slider::setMaximumD()
	 * @see maximum() minimum() setMinimum()
	 */
    void setMaximum(double newMax);

	/**
	 * Similar to QSlider::orientation()
	 * @see setOrientation() orientation()
	 */
	Qt::Orientation orientation(void) const;

	/**
	 * Similar to QDoubleSpinBox::precision()
	 * @see setDecimals()
	 */
	int decimals(void) const;

	/**
	 * Similar to QDoubleSpinBox::singleStep()
     * @see setSingleStep()
	 */
	double singleStep(void) const;

    bool wasImported(void) const { return imported; }

signals:
	/**
	 * @brief Emitted when spinBox's value changed
	 * @see init()
	 */
    void valueChanged(double);

public slots:
	/**
	 * Similar to QDoubleSpinBox::setValue() or DoubleSlider::setDoubleValue().
	 * @note Only the spinBox's value is changed (slider updates with signals).
	 */
    void setValue(double v);

	/**
	 * @brief Changes the orientation of the couple spinBox/slider.
	 *
	 * Horizontally the spin box is first (on the left) and the slider second.
	 *
	 * Vertically the slider is first (on the top) and the spin box second.
	 *
	 * Similar to QSlider::setOrientation()
     *
     * @note if the elements were imported the orientation cannot be changed
     *
	 * @see orientation()
	 */
    void setOrientation(Qt::Orientation orientation);

	/**
     * @brief Set how many decimals to use for interpretting doubles.
	 *
	 * Similar to QDoubleSpinBox::setPrecision()
	 *
	 * Changes the DoubleSlider's maximum integer value.
	 * @see decimals()
	 */
	void setDecimals(int precs);

	/**
	 * @brief Set the step value used when the user presses up or down on the spin box
	 *
	 * Similar to QDoubleSpinBox::setSingleStep()
	 *
	 * @see singleStep()
	 */
	void setSingleStep(double step);
};

} // namespace QtExtra

#endif // BOXSLIDER_HPP
