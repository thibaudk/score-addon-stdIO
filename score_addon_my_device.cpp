#include "score_addon_my_device.hpp"

#include <score/plugins/FactorySetup.hpp>

#include <MyDevice/ProtocolFactory.hpp>

score_addon_my_device::score_addon_my_device() { }

score_addon_my_device::~score_addon_my_device() { }

std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_my_device::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<Device::ProtocolFactory, MyDevice::ProtocolFactory>>(ctx, key);
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_my_device)
