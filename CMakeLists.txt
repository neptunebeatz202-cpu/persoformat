cmake_minimum_required(VERSION 3.22)

project(PersoFormant VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# JUCE — fetched automatically so no local install required
# ============================================================
include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(JUCE)

# ============================================================
# Plugin target
# ============================================================
juce_add_plugin(PersoFormant
    # Basic info
    COMPANY_NAME            "Chobits"
    PLUGIN_MANUFACTURER_CODE Chbt
    PLUGIN_CODE             PFmt
    FORMATS                 VST3 AU          # AU only compiles on macOS
    PRODUCT_NAME            "PersoFormant"
    BUNDLE_ID               "com.chobits.persoformant"
    PLUGIN_AU_MAIN_TYPE     "aufx"

    # Capabilities
    IS_SYNTH                FALSE
    NEEDS_MIDI_INPUT        FALSE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    # VST2 — disabled (no VST2 SDK in CI)
    # VST2_CATEGORY          kPlugCategEffect

    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Embed the Chobits background image as BinaryData
juce_add_binary_data(PersoFormantData SOURCES
    Resources/chobits.jpg
)
set_target_properties(PersoFormantData PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Source files
target_sources(PersoFormant PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

# JUCE modules
target_compile_definitions(PersoFormant PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(PersoFormant PRIVATE
    PersoFormantData
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_graphics
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
