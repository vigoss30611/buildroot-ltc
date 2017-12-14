/**
******************************************************************************
 @file scalableview.cpp

 @brief Implementation of the ScalableView

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
#include "QtExtra/scalableview.hpp"

QtExtra::ScalableView::ScalableView(QWidget *parent): QGraphicsView(parent)
{
    ratio = Qt::KeepAspectRatio;

    bZoom = false;
    currentZoom = 1.0;
    minZoom = 1.0;
    maxZoom = 1.0;
    stepZoom = 1.0;
}

void QtExtra::ScalableView::resizeEvent(QResizeEvent *pEv)
{
    QGraphicsScene *pScene = scene();

    if ( pScene )
    {
        if ( !bZoom  )
        {
            fitInView(0, 0, pScene->width(), pScene->height(), ratio);
        }
        else
        {
            setTransformationAnchor(QGraphicsView::AnchorViewCenter);
            fitInView(0, 0, pScene->width(), pScene->height(), ratio);
            //ensureVisible(0, 0, pScene->width(), pScene->height(), ratio);
            scale(currentZoom, currentZoom);
        }
    }
}

void QtExtra::ScalableView::mousePressEvent(QMouseEvent *mevent)
{
    emit mousePressed(mevent->pos(), mevent->button());
}

void QtExtra::ScalableView::wheelEvent(QWheelEvent *wevent)
{
    double wantedZoom = currentZoom;
    
    if ( !bZoom )
    {
        return;
    }

    if ( wevent->delta() > 0 )
    {
        // zoom in
        wantedZoom+=stepZoom;
    }
    else
    {
        wantedZoom-=stepZoom;
    }

    if ( setZoom(wantedZoom) == EXIT_SUCCESS )
    {
        emit zoomChanged(currentZoom);
    }
}

int QtExtra::ScalableView::setZoom(double z)
{
    if ( z < minZoom )
    {
        z = minZoom;
    }
    if ( z > maxZoom )
    {
        z = maxZoom;
    }
    if ( bZoom )
    {
        setTransformationAnchor(QGraphicsView::NoAnchor);
        scale(z/currentZoom, z/currentZoom);
        currentZoom = z;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
