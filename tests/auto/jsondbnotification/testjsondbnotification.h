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
#ifndef TESTJSONDBNOTIFICATION_H
#define TESTJSONDBNOTIFICATION_H

#include <QCoreApplication>
#include <QList>
#include <QTest>
#include <QFile>
#include <QProcess>
#include <QEventLoop>
#include <QDebug>
#include <QLocalSocket>
#include <QTimer>
#include <QJSValue>

#include <jsondb-client.h>
#include <jsondb-error.h>

#include <QAbstractItemModel>
#include "clientwrapper.h"
#include "../../shared/qmltestutil.h"

QT_USE_NAMESPACE_JSONDB

struct CallbackData {
    int action;
    int stateNumber;
    QVariantMap result;
};

class TestJsonDbNotification: public ClientWrapper
{
    Q_OBJECT
public:
    TestJsonDbNotification();
    ~TestJsonDbNotification();

    // Tricking moc to accept the signal in JsonDbNotify.Action
    Q_ENUMS(Actions)
    enum Actions { Create = 1, Update = 2, Remove = 4 };

    void deleteDbFiles();
private slots:
    void initTestCase();
    void cleanupTestCase();

    void singleObjectNotifications();
    void multipleObjectNotifications();
    void createNotification();

public slots:
    void notificationSlot(QVariant result, int action, int stateNumber);
    void errorSlot(int code, const QString &message);
    void notificationSlot2(QJSValue result, Actions action, int stateNumber);

protected slots:
    void timeout();

private:
    ComponentData *createComponent();
    ComponentData *createPartitionComponent();
    void deleteComponent(ComponentData *componentData);

private:
    QProcess *mProcess;
    QStringList mNotificationsReceived;
    QList<ComponentData*> mComponents;
    QString mPluginPath;
    bool mTimedOut;
    bool callbackError;
    int callbackErrorCode;
    QString callbackErrorMessage;
    QList<CallbackData> cbData;
    QEventLoop mEventLoop2;
};

#endif
