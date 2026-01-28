#pragma once

#include <juce_dsp/juce_dsp.h>
#include "Effects/EffectsDSP.h"

/**
 * PreampDSP - Three distinct preamp characters
 *
 * CATHODE (0): Warm vintage tube character
 *   - Asymmetric soft clipping with strong even harmonics
 *   - Slow attack/release for squishy compression feel
 *   - Warm low-mids, rolled-off highs
 *   - Effects: Ember, Velvet, Drift, Echo, Haze
 *
 * FILAMENT (1): Cold digital precision
 *   - Symmetric hard clipping with odd harmonics
 *   - Fast transient response, crystalline highs
 *   - Flat response with high-frequency shimmer
 *   - Effects: Fracture, Prism, Phase, Cascade, Glisten
 *
 * STEEL PLATE (2): Aggressive industrial rawness
 *   - Asymmetric clipping with rectification for brutal harmonics
 *   - Punchy attack, gritty sustain
 *   - Scooped mids, harsh presence peak
 *   - Effects: Scorch, Snarl, Shred, Grind, Rust
 */

enum class PreampType
{
    Cathode = 0,
    Filament = 1,
    SteelPlate = 2
};

class PreampDSP
{
public:
    PreampDSP();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    template <typename ProcessContext>
    void process(const ProcessContext& context);

    // Preamp type selection
    void setPreampType(int type);

    // Parameter setters (0.0 to 1.0 normalized range)
    void setDrive(float newDrive);
    void setTone(float newTone);
    void setOutputGain(float newOutput);

    // Cathode effect setters
    void setCathEmber(float mix);
    void setCathHaze(float mix);
    void setCathEcho(float mix);
    void setCathDrift(float mix);
    void setCathVelvet(float mix);

    // Filament effect setters
    void setFilFracture(float mix);
    void setFilGlisten(float mix);
    void setFilCascade(float mix);
    void setFilPhase(float mix);
    void setFilPrism(float mix);

    // Steel Plate effect setters
    void setSteelScorch(float mix);
    void setSteelRust(float mix);
    void setSteelGrind(float mix);
    void setSteelShred(float mix);
    void setSteelSnarl(float mix);

private:
    // ======================================
    // Preamp-specific saturation algorithms
    // ======================================

    // Cathode: Warm tube saturation with even harmonics
    float processCathodeSample(float input, float drive);

    // Filament: Clean digital precision with odd harmonics
    float processFilamentSample(float input, float drive);

    // Steel Plate: Aggressive industrial saturation
    float processSteelPlateSample(float input, float drive);

    // ======================================
    // State
    // ======================================
    PreampType currentPreampType = PreampType::Cathode;

    // Parameters
    juce::SmoothedValue<float> driveGain;
    juce::SmoothedValue<float> toneValue;
    juce::SmoothedValue<float> outputGain;

    // ======================================
    // Cathode-specific filters (warm, vintage)
    // ======================================
    juce::dsp::IIR::Filter<float> cathToneL;
    juce::dsp::IIR::Filter<float> cathToneR;
    juce::dsp::IIR::Filter<float> cathWarmthL;  // Low shelf boost
    juce::dsp::IIR::Filter<float> cathWarmthR;
    juce::dsp::IIR::Filter<float> cathRolloffL; // High rolloff
    juce::dsp::IIR::Filter<float> cathRolloffR;

    // Cathode state for tube-like behavior
    float cathLastSampleL = 0.0f;
    float cathLastSampleR = 0.0f;
    float cathBiasL = 0.0f;  // Simulates tube bias drift
    float cathBiasR = 0.0f;

    // ======================================
    // Filament-specific filters (cold, precise)
    // ======================================
    juce::dsp::IIR::Filter<float> filToneL;
    juce::dsp::IIR::Filter<float> filToneR;
    juce::dsp::IIR::Filter<float> filPresenceL;  // High shelf for shimmer
    juce::dsp::IIR::Filter<float> filPresenceR;

    // ======================================
    // Steel Plate-specific filters (aggressive)
    // ======================================
    juce::dsp::IIR::Filter<float> steelToneL;
    juce::dsp::IIR::Filter<float> steelToneR;
    juce::dsp::IIR::Filter<float> steelScoopL;    // Mid scoop
    juce::dsp::IIR::Filter<float> steelScoopR;
    juce::dsp::IIR::Filter<float> steelPresenceL; // Harsh presence
    juce::dsp::IIR::Filter<float> steelPresenceR;

    // Steel Plate state for gritty behavior
    float steelRectifyL = 0.0f;
    float steelRectifyR = 0.0f;

    // ======================================
    // Shared
    // ======================================
    juce::dsp::IIR::Filter<float> dcBlockerL;
    juce::dsp::IIR::Filter<float> dcBlockerR;

    double sampleRate = 44100.0;

    // ======================================
    // Effect Instances
    // ======================================

    // Cathode Effects (Warm, Vintage, Tube)
    EmberDSP cathEmber;    // Distortion
    VelvetDSP cathVelvet;  // Filter
    DriftDSP cathDrift;    // Modulation
    EchoDSP cathEcho;      // Delay
    HazeDSP cathHaze;      // Reverb

    // Filament Effects (Cold, Digital, Precise)
    FractureDSP filFracture;  // Distortion
    PrismDSP filPrism;        // Filter
    PhaseDSP filPhase;        // Modulation
    CascadeDSP filCascade;    // Delay
    GlistenDSP filGlisten;    // Reverb

    // Steel Plate Effects (Aggressive, Industrial, Raw)
    ScorchDSP steelScorch;  // Distortion
    SnarlDSP steelSnarl;    // Filter
    ShredDSP steelShred;    // Modulation
    GrindDSP steelGrind;    // Delay
    RustDSP steelRust;      // Reverb
};

// Template implementation
template <typename ProcessContext>
void PreampDSP::process(const ProcessContext& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    if (context.isBypassed)
    {
        outputBlock.copyFrom(inputBlock);
        return;
    }

    // Process preamp per-sample with type-specific saturation
    for (size_t sample = 0; sample < numSamples; ++sample)
    {
        float drive = driveGain.getNextValue();
        float tone = toneValue.getNextValue();
        float outGain = outputGain.getNextValue();

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto ch = static_cast<int>(channel);
            auto smp = static_cast<int>(sample);

            float input = inputBlock.getSample(ch, smp);
            float processed = 0.0f;

            // Apply preamp-specific saturation
            switch (currentPreampType)
            {
                case PreampType::Cathode:
                    processed = processCathodeSample(input, drive);
                    break;
                case PreampType::Filament:
                    processed = processFilamentSample(input, drive);
                    break;
                case PreampType::SteelPlate:
                    processed = processSteelPlateSample(input, drive);
                    break;
            }

            // Apply preamp-specific tone shaping
            switch (currentPreampType)
            {
                case PreampType::Cathode:
                {
                    // Update Cathode tone filters
                    float cutoff = 600.0f + (tone * 3000.0f);  // 600Hz to 3.6kHz - warmer range
                    auto toneCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                        sampleRate, cutoff, 0.6f, 0.6f + tone * 0.8f);

                    if (channel == 0)
                    {
                        *cathToneL.coefficients = *toneCoeffs;
                        processed = cathToneL.processSample(processed);
                        processed = cathWarmthL.processSample(processed);  // Low boost
                        processed = cathRolloffL.processSample(processed); // High rolloff
                        processed = dcBlockerL.processSample(processed);
                    }
                    else
                    {
                        *cathToneR.coefficients = *toneCoeffs;
                        processed = cathToneR.processSample(processed);
                        processed = cathWarmthR.processSample(processed);
                        processed = cathRolloffR.processSample(processed);
                        processed = dcBlockerR.processSample(processed);
                    }
                    break;
                }
                case PreampType::Filament:
                {
                    // Update Filament tone filters - precise, flat with shimmer
                    float cutoff = 1000.0f + (tone * 6000.0f);  // 1kHz to 7kHz - brighter range
                    auto toneCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                        sampleRate, cutoff, 0.707f, 0.7f + tone * 0.6f);

                    if (channel == 0)
                    {
                        *filToneL.coefficients = *toneCoeffs;
                        processed = filToneL.processSample(processed);
                        processed = filPresenceL.processSample(processed);  // Crystalline highs
                        processed = dcBlockerL.processSample(processed);
                    }
                    else
                    {
                        *filToneR.coefficients = *toneCoeffs;
                        processed = filToneR.processSample(processed);
                        processed = filPresenceR.processSample(processed);
                        processed = dcBlockerR.processSample(processed);
                    }
                    break;
                }
                case PreampType::SteelPlate:
                {
                    // Update Steel Plate tone filters - aggressive, scooped
                    float cutoff = 800.0f + (tone * 4000.0f);
                    auto toneCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                        sampleRate, cutoff, 1.5f, 0.5f + tone);

                    if (channel == 0)
                    {
                        *steelToneL.coefficients = *toneCoeffs;
                        processed = steelToneL.processSample(processed);
                        processed = steelScoopL.processSample(processed);    // Mid scoop
                        processed = steelPresenceL.processSample(processed); // Harsh presence
                        processed = dcBlockerL.processSample(processed);
                    }
                    else
                    {
                        *steelToneR.coefficients = *toneCoeffs;
                        processed = steelToneR.processSample(processed);
                        processed = steelScoopR.processSample(processed);
                        processed = steelPresenceR.processSample(processed);
                        processed = dcBlockerR.processSample(processed);
                    }
                    break;
                }
            }

            // Output gain
            float output = processed * outGain;
            outputBlock.setSample(ch, smp, output);
        }
    }

    // ======================================
    // Effects Chain Processing - ONLY active preamp's effects
    // ======================================

    auto numSamplesInt = static_cast<int>(numSamples);
    float* leftChannel = outputBlock.getChannelPointer(0);
    float* rightChannel = (numChannels > 1) ? outputBlock.getChannelPointer(1) : leftChannel;

    switch (currentPreampType)
    {
        case PreampType::Cathode:
            // Order: Distortion -> Filter -> Modulation -> Delay -> Reverb
            cathEmber.process(leftChannel, rightChannel, numSamplesInt);
            cathVelvet.process(leftChannel, rightChannel, numSamplesInt);
            cathDrift.process(leftChannel, rightChannel, numSamplesInt);
            cathEcho.process(leftChannel, rightChannel, numSamplesInt);
            cathHaze.process(leftChannel, rightChannel, numSamplesInt);
            break;

        case PreampType::Filament:
            filFracture.process(leftChannel, rightChannel, numSamplesInt);
            filPrism.process(leftChannel, rightChannel, numSamplesInt);
            filPhase.process(leftChannel, rightChannel, numSamplesInt);
            filCascade.process(leftChannel, rightChannel, numSamplesInt);
            filGlisten.process(leftChannel, rightChannel, numSamplesInt);
            break;

        case PreampType::SteelPlate:
            steelScorch.process(leftChannel, rightChannel, numSamplesInt);
            steelSnarl.process(leftChannel, rightChannel, numSamplesInt);
            steelShred.process(leftChannel, rightChannel, numSamplesInt);
            steelGrind.process(leftChannel, rightChannel, numSamplesInt);
            steelRust.process(leftChannel, rightChannel, numSamplesInt);
            break;
    }
}
