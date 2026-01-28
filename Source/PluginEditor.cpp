/*
  ==============================================================================
    Dre-Dimura - Plugin Editor Implementation
    Vintage preamp coloration utility

    Uses JUCE 8's native WebView relay system for reliable bidirectional
    parameter synchronization between C++ and the web UI.
  ==============================================================================
*/

#include "PluginEditor.h"
#include "ParameterIDs.h"

#if BEATCONNECT_ACTIVATION_ENABLED
#include <beatconnect/Activation.h>
#endif

static constexpr const char* DEV_SERVER_URL = "http://localhost:5173";

//==============================================================================
DreDimuraEditor::DreDimuraEditor(DreDimuraProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Create relays BEFORE WebView (required by JUCE 8 relay system)
    setupRelays();

    // Create WebView with full configuration
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
    preampTypeAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *apvts.getParameter(ParameterIDs::preampType), *preampTypeRelay, nullptr);

    // Cathode effect attachments
    cathEmberAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::cath_ember), *cathEmberRelay, nullptr);
    cathHazeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::cath_haze), *cathHazeRelay, nullptr);
    cathEchoAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::cath_echo), *cathEchoRelay, nullptr);
    cathDriftAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::cath_drift), *cathDriftRelay, nullptr);
    cathVelvetAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::cath_velvet), *cathVelvetRelay, nullptr);

    // Filament effect attachments
    filFractureAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::fil_fracture), *filFractureRelay, nullptr);
    filGlistenAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::fil_glisten), *filGlistenRelay, nullptr);
    filCascadeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::fil_cascade), *filCascadeRelay, nullptr);
    filPhaseAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::fil_phase), *filPhaseRelay, nullptr);
    filPrismAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::fil_prism), *filPrismRelay, nullptr);

    // Steel Plate effect attachments
    steelScorchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::steel_scorch), *steelScorchRelay, nullptr);
    steelRustAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::steel_rust), *steelRustRelay, nullptr);
    steelGrindAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::steel_grind), *steelGrindRelay, nullptr);
    steelShredAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::steel_shred), *steelShredRelay, nullptr);
    steelSnarlAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParameterIDs::steel_snarl), *steelSnarlRelay, nullptr);

    setSize(900, 520);
    setResizable(true, true);
    setResizeLimits(800, 460, 1200, 700);

    // Start meter update timer (30 fps for smooth metering)
    startTimerHz(30);
}

DreDimuraEditor::~DreDimuraEditor()
{
    stopTimer();
}

//==============================================================================
void DreDimuraEditor::setupRelays()
{
    // Relay names MUST match parameter IDs exactly
    driveRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::drive);
    toneRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::tone);
    outputRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::output);
    bypassRelay = std::make_unique<juce::WebToggleButtonRelay>(ParameterIDs::bypass);
    preampTypeRelay = std::make_unique<juce::WebComboBoxRelay>(ParameterIDs::preampType);

    // Cathode effect relays
    cathEmberRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::cath_ember);
    cathHazeRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::cath_haze);
    cathEchoRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::cath_echo);
    cathDriftRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::cath_drift);
    cathVelvetRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::cath_velvet);

    // Filament effect relays
    filFractureRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::fil_fracture);
    filGlistenRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::fil_glisten);
    filCascadeRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::fil_cascade);
    filPhaseRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::fil_phase);
    filPrismRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::fil_prism);

    // Steel Plate effect relays
    steelScorchRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::steel_scorch);
    steelRustRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::steel_rust);
    steelGrindRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::steel_grind);
    steelShredRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::steel_shred);
    steelSnarlRelay = std::make_unique<juce::WebSliderRelay>(ParameterIDs::steel_snarl);
}

//==============================================================================
void DreDimuraEditor::setupWebView()
{
    // ===========================================================================
    // STEP 1: Get resources directory - handle both Standalone and VST3 paths
    // ===========================================================================
    auto executableFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    auto executableDir = executableFile.getParentDirectory();

    // Try Standalone path first: executable/../Resources/WebUI
    resourcesDir = executableDir.getChildFile("Resources").getChildFile("WebUI");

    // If that doesn't exist, try VST3 path: executable/../../Resources/WebUI
    // VST3 structure: Plugin.vst3/Contents/x86_64-win/Plugin.vst3 (DLL)
    //                 Plugin.vst3/Contents/Resources/WebUI
    if (!resourcesDir.isDirectory())
    {
        resourcesDir = executableDir.getParentDirectory()
                           .getChildFile("Resources")
                           .getChildFile("WebUI");
    }

    // ===========================================================================
    // STEP 2: Build WebBrowserComponent with JUCE 8 options
    // ===========================================================================
    auto options = juce::WebBrowserComponent::Options()
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withNativeIntegrationEnabled()
        .withResourceProvider(
            [this](const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>
            {
                auto path = url;
                if (path.startsWith("/"))
                    path = path.substring(1);
                if (path.isEmpty())
                    path = "index.html";

                auto file = resourcesDir.getChildFile(path);
                if (!file.existsAsFile())
                    return std::nullopt;

                juce::String mimeType = "application/octet-stream";
                if (path.endsWith(".html")) mimeType = "text/html";
                else if (path.endsWith(".css")) mimeType = "text/css";
                else if (path.endsWith(".js")) mimeType = "application/javascript";
                else if (path.endsWith(".json")) mimeType = "application/json";
                else if (path.endsWith(".png")) mimeType = "image/png";
                else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) mimeType = "image/jpeg";
                else if (path.endsWith(".svg")) mimeType = "image/svg+xml";
                else if (path.endsWith(".woff")) mimeType = "font/woff";
                else if (path.endsWith(".woff2")) mimeType = "font/woff2";

                juce::MemoryBlock data;
                file.loadFileAsData(data);

                return juce::WebBrowserComponent::Resource{
                    std::vector<std::byte>(
                        reinterpret_cast<const std::byte*>(data.getData()),
                        reinterpret_cast<const std::byte*>(data.getData()) + data.getSize()),
                    mimeType.toStdString()
                };
            })
        // Register all relays
        .withOptionsFrom(*driveRelay)
        .withOptionsFrom(*toneRelay)
        .withOptionsFrom(*outputRelay)
        .withOptionsFrom(*bypassRelay)
        .withOptionsFrom(*preampTypeRelay)
        // Cathode effect relays
        .withOptionsFrom(*cathEmberRelay)
        .withOptionsFrom(*cathHazeRelay)
        .withOptionsFrom(*cathEchoRelay)
        .withOptionsFrom(*cathDriftRelay)
        .withOptionsFrom(*cathVelvetRelay)
        // Filament effect relays
        .withOptionsFrom(*filFractureRelay)
        .withOptionsFrom(*filGlistenRelay)
        .withOptionsFrom(*filCascadeRelay)
        .withOptionsFrom(*filPhaseRelay)
        .withOptionsFrom(*filPrismRelay)
        // Steel Plate effect relays
        .withOptionsFrom(*steelScorchRelay)
        .withOptionsFrom(*steelRustRelay)
        .withOptionsFrom(*steelGrindRelay)
        .withOptionsFrom(*steelShredRelay)
        .withOptionsFrom(*steelSnarlRelay)
        // Activation event listeners
        .withEventListener("activateLicense", [this](const juce::var& data) {
            handleActivateLicense(data);
        })
        .withEventListener("deactivateLicense", [this](const juce::var& data) {
            handleDeactivateLicense(data);
        })
        .withEventListener("getActivationStatus", [this](const juce::var&) {
            handleGetActivationStatus();
        })
        .withWinWebView2Options(
            juce::WebBrowserComponent::Options::WinWebView2()
                .withBackgroundColour(juce::Colour(0xff1a1a2e))
                .withStatusBarDisabled()
                .withUserDataFolder(
                    juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getChildFile("DreDimura_WebView2")));

    webView = std::make_unique<juce::WebBrowserComponent>(options);
    addAndMakeVisible(*webView);

    // ===========================================================================
    // STEP 3: Load URL based on build mode
    // ===========================================================================
#if DRE_DIMURA_DEV_MODE
    webView->goToURL(DEV_SERVER_URL);
#else
    webView->goToURL(webView->getResourceProviderRoot());
#endif
}

//==============================================================================
// Activation Handlers
//==============================================================================

void DreDimuraEditor::sendActivationState()
{
    if (!webView)
        return;

    juce::DynamicObject::Ptr data = new juce::DynamicObject();

#if BEATCONNECT_ACTIVATION_ENABLED
    auto* activation = processorRef.getActivation();

    bool isConfigured = activation != nullptr;
    bool isActivated = activation && activation->isActivated();

    data->setProperty("isConfigured", isConfigured);
    data->setProperty("isActivated", isActivated);

    if (isActivated && activation)
    {
        if (auto info = activation->getActivationInfo())
        {
            juce::DynamicObject::Ptr infoObj = new juce::DynamicObject();
            infoObj->setProperty("activationCode", juce::String(info->activationCode));
            infoObj->setProperty("machineId", juce::String(info->machineId));
            infoObj->setProperty("activatedAt", juce::String(info->activatedAt));
            infoObj->setProperty("currentActivations", info->currentActivations);
            infoObj->setProperty("maxActivations", info->maxActivations);
            infoObj->setProperty("isValid", info->isValid);
            data->setProperty("info", juce::var(infoObj.get()));
        }
    }
#else
    // Activation not enabled - report as not configured (no dialog shown)
    data->setProperty("isConfigured", false);
    data->setProperty("isActivated", true);  // Allow full access when activation disabled
#endif

    webView->emitEventIfBrowserIsVisible("activationState", juce::var(data.get()));
}

void DreDimuraEditor::handleActivateLicense(const juce::var& data)
{
#if BEATCONNECT_ACTIVATION_ENABLED
    juce::String code = data.getProperty("code", "").toString();
    if (code.isEmpty())
        return;

    // Use weak reference for async callback safety
    juce::Component::SafePointer<DreDimuraEditor> safeThis(this);

    auto* activation = processorRef.getActivation();
    if (!activation) return;

    activation->activateAsync(code.toStdString(),
        [safeThis](beatconnect::ActivationStatus status) {
            juce::MessageManager::callAsync([safeThis, status]() {
                if (!safeThis)
                    return;

                juce::DynamicObject::Ptr result = new juce::DynamicObject();

                juce::String statusStr;
                switch (status)
                {
                    case beatconnect::ActivationStatus::Valid:         statusStr = "valid"; break;
                    case beatconnect::ActivationStatus::Invalid:       statusStr = "invalid"; break;
                    case beatconnect::ActivationStatus::Revoked:       statusStr = "revoked"; break;
                    case beatconnect::ActivationStatus::MaxReached:    statusStr = "max_reached"; break;
                    case beatconnect::ActivationStatus::NetworkError:  statusStr = "network_error"; break;
                    case beatconnect::ActivationStatus::ServerError:   statusStr = "server_error"; break;
                    case beatconnect::ActivationStatus::NotConfigured: statusStr = "not_configured"; break;
                    case beatconnect::ActivationStatus::AlreadyActive: statusStr = "already_active"; break;
                    case beatconnect::ActivationStatus::NotActivated:  statusStr = "not_activated"; break;
                    default: statusStr = "unknown"; break;
                }
                result->setProperty("status", statusStr);

                // If successful, include activation info
                if (status == beatconnect::ActivationStatus::Valid ||
                    status == beatconnect::ActivationStatus::AlreadyActive)
                {
                    auto* activation = safeThis->processorRef.getActivation();
                    if (activation)
                    {
                        if (auto info = activation->getActivationInfo())
                        {
                            juce::DynamicObject::Ptr infoObj = new juce::DynamicObject();
                            infoObj->setProperty("activationCode", juce::String(info->activationCode));
                            infoObj->setProperty("machineId", juce::String(info->machineId));
                            infoObj->setProperty("activatedAt", juce::String(info->activatedAt));
                            infoObj->setProperty("currentActivations", info->currentActivations);
                            infoObj->setProperty("maxActivations", info->maxActivations);
                            infoObj->setProperty("isValid", info->isValid);
                            result->setProperty("info", juce::var(infoObj.get()));
                        }
                    }
                }

                safeThis->webView->emitEventIfBrowserIsVisible("activationResult", juce::var(result.get()));
            });
        });
#else
    juce::ignoreUnused(data);
#endif
}

void DreDimuraEditor::handleDeactivateLicense([[maybe_unused]] const juce::var& data)
{
#if BEATCONNECT_ACTIVATION_ENABLED
    juce::Component::SafePointer<DreDimuraEditor> safeThis(this);

    auto* activation = processorRef.getActivation();
    if (!activation) return;

    std::thread([safeThis, activation]() {
        auto status = activation->deactivate();

        juce::MessageManager::callAsync([safeThis, status]() {
            if (!safeThis)
                return;

            juce::DynamicObject::Ptr result = new juce::DynamicObject();
            juce::String statusStr;
            switch (status)
            {
                case beatconnect::ActivationStatus::Valid:         statusStr = "valid"; break;
                case beatconnect::ActivationStatus::NetworkError:  statusStr = "network_error"; break;
                case beatconnect::ActivationStatus::ServerError:   statusStr = "server_error"; break;
                case beatconnect::ActivationStatus::NotActivated:  statusStr = "not_activated"; break;
                default: statusStr = "unknown"; break;
            }
            result->setProperty("status", statusStr);

            safeThis->webView->emitEventIfBrowserIsVisible("deactivationResult", juce::var(result.get()));
        });
    }).detach();
#endif
}

void DreDimuraEditor::handleGetActivationStatus()
{
    sendActivationState();
}

//==============================================================================
void DreDimuraEditor::timerCallback()
{
    if (!webView)
        return;

    // Get levels from processor
    juce::DynamicObject::Ptr data = new juce::DynamicObject();
    data->setProperty("inputLevel", processorRef.getInputLevel());
    data->setProperty("outputLevel", processorRef.getOutputLevel());
    data->setProperty("inputLevelL", processorRef.getInputLevelL());
    data->setProperty("inputLevelR", processorRef.getInputLevelR());
    data->setProperty("outputLevelL", processorRef.getOutputLevelL());
    data->setProperty("outputLevelR", processorRef.getOutputLevelR());

    webView->emitEventIfBrowserIsVisible("audioLevels", juce::var(data.get()));
}

//==============================================================================
void DreDimuraEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));
}

void DreDimuraEditor::resized()
{
    if (webView != nullptr)
        webView->setBounds(getLocalBounds());
}
