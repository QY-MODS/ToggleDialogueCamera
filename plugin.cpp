#include <cassert>

#include "Settings.h"
#include "logger.h"

RE::UI* ui = nullptr;

bool zoom_enabled = false;

bool PostPostLoaded = false;
bool InputLoaded = false;

void ToggleDialogueCam() {
    auto plyr_c = RE::PlayerCamera::GetSingleton();
    auto thirdPersonState =
        static_cast<RE::ThirdPersonState*>(plyr_c->cameraStates[RE::CameraState::kThirdPerson].get());
    if (plyr_c->IsInFirstPerson()) {
        plyr_c->ForceThirdPerson();
        thirdPersonState->targetZoomOffset = thirdPersonState->savedZoomOffset;
        ;
    } else if (plyr_c->IsInThirdPerson()) {
        thirdPersonState->savedZoomOffset = thirdPersonState->currentZoomOffset;
        plyr_c->ForceFirstPerson();
    } else {
        logger::info("Player is in neither 1st nor 3rd person.");
    }
};

const char* get_purpose(int dn, int kk) {
    const char* failed = "failed";
    switch (dn) {
        case 0:
            for (std::size_t i = 0; i < keyboard.size(); ++i) {
                const auto& pair = keyboard[i];
                if (pair.second == kk) return pair.first;
            }
            return failed;
        case 1:
            for (std::size_t i = 0; i < mouse.size(); ++i) {
                const auto& pair = mouse[i];
                if (pair.second == kk) return pair.first;
            }
            return failed;
        case 2:
            for (std::size_t i = 0; i < gamepad.size(); ++i) {
                const auto& pair = gamepad[i];
                if (pair.second == kk) return pair.first;
            }
            return failed;
        default:
            return failed;
    }
}

bool EqStr(const char* str1, const char* str2) { return std::strcmp(str1, str2) == 0; }

bool PlayerCameraZoom(int a_device, bool _in) {
    auto player_cam = RE::PlayerCamera::GetSingleton();
    if (!player_cam->IsInThirdPerson()) return false;
    auto thirdPersonState =
        static_cast<RE::ThirdPersonState*>(player_cam->cameraStates[RE::CameraState::kThirdPerson].get());
    float amount = (a_device % 2) ? 0.1f : 0.025f;
    if (_in) {
        logger::info("Zooming in.");
        thirdPersonState->targetZoomOffset = std::max(thirdPersonState->targetZoomOffset - amount, 0.0f);
    } else {
        logger::info("Zooming out.");
        thirdPersonState->targetZoomOffset = std::min(thirdPersonState->targetZoomOffset + amount, 1.0f);
    }
    return true;
}

using InputEvents = RE::InputEvent*;
// https://github.com/SkyrimDev/
class OurEventSink : public RE::BSTEventSink<RE::InputEvent*> {
    OurEventSink() = default;
    OurEventSink(const OurEventSink&) = delete;
    OurEventSink(OurEventSink&&) = delete;
    OurEventSink& operator=(const OurEventSink&) = delete;
    OurEventSink& operator=(OurEventSink&&) = delete;

public:
    static OurEventSink* GetSingleton() {
        static OurEventSink singleton;
        return &singleton;
    }
    RE::BSEventNotifyControl ProcessEvent(const InputEvents* evns, RE::BSTEventSource<InputEvents>*) {
        if (!*evns) return RE::BSEventNotifyControl::kContinue;

        if (!(ui->IsMenuOpen(Settings::dialogue_menu_str))) return RE::BSEventNotifyControl::kContinue;

        for (RE::InputEvent* e = *evns; e; e = e->next) {
            if (e->eventType.get() == RE::INPUT_EVENT_TYPE::kButton) {
                RE::ButtonEvent* a_event = e->AsButtonEvent();
                uint32_t keyMask = a_event->idCode;
                auto _device = a_event->GetDevice();
                const char* purpose = get_purpose(_device, keyMask);

                if (EqStr(purpose, "failed")) continue;

                float duration = a_event->heldDownSecs;
                bool isPressed = a_event->value != 0 && duration >= 0;
                bool isReleased = a_event->value == 0 && duration != 0;
                bool _toggle = false;  // switch for 1st/3rd person

                if (isPressed) {
                    if (EqStr(purpose, "zoom") && !zoom_enabled) {
                        zoom_enabled = true;
                        logger::info("Zoom enabled");
                    } else if (EqStr(purpose, "zoom+") && zoom_enabled) {
                        if (!PlayerCameraZoom(_device, true)) continue;
                    } else if (EqStr(purpose, "zoom-") && zoom_enabled) {
                        if (!PlayerCameraZoom(_device, false)) continue;
                    }
                } else if (isReleased) {
                    if (EqStr(purpose, "zoom")) {
                        zoom_enabled = false;
                        logger::info("Zoom disabled");
                    } else if (EqStr(purpose, "toggle")) {
                        _toggle = true;
                    }
                }
                if (_toggle) {
                    logger::info("Toggling camera");
                    ToggleDialogueCam();
                    _toggle = false;
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
};

void OnMessage(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kInputLoaded:
            if (!InputLoaded) {
                RE::BSInputDeviceManager::GetSingleton()->AddEventSink(OurEventSink::GetSingleton());
                InputLoaded = true;
            }
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            if (!PostPostLoaded) {
                ui = RE::UI::GetSingleton();
                PostPostLoaded = true;
            }
            break;
    }
};

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SetupLog();
    SKSE::Init(skse);
    auto loaded = Settings::LoadSettings(); 
    if (loaded) logger::info("Settings loadedasd.");
    else logger::info("Could not load settings.");
    assert(loaded && "Could not load settings from ini file!");
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

    return true;
}
