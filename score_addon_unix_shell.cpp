#include "score_addon_unix_shell.hpp"
#include "score-addon-unix-shell/shell_protocol.cpp"
#include <score/plugins/FactorySetup.hpp>
#include <Protocols/ProtocolLibrary.hpp>

#include <QQmlEngine>

score_addon_unix_shell::score_addon_unix_shell()
{
  qmlRegisterType<Shell>("Ossia", 1, 0, "Shell");
  qRegisterMetaType<std::vector<ossia::net::node_base*>>(
      "std::vector<ossia::net::node_base*>");
}

score_addon_unix_shell::~score_addon_unix_shell() { }

std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_unix_shell::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<Device::ProtocolFactory, unix_shell::ProtocolFactory>>(ctx, key);
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_unix_shell)
