cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
project(INSERT_PLUGIN_NAME VERSION 0.0.1)

add_subdirectory("C:/JUCE" JUCE)

set(SourceFiles
        "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginConfig.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginEditor.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginEditor.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginProcessor.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginProcessor.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/dsp/PluginDSP.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/dsp/PluginDSP.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/parameters/ParameterIDs.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/parameters/ParameterLayout.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/parameters/ParameterLayout.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/ui/lookandfeel/PluginLookAndFeel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/ui/lookandfeel/PluginLookAndFeel.h"
)
#Destination folder for VST3 executable
set(VST3_OUTPUT_DIR "C:/Dev/Plugins/Bin/VST3")
#Destination folder for standalone executable
set(STANDALONE_OUTPUT_DIR "C:/Dev/Plugins/Bin/Standalone")



juce_add_plugin(${PROJECT_NAME}
          VERSION ${PROJECT_VERSION}                               
          ICON_BIG "images/LogoTheBends.png"
          ICON_SMALL "images/LogoTheBends.png"
          COMPANY_NAME Ar3Sama                        # Specify the name of the plugin's author
          IS_SYNTH FALSE                              # Is this a synth or an effect?
          NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
          NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
          IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
          EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # Does the editor need keyboard focus?
          COPY_PLUGIN_AFTER_BUILD FALSE              # Should the plugin be installed to a default location after building?
          PLUGIN_MANUFACTURER_CODE Juce               # A four-character manufacturer id with at least one upper-case character
          PLUGIN_CODE Dem0                            # A unique four-character plugin id with exactly one upper-case character
                                                      # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
          FORMATS VST3 Standalone                     # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
          PRODUCT_NAME "INSERT_PLUGIN_NAME")                 # The name of the final executable, which can differ from the target name



target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

target_compile_definitions(${PROJECT_NAME}
     PUBLIC   
          JUCE_WEB_BROWSER=0
          JUCE_USE_CURL=0
          JUCE_VST3_CAN_REPLACE_VST2=0
)

# Target VST3
set(VST3_TARGET ${PROJECT_NAME}_VST3)
# Target Standalone
set(STANDALONE_TARGET ${PROJECT_NAME}_Standalone)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_data_structures
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Copie automatique après build le VST3
add_custom_command(TARGET ${VST3_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying VST3 to ${VST3_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${VST3_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:${VST3_TARGET}>"
        "${VST3_OUTPUT_DIR}"
)

# Copie automatique après build le Standalone
add_custom_command(TARGET ${STANDALONE_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${STANDALONE_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:${STANDALONE_TARGET}>"
        "${STANDALONE_OUTPUT_DIR}"
)



