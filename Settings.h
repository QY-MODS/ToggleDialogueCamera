#pragma once
#include "SimpleIni.h"

// keyboard-gp & mouse-gp
using KeyValuePair = std::pair<const char*, int>;

namespace Settings {
    
    const std::string_view& dialogue_menu_str = "Dialogue Menu";
    constexpr auto path = L"Data/SKSE/Plugins/DialogueCameraToggle.ini";
    constexpr auto comment_zoom = ";Holding this key enables zoom. Set to -1 to disable.";
    constexpr auto comment_toggle = ";Press this key to switch between 1st and 3rd person view. Set to -1 to disable.";
    constexpr auto comment_zoom_plus = ";Key to zoom in. Set to -1 to disable.";
    constexpr auto comment_zoom_minus = ";Key to zoom out. Set to -1 to disable.";

    namespace kb {
        int zoom = 29;
		int toggle = 33;
        const char* zoom_str = "LCtrl";
        const char* toggle_str = "F";
        const auto comment_zoom = std::format(";Holding this key enables zoom. Default is {} ({}).", zoom, zoom_str);
        const auto comment_toggle =
            std::format(";Default is {} ({}).", toggle, toggle_str);
	};

    namespace gp {
        int zoom = 64;
        int toggle = 128;
        int zoom_plus = 10;
        int zoom_minus = 512;

        const char* zoom_str = "L3";
        const char* toggle_str = "R3";
        const char* zoom_plus_str = "R2";
		const char* zoom_minus_str = "R1";
        
        const auto comment_zoom = std::format(";Holding this key enables zoom. Default is {} ({}).", zoom, zoom_str);
        const auto comment_toggle = std::format(";Default is {} ({}).", toggle, toggle_str);
        const auto comment_zoom_plus = std::format(";Default is {} ({}).", zoom_plus, zoom_plus_str);
        const auto comment_zoom_minus = std::format(";Default is {} ({}).", zoom_minus, zoom_minus_str);

    };

    namespace ms {
		int zoom_plus = 264;
		int zoom_minus = 265;
        const char* zoom_plus_str = "Mouse Wheel Up";
        const char* zoom_minus_str = "Mouse Wheel Down";
		const auto comment_zoom_plus = std::format(";Default is {} ({}).", zoom_plus, zoom_plus_str);
		const auto comment_zoom_minus = std::format(";Default is {} ({}).", zoom_minus, zoom_minus_str);
	};

    void Set(CSimpleIniA& ini, const char* section, const char* key, int& val, const char* comment){
        val = ini.GetLongValue(section, key, (int)val);
        ini.SetLongValue(section, key, val, comment);
    };

    bool LoadSettings() {
        CSimpleIniA ini;
        ini.SetUnicode();

        auto err = ini.LoadFile(path);
        if (err < 0) return false;

        Set(ini, "Keyboard", "Zoom Subkey", kb::zoom, kb::comment_zoom.c_str());
        Set(ini, "Keyboard", "Toggle POV Key", kb::toggle, kb::comment_toggle.c_str());
        Set(ini, "Gamepad", "Zoom Subkey", gp::zoom, gp::comment_zoom.c_str());
        Set(ini, "Gamepad", "Toggle POV Key", gp::toggle, gp::comment_toggle.c_str());

        Set(ini, "Mouse", "Zoom In Key", ms::zoom_plus, ms::comment_zoom_plus.c_str());
        Set(ini, "Mouse", "Zoom Out Key", ms::zoom_minus, ms::comment_zoom_minus.c_str());
        Set(ini, "Gamepad", "Zoom In Key", gp::zoom_plus, gp::comment_zoom_plus.c_str());
        Set(ini, "Gamepad", "Zoom Out Key", gp::zoom_minus, gp::comment_zoom_minus.c_str());

        ini.SaveFile(path);

        return true;
    };

    class Settings {
    public:
        static Settings* GetSingleton() {
            static Settings singleton;
            return &singleton;
        }
        std::array<KeyValuePair, 2> keyboard = {{{"zoom", kb::zoom}, {"toggle", kb::toggle}}};
        std::array<KeyValuePair, 2> mouse = {
            {{"zoom+", ms::zoom_plus - 256}, {"zoom-", ms::zoom_minus - 256}}};  // 8 , 9
        std::array<KeyValuePair, 4> gamepad = {
            {{"zoom", gp::zoom}, {"toggle", gp::toggle}, {"zoom+", gp::zoom_plus}, {"zoom-", gp::zoom_minus}}};  // 64 , 128 , 10 , 512

    };
    
};