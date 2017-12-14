/**
******************************************************************************
 @file exception.hpp

 @brief Exception class definitions

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
#ifndef EXCEPTION
#define EXCEPTION

#include <QtCore/QString>
#include <QWidget>
#include <ostream>

/**
 * @brief Additional classes to the Qt Library
 */
namespace QtExtra {

	/**
	 * @brief An exception object capable of storing complex information about an error. Can be displayed on standard streams or QMessageBox.
	 *
	 * A second exception can be attached to it, and will be displayed "on cascade".
	 */
	class Exception
	{
	public:
	  static QString defTitleError; ///< @brief Default title for an error.
	  static QString defTitleWarning; ///< @brief Default title for a warning.

	private:
	  QString _title;
	  QString _message; 
	  QString _details; ///< @brief Extra information about the exception.

	  Exception* attached; ///< @brief Attached exception that will be displayed "on cascade"

	  bool _warning; ///< Is the execption non-fatal?

	  /**
	   * @brief Initialise the defaults.
	   *
	   * If _title is empty then the defTitleError or defTitleWarning is used according to _warning.
	   *
	   * attached is set to NULL.
	   */
	  void init(void);
	  
	public:
	  /**
	   * @brief Construct an error Exception.
	   * @see Delegates to init()
	   */
	  Exception(const QString& message, const QString& details = "", bool warning = false, const QString& title = "");

	  /**
	   * @brief Constrcut a single exception object out of several warning messages.
	   *
	   * Creates a message by adding each warning to one line (similar for the details).
	   * @see Delegates to init()
	   */
	  Exception(const QStringList& warningsTitles, const QStringList& warnings, const QString& title = "");

	  /**
	   * @brief Copy an exception.
	   *
	   * @note Usually copy constructors are using const references...
	   *
	   * Copy the title, message, details and warning from the ori Exception
	   *
	   * @see Delegates to init()
	   */
	  Exception(const Exception* ori);

	  /**
	   * @note Also deletes the attached Exception.
	   */
	  virtual ~Exception();

	  /**
	   * @note If an Exception is already attached it is deleted.
	   */
	  void attach(Exception* neo);

	  /**
	   * Uses QMessageBox::Warning or QMessageBox::Critical depending on the warning flag.
	   */
	  virtual void displayQMessageBox(QWidget* parent = 0);
	  virtual void displayOn(std::ostream& os);

	  bool warning(void) const { return _warning; }
	  QString message(void) const { return _message; }
	  QString details(void) const { return _details; }
	};

} // QtExtra end of namespace

#endif
