#include "shell_protocol.hpp"
#include <wobjectimpl.h>

W_OBJECT_IMPL(shell_wrapper)

void shell_wrapper::write(QByteArray arg)
{
} E_SIGNAL(OSSIA_EXPORT, write, arg)

void shell_wrapper::read(QByteArray arg)
{
} E_SIGNAL(OSSIA_EXPORT, read, arg)

W_OBJECT_IMPL(shell_protocol)

using shell_node = ossia::net::wrapped_node<shell_parameter_data, shell_parameter>;

class Shell : public QObject
{
};

shell_wrapper::~shell_wrapper()
{
}

shell_protocol::shell_protocol(const QString& program, const QByteArray& code)
  : shell{program},
    m_code{code}
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

      QObject::connect(&shell, &shell_wrapper::read,
                       this, &shell_protocol::on_read);
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

void shell_protocol::on_read(const QByteArray& a)
{
  QVariant ret;
  QMetaObject::invokeMethod(
        m_object, "onMessage",
        Q_RETURN_ARG(QVariant, ret),
        Q_ARG(QVariant, a));

  auto arr = ret.value<QJSValue>();
  // an array of {address, value} objects
  if (!arr.isArray())
    return;

  QJSValueIterator it(arr);
  while (it.hasNext())
  {
   it.next();
   auto val = it.value();
   auto addr = val.property("address");
   if (!addr.isString())
     continue;

   auto addr_txt = addr.toString().toStdString();
   auto n = find_node(m_device->get_root_node(), addr_txt);
   if (!n)
     continue;

   auto v = val.property("value");
   if (v.isNull())
     continue;

   if (auto addr = n->get_parameter())
   {
     qDebug() << "Applied value"
              << QString::fromStdString(value_to_pretty_string(
                                          ossia::qt::value_from_js(addr->value(), v)));
     addr->set_value(ossia::qt::value_from_js(addr->value(), v));
   }
  }
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
