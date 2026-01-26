#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "ParameterIDs.h"
#include "PreampDSP.h"

#if HAS_PROJECT_DATA
#include "ProjectData.h"
#endif

class DreDimuraProcessor : public juce::AudioProcessor
{
public:
    DreDimuraProcessor();
    ~DreDimuraProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // BeatConnect project data access
    bool hasActivationEnabled() const;
    juce::String getPluginId() const { return pluginId_; }
    juce::String getApiBaseUrl() const { return apiBaseUrl_; }
    juce::String getSupabaseKey() const { return supabasePublishableKey_; }

private:
    // Parameter layout creation
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Load BeatConnect project data
    void loadProjectData();

    // Parameter tree
    juce::AudioProcessorValueTreeState apvts;

    // Parameter pointers for real-time access
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* outputParam = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    // DSP
    PreampDSP preampDSP;

    // BeatConnect project data
    juce::String pluginId_;
    juce::String apiBaseUrl_;
    juce::String supabasePublishableKey_;
    juce::var buildFlags_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DreDimuraProcessor)
};
