// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <QSignalMapper>

#include "qgeomappixmapobject.h"
#include "qgeosearchmanager.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoaddress.h"
#include "qgeosearchreply.h"

using namespace QtMobility;

class StatusBarItem;

class MarkerPrivate;
class Marker : public QGeoMapPixmapObject
{
    Q_OBJECT

        Q_PROPERTY(MarkerType markerType READ markerType WRITE setMarkerType NOTIFY markerTypeChanged)
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QGeoAddress address READ address WRITE setAddress NOTIFY addressChanged)
        Q_PROPERTY(bool moveable READ moveable WRITE setMoveable NOTIFY moveableChanged)
public:
    enum MarkerType {
        MyLocationMarker,
        SearchMarker,
        WaypointMarker,
        StartMarker,
        EndMarker,
        PathMarker
    };

    explicit Marker(MarkerType type);

    MarkerType markerType() const;
    void setMarkerType(MarkerType type);

    QString name() const;
    QGeoAddress address() const;
    bool moveable() const;

public slots:
    void setName(QString name);
    void setAddress(QGeoAddress addr);
    void setMoveable(bool moveable);

signals:
    void markerTypeChanged(const MarkerType& type);
    void nameChanged(const QString& name);
    void addressChanged(const QGeoAddress& address);
    void moveableChanged(const bool& moveable);

private:
    MarkerPrivate* d;
};

class MarkerManagerPrivate;
class MarkerManager : public QObject
{
    Q_OBJECT
public:
    explicit MarkerManager(QGeoSearchManager* sm, QObject* parent = 0);
    ~MarkerManager();

    QGeoCoordinate myLocation() const;

public slots:
    void setMap(QGraphicsGeoMap* map);
    void setStatusBar(StatusBarItem* bar);
    void setMyLocation(QGeoCoordinate coord);
    void search(QString query, qreal radius = -1);
    void removeSearchMarkers();

signals:
    void searchError(QGeoSearchReply::Error error, QString errorString);
    void searchFinished();

private:
    MarkerManagerPrivate* d;

private slots:
    void replyFinished(QGeoSearchReply* reply);
    void myLocationChanged(QGeoCoordinate location);
    void reverseReplyFinished(QGeoSearchReply* reply);
};
