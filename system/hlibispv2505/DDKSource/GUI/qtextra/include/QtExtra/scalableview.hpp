/**
******************************************************************************
 @file scalableview.hpp

 @brief A QGraphicsView that scales with the wheel and according to its resize event

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
#ifndef SCALABLE_VIEW_HPP
#define SCALABLE_VIEW_HPP

#include <QGraphicsView>
#include <QMouseEvent>
#include <cstdio>

namespace QtExtra
{

/**
 * @brief A clickable sceen that also allow zooming and resizing according to widget's size
 */
class ScalableView: public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * @brief create a scalable view with zooming disabled and Qt::KeepAspectRatio
     */
    ScalableView(QWidget *parent=0);

    Qt::AspectRatioMode ratio; ///< @brief keeps aspect ratio or not when zoon is off

    bool bZoom; ///< @brief Enabled zooming (use scale istead of fit in view when resizing) - modify after creation
    double currentZoom; ///< @brief Current zoom value - modify with setZoom()
    double minZoom; ///< @brief minimum possible zoom - modify after creation
    double maxZoom; ///< @brief maximum possible zoom - modify after creation
    double stepZoom; ///< @brief zoom step when a wheel event is captured - modify after creation

signals:
    void mousePressed(QPointF pos, Qt::MouseButton btn);
    void zoomChanged(double z);

protected:
    virtual void resizeEvent(QResizeEvent *pEv);

    virtual void mousePressEvent(QMouseEvent *mevent);

    virtual void wheelEvent(QWheelEvent *wevent);

public slots:
    virtual int setZoom(double z);
};

} // namespace

#endif // SCALABLE_VIEW_HPP
