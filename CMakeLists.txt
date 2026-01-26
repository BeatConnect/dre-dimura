cmake_minimum_required(VERSION 3.22)
project(DreDimura VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dev mode option - enables hot reload from Vite dev server
option(DRE_DIMURA_DEV_MODE "Enable development mode with hot reload" OFF)

# BeatConnect activation option
option(BEATCONNECT_ENABLE_ACTIVATION "Enable BeatConnect activation system" OFF)

# Find JUCE - adjust path as needed
add_subdirectory(JUCE)

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "BeatConnect"
    COMPANY_WEBSITE "https://beatconnect.io"
    COMPANY_EMAIL "support@beatconnect.io"
    PLUGIN_MANUFACTURER_CODE Beat
    PLUGIN_CODE Drdm
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Dre-Dimura"
    BUNDLE_ID "io.beatconnect.dredimura"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    NEEDS_WEBVIEW2 TRUE
)

target_sources(${PROJECT_NAME}
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/ParameterIDs.h
        Source/PreampDSP.cpp
        Source/PreampDSP.h
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        $<IF:$<BOOL:${DRE_DIMURA_DEV_MODE}>,DRE_DIMURA_DEV_MODE=1,DRE_DIMURA_DEV_MODE=0>
)

# Windows WebView2 support
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_USE_WIN_WEBVIEW2=1)
endif()

# BeatConnect SDK Integration
if(EXISTS "${CMAKE_SOURCE_DIR}/resources/project_data.json")
    juce_add_binary_data(${PROJECT_NAME}_ProjectData
        HEADER_NAME "ProjectData.h"
        NAMESPACE ProjectData
        SOURCES resources/project_data.json
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_ProjectData)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_PROJECT_DATA=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_PROJECT_DATA=0)
endif()

if(BEATCONNECT_ENABLE_ACTIVATION AND EXISTS "${CMAKE_SOURCE_DIR}/beatconnect-sdk/sdk/activation/CMakeLists.txt")
    add_subdirectory(beatconnect-sdk/sdk/activation)
    target_link_libraries(${PROJECT_NAME} PRIVATE beatconnect_activation)
    target_compile_definitions(${PROJECT_NAME} PUBLIC BEATCONNECT_ACTIVATION_ENABLED=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC BEATCONNECT_ACTIVATION_ENABLED=0)
endif()

# Copy web resources to plugin bundle
if(NOT DRE_DIMURA_DEV_MODE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/Resources/WebUI/index.html")
        juce_add_binary_data(${PROJECT_NAME}_WebAssets
            HEADER_NAME "WebAssets.h"
            NAMESPACE WebAssets
            SOURCES
                Resources/WebUI/index.html
                Resources/WebUI/assets/index.js
                Resources/WebUI/assets/index.css
        )
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_WebAssets)
        target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_WEB_ASSETS=1)
    else()
        target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_WEB_ASSETS=0)
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_WEB_ASSETS=0)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
