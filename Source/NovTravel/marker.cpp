// Fill out your copyright notice in the Description page of Project Settings.


#include "marker.h"
#include "mapswidget.h"

#include <QPixmap>

#include "qlandmark.h"
#include "qgeoboundingcircle.h"

class MarkerPrivate
{
public:
    Marker::MarkerType type;
    QString name;
    bool moveable;
    QGeoAddress address;
};

Marker::Marker(MarkerType type) :
    QGeoMapPixmapObject(),
    d(new MarkerPrivate)
{
    setMarkerType(type);
}

void Marker::setMarkerType(MarkerType type)
{
    QString filename;
    QPoint offset;
    int scale;

    d->type = type;

    switch (d->type) {
    case MyLocationMarker:
        filename = ":/icons/mylocation.png";
        break;
    case SearchMarker:
        filename = ":/icons/searchmarker.png";
        break;
    case WaypointMarker:
        filename = ":/icons/waypointmarker.png";
        break;
    case StartMarker:
        filename = ":/icons/startmarker.png";
        break;
    case EndMarker:
        filename = ":/icons/endmarker.png";
        break;
    case PathMarker:
        filename = ":/icons/pathmarker.png";
        break;
    }

    if (d->type == MyLocationMarker) {
        offset = QPoint(-13, -13);
        scale = 25;
    }
    else {
        offset = QPoint(-15, -36);
        scale = 30;
    }

    setOffset(offset);
    setPixmap(QPixmap(filename).scaledToWidth(scale, Qt::SmoothTransformation));
}

void Marker::setAddress(QGeoAddress addr)
{
    if (d->address != addr) {
        d->address = addr;
        emit addressChanged(d->address);
    }
}

Marker::MarkerType Marker::markerType() const
{
    return d->type;
}

QString Marker::name() const
{
    return d->name;
}

QGeoAddress Marker::address() const
{
    return d->address;
}

bool Marker::moveable() const
{
    return d->moveable;
}

void Marker::setName(QString name)
{
    if (d->name != name) {
        d->name = name;
        emit nameChanged(d->name);
    }
}

void Marker::setMoveable(bool moveable)
{
    if (d->moveable != moveable) {
        d->moveable = moveable;
        emit moveableChanged(d->moveable);
    }
}


class MarkerManagerPrivate
{
public:
    Marker* myLocation;
    QList<Marker*> searchMarkers;

    // a reverse geocode request is currently running
    bool revGeocodeRunning;
    // a request is currently running, and my location has changed
    // since it started (ie, the request is stale)
    bool myLocHasMoved;

    QGraphicsGeoMap* map;
    StatusBarItem* status;
    QGeoSearchManager* searchManager;

    QSet<QGeoSearchReply*> forwardReplies;
    QSet<QGeoSearchReply*> reverseReplies;
};

MarkerManager::MarkerManager(QGeoSearchManager* searchManager, QObject* parent) :
    QObject(parent),
    d(new MarkerManagerPrivate)
{
    d->searchManager = searchManager;
    d->status = 0;
    d->revGeocodeRunning = false;
    d->myLocHasMoved = false;

    d->myLocation = new Marker(Marker::MyLocationMarker);
    d->myLocation->setName("Me");

    // hook the coordinateChanged() signal for reverse geocoding
    connect(d->myLocation, SIGNAL(coordinateChanged(QGeoCoordinate)),
        this, SLOT(myLocationChanged(QGeoCoordinate)));

    connect(d->searchManager, SIGNAL(finished(QGeoSearchReply*)),
        this, SLOT(replyFinished(QGeoSearchReply*)));
    connect(d->searchManager, SIGNAL(finished(QGeoSearchReply*)),
        this, SLOT(reverseReplyFinished(QGeoSearchReply*)));
}

MarkerManager::~MarkerManager()
{
    d->map->removeMapObject(d->myLocation);
    delete d->myLocation;
    removeSearchMarkers();
}

void MarkerManager::setStatusBar(StatusBarItem* bar)
{
    d->status = bar;
}

void MarkerManager::setMap(QGraphicsGeoMap* map)
{
    d->map = map;
    map->addMapObject(d->myLocation);
}

void MarkerManager::setMyLocation(QGeoCoordinate coord)
{
    d->myLocation->setCoordinate(coord);
}
