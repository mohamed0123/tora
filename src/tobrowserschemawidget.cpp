
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

#include <QCoreApplication>

#include "tobrowserschemawidget.h"
#include "tocodemodel.h"
#include "utils.h"
#include "toconfiguration.h"
#include "tocurrent.h"

toBrowserSchemaTableView::toBrowserSchemaTableView(QWidget * parent, const QString &type)
    : toResultTableView(true, false, parent),
      toBrowserSchemaBase()
{
    ObjectType = type;
    ForceRequery = false;
    if (!type.isEmpty())
        connect(this, SIGNAL(done()), this, SLOT(updateCache()));
}

QString toBrowserSchemaTableView::objectName()
{
    return selectedIndex(1).data(Qt::EditRole).toString();
}

void toBrowserSchemaTableView::changeParams(const QString & schema, const QString & filter)
{
    // If all objects of a specific type are to be displayed, try displaying them from the cache
    // This cannot work for filtered objects yet as we do not want to re-impelement the "like" operator.
    if (filter == "%" &&
        !ObjectType.isEmpty() &&
        !ForceRequery
        /* && toCurrentConnection(this).cacheAvailable(false)*/)
    {
        /* NOTE: that directories are not owned by any individual schema. Therefore they are
         * always stored under schema "SYS" in Oracle data dictionary. This is why in case
         * of directories we're not specifying schema name when checking the cache.
         */
        QString sch;
        if (ObjectType == "DIRECTORY")
            sch = "%";
        else
            sch = schema;
        if (toCurrentConnection(this).Cache->objectExists(sch, "TORA_LIST", ObjectType))
        {
            this->queryFromCache(sch, ObjectType);
            return;
        }
    }

    ForceRequery = false;
    Schema = schema;
    toResultTableView::changeParams(schema, filter);
}

void toBrowserSchemaTableView::updateCache(void)
{
    // Update list of objects
    QList<toCache::objectName> rows;
    toCache::objectName obj;
    obj.Owner = Schema;
    obj.Type = ObjectType;
    // TODO: Check that result model rows are NOT sorted in descending order as that would break updating of cache!!!
    toResultModel::RowList modelRows = this->Model->getRawData();
    for (QList<toResultModel::Row>::iterator i = modelRows.begin(); i != modelRows.end(); i++)
    {
        obj.Name = (*i)[1];
        rows.append(obj);
    }
    // NOTE: Oracle directories do not belong to any particular schema.
    //       Therefore they are saved as belonging to SYS schema.
    if (ObjectType == "DIRECTORY")
        Schema = "SYS";
    toCurrentConnection(this).Cache->updateObjects(Schema, ObjectType, rows);

    // Update information when list of this type of objects in this schema was updated
    // NOTE: Type is placed in the name field in order not to
    // mix this meta-information with actual list of objects.
    obj.Owner = Schema;
    obj.Type = "TORA_LIST";
    obj.Name = ObjectType;
    toCurrentConnection(this).Cache->addIfNotExists(obj);
} // updateCache

toBrowserSchemaCodeBrowser::toBrowserSchemaCodeBrowser(QWidget * parent)
    : QTreeView(parent),
      toBrowserSchemaBase()
{
    m_model = new toCodeModel(this);
    setModel(m_model);

    setAlternatingRowColors(true);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
             m_model, SLOT(addChildContent(const QModelIndex &)));
    connect(m_model, SIGNAL(dataReady()), this, SLOT(expandAll()));
}

QString toBrowserSchemaCodeBrowser::objectName()
{
    toCodeModelItem *item = static_cast<toCodeModelItem*>(currentIndex().internalPointer());

    if (!item || item->type() == "NULL")
        return "";

    if (item->type() == "PACKAGE"
        || item->type() == "FUNCTION"
        || item->type() == "MACRO"
        || item->type() == "PROCEDURE"
        || item->type() == "TYPE")
    {
        return item->display();
    }

    if (item->type() == "SPEC" || item->type() == "BODY")
        return item->parent()->display();

    return "";
}

QString toBrowserSchemaCodeBrowser::objectType()
{
    toCodeModelItem *item = static_cast<toCodeModelItem*>(currentIndex().internalPointer());
    if (item && item->parent())
    {
        return item->type();
    }
    return "";
}

void toBrowserSchemaCodeBrowser::changeParams(const QString & schema, const QString & filter)
{
    // WARNING: QCoreApplication::processEvents() is mandatory here
    //          to prevent random crashing on parent-tab widget switching
    //          in toBrowser. Dunno why...
    QCoreApplication::processEvents();
    m_model->refresh(toCurrentConnection(this), schema);
}
