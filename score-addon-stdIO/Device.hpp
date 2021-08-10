#pragma once
#include <Device/Protocol/DeviceInterface.hpp>

namespace stdIO
{
class DeviceImplementation final : public Device::OwningDeviceInterface
{
  W_OBJECT(DeviceImplementation)
public:
  DeviceImplementation(
      const Device::DeviceSettings& settings,
      const score::DocumentContext& ctx);
  ~DeviceImplementation();

  bool reconnect() override;
  void disconnect() override;

private:
  const score::DocumentContext& m_ctx;
};
}
