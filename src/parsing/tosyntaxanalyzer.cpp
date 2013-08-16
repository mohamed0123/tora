
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2009 Numerous Other Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 *      Specifically you are not permitted to link this program with the
 *      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
 *      And you are not permitted to distribute binaries compiled against
 *      these libraries.
 *
 *      You may link this product with any GPL'd Qt library.
 *
 * All trademarks belong to their respective owners.
 *

 * END_COMMON_COPYRIGHT_HEADER */

#include "parsing/tosyntaxanalyzer.h"

#include "editor/tohighlightedtext.h"

#include <QtCore/QString>
#include <QtCore/QRegExp>

#include <Qsci/qscilexer.h>
#include <Qsci/qscilexersql.h>

toSyntaxAnalyzer::statement::statement()
	: lineFrom(0)
	, lineTo(0)
	, posFrom(0)
	, posTo(0)
	, sql()
	, firstWord()
	, statementType(UNKNOWN) {};
toSyntaxAnalyzer::statement::statement(unsigned lf, unsigned lt)
	: lineFrom(lf)
	, lineTo(lt)
	, posFrom(0)
	, posTo(0)
	, sql()
	, firstWord()
	, statementType(UNKNOWN) {};
toSyntaxAnalyzer::statement& toSyntaxAnalyzer::statement::operator=(toSyntaxAnalyzer::statement const& other)
{
	lineFrom = other.lineFrom;
	lineTo = other.lineTo;
	posFrom = other.posFrom;
	posTo = other.posTo;
	firstWord = other.firstWord;
	sql = other.sql;
	statementType = other.statementType;
	return *this;
}

toSyntaxAnalyzer::toSyntaxAnalyzer(toHighlightedText *parent)
	: QObject(parent)
{}

toSyntaxAnalyzer::~toSyntaxAnalyzer()
{
}
