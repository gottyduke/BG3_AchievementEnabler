#include "RE/Noesis.h"
#include "DKUtil/Config.hpp"

using namespace DKUtil::Alias;

namespace Patches
{
	using patch_entry = std::tuple<void*, std::ptrdiff_t, const Patch*>;

	// 4.1.1.6897358
	namespace IsAddonLoaded
	{
		constexpr Patch Nop2 {
			"\x90\x90",
			2
		};

		void Commit()
		{
			patch_entry Patches[] = {
				/**
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
					  "E8 ?? ?? ?? ?? "        // call    sub_1419374B0
					  "E9 ?? ?? ?? ?? "        // jmp     loc_1419DCF8E
					  "4C 8D 45 B0 "           // lea     r8, [rbp-50h]
					  "49 8B D6 "              // mov     rdx, r14
					  "49 8B CC "              // mov     rcx, r12
					  "83 BD CC 01 00 00 00 "  // cmp     dword ptr [rbp+1CCh], 0
					  "0F 85 ?? ?? ?? ?? "     // jnz     loc_1419DCD8B [ => to patch, nop6 ]
					  "E8 ?? ?? ?? ?? "        // call    sub_141EAC3F0
					  "84 C0">(),              // test    al, al
					0x1b, &Nop6 },
				/**/
				// patch 8
				{ dku::Hook::Assembly::search_pattern<
						"E8 ?? ?? ?? ?? "		// call    sub_144167B00
						"90 "					// nop
						"40 84 FF "				// test    dil, dil
						"75 10 "				// jne     short loc_143BA8A05
						"48 FF C6 "				// inc     rsi
						"49 83 C6 78 "			// add     r14, 78h
						"48 3B F5 "				// cmp     rsi, rbp
						"0F 82 ?? ?? ?? ??">(),	// jb      loc_143BA8860 [ => to patch, jne => nop ]
					0x9, &Nop2 },
			};

			auto files = dku::Config::GetAllFiles<false>({}, ".dll");
			for (auto& file : files) {
				if (dku::string::iends_with(file, "dwrite.dll")) {
					WARN("bg3se installed, disabled self");
					return;
				}
			}

			for (auto i = 0; i < std::extent_v<decltype(Patches)>; ++i) {
				auto& [entry, offset, patch] = Patches[i];

				if (entry) {
					INFO("{:X}", AsAddress(entry));
					auto addr = AsAddress(dku::Hook::adjust_pointer(entry, offset));
					dku::Hook::WritePatch(addr, patch);
					INFO("IsAddonLoaded patch {} installed at {:X}", i + 1, addr);
				} else {
					WARN("IsAddonLoaded patch {} cannot be found", i + 1);
				}
			}
		}
	}  // namespace IsAddonLoaded
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

		INFO("game type : {}", dku::Hook::GetProcessName(::GetCurrentProcessId()));

		dku::Hook::Trampoline::AllocTrampoline(1 << 5);
		Patches::IsAddonLoaded::Commit();
	}

	return TRUE;
}
