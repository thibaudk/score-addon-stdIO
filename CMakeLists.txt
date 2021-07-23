cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include("${SCORE_SOURCE_DIR}/cmake/ScoreExternalAddon.cmake")
endif()

project(score_addon_my_device LANGUAGES CXX)

# General initialization
score_common_setup()

# Creation of the library
add_library(score_addon_my_device
  MyDevice/Device.cpp
  MyDevice/Device.hpp
  MyDevice/ProtocolFactory.cpp
  MyDevice/ProtocolFactory.hpp
  MyDevice/ProtocolSettingsWidget.cpp
  MyDevice/ProtocolSettingsWidget.hpp
  MyDevice/SpecificSettings.hpp
  MyDevice/SpecificSettingsSerialization.cpp

  score_addon_my_device.hpp
  score_addon_my_device.cpp
)

# Link
target_link_libraries(score_addon_my_device PUBLIC score_plugin_engine)

# Target-specific options
setup_score_plugin(score_addon_my_device)
