
**
****************************************************************************/

#include "GPStracker.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QMetaObject>

Q_DECLARE_METATYPE(QGeoRouteRequest::TravelMode)

NavigateDialog::NavigateDialog(QWidget* parent) :
    QDialog(parent)
{
    QFormLayout* formLayout = new QFormLayout;
    QVBoxLayout* vbox = new QVBoxLayout;

    addressEdit = new QLineEdit;
    formLayout->addRow("Address", addressEdit);

    modeCombo = new QComboBox;
    modeCombo->addItem("Car", qVariantFromValue(QGeoRouteRequest::CarTravel));
    modeCombo->addItem("Pedestrian", qVariantFromValue(QGeoRouteRequest::PedestrianTravel));
    modeCombo->addItem("Bicycle", qVariantFromValue(QGeoRouteRequest::BicycleTravel));
    modeCombo->addItem("Public Transit", qVariantFromValue(QGeoRouteRequest::PublicTransitTravel));
    formLayout->addRow("Mode", modeCombo);

    QDialogButtonBox* bb = new QDialogButtonBox;
    bb->addButton(QDialogButtonBox::Ok);
    bb->addButton(QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    vbox->addLayout(formLayout);
    vbox->addWidget(bb);

    setLayout(vbox);
    setWindowTitle("Directions to address");
}

NavigateDialog::~NavigateDialog()
{
}

QString NavigateDialog::destinationAddress() const
{
    return addressEdit->text();
}

QGeoRouteRequest::TravelModes NavigateDialog::travelMode() const
{
    QVariant v = modeCombo->itemData(modeCombo->currentIndex());
    QGeoRouteRequest::TravelModes mode;
    mode |= qVariantValue<QGeoRouteRequest::TravelMode>(v);
    return mode;
}