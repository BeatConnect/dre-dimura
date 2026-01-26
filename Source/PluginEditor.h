#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class DreDimuraEditor : public juce::AudioProcessorEditor
{
public:
    explicit DreDimuraEditor(DreDimuraProcessor&);
    ~DreDimuraEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setupWebView();
    void setupRelays();

#if BEATCONNECT_ACTIVATION_ENABLED
    void sendActivationState();
    void handleActivateLicense(const juce::var& data);
    void handleDeactivateLicense(const juce::var& data);
    void handleGetActivationStatus();
#endif

    DreDimuraProcessor& processorRef;

    // Parameter relays - MUST be created before WebView
    std::unique_ptr<juce::WebSliderRelay> driveRelay;
    std::unique_ptr<juce::WebSliderRelay> toneRelay;
    std::unique_ptr<juce::WebSliderRelay> outputRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> bypassRelay;

    // Parameter attachments - created AFTER WebView
    std::unique_ptr<juce::WebSliderParameterAttachment> driveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> toneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> outputAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> bypassAttachment;

    // WebView component
    std::unique_ptr<juce::WebBrowserComponent> webView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DreDimuraEditor)
};
