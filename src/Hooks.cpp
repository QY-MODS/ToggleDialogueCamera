#include "Hooks.h"
#include "logger.h"

// https://github.com/ersh1/Precision/blob/main/src/Hooks.h

void OnCameraUpdate::TESCamera_Update(RE::TESCamera* a_this) {
    /*_TESCamera_Update(a_this);
 	RE::NiUpdateData updateData;
    a_this->cameraRoot->UpdateDownwardPass(updateData, 0);*/
    logger::info("TESCamera_Update");
}