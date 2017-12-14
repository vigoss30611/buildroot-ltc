/**
******************************************************************************
 @file exception.cpp

 @brief Exception class implementation

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
#include "QtExtra/exception.hpp"

#include <QMessageBox>
#include <QtCore/QString>

#include <ostream>
#include "QtExtra/ostream.hpp"

using namespace QtExtra;

QString Exception::defTitleError = QObject::tr("Warning", "default exception message title for warning");
QString Exception::defTitleWarning = QObject::tr("Error", "default exception message title for error");

void Exception::init(void)
{
  if ( _title.isEmpty() )
    {
      if ( _warning )
        _title = defTitleError;
      else
        _title = defTitleWarning;
    }

  attached = 0;
}

Exception::Exception(const QString& message, const QString& details, bool warning, const QString& title)
	: _title(title), _message(message), _details(details), _warning(warning)
{
  init();
}

Exception::Exception(const QStringList& warningsTitles, const QStringList& warningsDetails, const QString& title)
	: _title(title)
{
  _warning = true;

  for ( int i = 0 ; i < warningsTitles.size() && i < warningsDetails.size() ; i++ )
    {
      _message += QString::number(i+1) + ": " + warningsTitles.at(i) + "\n";
      if ( ! warningsDetails.at(i).isEmpty() )
        _details += QString::number(i+1) + ": " + warningsDetails.at(i) + "\n";
    }

  init();
}

Exception::Exception(const Exception* ori)
{
  init();
  if ( ori )
    {
      _title = ori->_title;
      _message = ori->_message;
      _details = ori->_details;
      _warning = ori->_warning;
    }
}

Exception::~Exception()
{
  if ( attached )
    delete attached;
}

void Exception::attach(Exception* neo)
{
  if ( attached )
    delete attached;
  attached = neo;
}

void Exception::displayQMessageBox(QWidget* parent)
{
  QMessageBox* box;

  if ( _warning )
    box = new QMessageBox(QMessageBox::Warning, _title, _message, QMessageBox::Ok, parent);
  else
    box = new QMessageBox(QMessageBox::Critical, _title, _message, QMessageBox::Ok, parent);

  if ( ! _details.isEmpty() )
    {
      box->setDetailedText(_details);
    }

  box->exec();

  if ( attached )
    attached->displayQMessageBox(parent);
}

void Exception::displayOn(std::ostream& os)
{
  os << _title << std::endl;
  os << "----" << std::endl;
  os << _message << std::endl;

  if ( ! _details.isEmpty() )
    {
      os << "----" << std::endl;
      os << _details << std::endl;
    }

  if ( attached )
    attached->displayOn(os);
}
