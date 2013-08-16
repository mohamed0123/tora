
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2008 Numerous Other Contributors
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

#ifndef TROTL_TEST3_WINDOW_H
#define TROTL_TEST3_WINDOW_H

#include "core/tomainwindow.h"
//#include "core/toworksheet.h"
#include "core/todockbar.h"
#include "core/tobackgroundlabel.h"

#include <QtCore/QObject>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QMenuBar>
#include <QtCore/QSet>
#include <QtGui/QTextEdit>

class toWorkSpace;
class toConnection;

// thx to Martin Pejcoch
class XMdiSubWindow : public QMdiSubWindow
{
public:
    XMdiSubWindow( QWidget * parent = 0
                , Qt::WindowFlags flags = 0
                )
        : QMdiSubWindow( parent, flags )
    {
	    //showMaximized();
    }
    virtual void showEvent( QShowEvent * event )
    {
        QMdiSubWindow::showEvent( event );
        if( widget() )
            widget()->show();
    }
};

class Test3Window : public toMainWindow
{
	Q_OBJECT;

	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *exitAct;
	QAction *cutAct;
	QAction *copyAct;
	QAction *pasteAct;
	QAction *closeAct;
	QAction *closeAllAct;
	QAction *tileAct;
	QAction *cascadeAct;
	QAction *nextAct;
	QAction *previousAct;
	QAction *separatorAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *windowMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *toolsToolBar;

	toWorkSpace &Workspace;

	toDockbar *leftDockbar,*rightDockbar;
	
	QList<toConnection *> Connections;

	QAction * m_describeAction;
public:
	Test3Window(QString, QString, QString, QSet<QString>&);

	void createDockbars();
	void createDocklets();
	void moveDocklet(toDocklet *let, Qt::DockWidgetArea area);

	virtual void addCustomMenu(QMenu *) {};

	toConnection* addConnection(toConnection *conn)
	{
		Connections.insert(Connections.end(), conn);
		return conn;
	}

	virtual QList<toConnection*> const& connections(void) const
	{ 
		return Connections;
	};

	virtual toConnection& connection(const QString &) 
	{ 
		return *Connections.front(); 
	}

	virtual toConnection& currentConnection(void)
	{
		QList<toConnection *>::iterator i = Connections.begin();
		if(i != Connections.end())
			return *(*i);
		throw tr("Can't find active connection");
	}

	virtual toDockbar* dockbar(toDocklet *let);

private slots:
	void newFile();
	void addTool();
};


class MdiChild : public QTextEdit
{
	Q_OBJECT;
public:

	MdiChild()
	{
		setAttribute(Qt::WA_DeleteOnClose);
		//    isUntitled = true;
	}

	// void newFile();
	// bool loadFile(const QString &fileName);
	// bool save();
	// bool saveAs();
	// bool saveFile(const QString &fileName);
	// QString userFriendlyCurrentFile();
	// QString currentFile() { return curFile; }

protected:
	// void closeEvent(QCloseEvent *event);

private slots:
	//     void documentWasModified();

private:
	//     bool maybeSave();
	//     void setCurrentFile(const QString &fileName);
	//     QString strippedName(const QString &fullFileName);

	//     QString curFile;
	//     bool isUntitled;
};

#endif
