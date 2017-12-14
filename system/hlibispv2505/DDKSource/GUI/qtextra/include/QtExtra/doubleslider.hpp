/**
******************************************************************************
 @file doubleslider.hpp

 @brief DoubleSlider class definitions

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
#ifndef DOUBLESLIDER_HPP
#define DOUBLESLIDER_HPP

#include <QWidget>
#include <QSlider>

namespace QtExtra {

/**
 * @brief Slider that can handle double in its basic signals and slots
 *
 * The convertion from integer to double and vice versa is done by resolving the interval equivalence.
 * The integer interval (i.e. the one stored in QSlider as the slider's position, minimum and maximum) and the double interval (the one stored in this class) are considered equivalent.
 *
 * The convertion from the integer to the double is solved by the equation: <i>double = C1*integer+C2</i>.
 * Finding C1 and C2 is done solving the system <i>(A) minD = C1*minI+C2 && (B) maxD = C1*maxI+C2</i>.
 * <i>C1 = (maxD-minD)/(maxI-minI)</i> and <i>C2 = minD - C1*minI</i>.
 *
 * The transformation equations are <i>integer = (1/C1)*(double-minD)+minI</i> and <i>double = C1*(integer-minI)+minD</i>.
 */
class DoubleSlider: public QSlider
{
    Q_OBJECT
private:
    /**
     * @brief Initialise the components and links singals/slots
     */
    void init();

	double _mind; ///< @brief stored value of the mininumD property
	double _maxd; ///< @brief sotred value of the maximumD property

protected:
    
	/**
	 * @property DoubleSlider::minimumD
     * @brief The minimum value of the double interval
     *
     * Similar to QSlider::minimum property.
	 *
	 * @note Should _mind be adjusted if necessary to ensure the range remains valid (_mind=_maxd-1.0)?
     * But the slider's value is not changed (it is stored as int in the QSlider).
	 *
	 * @see minimumD() setMinimumD()
     */
	Q_PROPERTY(double minimumD READ minimumD WRITE setMinimumD);
	

    /**
	 * @property DoubleSlider::maximumD
     * @brief The maximum value of the double interval
     *
     * Similar to QSlider::maximum property.
	 *
	 * @note Should _maxd be adjusted if necessary to ensure the range remains valid (_mind=_maxd-1.0)?
     * But the slider's value is not changed (it is stored as int in the QSlider).
	 *
	 * @see maximumD() setMaximumD()
     */
	Q_PROPERTY(double maximumD READ maximumD WRITE setMaximumD);

    /**
     * @brief Converts the double to an integer
     *
     * The convertion is taking into account the "integer interval" and the "double interval".
     *
     * e.g. 0 to 1000 <=> 0.5 to 1.0, double value 0.75 is equivalent to the value 500 on the slider.
     *
     * @see fromInt()
     */
    int fromDouble(double v) const;

    /**
     * @brief Converts the integer to a double
     *
     * The convertion is taking into account the "integer interval" and the "double interval".
     *
     * e.g. 0 to 1000 <=> 1.0 to 1.8, integer value 750 on the slider is equivalent to the value 1.6
     *
     * @see fromDouble()
     */
    double fromInt(int v) const;

public:
    DoubleSlider(QWidget *parent=0);
    DoubleSlider(double minimum, double maximum, QWidget *parent = 0);

    /**
     * @brief Get the current value of the slider as a double
     *
     * Similar to QSlider::value()
     *
     * @see Delegates to fromInt() to convert the slider's value.
     */
    double doubleValue(void) const;

	void setMaximumD(double newMax);
    double maximumD(void) const { return _maxd; }

	void setMaximum(int newMax);

	void setMinimumD(double newMin);
    double minimumD(void) const { return _mind; }

	void setMinimum(int newMin);

    signals:
        /**
         * @brief Emited when the value changed
         *
         * Similar to QSlider::valueChanged(int)
         *
         * @see Emited using reemitValueChanged()
         */
        void doubleValueChanged(double newValue);

    public slots:
        /**
         * @brief Change the slider's value
         *
         * Similar to QSlider::setValue(int)
         *
         * @see Delegate to fromDouble() to convert the value to int and QSlider::setValue() to change the slider's position.
         */
        void setDoubleValue(double newValue);

    private slots:
        /**
         * @brief Re-emit the caught QSlider::valueChanged(int) as a valueChanged(double)
         */
        void reemitValueChanged(int newValue);
};

} // namespace

#endif // DOUBLESLIDER_HPP
