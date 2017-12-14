/**
******************************************************************************
 @file filewidget.cpp

 @brief FileWidget class implentation

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
#include "QtExtra/filewidget.hpp"

#include <QWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QMessageBox>

#include <QtCore/QString>

using namespace QtExtra;

// private
void FileWidget::init()
{
	QHBoxLayout *pLayout = new QHBoxLayout(this);

	pLineEdit = new QLineEdit();
	pLineEdit->setReadOnly(true);
	pLayout->addWidget(pLineEdit);

	pButton = new QPushButton();
	pLayout->addWidget(pButton);

	pLayout->setMargin(0);

	// connect
	connect( pButton, SIGNAL(clicked()), this, SLOT(searchFile()) );

	retranslate();
}

// protected

// public
FileWidget::FileWidget(QWidget *parent): QWidget(parent)
{
	init();
}

FileWidget::FileWidget(QFileDialog::FileMode mode, const QString& fileFilter, QWidget *parent): QWidget(parent)
{
	init();
	setFileMode(mode);
	setFileFilter(fileFilter);
}

QFileDialog::FileMode FileWidget::fileMode() const
{
	return _fileMode;
}

void FileWidget::setFileMode(QFileDialog::FileMode mode)
{
	_fileMode = mode;
}

QString FileWidget::fileFilter() const
{
	return _fileFilter;
}

void FileWidget::setFileFilter(const QString& fileFilter)
{
	_fileFilter = fileFilter;
}

QString FileWidget::filename() const
{
	return pLineEdit->text();
}

QString FileWidget::absolutePath() const
{
#ifndef QT_NO_TOOLTIP
	return pLineEdit->toolTip();
#else
	return _absolutePath;
#endif
}

// public slots

QString FileWidget::searchFile()
{
	QString result;
	switch(_fileMode)
	{
	case QFileDialog::ExistingFile:
		result = QFileDialog::getOpenFileName(this, tr("Open file"), "", _fileFilter);
		break;
	case QFileDialog::AnyFile:
		result = QFileDialog::getSaveFileName(this, tr("Save file"), "", _fileFilter);
		break;
	case QFileDialog::Directory:
	case QFileDialog::DirectoryOnly:
		result = QFileDialog::getExistingDirectory(this, tr("Open directory"), "");
		break;
	default:
		QMessageBox::critical(this, tr("Error"), tr("Unknown file mode"));
	}
	if ( !result.isEmpty() )
	{
		forceFile(result);
		emit fileChanged(result);
	}

	return result;
}

void FileWidget::clearFile()
{
    pLineEdit->setText("");
#ifndef QT_NO_TOOLTIP
	pLineEdit->setToolTip("");
#else
	_absolutePath = "";
#endif
}

bool FileWidget::forceFile(const QString& path)
{
	QFile file(path); /// @ qfile does not give the filename if setup like that - change to use a QString::split("/")

	if ( _fileMode == QFileDialog::AnyFile || file.exists() )
	{
		QString filename = file.fileName();

		pLineEdit->setText(filename);

#ifndef QT_NO_TOOLTIP
		pLineEdit->setToolTip(path);
#else
		_absolutePath = path;
#endif
		return true;
	}
	return false;
}

void FileWidget::retranslate()
{
	pButton->setText(tr("Browse"));
}

 // end of namespace QtExtra
