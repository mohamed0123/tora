/*****
*
* TOra - An Oracle Toolkit for DBA's and developers
* Copyright (C) 2003-2005 Quest Software, Inc
* Portions Copyright (C) 2005 Other Contributors
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
*      these libraries without written consent from Quest Software, Inc.
*      Observe that this does not disallow linking to the Qt Free Edition.
*
*      You may link this product with any GPL'd Qt library such as Qt/Free
*
* All trademarks belong to their respective owners.
*
*****/

#ifndef TOWORKSHEETSTATISTIC_H
#define TOWORKSHEETSTATISTIC_H

#include "config.h"

#include <list>
#include <map>

#include <qsplitter.h>
#include <qvbox.h>

class QLabel;
class QPopupMenu;
class QSplitter;
class toAnalyze;
class toBarChart;
class toListView;
class toWorksheetStatistic;

class toHideSplitter : public QSplitter
{
    Q_OBJECT
    toWorksheetStatistic *StatList;
public:
    toHideSplitter(QSplitter::Orientation o, QWidget *parent, toWorksheetStatistic *statlist)
            : QSplitter(o, parent), StatList(statlist)
    { }
public slots:
    void setHidden(bool hid);
};

class toWorksheetStatistic : public QVBox
{
    Q_OBJECT

    struct data
    {
        QVBox *Top;
        QLabel *Label;
        QSplitter *Charts;
        toListView *Statistics;
        toBarChart *Wait;
        toBarChart *IO;
        toListView *Plan;
    };

    std::list<data> Open;

    QPopupMenu *SaveMenu;
    QPopupMenu *RemoveMenu;

    static toAnalyze *Widget;
    toAnalyze *Tool;
    QSplitter *Splitter;
    QWidget *Dummy;

    QToolButton *ShowPlans;
    QToolButton *ShowCharts;
public:
    toWorksheetStatistic(QWidget *parent);
    ~toWorksheetStatistic();

    static void saveStatistics(std::map<QCString, QString> &stats);

    void addStatistics(std::map<QCString, QString> &stats);

    void updateSplitter(void);
public slots:

    virtual void showPlans(bool);
    virtual void showCharts(bool);

    virtual void save(int);
    virtual void remove
        (int);
    virtual void load(void);
    virtual void displayMenu(void);
};

#endif