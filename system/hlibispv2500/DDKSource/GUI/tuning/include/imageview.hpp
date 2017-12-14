/**
******************************************************************************
@file imageview.hpp

@brief A widget to display images from CVBuffer

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
#ifndef IMAGEVIEW_HPP
#define IMAGEVIEW_HPP

#include "ui/ui_imageview.h"

#include <QMainWindow>
#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QGraphicsScene>

#include "buffer.hpp"
namespace QtExtra {
    class ScalableView;
}//#include <QtExtra/scalableview.hpp>

class QContextMenuEvent;
class QCloseEvent;
class QMenu;
class QAction;

class QwtPlot;

/**
 * @note context menu policy is enable as Qt::DefaultContextMenu to enable the context menu - use Qt::PreventContextMenu or Qt::NoContextMenu to avoid triggering the menu
 */
class ImageView: public QMainWindow, public Ui::ImageView
{
    Q_OBJECT
private:
    CVBuffer sBuffer;

    // image part
    QGraphicsScene *pScene;
    QGraphicsPixmapItem *pPixmap; // to know if the image was already converted (used in clearButImage()) - do not delete

    // histogram part
    QwtPlot *histogramsPlots[4];
    QWidget *pHistTab;

    void init();
    
protected:
	QAction *saveAsJPG_act;
    QAction *saveAsFLX_act;
    QAction *saveAsYUV_act;

    virtual int loadQImage(QImage *pImage);
    virtual void computeHistograms();

	virtual void contextMenuEvent(QContextMenuEvent* event);
    virtual void closeEvent(QCloseEvent *event);

signals:
    void closed();

public:
	bool genHists; // If true will compute histograms in loadBuffer()

    ImageView(QWidget *parent = 0);
    ImageView(CVBuffer *pBuff, QWidget *parent = 0);
    ~ImageView();

    virtual int loadBuffer(CVBuffer sBuff, bool gammaCorrect=false);
    virtual const CVBuffer& getBuffer() const { return sBuffer; };

    void addGrid(const std::vector<cv::Rect> &coord, const QPen &pen = QPen(Qt::white), const QBrush &brush = QBrush());
    void addGrid(cv::Rect *coords, unsigned w, unsigned h, const QPen &pen = QPen(Qt::white), const QBrush &brush = QBrush());
	void markPoints(QList<QPointF> points, const QPen &pen = QPen(Qt::red), const QBrush &brush = QBrush());
	void clearImage();
    void showStatistics(bool show = true);

    QGraphicsScene* getScene();
    virtual int clearButImage(); // cleans the scene but re-applies the buffer if available

private slots:
    void mousePressedSlot(QPointF p, Qt::MouseButton button);
    void mouseReleasedSlot(qreal x, qreal y, Qt::MouseButton button);

public slots:
    virtual void saveBuffer(const QString &filename=QString()); // save displayed buffer as YUV or FLX according to format
    virtual void saveDisplay(const QString &filename=QString()); // saved displayed QImage as JPG
    virtual void retranslate();

signals:
    void mousePressed(qreal x, qreal y, Qt::MouseButton button);
    void mouseReleased(qreal x, qreal y, Qt::MouseButton button);
};

#endif // IMAGEVIEW_HPP
