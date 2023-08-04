#include "logger.h"

bool toggle_pressed = false;

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
        /*logger::info("{} {} {}", a_camera->cameraRoot->local.translate.x, a_camera->cameraRoot->local.translate.y,
                     a_camera->cameraRoot->local.translate.z);
        logger::info("{} {} {}", a_camera->cameraRoot->local.rotate.entry[0][0],
                     a_camera->cameraRoot->local.rotate.entry[0][1], a_camera->cameraRoot->local.rotate.entry[0][2]);*/
        /*a_camera->cameraRoot->local.translate += {105.0f, 105.0f, 105.0f};
        RE::NiUpdateData updateData;
        a_camera->cameraRoot->UpdateDownwardPass(updateData, 0);*/
        //toggle_pressed = false;
    }
}

RE::UI* ui = nullptr;
RE::PlayerCamera* player_cam = nullptr;
RE::PlayerControls* player_controls = nullptr;
RE::BSInputDeviceManager* input_device_manager = nullptr;
const std::string_view& dialogue_menu_str = "Dialogue Menu";
constexpr std::uint32_t toggle_code_keyboard = 47;
constexpr std::uint32_t toggle_code_gamepad = 128;
bool PostPostLoaded = false;
bool InputLoaded = false;

float GetZoomDistance(RE::PlayerCamera* plyr_c) {
    plyr_c->cameraRoot->local.translate;
    return 0;
}
    //void ToggleDialogueCam(RE::PlayerCamera* plyr_c) {
//    if (plyr_c->IsInFirstPerson()) {
//        // logger::info("Player is in 1st person.");
//        plyr_c->ForceThirdPerson();
//    } else if (plyr_c->IsInThirdPerson()) {
//        plyr_c->ForceFirstPerson();
//        // logger::info("Player is in 3rd person.");
//    } else {
//        logger::info("Player is in neither 1st nor 3rd person.");
//    }
//};

class OurEventSink : public RE::BSTEventSink<SKSE::CameraEvent>,
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

    RE::BSEventNotifyControl ProcessEvent(const SKSE::CameraEvent* a_event, RE::BSTEventSource<SKSE::CameraEvent>*) {
        if (!a_event) return RE::BSEventNotifyControl::kContinue;
        if (!a_event->oldState) return RE::BSEventNotifyControl::kContinue;
        if (!a_event->newState) return RE::BSEventNotifyControl::kContinue;
        if (toggle_pressed) return RE::BSEventNotifyControl::kContinue;

        /*if (a_event->oldState->id == RE::CameraStates::kThirdPerson) {
            logger::info("Third person camera state.");
            logger::info("{} {} {}", a_event->oldState->camera->cameraRoot->local.translate.x,
                         a_event->oldState->camera->cameraRoot->local.translate.y,
                         a_event->oldState->camera->cameraRoot->local.translate.z);
            logger::info("{} {} {}", a_event->oldState->camera->cameraRoot->local.rotate.entry[0][0],
                         a_event->oldState->camera->cameraRoot->local.rotate.entry[0][1],
                         a_event->oldState->camera->cameraRoot->local.rotate.entry[0][2]);
        }*/
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr, RE::BSTEventSource<RE::InputEvent*>*) {
        if (!eventPtr) return RE::BSEventNotifyControl::kContinue;

        auto* event = *eventPtr;
        if (!event) return RE::BSEventNotifyControl::kContinue;

        // if (!(ui->IsMenuOpen(dialogue_menu_str))) return RE::BSEventNotifyControl::kContinue;
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
            logger::info("Toggle pressed.");
            auto thirdPersonState =
                static_cast<RE::ThirdPersonState*>(player_cam->cameraStates[RE::CameraState::kThirdPerson].get());
            logger::info("THIRD PERSON STATE");
            logger::info("savedZoomOffset: {} pitchZoomOffset: {}, currentZoomOffset: {}, targetZoomOffset: {}",
                         thirdPersonState->savedZoomOffset, thirdPersonState->pitchZoomOffset,thirdPersonState->currentZoomOffset, thirdPersonState->targetZoomOffset);
            logger::info("posOffsetActual:");
            LogNP3(thirdPersonState->posOffsetActual);
            logger::info("posOffsetExpected:");
            LogNP3(thirdPersonState->posOffsetExpected);
            logger::info("rotation:");
            LogNP4(thirdPersonState->rotation);
            logger::info("translation:");
            LogNP3(thirdPersonState->translation);

            logger::info("FIRST PERSON STATE");
            auto firstPersonState = static_cast<RE::FirstPersonState*>(player_cam->cameraStates[RE::CameraState::kFirstPerson].get());
            logger::info("currentPitchOffset: {} targetPitchOffset: {}, sittingRotation: {}",
			        firstPersonState->currentPitchOffset, firstPersonState->targetPitchOffset, firstPersonState->sittingRotation);
            logger::info("lastPosition:");
            LogNP3(firstPersonState->lastPosition);
            // ToggleDialogueCam(player_cam);
            toggle_pressed = !toggle_pressed;
            player_cam = nullptr;
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
                SKSE::GetCameraEventSource()->AddEventSink(eventSink);
                PostPostLoaded = true;
            }
            break;
    }
};

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SetupLog();
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    OnCameraUpdate::Install();

    return true;
}