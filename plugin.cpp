#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include "logger.h"

/*auto* player_cam = RE::PlayerCamera::GetSingleton();
            if (player_cam->IsInFirstPerson()) {
                logger::info("Player is in 1st person.");
            } else if (player_cam->IsInThirdPerson()) {
                logger::info("Player is in 3rd person.");
            } else {
                logger::info("Player is in neither 1st nor 3rd person.");
            }*/

struct quantEventSink : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) {
        if (a_event->menuName == "Dialogue Menu" && a_event->opening) {
			logger::info("Dialogue Menu opened.");
		}
		return RE::BSEventNotifyControl::kContinue;
	}
};


auto* eventSink = new quantEventSink();
auto* eventSourceHolder = RE::PlayerControls::GetSingleton();



SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();

    SKSE::Init(skse);
    //SKSE::GetMessagingInterface()->RegisterListener(PrintToConsole);
    return true;
}