#include "SpecificSettings.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

template <>
void DataStreamReader::read(const stdIO::SpecificSettings& n)
{
  m_stream << n.program << n.text;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(stdIO::SpecificSettings& n)
{
  m_stream >> n.program >> n.text;
  checkDelimiter();
}

template <>
void JSONReader::read(const stdIO::SpecificSettings& n)
{
  obj["Program"] = n.program;
  obj["Text"] = n.text;
}

template <>
void JSONWriter::write(stdIO::SpecificSettings& n)
{
  n.program <<= obj["Program"];
  n.text <<= obj["Text"];
}
