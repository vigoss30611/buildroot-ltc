/**
******************************************************************************
 @file textparser.hpp

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

#include <QDialog>
#include <QWidget>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>

#ifndef TEXTPARSER_HPP
#define TEXTPARSER_HPP

namespace Ui
{
	class TextParser;
}

namespace QtExtra
{
	class TextParser: public QDialog
	{
		Q_OBJECT
	private:
		void init();

	protected:
		Ui::TextParser *base;
		QStringList _results;
		unsigned int _columns;
		unsigned int _lines;

	public:
		static void parse(const QString &text, const QStringList &separator, QStringList &output);

		/**
		 * @param columns number of expected colums
		 * @param lines number of expected lines
		 */
		TextParser(unsigned int columns, unsigned int lines, const QString &info = "", const QString &title = "", QWidget *parent = 0);

		void setInformation(const QString &info);
		
		QString information() const;
		QStringList separators() const;
		
		unsigned int columns() const { return _columns; }
		void setExpectedColumns(unsigned int columns) { _columns = columns; }

		unsigned int lines() const { return _lines; }
		void setExpectedLines(unsigned int lines) { _lines = lines; }

		const QStringList& results() const;
		int numberResults() const;

		/**
		 * @brief Tries to convert every string as a double
		 *
		 * @return the number of values added to the list
		 */
		int results(QList<double> &list) const;

	public slots:
		void extract();
		void preview();
	};
}

#endif // TEXTPARSER_HPP
