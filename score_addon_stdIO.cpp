#include "score_addon_stdIO.hpp"
#include <score/plugins/FactorySetup.hpp>
#include <Protocols/ProtocolLibrary.hpp>

#include <QQmlEngine>

score_addon_stdIO::score_addon_stdIO()
{
  qmlRegisterType<stdIO::stdIO>("Ossia", 1, 0, "stdIO");
  qRegisterMetaType<std::vector<ossia::net::node_base*>>(
      "std::vector<ossia::net::node_base*>");
}

score_addon_stdIO::~score_addon_stdIO() { }

std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_stdIO::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<Device::ProtocolFactory, stdIO::ProtocolFactory>>(ctx, key);
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_stdIO)
