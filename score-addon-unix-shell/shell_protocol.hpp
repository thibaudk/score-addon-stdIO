#ifndef SHELL_PROTOCOL_H
#define SHELL_PROTOCOL_H

#include <QThread>
#include <QQmlComponent>
#include <QObject>
#include <QQmlContext>
#include <QProcess>

#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>
#include <ossia/detail/logger.hpp>

struct shell_parameter_data_base
{
  shell_parameter_data_base() = default;
  shell_parameter_data_base(const shell_parameter_data_base&) = delete;
  shell_parameter_data_base(shell_parameter_data_base&& other) = default;
  shell_parameter_data_base& operator=(const shell_parameter_data_base&) = default;
  shell_parameter_data_base& operator=(shell_parameter_data_base&&) = default;

  shell_parameter_data_base(const QJSValue& val)
  {
    auto r = val.property("write");
    if (r.isString())
    {
      write = r.toString();
    }
    else
    {
      write = val.property("name").toString();
    }
  }

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

  shell_parameter_data(const std::string& name)
    : parameter_data{name}
  {
  }

  shell_parameter_data(const QJSValue& val)
    : parameter_data{ossia::qt::make_parameter_data(val)}
    , shell_parameter_data_base{val}
  {
  }

  bool valid() const noexcept
  {
    if (type) return true;
    return false;
  }
};

class OSSIA_EXPORT shell_wrapper final
    : public QObject
{
  W_OBJECT(shell_wrapper)

  QProcess process;
  QString path;
  QStringList args;

  void start(QByteArray arr = QByteArray()) {
    process.start(path, args << arr);
    ossia::logger().info(
        "Started QProcess: {}", process.errorString().toStdString());
  }

public:
  shell_wrapper(const QString& pgm) : process{}
  {
    args = pgm.split(' ');
    path = args[0];
    args.removeFirst();
    start();

    connect(this, &shell_wrapper::write, this,
            &shell_wrapper::on_write, Qt::QueuedConnection);

    connect(&process, &QProcess::readyRead, this,
            &shell_wrapper::on_read, Qt::QueuedConnection);
  }
  ~shell_wrapper();

  void write(QByteArray arg);
  void read(QByteArray arg);

  void on_write(QByteArray b)
  {
    if (process.state() == QProcess::NotRunning) start(b);
    else process.write(b);
  }; W_SLOT(on_write)

  void on_read()
  {
    while(process.canReadLine())
    {
      read(process.readLine());
    }
  }; W_SLOT(on_read)
};

using shell_parameter = ossia::net::wrapped_parameter<shell_parameter_data>;

class shell_protocol final
    : public QObject
    , public ossia::net::protocol_base
{
  W_OBJECT(shell_protocol)

  public:
    shell_protocol(const QString& program, const QByteArray& code);

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
  void on_read(const QByteArray&);

  QThread m_thread;
  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  ossia::net::device_base* m_device{};
  QObject* m_object{};
  mutable shell_wrapper shell;
  QByteArray m_code;
};

#endif // SHELL_PROTOCOL_H
