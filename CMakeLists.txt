cmake_minimum_required(VERSION 3.22)
project(MdottyEQKNOB VERSION 1.0.0)

# --- This expects a "JUCE" folder INSIDE this project folder ---
# (the GitHub Actions workflow checks JUCE out here automatically;
#  if building locally, just copy/clone JUCE into this same folder)
add_subdirectory(JUCE JUCE)

# Option B (alternative): use CMake's FetchContent to grab JUCE automatically.
# If you'd rather not manage the JUCE folder yourself, comment out the
# add_subdirectory line above and uncomment this block instead:
#
# include(FetchContent)
# FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 7.0.12)
# FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MdottyEQKNOB
    COMPANY_NAME "prodbymdotty"
    PLUGIN_MANUFACTURER_CODE Ykco          # 4 chars, first must be uppercase
    PLUGIN_CODE Mdek                       # 4 chars, unique per plugin
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MdottyEQKNOB"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
)

target_sources(MdottyEQKNOB PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(MdottyEQKNOB PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MdottyEQKNOB PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
