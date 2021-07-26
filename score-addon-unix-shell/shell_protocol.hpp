#ifndef SHELL_PROTOCOL_H
#define SHELL_PROTOCOL_H

#include <QThread>
#include <QQmlComponent>
#include <QObject>
#include <QQmlContext>

#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>

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

  shell_parameter_data(const std::string& name);

  shell_parameter_data(const QJSValue& val);
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
    shell_protocol(const QByteArray& code);

  ~shell_protocol() override;

  static shell_parameter_data read_data(const QJSValue& js);

private:
  bool pull(ossia::net::parameter_base&) override;
  bool push(
      const ossia::net::parameter_base& parameter_base,
      const ossia::value& v) override;
  bool push_raw(
      const ossia::net::full_parameter_data& parameter_base) override;
  void set_device(ossia::net::device_base& dev) override;
  bool observe(ossia::net::parameter_base&, bool b) override;
  bool update(ossia::net::node_base& node_base) override;

  QThread m_thread;
  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  ossia::net::device_base* m_device{};
  QObject* m_object{};
  QByteArray m_code;
};

Q_DECLARE_METATYPE(shell_parameter*)
W_REGISTER_ARGTYPE(shell_parameter*)
#endif // SHELL_PROTOCOL_H
