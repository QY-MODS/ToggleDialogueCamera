#include <cassert>

#include "Settings.h"

RE::UI* ui = nullptr;

bool zoom_enabled = false;
Settings::Settings* settings = nullptr;
bool PostPostLoaded = false;
bool InputLoaded = false;
bool listen_auto_zoom = true;
bool listen_gradual_zoom = false;



class OnCameraUpdate {
public:
    static void Install() {
        REL::Relocation<std::uintptr_t> hook1{REL::RelocationID(49852, 50784)};  // 84AB90, 876700

        auto& trampoline = SKSE::GetTrampoline();
        trampoline.create(14);
        _Update = trampoline.write_call<5>(hook1.address() + REL::Relocate(0x1A6, 0x1A6),
                                           Update);  // 84AD36, 8768A6
    }

private:
    static void Update(RE::TESCamera* a_this);

    static inline REL::Relocation<decltype(Update)> _Update;
};
void OnCameraUpdate::Update(RE::TESCamera* a_camera) {
    _Update(a_camera);
    if (listen_gradual_zoom) {
        auto* thirdPersonState = static_cast<RE::ThirdPersonState*>(a_camera->currentState.get());
        if (thirdPersonState->currentZoomOffset < -0.19f) {
            listen_gradual_zoom = false;
            logger::info("Forcing 1st after gradual");
            RE::PlayerCamera::GetSingleton()->ForceFirstPerson();
            logger::info("listen_gradual_zoom = false");
            listen_auto_zoom = true;
        }
    }
}


void ToggleDialogueCam() {
    listen_gradual_zoom = false; listen_auto_zoom = false;
    logger::info("listen_gradual_zoom = false,ToggleDialogueCam");
    auto plyr_c = RE::PlayerCamera::GetSingleton();
    auto thirdPersonState =
        static_cast<RE::ThirdPersonState*>(plyr_c->cameraStates[RE::CameraState::kThirdPerson].get());
    if (plyr_c->IsInFirstPerson()) {
        plyr_c->ForceThirdPerson();
        thirdPersonState->targetZoomOffset = thirdPersonState->savedZoomOffset;
    }
    else if (plyr_c->IsInThirdPerson()) {
        thirdPersonState->savedZoomOffset = thirdPersonState->currentZoomOffset;
        if (!Settings::os::instant_toggle) {
            listen_gradual_zoom = true;
            logger::info("listen_gradual_zoom = true,ToggleDialogueCam");
            thirdPersonState->targetZoomOffset = -0.2f;
            logger::info("Player is in 3rd person, gradually zooming in.");
            return;
        }
        plyr_c->ForceFirstPerson();
    }
    else {
        logger::info("Player is in neither 1st nor 3rd person.");
    }
    listen_auto_zoom = true;
};

const char* get_purpose(int dn, int kk) {
    const char* failed = "failed";
    switch (dn) {
    case 0:
        for (std::size_t i = 0; i < settings->keyboard.size(); ++i) {
            const auto& pair = settings->keyboard[i];
            if (pair.second == kk) return pair.first;
        }
        return failed;
    case 1:
        for (std::size_t i = 0; i < settings->mouse.size(); ++i) {
            const auto& pair = settings->mouse[i];
            if (pair.second == kk) return pair.first;
        }
        return failed;
    case 2:
        for (std::size_t i = 0; i < settings->gamepad.size(); ++i) {
            const auto& pair = settings->gamepad[i];
            if (pair.second == kk) return pair.first;
        }
        return failed;
    default:
        return failed;
    }
}

bool EqStr(const char* str1, const char* str2) { return std::strcmp(str1, str2) == 0; }

void PlayerCameraZoom(int a_device, bool _in) {
    listen_gradual_zoom = false; listen_auto_zoom = false;
    logger::info("listen_gradual_zoom = false,PlayerCameraZoom");
    auto player_cam = RE::PlayerCamera::GetSingleton();
    auto is_in_first = player_cam->IsInFirstPerson();

    auto thirdPersonState =
        static_cast<RE::ThirdPersonState*>(player_cam->cameraStates[RE::CameraState::kThirdPerson].get());
    float amount = (a_device % 2) ? 0.1f : 0.025f;
    if (_in) {
        //logger::info("Zooming in.");
        if (is_in_first);
        else if (thirdPersonState->currentZoomOffset < -0.19f) {
            player_cam->ForceFirstPerson();
        } else thirdPersonState->targetZoomOffset = std::max(thirdPersonState->targetZoomOffset - amount, -0.2f);
    }
    else {
        //logger::info("Zooming out.");
        if (is_in_first) {
            player_cam->ForceThirdPerson();
        } else thirdPersonState->targetZoomOffset = std::min(thirdPersonState->targetZoomOffset + amount, 1.0f);
    }
    listen_auto_zoom = true;
}



using InputEvents = RE::InputEvent*;
class OurEventSink :
    public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
    public RE::BSTEventSink<RE::InputEvent*> {
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

    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event,
                                          RE::BSTEventSource<RE::MenuOpenCloseEvent>*) {

        if (!event) return RE::BSEventNotifyControl::kContinue;
        if (!listen_auto_zoom) return RE::BSEventNotifyControl::kContinue;
        if (event->menuName != Settings::dialogue_menu_str) return RE::BSEventNotifyControl::kContinue;
        //if (!ui->IsMenuOpen(Settings::dialogue_menu_str)) return RE::BSEventNotifyControl::kContinue;

        if (event->opening && RE::PlayerCamera::GetSingleton()->IsInThirdPerson()){
            logger::info("Player is in 3rd person, forcing 1st person.(MenuOpenCloseEvent)");
            ToggleDialogueCam();
        } 
        else if (!event->opening && RE::PlayerCamera::GetSingleton()->IsInFirstPerson()) {
            logger::info("Player is in 1st person, forcing 3rd person.(MenuOpenCloseEvent)");
			ToggleDialogueCam();
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* evns, RE::BSTEventSource<RE::InputEvent*>*) {
        
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
                        // logger::info("Zoom enabled");
                    } 
                    else if (EqStr(purpose, "zoom+") &&
                                (zoom_enabled || _device == 2 && settings->gamepad[0].second < 0 ||
                                _device == 1 && settings->keyboard[0].second < 0)) {
                        PlayerCameraZoom(_device, true);
                    } 
                    else if (EqStr(purpose, "zoom-") &&
                                (zoom_enabled || _device == 2 && settings->gamepad[0].second < 0 ||
                                _device == 1 && settings->keyboard[0].second < 0)) {
                        PlayerCameraZoom(_device, false);
                    }
                } 
                else if (isReleased) {
                    if (EqStr(purpose, "zoom")) {
                        zoom_enabled = false;
                        // logger::info("Zoom disabled");
                    } else if (EqStr(purpose, "toggle")) {
                        _toggle = true;
                    }
                }
                if (_toggle) {
                    // logger::info("Toggling camera");
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
                if (Settings::os::auto_zoom) ui->AddEventSink<RE::MenuOpenCloseEvent>(OurEventSink::GetSingleton());
                PostPostLoaded = true;
            }
            break;
    }
};

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SetupLog();
    SKSE::Init(skse);
    logger::info("Plugin loaded.");
    auto loaded = Settings::LoadSettings(); 
    if (loaded) logger::info("Settings loaded.");
    else logger::info("Could not load settings.");
    assert(loaded && "Could not load settings from ini file!");
    settings = Settings::Settings::GetSingleton();
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    if (!Settings::os::instant_toggle) OnCameraUpdate::Install();

    return true;
}
