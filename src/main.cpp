
using namespace DKUtil::Alias;

namespace Patches
{
	static constexpr auto TestAlByte = dku::Hook::Assembly::make_pattern<"84 C0">();
	static constexpr dku::Hook::OpCode Xor = 0x33;

	using patch_entry = std::pair<void*, std::ptrdiff_t>;

	// 4.1.1.3624901
	void Commit()
	{
		patch_entry Patches[3] = {
			{ dku::Hook::Assembly::search_pattern<
				  "40 57 "
				  "48 83 EC 30 "
				  "48 8B 0D ?? ?? ?? ?? "
				  "48 8B FA "
				  "48 81 C1 D0 00 00 00 "
				  "E8 ?? ?? ?? ?? "
				  "48 8B C8 "
				  "E8 ?? ?? ?? ??">(),
				0x24 },
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
				  "E8 ?? ?? ?? ??">(),
				0x41 },
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
				  "E8 ?? ?? ?? ??">(),
				0x4A },
		};


		for (auto i = 0; i < std::extent_v<decltype(Patches)>; ++i) {
			auto& [entry, offset] = Patches[i];

			auto patch = AsAddress(dku::Hook::adjust_pointer(entry, offset));

			if (entry && TestAlByte.match(patch)) {
				INFO("patch {} committed at {:X}", i + 1, patch);
				dku::Hook::WriteImm(patch, Xor);
			} else {
				INFO("patch {} failed", i + 1);
			}
		}
	}
} // namespace Patches

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

		Patches::Commit();
	}

	return TRUE;
}
