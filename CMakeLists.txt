cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include("${SCORE_SOURCE_DIR}/cmake/ScoreExternalAddon.cmake")
endif()

project(score_addon_unix_shell LANGUAGES CXX)

# General initialization
score_common_setup()

# Creation of the library
add_library(score_addon_unix_shell
  score-addon-unix-shell/Device.cpp
  score-addon-unix-shell/Device.hpp
  score-addon-unix-shell/ProtocolFactory.cpp
  score-addon-unix-shell/ProtocolFactory.hpp
  score-addon-unix-shell/ProtocolSettingsWidget.cpp
  score-addon-unix-shell/ProtocolSettingsWidget.hpp
  score-addon-unix-shell/SpecificSettings.hpp
  score-addon-unix-shell/SpecificSettingsSerialization.cpp

  score_addon_unix_shell.hpp
  score_addon_unix_shell.cpp
)

# Link
target_link_libraries(score_addon_unix_shell PUBLIC
    score_plugin_engine
    score_plugin_protocols)

# Target-specific options
setup_score_plugin(score_addon_unix_shell)
