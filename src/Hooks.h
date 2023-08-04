#include "PCH.h"

class OnCameraUpdate {
public:
    static void Install() {
        REL::Relocation<std::uintptr_t> hook1{REL::RelocationID(49852, 50784)};  // 84AB90, 876700

        auto& trampoline = SKSE::GetTrampoline();
        _TESCamera_Update = trampoline.write_call<5>(hook1.address() + REL::Relocate(0x1A6, 0x1A6),
                                                     TESCamera_Update);  // 84AD36, 8768A6
    }

private:
    static void TESCamera_Update(RE::TESCamera* a_this);

    static inline REL::Relocation<decltype(TESCamera_Update)> _TESCamera_Update;
};