#include "RE/Noesis.h"
#include "DKUtil/Config.hpp"

using namespace DKUtil::Alias;

namespace Patches
{
	using patch_entry = std::tuple<void*, std::ptrdiff_t, const Patch*>;

	// 4.1.1.3648072
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
				/**
				{ dku::Hook::Assembly::search_pattern<
					  "40 57 "
					  "48 83 EC 30 "
					  "48 8B 0D ?? ?? ?? ?? "
					  "48 8B FA "
					  "48 81 C1 D0 00 00 00 "
					  "E8 ?? ?? ?? ?? "
					  "48 8B C8 "
					  "E8 ?? ?? ?? ?? "
					  "84 C0">(),
					0x24, 0x33 },
				{ dku::Hook::Assembly::search_pattern<
					  "40 53 "
					  "56 "
					  "57 "
					  "48 81 EC D0 04 00 00 "
					  "48 8B 05 ?? ?? ?? ?? "
					  "48 33 C4 "
					  "48 89 84 24 C0 04 00 00 "
					  "48 8B D9 "
					  "49 8B F8 "
					  "48 8B 0D ?? ?? ?? ?? "
					  "48 8B F2 "
					  "48 81 C1 D0 00 00 00 "
					  "E8 ?? ?? ?? ?? "
					  "48 8B C8 "
					  "E8 ?? ?? ?? ?? "
					  "84 C0">(),
					0x41, 0x33 },
				{ dku::Hook::Assembly::search_pattern<
					  "40 53 "
					  "55 "
					  "56 "
					  "57 "
					  "41 56 "
					  "41 57 "
					  "48 81 EC ?? ?? ?? ?? "
					  "48 8B 05 ?? ?? ?? ?? "
					  "48 33 C4 "
					  "48 89 84 24 ?? ?? ?? ?? "
					  "48 8B D9 "
					  "45 0F B7 F9 "
					  "48 8B 0D ?? ?? ?? ?? "
					  "49 8B F8 "
					  "48 81 C1 ?? ?? ?? ?? "
					  "48 8B EA "
					  "E8 ?? ?? ?? ?? "
					  "48 8B C8 "
					  "E8 ?? ?? ?? ?? "
					  "84 C0">(),
					0x4A, 0x33 },
				/**/

				// signature:
				// 48 89 5C 24 08 8B 59 14 45 33 DB 48 85 DB
				{ dku::Hook::Assembly::search_pattern<
					  "48 89 5C 24 08 "       // mov     [rsp+arg_0], rbx
					  "8B 59 14 "             // mov     ebx, [rcx+14h]
					  "45 33 DB "             // xor     r11d, r11d
					  "48 85 DB "             // test    rbx, rbx
					  "74 ?? "                // jz      short loc_143280CE3 [ => to patch, jz => jmp ]
					  "4C 8B 51 08 "          // mov     r10, [rcx+8]
					  "8B 0D ?? ?? ?? ?? "    // mov     ecx, cs:dword_145A38228
					  "8B 05 ?? ?? ?? ?? "    // mov     eax, cs:dword_145A38190
					  "8B 15 ?? ?? ?? ??">(), // mov     edx, cs:dword_145A3831C
					0xE, &Jmp },

				// search:
				// 4C 8D 45 E0 48 8B D6 49 8B CC 83 78 0C 00
				{ dku::Hook::Assembly::search_pattern<
					  "48 8D 4D 10 "       // lea     rcx, [rbp+3A0h+var_390]
					  "E8 ?? ?? ?? ?? "    // call    sub_1424EAE00
					  "4C 8D 45 E0 "       // lea     r8, [rbp+3A0h+Src]
					  "48 8B D6 "          // mov     rdx,rsi
					  "49 8B CC "          // mov     rcx,r12
					  "83 78 0C 00 "       // cmp     dword ptr [rax+0Ch], 0
					  "0F 85 ?? ?? ?? ?? " // jnz     __skip0 [ => to patch, nop6 ]
					  "E8 ?? ?? ?? ?? "    // call    sub_1410839E0
					  "84 C0">(),          // test    al,al
					0x17, &Nop6 },

				// new version (patch 4)
				// search:
				// 4C 8D 45 B0 48 8B D6 49 8B CF 83 BD CC 01 00 00 00
				{ dku::Hook::Assembly::search_pattern<
					  "E8 ?? ?? ?? ?? "         // call    sub_141A30C90
					  "E9 ?? ?? ?? ?? "         // jmp     loc_141AD532C
					  "4C 8D 45 B0 "            // lea     r8, [rbp+370h+Src]
					  "48 8B D6 "               // mov     rdx, rsi
					  "49 8B CF "               // mov     rcx, r15
					  "83 BD CC 01 00 00 00 "   // cmp     [rbp+370h+var_1A4], 0
					  "0F 85 ?? ?? ?? ?? "      // jnz     __skip0 [ => to patch, nop6 ]
					  "E8 ?? ?? ?? ?? "         // call    sub_141F9FA80
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
			/**
			auto* widget = _LoadFromFile(a_uiManager, a_unk2, a_xaml, a_request, a_allocator, a_flag);
			WidgetMemoryMap[a_request->file.string_view()] = widget;
			return widget;
			/**/

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
			/**
			// reloc retn for CreateWidget
			auto* addr = dku::Hook::Assembly::search_pattern<
				"90 "
				"48 85 DB "
				"74 ?? "
				"48 8D 4B 18 "
				"48 8B 01 "
				"FF 50 20 "
				"90 "
				"EB ?? "
				"49 8D 4F 30 "
				"4C 8B C6 "
				"49 8B D6 "
				"E8 ?? ?? ?? ?? "
				"48 81 C4 88 00 00 00 "
				"41 5F "
				"41 5E "
				"41 5D "
				"41 5C "
				"5F "
				"5E "
				"5B "
				"5D "
				"C3">();  // +313DB99
			if (!addr) {
				// safe to fail
				WARN("CreateWidgetRetn cannot be found!");
			} else {
				const auto createWidgetRetn = AsAddress(addr) + 0x35;
				dku::Hook::WriteImm(createWidgetRetn, RelocPointer);
				INFO("CreateWidgetRetn installed at {:X}", createWidgetRetn);
			}
			/**/

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

		dku::Hook::Trampoline::AllocTrampoline(1 << 5);
		Patches::IsAddonLoaded::Commit();
		//Patches::UiWidgetCreator::Commit();
	}

	return TRUE;
}
