/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef JSONDB_CLIENT_H
#define JSONDB_CLIENT_H

#include <QObject>
#include <QVariant>
#include <QList>
#include <QStringList>
#include <QScopedPointer>
#include <QUuid>

#include "jsondb-global.h"
#include "jsondb-error.h"
#include "jsondb-query.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_JSONDB

class QsonObject;

class JsonDbConnection;
class JsonDbClientPrivate;
class JsonDbNotification;

class Q_ADDON_JSONDB_EXPORT JsonDbClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect)
public:
    JsonDbClient(const QString &socketName, QObject *parent = 0);
    JsonDbClient(QObject *parent = 0);
    ~JsonDbClient();

    // this is private api
    explicit JsonDbClient(JsonDbConnection *connection, QObject *parent = 0);

    bool isConnected() const;

    enum NotifyType {
        NotifyCreate = 0x01,
        NotifyUpdate = 0x02,
        NotifyRemove = 0x04
    };
    Q_DECLARE_FLAGS(NotifyTypes, NotifyType)

    enum Status {
        Null         = 0,
        Connecting   = 1,
        Ready        = 2,
        Error        = 3
    };
    Status status() const;

    void setAutoReconnect(bool reconnect);
    bool autoReconnect() const;

    void connectToServer();
    void disconnectFromServer();

    QString errorString() const;

public slots:
    QT_DEPRECATED
    int find(const QsonObject &query, QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);
    QT_DEPRECATED
    int remove(const QString &query, QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);

    QT_DEPRECATED
    int create(const QsonObject &object, QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0)
    { return create(object, QString(), target, successSlot, errorSlot); }
    QT_DEPRECATED
    int update(const QsonObject &object, QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0)
    { return update(object, QString(), target, successSlot, errorSlot); }

    QT_DEPRECATED
    int remove(const QsonObject &object, QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0)
    { return remove(object, QString(), target, successSlot, errorSlot); }
    QT_DEPRECATED
    int notify(NotifyTypes types, const QString &query,
               QObject *notifyTarget = 0, const char *notifySlot = 0,
               QObject *responseTarget = 0, const char *responseSuccessSlot = 0, const char *responseErrorSlot = 0)
    { return notify(types, query, QString(), notifyTarget, notifySlot, responseTarget, responseSuccessSlot, responseErrorSlot); }
    QT_DEPRECATED
    int query(const QString &query, int offset, int limit, QObject *target, const char *successSlot, const char *errorSlot)
    { return this->query(query, offset, limit, QVariantMap(), QString(), target, successSlot, errorSlot); }
    QT_DEPRECATED
    int changesSince(int stateNumber, QStringList types, QObject *target, const char *successSlot, const char *errorSlot)
    { return changesSince(stateNumber, types, QString(), target, successSlot, errorSlot); }
    QT_DEPRECATED
    int query(const QString &query, int offset, int limit,
              const QString &partitionName,
              QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);

    QT_DEPRECATED
    int create(const QsonObject &object, const QString &partitionName,
               QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);
    QT_DEPRECATED
    int update(const QsonObject &object, const QString &partitionName,
               QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);
    QT_DEPRECATED
    int remove(const QsonObject &object, const QString &partitionName,
               QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);

    int create(const QVariant &object, const QString &partitionName = QString(),
               QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);
    int update(const QVariant &object, const QString &partitionName = QString(),
               QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);
    int remove(const QVariant &object, const QString &partitionName = QString(),
               QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);

    QT_DEPRECATED
    int notify(NotifyTypes types, const QString &query, const QString &partitionName,
               QObject *notifyTarget = 0, const char *notifySlot = 0,
               QObject *responseTarget = 0, const char *responseSuccessSlot = 0, const char *responseErrorSlot = 0);

    QString registerNotification(NotifyTypes types, const QString &query, const QString &partitionName = QString(),
                                 QObject *notifyTarget = 0, const char *notifySlot = 0,
                                 QObject *responseTarget = 0, const char *responseSuccessSlot = 0, const char *responseErrorSlot = 0);
    void unregisterNotification(const QString &notifyUuid);

    int query(const QString &query, int offset = 0, int limit = -1,
              const QVariantMap &bindings = QVariantMap(),
              const QString &partitionName = QString(),
              QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);
    int changesSince(int stateNumber, QStringList types = QStringList(), const QString &partitionName = QString(),
                     QObject *target = 0, const char *successSlot = 0, const char *errorSlot = 0);

    JsonDbQuery *query();
    JsonDbChangesSince *changesSince();

    QT_DEPRECATED
    int find(const QVariant &query);

Q_SIGNALS:
#ifdef qdoc
    void notified(const QString &notify_uuid, const JsonDbNotification &notification);
#else
    void notified(const QString &notify_uuid, const QtAddOn::JsonDb::JsonDbNotification &notification);
#endif
    void response(int id, const QVariant &object);
    void error(int id, int code, const QString &message);

    // these four are deprecated and will be removed
    void notified(const QString &notify_uuid, const QVariant &object, const QString &action);
    void notified(const QString &notify_uuid, const QsonObject &object, const QString &action);
    void response(int id, const QsonObject &object);
    void readyWrite();

    void disconnected();

    // signals for properties
    void statusChanged();

protected:
    bool event(QEvent *);

private:
    Q_DISABLE_COPY(JsonDbClient)
    Q_DECLARE_PRIVATE(JsonDbClient)
    QScopedPointer<JsonDbClientPrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _q_statusChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_handleResponse(int,QsonObject))
    Q_PRIVATE_SLOT(d_func(), void _q_handleError(int,int,QString))
    Q_PRIVATE_SLOT(d_func(), void _q_handleNotified(QString,QsonObject,QString))
    Q_PRIVATE_SLOT(d_func(), void _q_timeout())
    Q_PRIVATE_SLOT(d_func(), void _q_processQueue())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(JsonDbClient::NotifyTypes)

QT_END_NAMESPACE_JSONDB

QT_END_HEADER

// for compatibility, include struct here that is needed for notifications
#include "jsondb-notification.h"

#endif // JSONDB_CLIENT_H
