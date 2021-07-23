#pragma once
#include <score/tools/std/StringHash.hpp>

#include <verdigris>

namespace MyDevice
{
struct SpecificSettings
{
  int control{1234};
};
}

Q_DECLARE_METATYPE(MyDevice::SpecificSettings)
W_REGISTER_ARGTYPE(MyDevice::SpecificSettings)