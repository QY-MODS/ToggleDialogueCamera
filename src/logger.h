#include <spdlog/sinks/basic_file_sink.h>
#include "PCH.h"
namespace logger = SKSE::log;

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
    logger::info("Name of the plugin is {}.", pluginName);
}


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
            break;
    }
};

void LogNP3(RE::NiPoint3 np) { logger::info("({},{},{})", np.x, np.y, np.z); }
void LogNP4(RE::NiQuaternion np) { logger::info("({},{},{},{})", np.w,np.x, np.y, np.z); }