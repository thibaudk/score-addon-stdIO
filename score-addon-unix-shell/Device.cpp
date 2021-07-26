#include "Device.hpp"
#include "shell_protocol.hpp"
#include "SpecificSettings.hpp"

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include <score/document/DocumentContext.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <QDebug>
#include <QQmlEngine>
#include <QThread>

#include <wobjectimpl.h>

W_OBJECT_IMPL(unix_shell::DeviceImplementation)

namespace unix_shell
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
      qDebug() << "Shell created with: " << set.text;

      // Needed by most protocols:
      auto& ctx = m_ctx.plugin<Explorer::DeviceDocumentPlugin>().networkContext();

      auto protocol = std::make_unique<shell_protocol>(set.text.toUtf8());
      auto dev = std::make_unique<ossia::net::generic_device>(
            std::move(protocol), settings().name.toStdString());

      m_dev = std::move(dev);
      deviceChanged(nullptr, m_dev.get());
    }
    catch (const std::runtime_error& e)
    {
      qDebug() << "Shell error: " << e.what();
    }
    catch (...)
    {
      qDebug() << "Shell error";
    }

    return connected();
  }

  void DeviceImplementation::disconnect()
  {
    OwningDeviceInterface::disconnect();
  }
}
