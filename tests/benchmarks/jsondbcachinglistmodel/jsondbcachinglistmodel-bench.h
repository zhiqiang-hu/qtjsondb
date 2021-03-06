/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtAddOn.JsonDb module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef JsonDbCachingListModel_Bench_H
#define JsonDbCachingListModel_Bench_H

#include <QAbstractListModel>
#include "requestwrapper.h"
#include "qmltestutil.h"

QT_BEGIN_NAMESPACE
class QQmlEngine;
class QQmlComponent;
QT_END_NAMESPACE

QT_USE_NAMESPACE_JSONDB

class ModelData {
public:
    ModelData();
    ~ModelData();
    QQmlEngine *engine;
    QQmlComponent *component;
    QObject *model;
};

class JsonDbCachingListModelBench: public RequestWrapper
{
    Q_OBJECT
public:
    JsonDbCachingListModelBench();
    ~JsonDbCachingListModelBench();

    void deleteDbFiles();
    void connectListModel(QAbstractListModel *model);

public slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsRemoved(const QModelIndex &parent, int first, int last);
    void rowsMoved( const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row );
    void modelReset();
    void stateChanged();
    void callbackSlot(QVariant error, QVariant response);

protected slots:
    void timeout();
    void silentTimeout();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void ModelStartup();
    void ModelStartupTwoPartitions();
    void ModelStartupSorted();
    void getItemNotInCache();
    void deleteItem();
    void flicking();

private:
    void waitForExitOrTimeout();
    void waitForItemsCreated(int items);
    void waitForItemsUpdated(int items);
    void waitForStateOrTimeout();
    void waitForMs(int ms, int warningThreshold);
    void waitForItemChanged(bool waitForRemove = false);
    QStringList getOrderValues(QAbstractListModel *listModel);
    void getIndex(int index);
    QVariant getIndexRaw(QAbstractListModel *model, int index, int role);
    void createIndex(const QString &property, const QString &propertyType);
    QAbstractListModel *createModel();
    void deleteModel(QAbstractListModel *model);
    void deleteItems(const QString &type, const QString &partition);
    void resetWaitFlags();

private:
    QProcess *mProcess;
    QList<ModelData*> mModels;
    QString mPluginPath;

    // Response values
    bool mTimedOut;
    int mItemsCreated;
    int mItemsUpdated;
    int mItemsRemoved;
    bool mWaitingForStateChanged;
    bool mWaitingForRowsInserted;
    bool mWaitingForReset;
    bool mWaitingForChanged;
    bool mWaitingForRemoved;

};

#endif
