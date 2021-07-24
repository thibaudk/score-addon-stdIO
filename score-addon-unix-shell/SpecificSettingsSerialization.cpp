#include "SpecificSettings.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

template <>
void DataStreamReader::read(const unix_shell::SpecificSettings& n)
{
  m_stream << n.path << n.text;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(unix_shell::SpecificSettings& n)
{
  m_stream >> n.path >> n.text;
  checkDelimiter();
}

template <>
void JSONReader::read(const unix_shell::SpecificSettings& n)
{
  obj["Path"] = n.path;
  obj["Text"] = n.text;
}

template <>
void JSONWriter::write(unix_shell::SpecificSettings& n)
{
  n.path <<= obj["Path"];
  n.text <<= obj["Text"];
}
