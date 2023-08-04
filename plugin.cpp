#include "logger.h"


bool toggle_pressed = false;
float last_zoom = 0;

class OnCameraUpdate {
public:
    static void Install() {
        REL::Relocation<std::uintptr_t> hook1{REL::RelocationID(49852, 50784)};  // 84AB90, 876700

        auto& trampoline = SKSE::GetTrampoline();
        trampoline.create(16);
        _Update = trampoline.write_call<5>(hook1.address() + REL::Relocate(0x1A6, 0x1A6),
                                           Update);  // 84AD36, 8768A6
    }

private:
    static void Update(RE::TESCamera* a_this);

    static inline REL::Relocation<decltype(Update)> _Update;
};
void OnCameraUpdate::Update(RE::TESCamera* a_camera) {
    _Update(a_camera);
    if (toggle_pressed) {
        auto* thirdPersonState = static_cast<RE::ThirdPersonState*>(a_camera->currentState.get());
        thirdPersonState->targetZoomOffset = last_zoom;
        toggle_pressed = false;
        last_zoom = 0;
    }
}
RE::UI* ui = nullptr;
RE::PlayerCamera* player_cam = nullptr;
RE::BSInputDeviceManager* input_device_manager = nullptr;
const std::string_view& dialogue_menu_str = "Dialogue Menu";
constexpr std::uint32_t toggle_code_keyboard = 33;
constexpr std::uint32_t toggle_code_gamepad = 128;
bool PostPostLoaded = false;
bool InputLoaded = false;

void ToggleDialogueCam(RE::PlayerCamera* plyr_c) {
    if (plyr_c->IsInFirstPerson()) {
        if (!last_zoom) {
            auto thirdPersonState =
                static_cast<RE::ThirdPersonState*>(plyr_c->cameraStates[RE::CameraState::kThirdPerson].get());
            last_zoom = thirdPersonState->savedZoomOffset;
        }
        plyr_c->ForceThirdPerson();
        toggle_pressed = true;
    } else if (plyr_c->IsInThirdPerson()) {
        if (!last_zoom) {
            auto thirdPersonState =
                static_cast<RE::ThirdPersonState*>(plyr_c->cameraStates[RE::CameraState::kThirdPerson].get());
            last_zoom = thirdPersonState->currentZoomOffset;
        }
        plyr_c->ForceFirstPerson();
    } else {
        logger::info("Player is in neither 1st nor 3rd person.");
    }
};

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

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr, RE::BSTEventSource<RE::InputEvent*>*) {
        if (!eventPtr) return RE::BSEventNotifyControl::kContinue;

        auto* event = *eventPtr;
        if (!event) return RE::BSEventNotifyControl::kContinue;

        if (!(ui->IsMenuOpen(dialogue_menu_str))) return RE::BSEventNotifyControl::kContinue;
        if (event->GetEventType() != RE::INPUT_EVENT_TYPE::kButton) return RE::BSEventNotifyControl::kContinue;
        auto* buttonEvent = event->AsButtonEvent();
        if (!(buttonEvent->IsUp())) return RE::BSEventNotifyControl::kContinue;
        bool _toggle = false;
        auto dxScanCode = buttonEvent->GetIDCode();
        
        // 0 = keyboard,1 = mouse,  2 = gamepad
        int _device = event->GetDevice();
        
        if (_device == 0 && dxScanCode == toggle_code_keyboard) {
             _toggle = true;
        } else if (_device == 2 && dxScanCode == toggle_code_gamepad) {
		    _toggle = true;
        }
        if (_toggle) {
            player_cam = RE::PlayerCamera::GetSingleton();
            ToggleDialogueCam(player_cam);
            player_cam = nullptr;
        }

        return RE::BSEventNotifyControl::kContinue;
    }
};


void OnMessage(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kInputLoaded:
            if (!InputLoaded) {
                input_device_manager = RE::BSInputDeviceManager::GetSingleton();
                input_device_manager->AddEventSink(OurEventSink::GetSingleton());
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
    OnCameraUpdate::Install();
    
    return true;
}


