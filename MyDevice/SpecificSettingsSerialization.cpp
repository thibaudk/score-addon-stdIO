#include "SpecificSettings.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

template <>
void DataStreamReader::read(const MyDevice::SpecificSettings& n)
{
  m_stream << n.control;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(MyDevice::SpecificSettings& n)
{
  m_stream >> n.control;
  checkDelimiter();
}

template <>
void JSONReader::read(const MyDevice::SpecificSettings& n)
{
  obj["Control"] = n.control;
}

template <>
void JSONWriter::write(MyDevice::SpecificSettings& n)
{
  n.control <<= obj["Control"];
}