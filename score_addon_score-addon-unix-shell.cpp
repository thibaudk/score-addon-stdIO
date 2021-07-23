#include "score_addon_score-addon-unix-shell.hpp"

#include <score/plugins/FactorySetup.hpp>

#include <score-addon-unix-shell/ProtocolFactory.hpp>

score_addon_score-addon-unix-shell::score_addon_score-addon-unix-shell() { }

score_addon_score-addon-unix-shell::~score_addon_score-addon-unix-shell() { }

std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_score-addon-unix-shell::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<Device::ProtocolFactory, score-addon-unix-shell::ProtocolFactory>>(ctx, key);
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_score-addon-unix-shell)
