
using namespace DKUtil::Alias;


namespace
{
	// 4.1.1.3624901
	void Patch1_Commit()
	{
		auto* entry = dku::Hook::Assembly::search_pattern<
			"40 57 "
			"48 83 EC 30 "
			"48 8B 0D ?? ?? ?? ?? "
			"48 8B FA "
			"48 81 C1 D0 00 00 00 "
			"E8 ?? ?? ?? ?? "
			"48 8B C8 "
			"E8 ?? ?? ?? ?? "
			"84 C0">();
		auto* patch = dku::Hook::adjust_pointer(entry, 0x26);

		if (entry && dku::Hook::Assembly::make_pattern<"74 ??">().match(AsAddress(patch))) {
			dku::Hook::WriteImm(patch, static_cast<dku::Hook::Imm8>(0xEB));
			INFO("patch 1 committed: {:X}", AsAddress(patch));
		}
	}

	void Patch2_Commit()
	{
		auto* entry = dku::Hook::Assembly::search_pattern<
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
			"84 C0">();
		auto* patch = dku::Hook::adjust_pointer(entry, 0x43);

		if (entry && dku::Hook::Assembly::make_pattern<"75 ??">().match(AsAddress(patch))) {
			dku::Hook::WriteImm(patch, static_cast<dku::Hook::Imm16>(0x9090));
			INFO("patch 2 committed: {:X}", AsAddress(patch));
		}
	}
}


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
		auto& base = dku::Hook::Module::get();
		auto [start, size] = base.section(dku::Hook::Module::Section::textx);
		INFO("module: {:X} textx: {:X} {:X}", base.base(), start, size);

		Patch1_Commit();
		Patch2_Commit();
	}

	return TRUE;
}
