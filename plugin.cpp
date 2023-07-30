
#include <spdlog/sinks/basic_file_sink.h>
#include "logger.h"

void PrintToConsole(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kDataLoaded:          
            break;
        case SKSE::MessagingInterface::kNewGame:
            break;
        case SKSE::MessagingInterface::kPostLoad:
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            logger::info("PostPostLoad");
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
            logger::info("PostLoadGame");
            /*auto* player_cam = RE::PlayerCamera::GetSingleton();
            if (player_cam->IsInFirstPerson()) {
                logger::info("Player is in 1st person.");
            } else if (player_cam->IsInThirdPerson()) {
                logger::info("Player is in 3rd person.");
            } else {
                logger::info("Player is in neither 1st nor 3rd person.");
            }*/
            break;
    }
};


SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();

    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(PrintToConsole);
    return true;
}