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
    driveParam = apvts.getRawParameterValue(ParameterIDs::drive);
    toneParam = apvts.getRawParameterValue(ParameterIDs::tone);
    outputParam = apvts.getRawParameterValue(ParameterIDs::output);
    bypassParam = apvts.getRawParameterValue(ParameterIDs::bypass);

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

    // Drive: 0% to 100%, default 25%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::drive, ParameterIDs::kStateVersion),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.25f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; },
        nullptr
    ));

    // Tone: 0% (dark) to 100% (bright), default 50%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::tone, ParameterIDs::kStateVersion),
        "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; },
        nullptr
    ));

    // Output: 0% to 100%, default 50% (unity gain)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::output, ParameterIDs::kStateVersion),
        "Output",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {
            float db = -12.0f + (value * 18.0f);
            return juce::String(db, 1) + " dB";
        },
        nullptr
    ));

    // Bypass
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::bypass, ParameterIDs::kStateVersion),
        "Bypass",
        false
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

    // Check bypass
    bool bypassed = bypassParam->load() > 0.5f;

    if (bypassed)
    {
        preampDSP.reset();  // Reset smoothed values to prevent clicks
        return;
    }

    // Update DSP parameters
    preampDSP.setDrive(driveParam->load());
    preampDSP.setTone(toneParam->load());
    preampDSP.setOutputGain(outputParam->load());

    // Process audio
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    preampDSP.process(context);
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
