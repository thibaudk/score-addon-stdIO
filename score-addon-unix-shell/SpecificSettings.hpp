#pragma once
#include <score/tools/std/StringHash.hpp>

#include <verdigris>

namespace score-addon-unix-shell
{
struct SpecificSettings
{
  int control{1234};
};
}

Q_DECLARE_METATYPE(score-addon-unix-shell::SpecificSettings)
W_REGISTER_ARGTYPE(score-addon-unix-shell::SpecificSettings)