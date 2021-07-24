#include "Device.hpp"

#include "SpecificSettings.hpp"

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include <score/document/DocumentContext.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <QDebug>
#include <QObject>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QThread>

#include <wobjectimpl.h>

W_OBJECT_IMPL(unix_shell::DeviceImplementation)

namespace unix_shell
{
  DeviceImplementation::DeviceImplementation(
        const Device::DeviceSettings& settings,
        const score::DocumentContext& ctx)
    : OwningDeviceInterface{settings}
    , m_ctx{ctx}
  {
    m_capas.canRefreshTree = true;
    m_capas.canAddNode = false;
    m_capas.canRemoveNode = false;
    m_capas.canRenameNode = false;
    m_capas.canSetProperties = false;
    m_capas.canSerialize = false;
  }

  DeviceImplementation::~DeviceImplementation() { }

  bool DeviceImplementation::reconnect()
  {
    disconnect();

    try
    {
      const auto& set
          = m_settings.deviceSpecificSettings.value<SpecificSettings>();
      qDebug() << "Shell created with: " << set.text;

      // Needed by most protocols:
      auto& ctx = m_ctx.plugin<Explorer::DeviceDocumentPlugin>().networkContext();

      auto protocol = std::make_unique<ossia::net::multiplex_protocol>();
      auto dev = std::make_unique<ossia::net::generic_device>(
            std::move(protocol), settings().name.toStdString());

      m_dev = std::move(dev);
      deviceChanged(nullptr, m_dev.get());
    }
    catch (const std::runtime_error& e)
    {
      qDebug() << "Shell error: " << e.what();
    }
    catch (...)
    {
      qDebug() << "Shell error";
    }

    return connected();
  }

  void DeviceImplementation::disconnect()
  {
    OwningDeviceInterface::disconnect();
  }
}

struct shell_parameter_data_base
{
  shell_parameter_data_base() = default;
  shell_parameter_data_base(const shell_parameter_data_base&) = delete;
  shell_parameter_data_base(shell_parameter_data_base&& other)
    : read{std::move(other.read)}
    , write{std::move(other.write)}
  {
  }

  shell_parameter_data_base& operator=(const shell_parameter_data_base&) = delete;
  shell_parameter_data_base& operator=(shell_parameter_data_base&&) = delete;

  shell_parameter_data_base(const QJSValue& val)
    : read{val.property("read")}
    , write{val.property("write")}
  {
  }

  bool valid(const QJSValue& val) const noexcept
  {
    return !val.isUndefined() && !val.isNull();
  }
  bool valid() const noexcept
  {
    return valid(write) || valid(read);
  }

  QJSValue read;
  QJSValue write;
  std::mutex source_lock;
};

struct shell_parameter_data final
    : public shell_parameter_data_base
    , public ossia::net::parameter_data
{
  using base_data_type = shell_parameter_data_base;
  shell_parameter_data() = default;
  shell_parameter_data(const shell_parameter_data&) = delete;
  shell_parameter_data(shell_parameter_data&&) = default;
  shell_parameter_data& operator=(const shell_parameter_data&) = delete;
  shell_parameter_data& operator=(shell_parameter_data&&) = delete;

  shell_parameter_data(const std::string& name)
    : parameter_data{name}
  {
  }

  shell_parameter_data(const QJSValue& val)
    : parameter_data{ossia::qt::make_parameter_data(val)}
    , shell_parameter_data_base{val}
  {
  }
};

class shell_protocol;
struct shell_parameter final
    : ossia::net::wrapped_parameter<shell_parameter_data>
    , Nano::Observer
{
  using ossia::net::wrapped_parameter<shell_parameter_data>::wrapped_parameter;
  ~shell_parameter() override
  {
  }

  void connect(ossia::net::parameter_base& s, shell_protocol& proto);
};

using shell_node = ossia::net::wrapped_node<shell_parameter_data, shell_parameter>;

class shell_protocol final
    : public QObject
    , public ossia::net::protocol_base
{
  W_OBJECT(shell_protocol)

  public:
    shell_protocol(const QByteArray& code)
  : m_code{code}
{
  m_engine = new QQmlEngine{this};
  m_component = new QQmlComponent{m_engine};

  QObject::connect(
      m_component,
      &QQmlComponent::statusChanged,
      this,
      [=](QQmlComponent::Status status) {
    qDebug() << status;
    qDebug() << m_component->errorString();
    if (!m_device)
      return;

    switch (status)
    {
    case QQmlComponent::Status::Ready:
    {
      m_object = m_component->create();
      m_object->setParent(m_engine->rootContext());

      QVariant ret;
      QMetaObject::invokeMethod(
          m_object, "createTree", Q_RETURN_ARG(QVariant, ret));
      ossia::qt::create_device<
      ossia::net::device_base,
      shell_node,
      shell_protocol>(*m_device, ret.value<QJSValue>());
      return;
    }
    case QQmlComponent::Status::Loading:
      return;
    case QQmlComponent::Status::Null:
    case QQmlComponent::Status::Error:
      qDebug() << m_component->errorString();
      return;
    }
  });

  this->moveToThread(&m_thread);
  m_thread.start();
}

~shell_protocol() override
{
  m_thread.exit();
  m_thread.wait();
}

static shell_parameter_data read_data(const QJSValue& js) { return js; }

private:
void reset_tree()
{
 // Initialize the roots
  if (!m_device)
    return;

  {

  }
}

bool pull(ossia::net::parameter_base&) override { return false; }
bool push(
    const ossia::net::parameter_base& parameter_base,
    const ossia::value& v) override { return true; }
bool push_raw(
    const ossia::net::full_parameter_data& parameter_base) override { return false; }
void set_device(ossia::net::device_base& dev) override {}
bool observe(ossia::net::parameter_base&, bool b) override { return false; }
bool update(ossia::net::node_base& node_base) override { return true; }

QThread m_thread;
QQmlEngine* m_engine{};
QQmlComponent* m_component{};

ossia::net::device_base* m_device{};
QObject* m_object{};
QByteArray m_code;
};
