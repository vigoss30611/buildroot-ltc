/**
******************************************************************************
 @file filewidget.hpp

 @brief FileWidget class definitions

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
#include <QWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>

#include <QtCore/QString>

#ifndef FILEWIDGET_HPP
#define FILEWIDGET_HPP

namespace QtExtra
{

class FileWidget: public QWidget
{
	Q_OBJECT
private:
	QPushButton *pButton;
	QLineEdit *pLineEdit;

	QFileDialog::FileMode _fileMode;
	QString _fileFilter;
#ifdef QT_NO_TOOLTIP
	QString _absolutePath;
#endif

	void init();

protected:
	
public:
	FileWidget(QWidget *parent = 0);
	FileWidget(QFileDialog::FileMode mode, const QString& fileFilter, QWidget *parent = 0);

	QFileDialog::FileMode fileMode() const;
	/**
	 * @param mode QFileDialog::AnyFile to save a file, QFileDialog::ExistingFile to load a file, QFileDialog::Directory (or DirectoryOnly) to open existing directory.
	 */
	void setFileMode(QFileDialog::FileMode mode);

	QString fileFilter() const;
	void setFileFilter(const QString& fileFilter);

	QString absolutePath() const;
	QString filename() const;

    QPushButton* button() { return pButton; }

public slots:
	QString searchFile();
	bool forceFile(const QString& path); // does not emit
    void clearFile(); // remove the current file

	void retranslate();

signals:
	void fileChanged(QString newPath);

};

} // end of namespace QtExtra

#endif // FILEWIDGET_HPP
