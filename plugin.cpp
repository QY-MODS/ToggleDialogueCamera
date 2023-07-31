#include <spdlog/sinks/basic_file_sink.h>
#include "logger.h"
#include "customEventSink.h"

RE::UI* ui = nullptr;
RE::PlayerCamera* player_cam = nullptr;
const std::string_view& dialogue_menu_str = "Dialogue Menu";
constexpr std::uint32_t first_person_keyboard = 33;
bool PostPostLoaded = false;
bool InputLoaded = false;

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

        if (event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton && ui->IsMenuOpen(dialogue_menu_str)) {
            auto* buttonEvent = event->AsButtonEvent();
            auto dxScanCode = buttonEvent->GetIDCode();
            auto _heldDownSecs = buttonEvent->heldDownSecs;
            player_cam = RE::PlayerCamera::GetSingleton();
            if (dxScanCode == first_person_keyboard && buttonEvent->IsUp()) {
                if (player_cam->IsInFirstPerson()) {
                    logger::info("Player is in 1st person.");
                    player_cam->ForceThirdPerson();
                }
                else if (player_cam->IsInThirdPerson()) {
					player_cam->ForceFirstPerson();
                    logger::info("Player is in 3rd person.");
                } else {
                    logger::info("Player is in neither 1st nor 3rd person.");
                }
                player_cam = nullptr;
                logger::info("Pressed key {} for {} seconds", dxScanCode, _heldDownSecs);
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


