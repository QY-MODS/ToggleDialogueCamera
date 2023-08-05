#include "logger.h"

RE::UI* ui = nullptr;
RE::PlayerCamera* player_cam = nullptr;
const std::string_view& dialogue_menu_str = "Dialogue Menu";
constexpr std::uint32_t toggle_code_kb = 33;
constexpr std::uint32_t toggle_code_gp = 128;
const int zoom_enabler_kb = 29;
const int zoom_enabler_gp = 64;
constexpr std::uint32_t zoom_in_code_gp = 10;
constexpr std::uint32_t zoom_out_code_gp = 512;
constexpr std::uint32_t zoom_in_code_m = 8;
constexpr std::uint32_t zoom_out_code_m = 9;
bool zoom_enabled = false;

bool PostPostLoaded = false;
bool InputLoaded = false;

void ToggleDialogueCam(RE::PlayerCamera* plyr_c) {
    auto thirdPersonState = static_cast<RE::ThirdPersonState*>(plyr_c->cameraStates[RE::CameraState::kThirdPerson].get());
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

using InputEvents = RE::InputEvent*;
//https://github.com/SkyrimDev/
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

        if (!(ui->IsMenuOpen(dialogue_menu_str))) return RE::BSEventNotifyControl::kContinue;

        for (RE::InputEvent* e = *evns; e; e = e->next) {
            if (e->eventType.get() == RE::INPUT_EVENT_TYPE::kButton) {

                bool _toggle = false; // switch for 1st/3rd person

                RE::ButtonEvent* a_event = e->AsButtonEvent();

                uint32_t keyMask = a_event->idCode;
                uint32_t _device = a_event->GetDevice();
                if (!zoom_enabled) {
                    if (_device == 0 && keyMask != toggle_code_kb && keyMask != zoom_enabler_kb) continue;
                    if (_device == 2 && keyMask != toggle_code_gp && keyMask != zoom_enabler_gp) continue;
                }

                float duration = a_event->heldDownSecs;
                bool isPressed = a_event->value != 0 && duration >= 0;
                bool isReleased = a_event->value == 0 && duration != 0;
                
                player_cam = RE::PlayerCamera::GetSingleton();

                if (isPressed && !(_device ? zoom_enabler_gp < 0 : zoom_enabler_kb < 0)) {  // if zoom is possible
                    if (!zoom_enabled &&  (keyMask == zoom_enabler_kb || keyMask == zoom_enabler_gp)) {
                        zoom_enabled = true;
                        logger::info("Zoom enabled");
                        continue;
                    }
                    if (zoom_enabled) {
                        if (!player_cam->IsInThirdPerson() || !_device) continue;
                        auto thirdPersonState = static_cast<RE::ThirdPersonState*>(player_cam->cameraStates[RE::CameraState::kThirdPerson].get());
                        float zoom_amount = (_device % 2) ? 0.1f : 0.025f;
                        if (keyMask == zoom_in_code_m && _device == 1 || keyMask == zoom_in_code_gp && _device == 2) {
                            thirdPersonState->targetZoomOffset =
                                std::max(thirdPersonState->targetZoomOffset - zoom_amount, -0.2f);
                            logger::info("Zooming in");
                        } 
                        else if (keyMask == zoom_out_code_m && _device == 1 || keyMask == zoom_out_code_gp && _device == 2) {
                            thirdPersonState->targetZoomOffset =
                                std::min(thirdPersonState->targetZoomOffset + zoom_amount, 1.0f);
                            logger::info("Zooming out");
                        }
                    }
                } 
                else if (isReleased) {
                    if (keyMask == zoom_enabler_kb || keyMask == zoom_enabler_gp) {
                        zoom_enabled = false;
                        logger::info("Zoom disabled");
                    } 
                    else if (keyMask == toggle_code_kb || keyMask == toggle_code_gp) {
                        _toggle = true;
                    }
                }
                if (_toggle) {
                    logger::info("Toggling camera");
                    ToggleDialogueCam(player_cam);
                    _toggle = false;
                }
                player_cam = nullptr;
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

SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    
    return true;
}


