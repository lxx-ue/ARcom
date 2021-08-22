// Fill out your copyright notice in the Description page of Project Settings.


#include "GPStracker.h"
#include "mapswidget.h"

Navigator::Navigator(QGeoRoutingManager* routingManager,
    QGeoSearchManager* searchManager,
    MapsWidget* mapsWidget, const QString& address,
    const QGeoRouteRequest& requestTemplate) :
    address(address),
    request(requestTemplate),
    routingManager(routingManager),
    searchManager(searchManager),
    mapsWidget(mapsWidget),
    routeObject(0),
    endMarker(0),
    startMarker(0)
{
}

Navigator::~Navigator()
{
    if (routeObject) {
        mapsWidget->map()->removeMapObject(routeObject);
        delete routeObject;
    }
    if (endMarker) {
        mapsWidget->map()->removeMapObject(endMarker);
        delete endMarker;
    }
    if (startMarker) {
        mapsWidget->map()->removeMapObject(startMarker);
        delete startMarker;
    }
}

void Navigator::start()
{
    QList<QGeoCoordinate> waypoints = request.waypoints();
    waypoints.append(mapsWidget->markerManager()->myLocation());
    request.setWaypoints(waypoints);

    startMarker = new Marker(Marker::StartMarker);
    startMarker->setCoordinate(mapsWidget->markerManager()->myLocation());
    startMarker->setName("Start point");
    mapsWidget->map()->addMapObject(startMarker);

    addressReply = searchManager->search(address);
    if (addressReply->isFinished()) {
        on_addressSearchFinished();
    }
    else {
        connect(addressReply, SIGNAL(error(QGeoSearchReply::Error, QString)),
            this, SIGNAL(searchError(QGeoSearchReply::Error, QString)));
        connect(addressReply, SIGNAL(finished()),
            this, SLOT(on_addressSearchFinished()));
    }
}

void Navigator::on_addressSearchFinished()
{
    if (addressReply->places().size() <= 0) {
        addressReply->deleteLater();
        return;
    }

    QGeoPlace place = addressReply->places().at(0);

    QList<QGeoCoordinate> waypoints = request.waypoints();
    waypoints.append(place.coordinate());
    request.setWaypoints(waypoints);

    routeReply = routingManager->calculateRoute(request);
    if (routeReply->isFinished()) {
        on_routingFinished();
    }
    else {
        connect(routeReply, SIGNAL(error(QGeoRouteReply::Error, QString)),
            this, SIGNAL(routingError(QGeoRouteReply::Error, QString)));
        connect(routeReply, SIGNAL(finished()),
            this, SLOT(on_routingFinished()));
    }

    endMarker = new Marker(Marker::EndMarker);
    endMarker->setCoordinate(place.coordinate());
    endMarker->setAddress(place.address());
    endMarker->setName("Destination");
    mapsWidget->map()->addMapObject(endMarker);

    addressReply->deleteLater();
}

QGeoRoute Navigator::route() const
{
    return firstRoute;
}

void Navigator::on_routingFinished()
{
    if (routeReply->routes().size() <= 0) {
        emit routingError(QGeoRouteReply::NoError, "No valid routes returned");
        routeReply->deleteLater();
        return;
    }

    QGeoRoute route = routeReply->routes().at(0);
    firstRoute = route;

    routeObject = new QGeoMapRouteObject;
    routeObject->setRoute(route);
    routeObject->setPen(QPen(Qt::blue, 2.0));

    mapsWidget->map()->addMapObject(routeObject);

    emit finished();
    routeReply->deleteLater();
}