#ifndef SHELL_PROTOCOL_H
#define SHELL_PROTOCOL_H

#include <QThread>
#include <QQmlComponent>
#include <QObject>
#include <QQmlContext>
#include <QProcess>

#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/detail/logger.hpp>

namespace ossia
{
namespace net
{

struct stdIO_parameter_data_base
{
  stdIO_parameter_data_base() = default;
  stdIO_parameter_data_base(const stdIO_parameter_data_base&) = delete;
  stdIO_parameter_data_base(stdIO_parameter_data_base&& other) = default;
  stdIO_parameter_data_base& operator=(const stdIO_parameter_data_base&) = default;
  stdIO_parameter_data_base& operator=(stdIO_parameter_data_base&&) = default;

  stdIO_parameter_data_base(const QJSValue& val)
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

  QString write;
};

struct stdIO_parameter_data final
    : public stdIO_parameter_data_base
    , public parameter_data
{
  using base_data_type = stdIO_parameter_data_base;
  stdIO_parameter_data() = default;
  stdIO_parameter_data(const stdIO_parameter_data&) = delete;
  stdIO_parameter_data(stdIO_parameter_data&&) = default;
  stdIO_parameter_data& operator=(const stdIO_parameter_data&) = delete;
  stdIO_parameter_data& operator=(stdIO_parameter_data&&) = delete;

  stdIO_parameter_data(const std::string& name)
    : parameter_data{name}
  {
  }

  stdIO_parameter_data(const QJSValue& val)
    : parameter_data{ossia::qt::make_parameter_data(val)}
    , stdIO_parameter_data_base{val}
  {
  }

  bool valid() const noexcept
  {
    if (type) return true;
    return false;
  }
};

class OSSIA_EXPORT stdIO_wrapper final
    : public QObject
{
  W_OBJECT(stdIO_wrapper)

  QProcess process;
  QString path;
  QStringList args;

  void start(QByteArray arr = QByteArray()) {
    process.start(path, args << arr);
    ossia::logger().info(
        "Started QProcess: {}", process.errorString().toStdString());
  }

public:
  stdIO_wrapper(const QString& pgm) : process{}
  {
    args = pgm.split(' ');
    path = args[0];
    args.removeFirst();
    start();

    connect(this, &stdIO_wrapper::write, this,
            &stdIO_wrapper::on_write, Qt::QueuedConnection);

    connect(&process, &QProcess::readyRead, this,
            &stdIO_wrapper::on_read, Qt::QueuedConnection);
  }
  ~stdIO_wrapper();

  void write(QByteArray arg_1) E_SIGNAL(OSSIA_EXPORT, write, arg_1)
  void read(QByteArray arg_1) E_SIGNAL(OSSIA_EXPORT, read, arg_1)

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

using stdIO_parameter = wrapped_parameter<stdIO_parameter_data>;

class OSSIA_EXPORT stdIO_protocol final
    : public QObject
    , public protocol_base
{
  public:
    stdIO_protocol(const QString& program, const QByteArray& code);

  ~stdIO_protocol() override;

  static stdIO_parameter_data read_data(const QJSValue& js);

private:
  bool pull(parameter_base&) override;
  bool push(
      const parameter_base& parameter_base,
      const ossia::value& v) override;
  bool push_raw(
      const full_parameter_data& parameter_base) override;
  void set_device(device_base& dev) override;
  bool observe(parameter_base&, bool b) override;
  bool update(node_base& node_base) override;
  void on_read(const QByteArray&);

  QThread m_thread;
  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  device_base* m_device{};
  QObject* m_object{};
  mutable stdIO_wrapper stdIO;
  QByteArray m_code;
};

}
}
#endif // SHELL_PROTOCOL_H
