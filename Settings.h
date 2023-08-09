#pragma once

// keyboard-gp & mouse-gp
using KeyValuePair = std::pair<const char*, int>;

std::array<KeyValuePair, 2> keyboard = {{{"zoom", 29}, {"toggle", 33}}};  // 29 , 33
std::array<KeyValuePair, 2> mouse = {{{"zoom+", 8}, {"zoom-", 9}}};       // 8 , 9
std::array<KeyValuePair, 4> gamepad = {
    {{"zoom", 64}, {"toggle", 128}, {"zoom+", 10}, {"zoom-", 512}}};  // 64 , 128 , 10 , 512


namespace Settings {
    
    const std::string_view& dialogue_menu_str = "Dialogue Menu";
    constexpr auto path = L"Data/SKSE/Plugins/DialogueCameraToggle.ini";

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

    //bool LoadSettings() { return true; };
    bool LoadSettings();
    
};