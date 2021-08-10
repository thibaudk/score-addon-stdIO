#include "ProtocolSettingsWidget.hpp"

#include "ProtocolFactory.hpp"
#include "SpecificSettings.hpp"

#include <Library/LibrarySettings.hpp>
#include <State/Widgets/AddressFragmentLineEdit.hpp>
#include <Process/Script/ScriptWidget.hpp>

#include <score/application/ApplicationContext.hpp>
#include <score/model/tree/TreeNodeItemModel.hpp>
#include <score/tools/FindStringInFile.hpp>

#include <ossia/detail/config.hpp>

#include <ossia/detail/flat_map.hpp>

#include <QCodeEditor>
#include <QFormLayout>
#include <QVariant>

#include <wobjectimpl.h>

namespace stdIO
{

W_OBJECT_IMPL(ProtocolSettingsWidget)

ProtocolSettingsWidget::ProtocolSettingsWidget(QWidget* parent)
    : Device::ProtocolSettingsWidget(parent)
{
  m_deviceNameEdit = new State::AddressFragmentLineEdit{this};
  m_deviceNameEdit->setText("stdIO");

  m_programEdit = new State::AddressFragmentLineEdit{this};
  m_programEdit->setText("/bin/bash");

  m_codeEdit = Process::createScriptWidget("JS");
  m_codeEdit->setPlainText("");

  auto layout = new QFormLayout;
  layout->addRow(tr("Name"), m_deviceNameEdit);
  layout->addRow(tr("program"), m_programEdit);
  layout->addRow(m_codeEdit);

  setLayout(layout);
}

ProtocolSettingsWidget::~ProtocolSettingsWidget() { }

Device::DeviceSettings ProtocolSettingsWidget::getSettings() const
{
  // TODO should be = m_settings to follow the other patterns.
  Device::DeviceSettings s;
  s.name = m_deviceNameEdit->text();
  s.protocol = ProtocolFactory::static_concreteKey();

  SpecificSettings settings{};
  settings.program = m_programEdit->text();
  settings.text = m_codeEdit->toPlainText();

  s.deviceSpecificSettings = QVariant::fromValue(settings);
  return s;
}

void ProtocolSettingsWidget::setSettings(
    const Device::DeviceSettings& settings)
{
  m_deviceNameEdit->setText(settings.name);
  const auto& specif
      = settings.deviceSpecificSettings.value<SpecificSettings>();
}
}
