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

    // ======================================
    // Cathode-specific filters (warm, vintage character)
    // ======================================

    // Main tone control
    auto cathToneCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 1500.0f, 0.6f, 1.0f);
    cathToneL.coefficients = cathToneCoeffs;
    cathToneR.coefficients = cathToneCoeffs;

    // Warmth: Low shelf boost at 120Hz for body
    auto warmthCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 120.0f, 0.7f, 1.4f);  // +3dB low boost
    cathWarmthL.coefficients = warmthCoeffs;
    cathWarmthR.coefficients = warmthCoeffs;

    // High rolloff: Gentle LP at 8kHz for vintage darkness
    auto rolloffCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, 8000.0f, 0.5f);
    cathRolloffL.coefficients = rolloffCoeffs;
    cathRolloffR.coefficients = rolloffCoeffs;

    // ======================================
    // Filament-specific filters (cold, precise character)
    // ======================================

    // Main tone control
    auto filToneCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 4000.0f, 0.707f, 1.0f);
    filToneL.coefficients = filToneCoeffs;
    filToneR.coefficients = filToneCoeffs;

    // Presence: High shelf at 10kHz for crystalline shimmer
    auto presenceCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 10000.0f, 0.707f, 1.3f);  // +2.5dB air
    filPresenceL.coefficients = presenceCoeffs;
    filPresenceR.coefficients = presenceCoeffs;

    // ======================================
    // Steel Plate-specific filters (aggressive character)
    // ======================================

    // Main tone control
    auto steelToneCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 2500.0f, 1.5f, 1.0f);
    steelToneL.coefficients = steelToneCoeffs;
    steelToneR.coefficients = steelToneCoeffs;

    // Mid scoop: Cut at 400Hz for that scooped metal tone
    auto scoopCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 400.0f, 1.2f, 0.6f);  // -4dB mid cut
    steelScoopL.coefficients = scoopCoeffs;
    steelScoopR.coefficients = scoopCoeffs;

    // Harsh presence: Aggressive peak at 3.5kHz
    auto harshCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 3500.0f, 2.0f, 1.8f);  // +5dB presence spike
    steelPresenceL.coefficients = harshCoeffs;
    steelPresenceR.coefficients = harshCoeffs;

    // ======================================
    // Shared: DC blocker
    // ======================================
    auto dcCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 10.0f);
    dcBlockerL.coefficients = dcCoeffs;
    dcBlockerR.coefficients = dcCoeffs;

    // ======================================
    // Prepare all effects
    // ======================================

    // Cathode effects
    cathEmber.prepare(spec);
    cathHaze.prepare(spec);
    cathEcho.prepare(spec);
    cathDrift.prepare(spec);
    cathVelvet.prepare(spec);

    // Filament effects
    filFracture.prepare(spec);
    filGlisten.prepare(spec);
    filCascade.prepare(spec);
    filPhase.prepare(spec);
    filPrism.prepare(spec);

    // Steel Plate effects
    steelScorch.prepare(spec);
    steelRust.prepare(spec);
    steelGrind.prepare(spec);
    steelShred.prepare(spec);
    steelSnarl.prepare(spec);

    reset();
}

void PreampDSP::reset()
{
    // Reset all filters
    cathToneL.reset();
    cathToneR.reset();
    cathWarmthL.reset();
    cathWarmthR.reset();
    cathRolloffL.reset();
    cathRolloffR.reset();

    filToneL.reset();
    filToneR.reset();
    filPresenceL.reset();
    filPresenceR.reset();

    steelToneL.reset();
    steelToneR.reset();
    steelScoopL.reset();
    steelScoopR.reset();
    steelPresenceL.reset();
    steelPresenceR.reset();

    dcBlockerL.reset();
    dcBlockerR.reset();

    // Reset state variables
    cathLastSampleL = cathLastSampleR = 0.0f;
    cathBiasL = cathBiasR = 0.0f;
    steelRectifyL = steelRectifyR = 0.0f;

    // Reset smoothed values
    driveGain.reset(sampleRate, 0.02);
    toneValue.reset(sampleRate, 0.02);
    outputGain.reset(sampleRate, 0.02);

    // Reset all effects
    cathEmber.reset();
    cathHaze.reset();
    cathEcho.reset();
    cathDrift.reset();
    cathVelvet.reset();

    filFracture.reset();
    filGlisten.reset();
    filCascade.reset();
    filPhase.reset();
    filPrism.reset();

    steelScorch.reset();
    steelRust.reset();
    steelGrind.reset();
    steelShred.reset();
    steelSnarl.reset();
}

void PreampDSP::setPreampType(int type)
{
    currentPreampType = static_cast<PreampType>(juce::jlimit(0, 2, type));
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

// ======================================
// CATHODE: Warm vintage tube saturation
// ======================================
// Character: Soft, squishy, warm. Strong even harmonics (2nd, 4th).
// Asymmetric clipping favoring positive half-cycles.
// Slow attack simulates tube heating/bias recovery.
float PreampDSP::processCathodeSample(float input, float drive)
{
    // Input gain with gentle curve (tube input stage)
    float gained = input * (1.0f + drive * 2.5f);

    // Simulate slow bias drift (creates subtle compression feel)
    // Bias follows the signal envelope slowly
    float biasTarget = gained * 0.1f;
    cathBiasL = cathBiasL * 0.9995f + biasTarget * 0.0005f;  // Very slow tracking

    // Apply bias offset (creates asymmetry)
    float biased = gained + cathBiasL * drive;

    // Tube-style saturation: asymmetric soft clipping
    // Positive: softer, rounder (triode-like)
    // Negative: slightly harder (more compression)
    float saturated;
    if (biased > 0.0f)
    {
        // Soft positive clipping with polynomial (even harmonics)
        float x = biased;
        saturated = x - (x * x * x / 3.0f);  // Soft cubic
        saturated = std::tanh(saturated * 0.8f) * 1.1f;
    }
    else
    {
        // Slightly harder negative clipping
        saturated = std::tanh(biased * 1.1f);
    }

    // Add subtle second harmonic (tube characteristic)
    float harmonic2 = saturated * saturated * 0.15f * drive;
    saturated += harmonic2;

    // Gentle slew rate limiting (tubes can't change instantly)
    float slewLimit = 0.3f + (1.0f - drive) * 0.7f;  // Slower at high drive
    float delta = saturated - cathLastSampleL;
    if (std::abs(delta) > slewLimit)
    {
        saturated = cathLastSampleL + (delta > 0 ? slewLimit : -slewLimit);
    }
    cathLastSampleL = saturated;

    return saturated * 0.8f;  // Output scaling
}

// ======================================
// FILAMENT: Cold digital precision
// ======================================
// Character: Clean, precise, crystalline. Odd harmonics (3rd, 5th).
// Symmetric clipping, fast transient response.
// Mathematical precision, no warmth.
float PreampDSP::processFilamentSample(float input, float drive)
{
    // Linear input gain (no coloration)
    float gained = input * (1.0f + drive * 3.0f);

    // Symmetric waveshaping with odd harmonics
    // Chebyshev-style polynomial for clean odd harmonics
    float x = gained;
    float x2 = x * x;
    float x3 = x2 * x;
    float x5 = x3 * x2;

    // Odd harmonic series: fundamental + 3rd + 5th
    float shaped = x - (x3 * 0.2f * drive) + (x5 * 0.05f * drive);

    // Hard limiter with slight knee (digital precision)
    float threshold = 1.0f - drive * 0.3f;  // Lower threshold at high drive
    if (std::abs(shaped) > threshold)
    {
        float excess = std::abs(shaped) - threshold;
        float knee = threshold + excess * 0.3f;  // Slight softening at limit
        shaped = (shaped > 0 ? 1.0f : -1.0f) * std::min(knee, 1.0f);
    }

    // No slew limiting - instant transient response

    return shaped * 0.85f;
}

// ======================================
// STEEL PLATE: Aggressive industrial saturation
// ======================================
// Character: Brutal, raw, punchy. Mixed harmonics with rectification.
// Asymmetric with partial rectification for extreme grit.
// Fast attack, gritty sustain.
float PreampDSP::processSteelPlateSample(float input, float drive)
{
    // Aggressive input gain
    float gained = input * (1.0f + drive * 4.0f);

    // Rectification blend (adds brutal even harmonics and DC offset character)
    float rectified = std::abs(gained);
    float rectMix = drive * 0.25f;  // More rectification at high drive
    float blended = gained * (1.0f - rectMix) + rectified * rectMix;

    // Hard asymmetric clipping (industrial character)
    float clipped;
    if (blended > 0.0f)
    {
        // Positive: hard clip with foldback for nastiness
        float x = blended;
        if (x > 1.0f)
        {
            float over = x - 1.0f;
            x = 1.0f - over * 0.3f * drive;  // Foldback distortion
        }
        clipped = std::tanh(x * 1.5f);
    }
    else
    {
        // Negative: even harder, more aggressive
        clipped = std::tanh(blended * 2.0f) * 0.9f;
    }

    // Add grit: subtle crossover distortion simulation
    float crossover = 0.02f * drive;
    if (std::abs(clipped) < crossover)
    {
        clipped = clipped * (std::abs(clipped) / crossover);  // Dead zone
    }

    // Track rectification state for extra grit
    steelRectifyL = steelRectifyL * 0.95f + rectified * 0.05f;
    float grit = steelRectifyL * drive * 0.1f;
    clipped += grit * (clipped > 0 ? 1.0f : -1.0f);

    // Slight compression on peaks (punch)
    if (std::abs(clipped) > 0.8f)
    {
        float excess = std::abs(clipped) - 0.8f;
        clipped = (clipped > 0 ? 1.0f : -1.0f) * (0.8f + excess * 0.5f);
    }

    return clipped * 0.75f;
}

// ======================================
// Cathode Effect Setters
// ======================================
void PreampDSP::setCathEmber(float mix) { cathEmber.setMix(mix); }
void PreampDSP::setCathHaze(float mix) { cathHaze.setMix(mix); }
void PreampDSP::setCathEcho(float mix) { cathEcho.setMix(mix); }
void PreampDSP::setCathDrift(float mix) { cathDrift.setMix(mix); }
void PreampDSP::setCathVelvet(float mix) { cathVelvet.setMix(mix); }

// ======================================
// Filament Effect Setters
// ======================================
void PreampDSP::setFilFracture(float mix) { filFracture.setMix(mix); }
void PreampDSP::setFilGlisten(float mix) { filGlisten.setMix(mix); }
void PreampDSP::setFilCascade(float mix) { filCascade.setMix(mix); }
void PreampDSP::setFilPhase(float mix) { filPhase.setMix(mix); }
void PreampDSP::setFilPrism(float mix) { filPrism.setMix(mix); }

// ======================================
// Steel Plate Effect Setters
// ======================================
void PreampDSP::setSteelScorch(float mix) { steelScorch.setMix(mix); }
void PreampDSP::setSteelRust(float mix) { steelRust.setMix(mix); }
void PreampDSP::setSteelGrind(float mix) { steelGrind.setMix(mix); }
void PreampDSP::setSteelShred(float mix) { steelShred.setMix(mix); }
void PreampDSP::setSteelSnarl(float mix) { steelSnarl.setMix(mix); }
