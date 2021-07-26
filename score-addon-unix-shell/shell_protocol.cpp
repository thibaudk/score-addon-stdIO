#include "shell_protocol.hpp"
#include <wobjectimpl.h>

W_OBJECT_IMPL(shell_protocol)
using shell_node = ossia::net::wrapped_node<shell_parameter_data, shell_parameter>;

//namespace Protocols
//{
class Shell : public QObject
{
};
//}

shell_protocol::shell_protocol(const QByteArray& code)
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

shell_protocol::~shell_protocol()
{
  m_thread.exit();
  m_thread.wait();
}

shell_parameter_data shell_protocol::read_data(const QJSValue& js) { return js; }

bool shell_protocol::pull(ossia::net::parameter_base&) { return false; }

bool shell_protocol::push(const ossia::net::parameter_base& parameter_base, const ossia::value& v) { return true; }

bool shell_protocol::push_raw(const ossia::net::full_parameter_data& parameter_base) { return false; }

void shell_protocol::set_device(ossia::net::device_base &dev)
{
  m_device = &dev;
  m_component->setData(m_code, QUrl{});
}

bool shell_protocol::observe(ossia::net::parameter_base&, bool b) { return false; }

bool shell_protocol::update(ossia::net::node_base& node_base) { return true; }

shell_parameter_data::shell_parameter_data(const std::string& name)
  : parameter_data{name}
{
}

shell_parameter_data::shell_parameter_data(const QJSValue& val)
  : parameter_data{ossia::qt::make_parameter_data(val)}
  , shell_parameter_data_base{val}
{
}
