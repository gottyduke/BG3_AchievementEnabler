#pragma once

#define is_at(M, O, C) static_assert(offsetof(C, M) == O)
namespace RE
{
        namespace ls
	{
		// placeholder type
		struct UIWidget
		{};
	}

    namespace Noesis
    {
		// because MSVC compiles std::string differently in debug mode lol
		struct string
		{
			std::uint32_t length;
			std::uint16_t flag04;
			std::uint8_t unk06;
			bool unk07;
			char buffer[16]; // possible union with small string optimization

			constexpr auto string_view() const noexcept { return std::string_view{ buffer, length }; }
		};
		static_assert(sizeof(string) == 0x18);

		struct XamlLoadRequest
		{
			void* vtbl;
			std::uint32_t unk08;
			std::uint32_t unk0C;
			string file;  // "SomeWidget.xaml"
			std::uint64_t unk28;
			string stack;  // "HUD" / "Notification" / ""
			void* callable;
			string category;  // "None"
							  // skipped
		};
		is_at(file, 0x10, XamlLoadRequest);
		is_at(stack, 0x30, XamlLoadRequest);
		is_at(category, 0x50, XamlLoadRequest);
        
		static inline std::add_pointer_t<void(RE::ls::UIWidget*, bool)> _SetVisiblityMask{ nullptr };
    } // namespace Noesis
} // namespace RE