#include "ProtocolSettingsWidget.hpp"

#include "ProtocolFactory.hpp"
#include "SpecificSettings.hpp"

#include <Library/LibrarySettings.hpp>
#include <State/Widgets/AddressFragmentLineEdit.hpp>

#include <score/application/ApplicationContext.hpp>
#include <score/model/tree/TreeNodeItemModel.hpp>
#include <score/tools/FindStringInFile.hpp>

#include <ossia/detail/config.hpp>

#include <ossia/detail/flat_map.hpp>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDirIterator>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QVariant>

#include <wobjectimpl.h>

W_OBJECT_IMPL(unix_shell::ProtocolSettingsWidget)

namespace unix_shell
{
ProtocolSettingsWidget::ProtocolSettingsWidget(QWidget* parent)
    : Device::ProtocolSettingsWidget(parent)
{
  m_deviceNameEdit = new State::AddressFragmentLineEdit{this};
  m_deviceNameEdit->setText("Shell");

  m_control = new QSpinBox{this};
  m_control->setRange(1, 65535);

  auto layout = new QFormLayout;
  layout->addRow(tr("Name"), m_deviceNameEdit);
  layout->addRow(tr("Control"), m_control);

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
  settings.control = this->m_control->value();
  s.deviceSpecificSettings = QVariant::fromValue(settings);

  return s;
}

void ProtocolSettingsWidget::setSettings(
    const Device::DeviceSettings& settings)
{
  m_deviceNameEdit->setText(settings.name);
  const auto& specif
      = settings.deviceSpecificSettings.value<SpecificSettings>();
  m_control->setValue(specif.control);
}
}
