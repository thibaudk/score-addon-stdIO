#include "stdIO_protocol.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(stdIO::stdIO_wrapper)

namespace stdIO
{

stdIO_protocol::stdIO_protocol(const QString& program, const QByteArray& code)
  : stdIO{program}
  , m_code{code}
{
  m_engine = new QQmlEngine{};
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
      stdIO_node,
      stdIO_protocol>(*m_device, ret.value<QJSValue>());

      QObject::connect(&stdIO, &stdIO_wrapper::read,
                       this, &stdIO_protocol::on_read);
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
}

void stdIO_protocol::on_read(const QByteArray& a)
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

stdIO_protocol::~stdIO_protocol()
{

}

stdIO_parameter_data stdIO_protocol::read_data(const QJSValue& js) { return js; }

bool stdIO_protocol::pull(ossia::net::parameter_base&) { return false; }

bool stdIO_protocol::push(const ossia::net::parameter_base& parameter_base, const ossia::value& v)
{
  auto& ad = dynamic_cast<const stdIO_parameter&>(parameter_base);
  auto str = ad.data().write;
  switch (parameter_base.get_value_type())
  {
  case ossia::val_type::FLOAT:
    str.replace("$val", QString::number(v.get<float>(), 'g', 4));
    break;
  case ossia::val_type::INT:
    str.replace("$val", QString::number(v.get<int32_t>()));
    break;
  case ossia::val_type::BOOL:
    str.replace("$val", v.get<bool>() ? "1" : "0");
    break;
  case ossia::val_type::CHAR:
    str.replace("$val", QString{v.get<char>()});
    break;
  case ossia::val_type::VEC2F:
  {
    auto& vec = v.get<ossia::vec2f>();
    str.replace("$val", QString{"%1 %2"}.arg(vec[0]).arg(vec[1]));
    break;
  }
  case ossia::val_type::VEC3F:
  {
    auto& vec = v.get<ossia::vec3f>();
    str.replace("$val", QString{"%1 %2 %3"}.arg(vec[0]).arg(vec[1]).arg(vec[2]));
    break;
  }
  case ossia::val_type::VEC4F:
  {
    auto& vec = v.get<ossia::vec4f>();
    str.replace("$val", QString{"%1 %2 %3 %4"}.arg(vec[0]).arg(vec[1]).arg(vec[2]).arg(vec[3]));
    break;
  }
  case ossia::val_type::LIST:
    str.replace("val", "TODO");
    break;
  case ossia::val_type::IMPULSE:
    str.replace("val", "TODO");
    break;
  default:
    throw std::runtime_error("stdIO_protocol::push: bad type");
  }

  qDebug() << str;
  stdIO.write(str.toUtf8());
  return false;
}

bool stdIO_protocol::push_raw(const ossia::net::full_parameter_data& parameter_base)
{ return false; }

void stdIO_protocol::set_device(ossia::net::device_base &dev)
{
  m_device = &dev;
  m_component->setData(m_code, QUrl{});
}

bool stdIO_protocol::observe(ossia::net::parameter_base&, bool b) { return false; }

bool stdIO_protocol::update(ossia::net::node_base& node_base) { return true; }
}
