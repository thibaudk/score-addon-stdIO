#include "SpecificSettings.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

template <>
void DataStreamReader::read(const unix_shell::SpecificSettings& n)
{
  m_stream << n.control;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(unix_shell::SpecificSettings& n)
{
  m_stream >> n.control;
  checkDelimiter();
}

template <>
void JSONReader::read(const unix_shell::SpecificSettings& n)
{
  obj["Control"] = n.control;
}

template <>
void JSONWriter::write(unix_shell::SpecificSettings& n)
{
  n.control <<= obj["Control"];
}
