/**
******************************************************************************
@file textparser.cpp

@brief Implementation of TextParser

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
#include "QtExtra/textparser.hpp"

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QTableWidget>
#include <QMessageBox>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>

#include "ui/ui_dialog_textparser.h"

using namespace QtExtra;

/*
 * private
 */

void TextParser::init()
{
	base = new Ui::TextParser();
	base->setupUi(this);
}

/*
 * protected
 */

/*test string 5x2 with sep{ ;//;\n}
i am //so are you
me // yes you
*/

void TextParser::parse(const QString &text, const QStringList &separators, QStringList &output)
{
	QStringList tmp = text.split(separators.at(0));

	for ( int sep = 1 ; sep < separators.size() ; sep++ )
	{
		QStringList copy = tmp;
		QString separator = separators.at(sep);
		tmp.clear();

		for ( int str = 0 ; str < copy.size() ; str++ )
		{
			QString current = copy.at(str);
			tmp += current.split(separator);
		}
	}

	output += tmp;
}

/*
 * public
 */

TextParser::TextParser(unsigned int columns, unsigned int lines, const QString &info, const QString &title, QWidget *parent)
	: QDialog(parent), _columns(columns), _lines(lines)
{
	init();
	if ( !info.isEmpty() )
		base->_information->setText(info);
	if ( !title.isEmpty() )
		setWindowTitle(title);
}

void TextParser::setInformation(const QString &info)
{
	base->_information->setText(info);
}
		
QString TextParser::information() const
{
	return base->_information->text();
}

QStringList TextParser::separators() const
{
	QStringList sep;

	if ( base->sep_coma->isChecked() ) sep.push_back(",");
	if ( base->sep_line->isChecked() ) sep.push_back("\n");
	if ( base->sep_space->isChecked() ) sep.push_back(" ");
	if ( base->sep_tab->isChecked() ) sep.push_back("\t");
	if ( base->sep_other->isEnabled() ) sep.push_back(base->sep_other->text());

	return sep;
}

const QStringList& TextParser::results() const
{
	return _results;
}

int TextParser::numberResults() const
{
	return _results.size();
}

/*
 * public slots
 */

void TextParser::extract()
{
	int found = 0, expected = _columns*_lines;

	_results.clear();
	parse(base->source->toPlainText(), separators(), _results);
	found = _results.size();

	if ( found != expected )
	{
		if ( expected > found )
		{
			QMessageBox::warning(this, tr("Error"), tr("%n element(s) are missing from the list.", "", expected-found));
		}
		else
		{
			if ( QMessageBox::question(this, tr("Error"), 
				tr("%n element(s) are present but not expected in the list.\nDo you want to accept the list nonetheless?", "", found-expected),
				QMessageBox::Yes, QMessageBox::No
				) == QMessageBox::Yes )
			{
				accept();
			}
		}
	}
	else
	{
		accept();
	}
}

void TextParser::preview()
{
	QWidget *preview = NULL;
	QStringList tmpResults;
	int found = 0, expected = _columns*_lines;

	parse(base->source->toPlainText(), separators(), tmpResults);
	
	found = tmpResults.size();

	if ( found != expected )
	{
		QString message;
		if ( expected > found )
		{
			message = tr("%n element(s) are missing from the list.", "preview missing text", expected-found);
		}
		else
		{
			message = tr("%n element(s) are present but not expected in the list.", "preview missing text", found-expected);
		}

		if ( QMessageBox::question(this, tr("Warning"), message + tr("\nDo you still want to preview it?", "adding the preview missing on the line before"),
			QMessageBox::Yes, QMessageBox::No) == QMessageBox::No )
		{
			return;
		}
	}

	preview = new QWidget(this, Qt::Window);
	preview->setWindowTitle(this->windowTitle() + tr(" preview", "added to dialog window title when doing preview"));

	QVBoxLayout *previewLayout = new QVBoxLayout(preview);
	//QScrollArea *valueArea = new QScrollArea();
	//QGridLayout *valueLayout = new QGridLayout(valueArea);
	QTableWidget *valueArea = new QTableWidget(_lines, _columns, preview);
	
	valueArea->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	previewLayout->addWidget(new QLabel(tr("Expecting: %n columns ", "nb columns", _columns) + tr("%n lines", "nb lines", _lines))); // -1 extends to far right
	//previewLayout->addWidget(valueArea);
	previewLayout->addWidget(valueArea);
	
	for ( unsigned int j = 0 ; j < _lines ; j++ )
	{
		for ( unsigned int i = 0 ; i < _columns ; i++ )
		{
			int current = j*_columns +i;

			if ( current < tmpResults.size() )
			{
				//valueLayout->addWidget(new QLabel(tmpResults.at(current)), j+1, i);
				valueArea->setItem(j, i, new QTableWidgetItem(tmpResults.at(current)));
			}
		} // for i
	} // for j
	valueArea->resizeColumnsToContents();

	preview->show();
}

int TextParser::results(QList<double> &list) const
{
	int added = 0;

	for ( int i = 0 ; i < _results.size() ; i++ )
	{
		bool ok;
		double v = _results.at(i).toDouble(&ok);

		if ( ok )
		{
			list.push_back(v);
			added++;
		}
	}

	return added;
}
