#pragma once
#include <score/tools/std/StringHash.hpp>

#include <verdigris>

namespace unix_shell
{
struct SpecificSettings
{
  QString path;
  QString text;
};
}

Q_DECLARE_METATYPE(unix_shell::SpecificSettings)
W_REGISTER_ARGTYPE(unix_shell::SpecificSettings)
