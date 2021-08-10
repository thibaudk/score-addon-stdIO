#pragma once
#include <score/tools/std/StringHash.hpp>

#include <verdigris>

namespace stdIO
{
struct SpecificSettings
{
  QString program;
  QString text;
};
}

Q_DECLARE_METATYPE(stdIO::SpecificSettings)
W_REGISTER_ARGTYPE(stdIO::SpecificSettings)
