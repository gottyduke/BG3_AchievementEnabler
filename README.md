# BG3_AchievementEnabler
 Enables steam achievement for Baldur's Gate 3 with mods enabled.

## Requirements

- [CMake](https://cmake.org/)
  - Add this to your `PATH`
- [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
- [Vcpkg](https://github.com/microsoft/vcpkg)
  - Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
  - Desktop development with C++
- [Baldur's Gate 3 Steam Distribution](https://store.steampowered.com/app/1086940/Baldurs_Gate_3/)
  - Add the environment variable `BG3PATH` with the value as path to game install folder
  
## Register Visual Studio as a Generator

- Open `x64 Native Tools Command Prompt`
- Run `cmake`
- Close the cmd window

## Building

```
git clone https://github.com/gottyduke/BG3_AchievementEnabler.git
cd BG3_AchievementEnabler
.\build-release.ps1
```

## License

[MIT](LICENSE)

## Credits

- [Kassent's NativeModLoader](https://www.nexusmods.com/divinityoriginalsin2/mods/210?tab=description)([GitHub](https://github.com/kassent))
- [Ryan for his commonLibSSE code](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE) which was referenced in DKUtil.


---


# BG3_AchievementEnabler
 启用带有模组的《博德之门3》的Steam成就。

## 需求

- [CMake](https://cmake.org/)
  - 把这个添加到你的`PATH`
- [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
- [Vcpkg](https://github.com/microsoft/vcpkg)
  - 用`VCPKG_ROOT`设置环境变量，并把包含vcpkg的文件夹路径作为值
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
  - 带有C++的桌面开发
- [Baldur's Gate 3 Steam Distribution](https://store.steampowered.com/app/1086940/Baldurs_Gate_3/)
  - 用`BG3PATH`设置环境变量，并把steam版本游戏安装文件夹路径作为值

## 将Visual Studio注册为生成器

- 打开`x64 Native Tools Command Prompt`
- 运行`cmake`
- 关闭cmd窗口

## 构建

git clone https://github.com/gottyduke/BG3_AchievementEnabler.git
cd BG3_AchievementEnabler
.\build-release.ps1
## 许可

[MIT](LICENSE)

## 鸣谢

- [Kassent的NativeModLoader](https://www.nexusmods.com/divinityoriginalsin2/mods/210?tab=description)([GitHub](https://github.com/kassent))
- [Ryan的commonLibSSE代码](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE)