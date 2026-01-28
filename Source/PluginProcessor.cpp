/*
  ==============================================================================
    Dre-Dimura - Audio Processor Implementation
    Vintage preamp coloration utility
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DreDimuraProcessor::DreDimuraProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Cache parameter pointers for real-time access
    preampTypeParam = apvts.getRawParameterValue(ParameterIDs::preampType);
    driveParam = apvts.getRawParameterValue(ParameterIDs::drive);
    toneParam = apvts.getRawParameterValue(ParameterIDs::tone);
    outputParam = apvts.getRawParameterValue(ParameterIDs::output);
    bypassParam = apvts.getRawParameterValue(ParameterIDs::bypass);

    // Cache Cathode effect parameters
    cathEmberParam = apvts.getRawParameterValue(ParameterIDs::cath_ember);
    cathHazeParam = apvts.getRawParameterValue(ParameterIDs::cath_haze);
    cathEchoParam = apvts.getRawParameterValue(ParameterIDs::cath_echo);
    cathDriftParam = apvts.getRawParameterValue(ParameterIDs::cath_drift);
    cathVelvetParam = apvts.getRawParameterValue(ParameterIDs::cath_velvet);

    // Cache Filament effect parameters
    filFractureParam = apvts.getRawParameterValue(ParameterIDs::fil_fracture);
    filGlistenParam = apvts.getRawParameterValue(ParameterIDs::fil_glisten);
    filCascadeParam = apvts.getRawParameterValue(ParameterIDs::fil_cascade);
    filPhaseParam = apvts.getRawParameterValue(ParameterIDs::fil_phase);
    filPrismParam = apvts.getRawParameterValue(ParameterIDs::fil_prism);

    // Cache Steel Plate effect parameters
    steelScorchParam = apvts.getRawParameterValue(ParameterIDs::steel_scorch);
    steelRustParam = apvts.getRawParameterValue(ParameterIDs::steel_rust);
    steelGrindParam = apvts.getRawParameterValue(ParameterIDs::steel_grind);
    steelShredParam = apvts.getRawParameterValue(ParameterIDs::steel_shred);
    steelSnarlParam = apvts.getRawParameterValue(ParameterIDs::steel_snarl);

    // Load BeatConnect configuration
    loadProjectData();
}

DreDimuraProcessor::~DreDimuraProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout DreDimuraProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Preamp Type: 0=Cathode, 1=Filament, 2=Steel Plate
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::preampType, ParameterIDs::kStateVersion),
        "Preamp Type",
        juce::StringArray{ "Cathode", "Filament", "Steel Plate" },
        0  // Default to Cathode
    ));

    // Drive: 0% to 100%, default 25%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::drive, ParameterIDs::kStateVersion),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.25f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    // Tone: 0% (dark) to 100% (bright), default 50%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::tone, ParameterIDs::kStateVersion),
        "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    // Output: 0% to 100%, default 50% (unity gain)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::output, ParameterIDs::kStateVersion),
        "Output",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) {
                float db = -12.0f + (value * 18.0f);
                return juce::String(db, 1) + " dB";
            })
    ));

    // Bypass
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::bypass, ParameterIDs::kStateVersion),
        "Bypass",
        false
    ));

    // ======================================
    // Cathode Effects (Warm, Vintage, Tube)
    // ======================================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::cath_ember, ParameterIDs::kStateVersion),
        "Ember",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::cath_haze, ParameterIDs::kStateVersion),
        "Haze",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::cath_echo, ParameterIDs::kStateVersion),
        "Echo",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::cath_drift, ParameterIDs::kStateVersion),
        "Drift",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::cath_velvet, ParameterIDs::kStateVersion),
        "Velvet",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    // ======================================
    // Filament Effects (Cold, Digital, Precise)
    // ======================================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::fil_fracture, ParameterIDs::kStateVersion),
        "Fracture",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::fil_glisten, ParameterIDs::kStateVersion),
        "Glisten",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::fil_cascade, ParameterIDs::kStateVersion),
        "Cascade",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::fil_phase, ParameterIDs::kStateVersion),
        "Phase",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::fil_prism, ParameterIDs::kStateVersion),
        "Prism",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    // ======================================
    // Steel Plate Effects (Aggressive, Industrial, Raw)
    // ======================================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::steel_scorch, ParameterIDs::kStateVersion),
        "Scorch",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::steel_rust, ParameterIDs::kStateVersion),
        "Rust",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::steel_grind, ParameterIDs::kStateVersion),
        "Grind",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::steel_shred, ParameterIDs::kStateVersion),
        "Shred",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::steel_snarl, ParameterIDs::kStateVersion),
        "Snarl",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) { return juce::String(int(value * 100)) + "%"; })
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
void DreDimuraProcessor::loadProjectData()
{
#if HAS_PROJECT_DATA
    int dataSize = 0;
    const char* data = ProjectData::getNamedResource("project_data_json", dataSize);

    if (data == nullptr || dataSize == 0)
        return;

    auto parsed = juce::JSON::parse(juce::String::fromUTF8(data, dataSize));
    if (parsed.isVoid())
        return;

    // Extract BeatConnect configuration
    pluginId_ = parsed.getProperty("pluginId", "").toString();
    apiBaseUrl_ = parsed.getProperty("apiBaseUrl", "").toString();
    supabasePublishableKey_ = parsed.getProperty("supabasePublishableKey", "").toString();
    buildFlags_ = parsed.getProperty("flags", juce::var());

#if BEATCONNECT_ACTIVATION_ENABLED
    bool enableActivation = static_cast<bool>(buildFlags_.getProperty("enableActivationKeys", false));

    if (enableActivation && pluginId_.isNotEmpty())
    {
        // Create instance-based activation (not singleton!)
        activation_ = std::make_unique<beatconnect::Activation>();

        beatconnect::ActivationConfig config;
        config.apiBaseUrl = apiBaseUrl_.toStdString();
        config.pluginId = pluginId_.toStdString();
        config.supabaseKey = supabasePublishableKey_.toStdString();
        activation_->configure(config);
    }
#endif
#endif
}

bool DreDimuraProcessor::hasActivationEnabled() const
{
#if HAS_PROJECT_DATA && BEATCONNECT_ACTIVATION_ENABLED
    return static_cast<bool>(buildFlags_.getProperty("enableActivationKeys", false));
#else
    return false;
#endif
}

//==============================================================================
const juce::String DreDimuraProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DreDimuraProcessor::acceptsMidi() const { return false; }
bool DreDimuraProcessor::producesMidi() const { return false; }
bool DreDimuraProcessor::isMidiEffect() const { return false; }
double DreDimuraProcessor::getTailLengthSeconds() const { return 0.0; }

int DreDimuraProcessor::getNumPrograms() { return 1; }
int DreDimuraProcessor::getCurrentProgram() { return 0; }
void DreDimuraProcessor::setCurrentProgram(int) {}
const juce::String DreDimuraProcessor::getProgramName(int) { return {}; }
void DreDimuraProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void DreDimuraProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP with 2x headroom for variable buffer sizes
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock * 2);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    preampDSP.prepare(spec);
}

void DreDimuraProcessor::releaseResources()
{
    preampDSP.reset();
}

bool DreDimuraProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void DreDimuraProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Measure input levels before processing
    const float decay = 0.9f;  // Smooth decay for meter ballistics

    if (totalNumInputChannels > 0)
    {
        float peakL = buffer.getMagnitude(0, 0, buffer.getNumSamples());
        inputLevelL.store(std::max(peakL, inputLevelL.load() * decay));
    }
    if (totalNumInputChannels > 1)
    {
        float peakR = buffer.getMagnitude(1, 0, buffer.getNumSamples());
        inputLevelR.store(std::max(peakR, inputLevelR.load() * decay));
    }

    // Check bypass
    bool bypassed = bypassParam->load() > 0.5f;

    if (bypassed)
    {
        preampDSP.reset();  // Reset smoothed values to prevent clicks
        // Reset output meters when bypassed
        outputLevelL.store(outputLevelL.load() * decay);
        outputLevelR.store(outputLevelR.load() * decay);
        return;
    }

    // Update DSP parameters
    int preampType = static_cast<int>(preampTypeParam->load());
    preampDSP.setPreampType(preampType);
    preampDSP.setDrive(driveParam->load());
    preampDSP.setTone(toneParam->load());
    preampDSP.setOutputGain(outputParam->load());

    // Update Cathode effect parameters (only processed when Cathode is active)
    preampDSP.setCathEmber(cathEmberParam->load());
    preampDSP.setCathHaze(cathHazeParam->load());
    preampDSP.setCathEcho(cathEchoParam->load());
    preampDSP.setCathDrift(cathDriftParam->load());
    preampDSP.setCathVelvet(cathVelvetParam->load());

    // Update Filament effect parameters
    preampDSP.setFilFracture(filFractureParam->load());
    preampDSP.setFilGlisten(filGlistenParam->load());
    preampDSP.setFilCascade(filCascadeParam->load());
    preampDSP.setFilPhase(filPhaseParam->load());
    preampDSP.setFilPrism(filPrismParam->load());

    // Update Steel Plate effect parameters
    preampDSP.setSteelScorch(steelScorchParam->load());
    preampDSP.setSteelRust(steelRustParam->load());
    preampDSP.setSteelGrind(steelGrindParam->load());
    preampDSP.setSteelShred(steelShredParam->load());
    preampDSP.setSteelSnarl(steelSnarlParam->load());

    // Process audio
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    preampDSP.process(context);

    // Measure output levels after processing
    if (totalNumOutputChannels > 0)
    {
        float peakL = buffer.getMagnitude(0, 0, buffer.getNumSamples());
        outputLevelL.store(std::max(peakL, outputLevelL.load() * decay));
    }
    if (totalNumOutputChannels > 1)
    {
        float peakR = buffer.getMagnitude(1, 0, buffer.getNumSamples());
        outputLevelR.store(std::max(peakR, outputLevelR.load() * decay));
    }
}

//==============================================================================
bool DreDimuraProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* DreDimuraProcessor::createEditor()
{
    return new DreDimuraEditor(*this);
}

//==============================================================================
void DreDimuraProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    // Add version info for future compatibility
    state.setProperty("stateVersion", ParameterIDs::kStateVersion, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DreDimuraProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        auto tree = juce::ValueTree::fromXml(*xmlState);

        // Check version for future migration
        int version = tree.getProperty("stateVersion", 0);
        juce::ignoreUnused(version);

        apvts.replaceState(tree);
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DreDimuraProcessor();
}
