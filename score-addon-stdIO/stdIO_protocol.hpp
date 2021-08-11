#ifndef STDIO_PROTOCOL_H
#define STDIO_PROTOCOL_H

#include <QQmlComponent>
#include <QObject>
#include <QQmlContext>
#include <QProcess>

#include <verdigris>

#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/detail/logger.hpp>

namespace stdIO
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
    , public ossia::net::parameter_data
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
        "Started QProcess {}", process.readAllStandardError().toStdString());
  }

public:
  stdIO_wrapper(const QString& pgm) : process{}
  {
    args = pgm.split(' ');
    path = args[0];
    args.removeFirst();
    start();

    connect(
          this, &stdIO_wrapper::write, this, &stdIO_wrapper::on_write,
          Qt::QueuedConnection);

    connect(
          &process, &QProcess::readyReadStandardOutput, this, &stdIO_wrapper::on_read,
          Qt::QueuedConnection);
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
//      read(process.readLine());
      qDebug() << process.readLine();
    }
  }; W_SLOT(on_read)
};

using stdIO_parameter = ossia::net::wrapped_parameter<stdIO_parameter_data>;
using stdIO_node = ossia::net::wrapped_node<stdIO_parameter_data, stdIO_parameter>;

class OSSIA_EXPORT stdIO_protocol final
    : public QObject
    , public ossia::net::protocol_base
{
public:
  stdIO_protocol(const QString& program, const QByteArray& code);

  ~stdIO_protocol() override;

  static stdIO_parameter_data read_data(const QJSValue& js);

  bool pull(ossia::net::parameter_base&) override;
  bool push(
      const ossia::net::parameter_base& parameter_base,
      const ossia::value& v) override;
  bool push_raw(
      const ossia::net::full_parameter_data& parameter_base) override;
  void set_device(ossia::net::device_base& dev) override;
  bool observe(ossia::net::parameter_base&, bool b) override;
  bool update(ossia::net::node_base& node_base) override;

private:
  void on_read(const QByteArray&);
  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  ossia::net::device_base* m_device{};
  QObject* m_object{};
  mutable stdIO_wrapper stdIO;
  QByteArray m_code;
};

}
#endif // STDIO_PROTOCOL_H
