#include <spdlog/sinks/basic_file_sink.h>
#include "logger.h"
////https://github.com/aspck/SkyrimVR-CommonLib-Example
//#include "customEventSink.h"

RE::UI* ui = nullptr;
RE::PlayerCamera* player_cam = nullptr;
RE::PlayerControls* player_controls = nullptr;
RE::BSInputDeviceManager* input_device_manager = nullptr;
bool _usingGamepad = false;
const std::string_view& dialogue_menu_str = "Dialogue Menu";
constexpr std::uint32_t toggle_code_keyboard = 33;
constexpr std::uint32_t toggle_code_gamepad = 128;
bool PostPostLoaded = false;
bool InputLoaded = false;

void ToggleDialogueCam(RE::PlayerCamera* plyr_c) {
    if (plyr_c->IsInFirstPerson()) {
        //logger::info("Player is in 1st person.");
        plyr_c->ForceThirdPerson();
    } else if (plyr_c->IsInThirdPerson()) {
        plyr_c->ForceFirstPerson();
        //logger::info("Player is in 3rd person.");
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
    
    return true;
}


