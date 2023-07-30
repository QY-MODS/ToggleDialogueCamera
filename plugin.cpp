
#include <spdlog/sinks/basic_file_sink.h>


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
    RE::ConsoleLog* asd = RE::ConsoleLog::GetSingleton();
    const char* a = message->sender;

    switch (message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            asd->Print(a);
            asd->Print("asd");

            // get plugin names
            /*const auto handler = RE::TESDataHandler::GetSingleton();
            for (auto& mods = handler->files; const auto mod : mods)
                logger::info("{}", mod->GetFilename());*/
            /////////////////////////////////////
            
            break;
        case SKSE::MessagingInterface::kNewGame:
            asd->Print("New game loaded.");
            
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            logger::info("Post post loaded.");
            logger::info("VS Code.");
            
            break;
        case SKSE::MessagingInterface::kPostLoad:
            auto* resss = RE::GetINISetting("Resolution:Render");
            if (!resss) logger::info("nop1");
            else {
                logger::info("ini setting: {}", resss->GetString());
            }

            auto* generalll = RE::GetINISetting("sLanguage:General");
            if (!generalll) 
                logger::info("nop2");
            else {
                logger::info("ini setting: {}", generalll->GetString());
            }
            break;
    }
};


SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    
    auto* iSizeW = RE::GetINISetting("iSize W:Display");
    if (iSizeW)
    {
        logger::info("ini setting: {}", iSizeW->GetSInt());
        iSizeW->data.i = 160;
        logger::info("ini setting: {}", iSizeW->GetSInt());
        auto* gamesettings_collection = RE::GameSettingCollection::GetSingleton();
        gamesettings_collection->WriteSetting(iSizeW);
        logger::info("I reached here");
    }

    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(PrintToConsole);
    return true;
}