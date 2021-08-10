cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include("${SCORE_SOURCE_DIR}/cmake/ScoreExternalAddon.cmake")
endif()

project(score_addon_stdIO LANGUAGES CXX)

# General initialization
score_common_setup()

# Creation of the library
add_library(score_addon_stdIO
  score-addon-stdIO/Device.cpp
  score-addon-stdIO/Device.hpp
  score-addon-stdIO/stdIO_protocol.cpp
  score-addon-stdIO/stdIO_protocol.hpp
  score-addon-stdIO/ProtocolFactory.cpp
  score-addon-stdIO/ProtocolFactory.hpp
  score-addon-stdIO/ProtocolSettingsWidget.cpp
  score-addon-stdIO/ProtocolSettingsWidget.hpp
  score-addon-stdIO/SpecificSettings.hpp
  score-addon-stdIO/SpecificSettingsSerialization.cpp

  score_addon_stdIO.hpp
  score_addon_stdIO.cpp
)

# Link
target_link_libraries(score_addon_stdIO PUBLIC
    score_plugin_engine
    score_plugin_protocols)

# Target-specific options
setup_score_plugin(score_addon_stdIO)
