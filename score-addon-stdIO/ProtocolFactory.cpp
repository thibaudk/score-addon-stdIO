#include "ProtocolFactory.hpp"

#include "Device.hpp"
#include "ProtocolSettingsWidget.hpp"
#include "SpecificSettings.hpp"

#include <State/Widgets/AddressFragmentLineEdit.hpp>
#include <Protocols/LibraryDeviceEnumerator.hpp>

#include <score/application/ApplicationContext.hpp>
#include <score/widgets/SignalUtils.hpp>

#include <ossia/detail/config.hpp>

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QObject>

namespace stdIO
{

QString ProtocolFactory::prettyName() const noexcept
{
  return QObject::tr("StdIO");
}

QString ProtocolFactory::category() const noexcept
{
  return StandardCategories::util;
}

Device::DeviceEnumerator*
ProtocolFactory::getEnumerator(const score::DocumentContext& ctx) const
{
  return new Protocols::LibraryDeviceEnumerator{
      "Ossia.StdIO",
      {"*.qml"},
      ProtocolFactory::static_concreteKey(),
      [](const QByteArray& arr) {
        return QVariant::fromValue(SpecificSettings{arr, {}});
      },
      ctx};
}

Device::DeviceInterface* ProtocolFactory::makeDevice(
    const Device::DeviceSettings& settings,
    const Explorer::DeviceDocumentPlugin& plugin,
    const score::DocumentContext& ctx)
{
  return new DeviceImplementation{settings};
}

const Device::DeviceSettings& ProtocolFactory::defaultSettings() const noexcept
{
  static const Device::DeviceSettings& settings = [&]
  {
    Device::DeviceSettings s;
    s.protocol = concreteKey();
    s.name = "StdIO";
    SpecificSettings settings;
    s.deviceSpecificSettings = QVariant::fromValue(settings);
    return s;
  }();

  return settings;
}

Device::ProtocolSettingsWidget* ProtocolFactory::makeSettingsWidget()
{
  return new ProtocolSettingsWidget;
}

QVariant ProtocolFactory::makeProtocolSpecificSettings(
    const VisitorVariant& visitor) const
{
  return makeProtocolSpecificSettings_T<SpecificSettings>(visitor);
}

void ProtocolFactory::serializeProtocolSpecificSettings(
    const QVariant& data,
    const VisitorVariant& visitor) const
{
  serializeProtocolSpecificSettings_T<SpecificSettings>(data, visitor);
}

bool ProtocolFactory::checkCompatibility(
    const Device::DeviceSettings& a,
    const Device::DeviceSettings& b) const noexcept
{
  return false;
}
}

