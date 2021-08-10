#pragma once
#include <score/application/ApplicationContext.hpp>

#include <score/command/Command.hpp>
#include <score/command/CommandGeneratorMap.hpp>

#include <score/plugins/InterfaceList.hpp>
#include <score/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>

#include <score-addon-stdIO/ProtocolFactory.hpp>

#include <utility>
#include <vector>

class score_addon_stdIO final
    : public score::Plugin_QtInterface
    , public score::FactoryInterface_QtInterface
{
  SCORE_PLUGIN_METADATA(1, "c7cdca9c-1f41-4fbb-a195-d29b1326506a")

public:
  score_addon_stdIO();
  ~score_addon_stdIO() override;

private:
  std::vector<std::unique_ptr<score::InterfaceBase>> factories(
      const score::ApplicationContext& ctx,
      const score::InterfaceKey& key) const override;
};
