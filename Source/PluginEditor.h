/*
  ==============================================================================
    Dre-Dimura - Plugin Editor (JUCE 8 WebView UI)
    Vintage preamp coloration utility

    Uses JUCE 8's native WebView relay system for bidirectional parameter sync.
  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

//==============================================================================
class DreDimuraEditor : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    explicit DreDimuraEditor(DreDimuraProcessor&);
    ~DreDimuraEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    DreDimuraProcessor& processorRef;

    //==============================================================================
    // WebView component
    std::unique_ptr<juce::WebBrowserComponent> webView;
    juce::File resourcesDir;

    //==============================================================================
    // JUCE 8 Parameter Relays - MUST be created before WebView
    std::unique_ptr<juce::WebSliderRelay> driveRelay;
    std::unique_ptr<juce::WebSliderRelay> toneRelay;
    std::unique_ptr<juce::WebSliderRelay> outputRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> bypassRelay;
    std::unique_ptr<juce::WebComboBoxRelay> preampTypeRelay;

    // Cathode effect relays
    std::unique_ptr<juce::WebSliderRelay> cathEmberRelay;
    std::unique_ptr<juce::WebSliderRelay> cathHazeRelay;
    std::unique_ptr<juce::WebSliderRelay> cathEchoRelay;
    std::unique_ptr<juce::WebSliderRelay> cathDriftRelay;
    std::unique_ptr<juce::WebSliderRelay> cathVelvetRelay;

    // Filament effect relays
    std::unique_ptr<juce::WebSliderRelay> filFractureRelay;
    std::unique_ptr<juce::WebSliderRelay> filGlistenRelay;
    std::unique_ptr<juce::WebSliderRelay> filCascadeRelay;
    std::unique_ptr<juce::WebSliderRelay> filPhaseRelay;
    std::unique_ptr<juce::WebSliderRelay> filPrismRelay;

    // Steel Plate effect relays
    std::unique_ptr<juce::WebSliderRelay> steelScorchRelay;
    std::unique_ptr<juce::WebSliderRelay> steelRustRelay;
    std::unique_ptr<juce::WebSliderRelay> steelGrindRelay;
    std::unique_ptr<juce::WebSliderRelay> steelShredRelay;
    std::unique_ptr<juce::WebSliderRelay> steelSnarlRelay;

    //==============================================================================
    // Parameter Attachments - created AFTER WebView
    std::unique_ptr<juce::WebSliderParameterAttachment> driveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> toneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> outputAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> bypassAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> preampTypeAttachment;

    // Cathode effect attachments
    std::unique_ptr<juce::WebSliderParameterAttachment> cathEmberAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> cathHazeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> cathEchoAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> cathDriftAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> cathVelvetAttachment;

    // Filament effect attachments
    std::unique_ptr<juce::WebSliderParameterAttachment> filFractureAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filGlistenAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filCascadeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filPhaseAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filPrismAttachment;

    // Steel Plate effect attachments
    std::unique_ptr<juce::WebSliderParameterAttachment> steelScorchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> steelRustAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> steelGrindAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> steelShredAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> steelSnarlAttachment;

    //==============================================================================
    void setupWebView();
    void setupRelays();

    //==============================================================================
    // Timer callback for meter updates
    void timerCallback() override;

    //==============================================================================
    // Activation handlers (always declared, conditionally implemented)
    void sendActivationState();
    void handleActivateLicense(const juce::var& data);
    void handleDeactivateLicense(const juce::var& data);
    void handleGetActivationStatus();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DreDimuraEditor)
};
