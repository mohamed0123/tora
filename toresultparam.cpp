//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 Underscore AB
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
 *      these libraries without written consent from Underscore AB. Observe
 *      that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

#include "utils.h"

#include "toconf.h"
#include "toresultparam.h"
#include "tosql.h"
#include "toresultview.h"
#include "tomemoeditor.h"
#include "toconnection.h"
#include "toresultlong.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtooltip.h>

#include "toresultparam.moc"

#include "icons/database.xpm"
#include "icons/filesave.xpm"
#include "icons/scansource.xpm"
#include "icons/tocurrent.xpm"
#include "icons/trash.xpm"

static toSQL SQLParams("toResultParam:ListParam",
		       "select name \"Parameter\",value \"Value\",' ' \"Changed\",\n"
		       "       description \"Description\", num \" Number\",\n"
		       "       type \" Type\",isdefault \" Default\",\n"
		       "       isses_modifiable \" Sesmod\",issys_modifiable \" Sysmod\"\n"
		       "  from v$parameter order by name",
		       "List parameters available in the database");

static toSQL SQLHiddenParams("toResultParam:ListHidden",
			     "SELECT KSPPINM \"Parameter\",\n"
			     "       KSPFTCTXVL \"Value\",\n"
			     "       ' ' \"Changed\",\n"
			     "       KSPPDESC \"Description\",\n"
			     "       KSPFTCTXPN \" Num\",\n"
			     "       ksppity \" Type\",\n"
			     "       KSPFTCTXDF \" Default\",\n"
			     "       DECODE(MOD(TRUNC(KSPPIFLG/256),2),0,'FALSE','TRUE') \" Sesmod\",\n"
			     "       DECODE(MOD(TRUNC(KSPPIFLG/65536),8),0,'FALSE','TRUE') \" Sysmod\"\n"
			     "  FROM X$KSPPI x,\n"
			     "       X$KSPPCV2 y\n"
			     " WHERE x.INDX+1=y.KSPFTCTXPN ORDER BY KSPPINM",
			     "List parameters available in the database including hidden parameters");

bool toResultParam::canHandle(toConnection &conn)
{
  return toIsOracle(conn);
}

toResultParam::toResultParam(QWidget *parent,const char *name)
  : QVBox(parent,name)
{
  QToolBar *toolbar=toAllocBar(this,"Parameter editor");
  Hidden=new QToolButton(toolbar);
  Hidden->setToggleButton(true);
  Hidden->setIconSet(QIconSet(QPixmap((const char **)scansource_xpm)),false);
  connect(Hidden,SIGNAL(toggled(bool)),this,SLOT(showHidden(bool)));
  QToolTip::add(Hidden,"Display hidden parameters. This will only word if you are logged in as the sys user.");
  toolbar->addSeparator();

  new QToolButton(QPixmap((const char **)filesave_xpm),
		  "Generate pfile",
		  "Generate pfile",
		  this,SLOT(generateFile()),toolbar);
  toolbar->addSeparator();
  new QToolButton(QPixmap((const char **)database_xpm),
		  "Apply changes to system",
		  "Apply changes to system",
		  this,SLOT(applySystem()),toolbar);
  new QToolButton(QPixmap((const char **)tocurrent_xpm),
		  "Apply changes to session",
		  "Apply changes to session",
		  this,SLOT(applySession()),toolbar);
  toolbar->addSeparator();
  new QToolButton(QPixmap((const char **)trash_xpm),
		  "Drop current changes",
		  "Drop current changes",
		  this,SLOT(dropChanges()),toolbar);
  toolbar->setStretchableWidget(new QLabel(toolbar));

  Params=new toResultLong(false,false,toQuery::Background,this);
  Params->setSQL(SQLParams);
  Params->setReadAll(true);
  Params->setSelectionMode(QListView::Single);
  connect(Params,SIGNAL(selectionChanged()),this,SLOT(changeItem()));
  connect(Params,SIGNAL(done()),this,SLOT(done()));
  Value=new QLineEdit(this);
  Value->setEnabled(false);
  LastItem=-1;
}

void toResultParam::showHidden(bool hid)
{
  if (hid)
    Params->setSQL(SQLHiddenParams);
  else
    Params->setSQL(SQLParams);
  refresh();
}

void toResultParam::query(const QString &sql,const toQList &param)
{
  saveChange();
  LastItem=-1;

  Params->refresh();
}

void toResultParam::dropChanges(void)
{
  NewValues.clear();
  refresh();
}

void toResultParam::done()
{
  for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
    std::map<int,QString>::iterator i=NewValues.find(item->text(4).toInt());
    if (i!=NewValues.end()) {
      item->setText(1,(*i).second);
      item->setText(6,"FALSE");
      item->setText(2,"Changed");
    }
  }
}

void toResultParam::saveChange()
{
  if(LastItem>=0&&LastValue!=Value->text()) {
    NewValues[LastItem]=Value->text();
    LastValue=Value->text();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(4).toInt()==LastItem) {
	item->setText(1,LastValue);
	item->setText(6,"FALSE");
	item->setText(2,"Changed");
	break;
      }
    }
  }
}

void toResultParam::generateFile(void)
{
  saveChange();
  QString str=QString("# Generated by TOra version %1\n\n").arg(TOVERSION);
  QRegExp comma("\\s*,\\s+");
  for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
    if (item->text(6)=="FALSE") {
      str+=item->text(0);
      str+=" = ";
      if(item->text(5)=="2") {
	QStringList lst=QStringList::split(comma,item->text(1));
	if (lst.count()>1)
	  str+="( ";
	for(unsigned int i=0;i<lst.count();i++) {
	  if (i>0)
	    str+=", ";
	  str+="\""+lst[i]+"\"";
	}
	if (lst.count()>1)
	  str+=" )";
      } else
	str+=item->text(1);
      str+="\n";
    }
  }
  connect(new toMemoEditor(this,str,0,0),SIGNAL(changeData(int,int,const QString &)),
	  this,SLOT(changedData(int,int,const QString &)));
}

void toResultParam::applySession(void)
{
  try {
    saveChange();
    toConnection &conn=connection();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(2)=="Changed") {
	try {
	  if (item->text(7)!="FALSE") {
	    QString str="ALTER SESSION SET ";
	    str+=item->text(0);
	    str+=" = ";
	    if (item->text(5)=="2") {
	      str+="'";
	      str+=item->text(1);
	      str+="'";
	    } else
	      str+=item->text(1);
	    conn.allExecute(str);
	    std::map<int,QString>::iterator i=NewValues.find(item->text(4).toInt());
	    if (i!=NewValues.end())
	      NewValues.erase(i);
	  }
	} TOCATCH
	}
    }
  } TOCATCH
  refresh();
}

void toResultParam::applySystem(void)
{
  try {
    saveChange();
    toConnection &conn=connection();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(2)=="Changed") {
	try {
	  if (item->text(8)!="FALSE") {
	    QString str="ALTER SYSTEM SET ";
	    str+=item->text(0);
	    str+=" = ";
	    if (item->text(5)=="2") {
	      str+="'";
	      str+=item->text(1);
	      str+="'";
	    } else
	      str+=item->text(1);
	    conn.execute(str);
	    std::map<int,QString>::iterator i=NewValues.find(item->text(4).toInt());
	    if (i!=NewValues.end())
	      NewValues.erase(i);
	  }
	} TOCATCH
      }
    }
  } TOCATCH
  refresh();
}

void toResultParam::changeItem(void)
{
  saveChange();

  QListViewItem *item=Params->selectedItem();
  if (item) {
    LastItem=item->text(4).toInt();
    LastValue=item->text(1);
    Value->setText(LastValue);
    Value->setEnabled(true);
  } else {
    LastItem=-1;
    Value->setEnabled(false);
  }
}

void toResultParam::changedData(int row,int col,const QString &data)
{
  QString file=toSaveFilename(QString::null,"*.pfile",this);
  if (!file.isEmpty())
    toWriteFile(file,data);
}
