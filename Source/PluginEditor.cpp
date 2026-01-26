#include "PluginEditor.h"
#include "ParameterIDs.h"

#if BEATCONNECT_ACTIVATION_ENABLED
#include <beatconnect/Activation.h>
#endif

DreDimuraEditor::DreDimuraEditor(DreDimuraProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Create relays BEFORE WebView (required by JUCE 8 relay system)
    setupRelays();

    // Create WebView
    setupWebView();

    // Create attachments AFTER WebView
    auto& apvts = processorRef.getAPVTS();
    driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::drive), *driveRelay, nullptr);
    toneAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::tone), *toneRelay, nullptr);
    outputAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::output), *outputRelay, nullptr);
    bypassAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParameterIDs::bypass), *bypassRelay, nullptr);

    setSize(500, 350);
    setResizable(true, true);
    setResizeLimits(400, 280, 800, 560);
}

DreDimuraEditor::~DreDimuraEditor()
{
}

void DreDimuraEditor::setupRelays()
{
    // Relay names MUST match parameter IDs exactly
    driveRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::drive);
    toneRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::tone);
    outputRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::output);
    bypassRelay = std::make_unique<juce::WebToggleButtonRelay>(ParameterIDs::bypass);
}

void DreDimuraEditor::setupWebView()
{
    juce::WebBrowserComponent::Options options;

    // Add relay options
    options = options
        .withOptionsFrom(*driveRelay)
        .withOptionsFrom(*toneRelay)
        .withOptionsFrom(*outputRelay)
        .withOptionsFrom(*bypassRelay);

#if BEATCONNECT_ACTIVATION_ENABLED
    // Activation event listeners
    options = options
        .withEventListener("activateLicense", [this](const juce::var& data) {
            handleActivateLicense(data);
        })
        .withEventListener("deactivateLicense", [this](const juce::var& data) {
            handleDeactivateLicense(data);
        })
        .withEventListener("getActivationStatus", [this](const juce::var&) {
            handleGetActivationStatus();
        });
#endif

    // Create WebView with options
    webView = std::make_unique<juce::WebBrowserComponent>(options);
    addAndMakeVisible(*webView);

    // Load UI based on build mode
#if DRE_DIMURA_DEV_MODE
    // Development: hot reload from Vite dev server
    webView->goToURL("http://localhost:5173");
#elif HAS_WEB_ASSETS
    // Production: load from bundled assets
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#else
    // Fallback: show placeholder
    webView->goToURL("about:blank");
#endif
}

#if BEATCONNECT_ACTIVATION_ENABLED
void DreDimuraEditor::sendActivationState()
{
    auto& activation = beatconnect::Activation::getInstance();
    juce::DynamicObject::Ptr data = new juce::DynamicObject();
    data->setProperty("isConfigured", activation.isConfigured());
    data->setProperty("isActivated", activation.isActivated());

    if (activation.isActivated())
    {
        auto info = activation.getActivationInfo();
        juce::DynamicObject::Ptr infoObj = new juce::DynamicObject();
        infoObj->setProperty("activationCode", juce::String(info.activationCode));
        infoObj->setProperty("machineId", juce::String(info.machineId));
        infoObj->setProperty("activatedAt", juce::String(info.activatedAt));
        infoObj->setProperty("currentActivations", info.currentActivations);
        infoObj->setProperty("maxActivations", info.maxActivations);
        infoObj->setProperty("isValid", info.isValid);
        data->setProperty("info", juce::var(infoObj.get()));
    }

    webView->emitEventIfBrowserIsVisible("activationState", juce::var(data.get()));
}

void DreDimuraEditor::handleActivateLicense(const juce::var& data)
{
    auto code = data.getProperty("code", "").toString().toStdString();

    beatconnect::Activation::getInstance().activate(code,
        [this](beatconnect::ActivationStatus status, const beatconnect::ActivationInfo& info) {
            juce::MessageManager::callAsync([this, status, info]() {
                juce::DynamicObject::Ptr result = new juce::DynamicObject();
                result->setProperty("status", juce::String(beatconnect::statusToString(status)));

                if (status == beatconnect::ActivationStatus::Valid)
                {
                    juce::DynamicObject::Ptr infoObj = new juce::DynamicObject();
                    infoObj->setProperty("activationCode", juce::String(info.activationCode));
                    infoObj->setProperty("machineId", juce::String(info.machineId));
                    infoObj->setProperty("activatedAt", juce::String(info.activatedAt));
                    infoObj->setProperty("currentActivations", info.currentActivations);
                    infoObj->setProperty("maxActivations", info.maxActivations);
                    infoObj->setProperty("isValid", info.isValid);
                    result->setProperty("info", juce::var(infoObj.get()));
                }

                webView->emitEventIfBrowserIsVisible("activationResult", juce::var(result.get()));
            });
        });
}

void DreDimuraEditor::handleDeactivateLicense(const juce::var&)
{
    beatconnect::Activation::getInstance().deactivate(
        [this](beatconnect::ActivationStatus status) {
            juce::MessageManager::callAsync([this, status]() {
                juce::DynamicObject::Ptr result = new juce::DynamicObject();
                result->setProperty("status", juce::String(beatconnect::statusToString(status)));
                webView->emitEventIfBrowserIsVisible("deactivationResult", juce::var(result.get()));
            });
        });
}

void DreDimuraEditor::handleGetActivationStatus()
{
    sendActivationState();
}
#endif

void DreDimuraEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));
}

void DreDimuraEditor::resized()
{
    if (webView != nullptr)
        webView->setBounds(getLocalBounds());
}
