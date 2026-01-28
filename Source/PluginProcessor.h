/*
  ==============================================================================
    Dre-Dimura - Audio Processor
    Vintage preamp coloration utility

    Recreates the preamp stages of vintage effects and peripherals,
    providing "accidental mojo" and unique coloration for digital guitar tones.
  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "ParameterIDs.h"
#include "PreampDSP.h"

#if HAS_PROJECT_DATA
#include "ProjectData.h"
#endif

#if BEATCONNECT_ACTIVATION_ENABLED
#include <beatconnect/Activation.h>
#endif

//==============================================================================
class DreDimuraProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    DreDimuraProcessor();
    ~DreDimuraProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    //==============================================================================
    // BeatConnect Integration
    juce::String getPluginId() const { return pluginId_; }
    juce::String getApiBaseUrl() const { return apiBaseUrl_; }
    juce::String getSupabaseKey() const { return supabasePublishableKey_; }
    bool hasActivationEnabled() const;

#if BEATCONNECT_ACTIVATION_ENABLED
    // Each processor owns its own Activation instance (no static/singleton!)
    beatconnect::Activation* getActivation() { return activation_.get(); }
    const beatconnect::Activation* getActivation() const { return activation_.get(); }
    bool hasActivation() const { return activation_ != nullptr; }
#endif

private:
    //==============================================================================
    // Parameter layout creation
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Load BeatConnect project data
    void loadProjectData();

    //==============================================================================
    // Parameter tree
    juce::AudioProcessorValueTreeState apvts;

    // Parameter pointers for real-time access
    std::atomic<float>* preampTypeParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* outputParam = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    // Cathode effect parameters
    std::atomic<float>* cathEmberParam = nullptr;
    std::atomic<float>* cathHazeParam = nullptr;
    std::atomic<float>* cathEchoParam = nullptr;
    std::atomic<float>* cathDriftParam = nullptr;
    std::atomic<float>* cathVelvetParam = nullptr;

    // Filament effect parameters
    std::atomic<float>* filFractureParam = nullptr;
    std::atomic<float>* filGlistenParam = nullptr;
    std::atomic<float>* filCascadeParam = nullptr;
    std::atomic<float>* filPhaseParam = nullptr;
    std::atomic<float>* filPrismParam = nullptr;

    // Steel Plate effect parameters
    std::atomic<float>* steelScorchParam = nullptr;
    std::atomic<float>* steelRustParam = nullptr;
    std::atomic<float>* steelGrindParam = nullptr;
    std::atomic<float>* steelShredParam = nullptr;
    std::atomic<float>* steelSnarlParam = nullptr;

    //==============================================================================
    // DSP
    PreampDSP preampDSP;

    //==============================================================================
    // Metering - atomic for thread-safe access from UI
    std::atomic<float> inputLevelL{ 0.0f };
    std::atomic<float> inputLevelR{ 0.0f };
    std::atomic<float> outputLevelL{ 0.0f };
    std::atomic<float> outputLevelR{ 0.0f };

public:
    // Level getters for UI metering
    float getInputLevel() const { return std::max(inputLevelL.load(), inputLevelR.load()); }
    float getOutputLevel() const { return std::max(outputLevelL.load(), outputLevelR.load()); }
    float getInputLevelL() const { return inputLevelL.load(); }
    float getInputLevelR() const { return inputLevelR.load(); }
    float getOutputLevelL() const { return outputLevelL.load(); }
    float getOutputLevelR() const { return outputLevelR.load(); }

private:

    //==============================================================================
    // BeatConnect project data
    juce::String pluginId_;
    juce::String apiBaseUrl_;
    juce::String supabasePublishableKey_;
    juce::var buildFlags_;

#if BEATCONNECT_ACTIVATION_ENABLED
    // Instance-based activation - avoids static member issues with multiple plugins
    std::unique_ptr<beatconnect::Activation> activation_;
#endif

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DreDimuraProcessor)
};
