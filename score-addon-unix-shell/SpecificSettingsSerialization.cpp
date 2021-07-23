#include "SpecificSettings.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

template <>
void DataStreamReader::read(const score-addon-unix-shell::SpecificSettings& n)
{
  m_stream << n.control;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(score-addon-unix-shell::SpecificSettings& n)
{
  m_stream >> n.control;
  checkDelimiter();
}

template <>
void JSONReader::read(const score-addon-unix-shell::SpecificSettings& n)
{
  obj["Control"] = n.control;
}

template <>
void JSONWriter::write(score-addon-unix-shell::SpecificSettings& n)
{
  n.control <<= obj["Control"];
}