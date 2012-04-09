/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtAddOn.JsonDb module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef JSONDB_INDEX_H
#define JSONDB_INDEX_H

#include <QObject>
#include <QJSEngine>
#include <QPointer>
#include <QStringList>

#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>

#include "jsondbobject.h"

#include "jsondbpartitionglobal.h"
#include "jsondbobjectkey.h"
#include "jsondbmanagedbtreetxn.h"
#include "jsondbcollator.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_JSONDB_PARTITION

class JsonDbManagedBtree;
class JsonDbPartition;
class JsonDbObjectTable;

class Q_JSONDB_PARTITION_EXPORT JsonDbIndex : public QObject
{
    Q_OBJECT
public:
    JsonDbIndex(const QString &fileName, const QString &indexName, const QString &propertyName,
                const QString &propertyType, const QStringList &objectType, const QString &locale, const QString &collation,
                const QString &casePreference, Qt::CaseSensitivity caseSensitivity,
                JsonDbObjectTable *objectTable);
    ~JsonDbIndex();

    QString propertyName() const { return mPropertyName; }
    QStringList fieldPath() const { return mPath; }
    QString propertyType() const { return mPropertyType; }
    QStringList objectType() const { return mObjectType; }

    JsonDbManagedBtree *bdb();

    bool setPropertyFunction(const QString &propertyFunction);
    void indexObject(const ObjectKey &objectKey, JsonDbObject &object, quint32 stateNumber);
    void deindexObject(const ObjectKey &objectKey, JsonDbObject &object, quint32 stateNumber);
    QList<QJsonValue> indexValues(JsonDbObject &object);

    quint32 stateNumber() const;

    JsonDbManagedBtreeTxn begin();
    bool commit(quint32);
    bool abort();
    bool clearData();

    void checkIndex();
    void setCacheSize(quint32 cacheSize);
    bool open();
    void close();
    bool exists() const;

    static bool validateIndex(const JsonDbObject &newIndex, const JsonDbObject &oldIndex, QString &message);
    static QString determineName(const JsonDbObject &index);

private:
    QJsonValue indexValue(const QJsonValue &v);

private slots:
    void propertyValueEmitted(QJSValue);

private:
    JsonDbObjectTable *mObjectTable;
    QString mFileName;
    QString mIndexName;
    QString mPropertyName;
    QStringList mPath;
    QString mPropertyType;
    QStringList mObjectType;
    QString mLocale;
    QString mCollation;
    QString mCasePreference;
    Qt::CaseSensitivity mCaseSensitivity;
#ifndef NO_COLLATION_SUPPORT
    JsonDbCollator mCollator;
#endif
    quint32 mStateNumber;
    QScopedPointer<JsonDbManagedBtree> mBdb;
    QJSEngine *mScriptEngine;
    QJSValue   mPropertyFunction;
    QList<QJsonValue> mFieldValues;
    JsonDbManagedBtreeTxn mWriteTxn;
    quint32 mCacheSize;
};

class IndexSpec {
public:
    QString name;
    QString propertyName;
    QStringList path;
    QString propertyType;
    QString locale;
    QString collation;
    QString casePreference;
    Qt::CaseSensitivity caseSensitivity;
    QStringList objectType;
    bool    lazy;
    QPointer<JsonDbIndex> index;
};

QT_END_NAMESPACE_JSONDB_PARTITION

QT_END_HEADER

#endif // JSONDB_INDEX_H