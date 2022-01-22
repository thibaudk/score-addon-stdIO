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

#include <QTextEdit>
#include <QCodeEditor>
#include <QFormLayout>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlComponent>

#include <wobjectimpl.h>

namespace stdIO
{

W_OBJECT_IMPL(ProtocolSettingsWidget)

ProtocolSettingsWidget::ProtocolSettingsWidget(QWidget* parent)
    : Device::ProtocolSettingsWidget(parent)
{
  m_deviceNameEdit = new State::AddressFragmentLineEdit{this};
  m_deviceNameEdit->setText("StdIO");

  m_programEdit = new QLineEdit{this};
  m_programEdit->clear();

  m_codeEdit = Process::createScriptWidget("JS");
  m_codeEdit->setPlainText("");

  connect(static_cast<QCodeEditor*>(m_codeEdit),
          &QCodeEditor::editingFinished,
          this,
          &ProtocolSettingsWidget::parseProgram);

  auto layout = new QFormLayout;
  layout->addRow(tr("Name"), m_deviceNameEdit);
  layout->addRow(tr("program"), m_programEdit);
  layout->addRow(m_codeEdit);

  setLayout(layout);
}

ProtocolSettingsWidget::~ProtocolSettingsWidget() { }

Device::DeviceSettings ProtocolSettingsWidget::getSettings() const
{
  Device::DeviceSettings m_settings;
  m_settings.name = m_deviceNameEdit->text();
  m_settings.protocol = ProtocolFactory::static_concreteKey();

  SpecificSettings settings{};
  settings.program = m_programEdit->text();
  settings.text = m_codeEdit->toPlainText();

  m_settings.deviceSpecificSettings = QVariant::fromValue(settings);
  return m_settings;
}

void ProtocolSettingsWidget::setSettings(
    const Device::DeviceSettings& settings)
{
  m_deviceNameEdit->setText(settings.name);
  const auto& specif
      = settings.deviceSpecificSettings.value<SpecificSettings>();

  m_programEdit->setText(specif.program);
  m_codeEdit->setPlainText(specif.text);
}

void ProtocolSettingsWidget::parseProgram()
{
  auto engine = new QQmlEngine;
  auto component = new QQmlComponent{engine};

  connect(
        component,
        &QQmlComponent::statusChanged,
        this,
        [=](QQmlComponent::Status status) {
    switch (status)
    {
    case QQmlComponent::Status::Ready:
    {
      auto object = component->create();
      if (auto prop = object->property("program").toString();
          !prop.isEmpty())
      {
        m_programEdit->setText(prop);
      }
      else
      object->deleteLater();
      break;
    }
    default:
      qDebug() << status << component->errorString();
    }
    component->deleteLater();
    engine->deleteLater();
  });

  component->setData(m_codeEdit->document()->toPlainText().toUtf8(), QUrl{});
}

}
