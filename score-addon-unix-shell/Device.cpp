#include "Device.hpp"

#include "SpecificSettings.hpp"

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include <score/document/DocumentContext.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>

#include <QDebug>

#include <wobjectimpl.h>

W_OBJECT_IMPL(score-addon-unix-shell::DeviceImplementation)

namespace score-addon-unix-shell
{
DeviceImplementation::DeviceImplementation(
    const Device::DeviceSettings& settings,
    const score::DocumentContext& ctx)
    : OwningDeviceInterface{settings}
    , m_ctx{ctx}
{
  m_capas.canRefreshTree = true;
  m_capas.canAddNode = false;
  m_capas.canRemoveNode = false;
  m_capas.canRenameNode = false;
  m_capas.canSetProperties = false;
  m_capas.canSerialize = false;
}

DeviceImplementation::~DeviceImplementation() { }

bool DeviceImplementation::reconnect()
{
  disconnect();

  try
  {
    const auto& set
        = m_settings.deviceSpecificSettings.value<SpecificSettings>();
    qDebug() << "score-addon-unix-shell created with: " << set.control;

    // Needed by most protocols:
    auto& ctx = m_ctx.plugin<Explorer::DeviceDocumentPlugin>().asioContext;

    auto protocol = std::make_unique<ossia::net::multiplex_protocol>();
    auto dev = std::make_unique<ossia::net::generic_device>(
        std::move(protocol), settings().name.toStdString());

    m_dev = std::move(dev);
    deviceChanged(nullptr, m_dev.get());
  }
  catch (const std::runtime_error& e)
  {
    qDebug() << "score-addon-unix-shell error: " << e.what();
  }
  catch (...)
  {
    qDebug() << "score-addon-unix-shell error";
  }

  return connected();
}

void DeviceImplementation::disconnect()
{
  OwningDeviceInterface::disconnect();
}
}