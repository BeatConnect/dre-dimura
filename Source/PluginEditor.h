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
class DreDimuraEditor : public juce::AudioProcessorEditor
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

    //==============================================================================
    // Parameter Attachments - created AFTER WebView
    std::unique_ptr<juce::WebSliderParameterAttachment> driveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> toneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> outputAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> bypassAttachment;

    //==============================================================================
    void setupWebView();
    void setupRelays();

    //==============================================================================
    // Activation handlers (always declared, conditionally implemented)
    void sendActivationState();
    void handleActivateLicense(const juce::var& data);
    void handleDeactivateLicense(const juce::var& data);
    void handleGetActivationStatus();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DreDimuraEditor)
};
