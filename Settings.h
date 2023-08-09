#pragma once
#include "SimpleIni.h"

// keyboard-gp & mouse-gp
using KeyValuePair = std::pair<const char*, int>;

std::array<KeyValuePair, 2> keyboard;  // 29 , 33
std::array<KeyValuePair, 2> mouse = {{{"zoom+", 8}, {"zoom-", 9}}};       // 8 , 9
std::array<KeyValuePair, 4> gamepad = {
    {{"zoom", 64}, {"toggle", 128}, {"zoom+", 10}, {"zoom-", 512}}};  // 64 , 128 , 10 , 512


namespace Settings {
    
    const std::string_view& dialogue_menu_str = "Dialogue Menu";
    constexpr auto path = L"Data/SKSE/Plugins/DialogueCameraToggle.ini";
    constexpr auto comment_zoom = ";Holding this key enables zoom. Set to -1 to disable.";
    constexpr auto comment_toggle = ";Press this key to switch between 1st and 3rd person view. Set to -1 to disable.";
    constexpr auto comment_zoom_plus = ";Key to zoom in. Set to -1 to disable.";
    constexpr auto comment_zoom_minus = ";Key to zoom out. Set to -1 to disable.";

    namespace kb {
        int zoom;
		int toggle;
	};

    namespace gp {
        int zoom;
        int toggle;
        int zoom_plus;
        int zoom_minus;
    };

    namespace mouse {
		int zoom_plus;
		int zoom_minus;
	};

    std::array<KeyValuePair, 2> keyboard = {{{"zoom", kb::zoom}, {"toggle", kb::toggle}}};

    void Set(CSimpleIniA& ini, const char* section, const char* key, int& val, int def_val, const char* comment){
        val = ini.GetLongValue(section, key, def_val);
        ini.SetLongValue(section, key, val, comment);
    };

    bool LoadSettings() {
        CSimpleIniA ini;
        ini.SetUnicode();

        auto err = ini.LoadFile(path);
        if (err < 0) return false;

        Set(ini, "Keyboard", "Zoom Subkey", kb::zoom, 29, comment_zoom);
        Set(ini, "Keyboard", "Toggle Subkey", kb::toggle, 33, comment_toggle);
        Set(ini, "Gamepad", "Zoom Subkey", gp::zoom, 64, comment_zoom);
        Set(ini, "Gamepad", "Toggle Subkey", gp::toggle, 128, comment_toggle);

        Set(ini, "Mouse", "Zoom In Key", mouse::zoom_plus, 8, comment_zoom_plus);
        Set(ini, "Mouse", "Zoom Out Key", mouse::zoom_minus, 9, comment_zoom_minus);
        Set(ini, "Gamepad", "Zoom In Key", gp::zoom_plus, 10, comment_zoom_plus);
        Set(ini, "Gamepad", "Zoom Out Key", gp::zoom_minus, 512, comment_zoom_minus);

        ini.SaveFile(path);

        return true;
    };
    
};