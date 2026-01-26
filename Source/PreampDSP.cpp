#include "PreampDSP.h"

PreampDSP::PreampDSP()
{
}

void PreampDSP::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // Smoothed values for click-free parameter changes
    driveGain.reset(sampleRate, 0.02);  // 20ms smoothing
    toneValue.reset(sampleRate, 0.02);
    outputGain.reset(sampleRate, 0.02);

    // Initialize tone filters
    auto toneCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 2000.0f, 0.707f, 1.0f);
    toneFilterL.coefficients = toneCoeffs;
    toneFilterR.coefficients = toneCoeffs;

    // DC blocker - high pass at 10 Hz
    auto dcCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 10.0f);
    dcBlockerL.coefficients = dcCoeffs;
    dcBlockerR.coefficients = dcCoeffs;

    reset();
}

void PreampDSP::reset()
{
    toneFilterL.reset();
    toneFilterR.reset();
    dcBlockerL.reset();
    dcBlockerR.reset();

    lastSampleL = 0.0f;
    lastSampleR = 0.0f;

    // Reset smoothed values to prevent clicks after bypass
    driveGain.reset(sampleRate, 0.02);
    toneValue.reset(sampleRate, 0.02);
    outputGain.reset(sampleRate, 0.02);
}

void PreampDSP::setDrive(float newDrive)
{
    driveGain.setTargetValue(newDrive);
}

void PreampDSP::setTone(float newTone)
{
    toneValue.setTargetValue(newTone);
}

void PreampDSP::setOutputGain(float newOutput)
{
    // Convert 0-1 range to useful gain range (approximately -12dB to +6dB)
    float gainDb = -12.0f + (newOutput * 18.0f);
    float linearGain = juce::Decibels::decibelsToGain(gainDb);
    outputGain.setTargetValue(linearGain);
}

float PreampDSP::softClip(float x)
{
    // Asymmetric soft clipping to emulate tube characteristics
    // Positive half cycles clip slightly harder (even harmonics)
    if (x > 0.0f)
    {
        // Softer positive clipping
        return std::tanh(x * 0.9f);
    }
    else
    {
        // Slightly harder negative clipping for asymmetry
        return std::tanh(x * 1.1f);
    }
}

float PreampDSP::transformerSaturate(float x)
{
    // Transformer saturation - subtle, symmetric
    // Adds gentle compression and very subtle harmonic content
    const float threshold = 0.7f;

    if (std::abs(x) < threshold)
    {
        return x;
    }

    // Soft knee compression above threshold
    float sign = (x > 0.0f) ? 1.0f : -1.0f;
    float absX = std::abs(x);
    float knee = threshold + (absX - threshold) * 0.5f;

    return sign * std::min(knee, 1.0f);
}

float PreampDSP::processSample(float input)
{
    float transformed = transformerSaturate(input);
    return softClip(transformed);
}
