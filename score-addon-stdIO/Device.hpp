#pragma once
#include <Device/Protocol/DeviceInterface.hpp>

namespace stdIO
{
class DeviceImplementation final : public Device::OwningDeviceInterface
{
public:
  DeviceImplementation(
      const Device::DeviceSettings& settings);
  ~DeviceImplementation();

  bool reconnect() override;
  void disconnect() override;
};
}
