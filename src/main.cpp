#include "RE/Noesis.h"
#include "DKUtil/Config.hpp"

using namespace DKUtil::Alias;

namespace Patches
{
	using patch_entry = std::tuple<void*, std::ptrdiff_t, const Patch*>;

	// 4.1.1.4079877
	namespace IsAddonLoaded
	{
		constexpr Patch Jmp{
			"\xEB",
			1
		};

		constexpr Patch Nop6{
			"\x90\x90\x90\x90\x90\x90",
			6
		};

		void Commit()
		{
			patch_entry Patches[] = {
				{ dku::Hook::Assembly::search_pattern<
                      "48 89 5C 24 08 "       //mov     [rsp+8], rbx
                      "48 89 7C 24 10 "       //mov     [rsp+10h], rdi
                      "8B 79 14 "             //mov     edi, [rcx+14h]
                      "33 DB "                //xor     ebx, ebx
                      "48 85 FF "             //test    rdi, rdi
                      "74 ?? "                //jz      short loc_1431B3680 [ => to patch, jz => jmp ]
                      "4C 8B 59 08 "          //mov     r11, [rcx+8]
                      "8B 0D ?? ?? ?? ?? "    //mov     ecx, cs:dword_145725CD8
                      "8B 05 ?? ?? ?? ?? "    //mov     eax, cs:dword_145725C3C
                      "8B 15 ?? ?? ?? ??">(), //mov     edx, cs:dword_145725DCC
					0x12, &Jmp },

				// new version (patch 5)
				{ dku::Hook::Assembly::search_pattern<
                      "E8 ?? ?? ?? ?? "         // call    sub_1419374B0
                      "E9 ?? ?? ?? ?? "         // jmp     loc_1419DCF8E
                      "4C 8D 45 B0 "            // lea     r8, [rbp-50h]
                      "49 8B D6 "               // mov     rdx, r14
                      "49 8B CC "               // mov     rcx, r12
                      "83 BD CC 01 00 00 00 "   // cmp     dword ptr [rbp+1CCh], 0
                      "0F 85 ?? ?? ?? ?? "      // jnz     loc_1419DCD8B [ => to patch, nop6 ]
                      "E8 ?? ?? ?? ?? "         // call    sub_141EAC3F0
                      "84 C0">(),               // test    al, al
					0x1b, &Nop6 },
			};

			auto files = dku::Config::GetAllFiles<false>({}, ".dll");
			auto win = true;
			for (auto& file : files) {
				if (dku::string::iends_with(file, "dwrite.dll")) {
					WARN("bg3se installed, disabled self");
					win = false;
				}
			}

			for (auto i = 0; i < std::extent_v<decltype(Patches)>; ++i) {
				auto& [entry, offset, patch] = Patches[i];

				if (entry) {
					auto addr = AsAddress(dku::Hook::adjust_pointer(entry, offset));

					if (win) {
						dku::Hook::WritePatch(addr, patch);
						INFO("IsAddonLoaded patch {} installed at {:X}", i + 1, addr);
					}
				} else {
					WARN("IsAddonLoaded patch {} cannot be found", i + 1);
				}
			}
		}
	}  // namespace IsAddonLoaded

	class UiWidgetCreator
	{
		// game loads widget from "Widgets\\" folder
		static void Hook_CreateWidget(
			void* a_uiManager,
			void* a_unk2,
			const char** a_xaml,  // heap
			RE::Noesis::XamlLoadRequest* a_request,
			void* a_allocator,
			bool a_flag)
		{
			return _LoadFromFile(a_uiManager, a_unk2, a_xaml, a_request, a_allocator, a_flag);
		}

		static inline std::add_pointer_t<decltype(Hook_CreateWidget)> _LoadFromFile{ nullptr };

	public:
		static constexpr OpCode RelocPointer[4]{
			0x48, 0x89, 0xC8,  // mov rax, rcx
			0xC3               // retn
		};

		static void Commit()
		{
			// CreateWidget callsite
			auto* addr = dku::Hook::Assembly::search_pattern<
				"48 8B CF "
				"E8 ?? ?? ?? ?? "
				"90 "
				"83 7D 74 10 "
				"72 ?? "
				"48 8B 4D 60 "
				"E8 ?? ?? ?? ?? "
				"90 "
				"EB ?? "
				"0F B6 85 20 05 00 00 "
				"88 44 24 28 "
				"48 89 7C 24 20 "
				"4D 8B CE "
				"4C 8D 45 E8 "
				"48 8B D3 "
				"49 8B CD "
				"E8 ?? ?? ?? ??">();  // +313E0E2
			if (!addr) {
				// safe to fail
				WARN("CreateWidgetHook cannot be found!");
			} else {
				const auto callsite = AsAddress(addr) + 0x38;
				_LoadFromFile = dku::Hook::write_call<5>(callsite, Hook_CreateWidget);
				INFO("CreateWidgetHook installed at {:X}", callsite);
			}
		}

		static inline std::unordered_map<std::string_view, RE::ls::UIWidget*> WidgetMemoryMap;
	};
}  // namespace Patches

BOOL APIENTRY DllMain(HMODULE a_hModule, DWORD a_ul_reason_for_call, LPVOID a_lpReserved)
{
	if (a_ul_reason_for_call == DLL_PROCESS_ATTACH) {
#ifndef NDEBUG
		while (!IsDebuggerPresent()) {
			Sleep(100);
		}
#endif

		// stuff
		dku::Logger::Init(Plugin::NAME, std::to_string(Plugin::Version));

		INFO("game type : {}", dku::Hook::GetProcessName());

		dku::Hook::Trampoline::AllocTrampoline(1 << 6);
		Patches::IsAddonLoaded::Commit();
	}

	return TRUE;
}
