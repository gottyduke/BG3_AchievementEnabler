
using namespace DKUtil::Alias;

namespace
{
	static constexpr auto TestAlByte = dku::Hook::Assembly::make_pattern<"84 C0">();
	static constexpr dku::Hook::OpCode Xor = 0x33;

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
			"E8 ?? ?? ?? ??">();
		auto patch = AsAddress(dku::Hook::adjust_pointer(entry, 0x24));

		if (entry && TestAlByte.match(patch)) {
			dku::Hook::WriteImm(patch, Xor);
			INFO("patch 1 committed : {:X}", patch);
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
			"E8 ?? ?? ?? ??">();
		auto patch = AsAddress(dku::Hook::adjust_pointer(entry, 0x41));

		if (entry && TestAlByte.match(patch)) {
			dku::Hook::WriteImm(patch, Xor);
			INFO("patch 2 committed : {:X}", patch);
		}
	}

	void Patch3_Commit()
	{
		auto* entry = dku::Hook::Assembly::search_pattern<
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
			"E8 ?? ?? ?? ??">();
		auto patch = AsAddress(dku::Hook::adjust_pointer(entry, 0x4A));

		if (entry && TestAlByte.match(patch)) {
			dku::Hook::WriteImm(patch, Xor);
			INFO("patch 3 committed : {:X}", patch);
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

		Patch1_Commit();
		Patch2_Commit();
		Patch3_Commit();
	}

	return TRUE;
}
