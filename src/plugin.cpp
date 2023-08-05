#include "logger.h"


RE::UI* ui = nullptr;
RE::PlayerCamera* player_cam = nullptr;
RE::BSInputDeviceManager* input_device_manager = nullptr;
const std::string_view& dialogue_menu_str = "Dialogue Menu";
constexpr std::uint32_t toggle_code_keyboard = 33;
constexpr std::uint32_t toggle_code_gamepad = 128;
constexpr std::uint32_t zoom_code_keyboard = 29;
constexpr std::uint32_t zoom_code_gamepad = 64;
bool PostPostLoaded = false;
bool InputLoaded = false;
bool zoom_enabled = false;

using InputEvents = RE::InputEvent*;
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
	if (!*evns)
		return RE::BSEventNotifyControl::kContinue;

	for (RE::InputEvent* e = *evns; e; e = e->next) {
		switch (e->eventType.get()) {
            case RE::INPUT_EVENT_TYPE::kButton :
				RE::ButtonEvent* a_event = e->AsButtonEvent();

				uint32_t keyMask = a_event->idCode;

				float duration = a_event->heldDownSecs;
				bool isPressed = a_event->value != 0 && duration >= 0;
				bool isReleased = a_event->value == 0 && duration != 0;

				if (isPressed) {
                    if ((keyMask == zoom_code_keyboard || keyMask == zoom_code_gamepad) && !zoom_enabled) {
                        zoom_enabled = true;
                        continue;
                    }
                    if (zoom_enabled){
                        if (keyMask == 8) {
                            auto thirdPersonState = static_cast<RE::ThirdPersonState*>(
                                RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
                            thirdPersonState->targetZoomOffset = std::max(thirdPersonState->targetZoomOffset - 0.1f, -0.2f);
                        } else if (keyMask == 9) {
                            auto thirdPersonState = static_cast<RE::ThirdPersonState*>(
                                RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
                            thirdPersonState->targetZoomOffset = std::min(thirdPersonState->targetZoomOffset + 0.1f, 1.0f);
                        }
                    }
				}
				else if (isReleased) {
                    if (keyMask == zoom_code_keyboard || keyMask == zoom_code_gamepad) {
                        zoom_enabled = false;
                    }
				}
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}
};

auto* eventSink = OurEventSink::GetSingleton();
void OnMessage(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kInputLoaded:
            if (!InputLoaded) {
                RE::BSInputDeviceManager::GetSingleton()->AddEventSink(eventSink);
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
    SKSE::Init(skse);
    SetupLog();
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

    return true;
}