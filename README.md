# Export Path Dock

An OBS Studio dock for changing where your recordings are saved, without digging through **Settings → Output**.

## Features

- Shows the recording folder OBS will use next, for whichever output mode you're in (Simple, Advanced, or Advanced with Custom Output)
- Type a folder and press Enter, or click **Browse…** to pick one
- **Open Folder** jumps straight to your recordings
- **Recent** remembers the last 10 folders, shared across all profiles
- Offers to create folders that don't exist, and refuses ones OBS can't write to
- Changing the folder mid-recording is allowed; it applies to the next recording (the replay buffer follows the same folder)
- Stays in sync if you change the folder in Settings or switch profiles

## Requirements

- OBS Studio 31.1 or newer
- Windows 10/11 (x64), macOS 12 or newer, or Ubuntu 24.04 (x86_64)

## Installation

Download the package for your platform from the [Releases](https://github.com/sememtac/obs-export-path-dock/releases) page.

- **Windows:** extract the zip into `C:\ProgramData\obs-studio\plugins\`, so you end up with `C:\ProgramData\obs-studio\plugins\export-path-dock\bin\64bit\export-path-dock.dll`.
- **macOS:** run the `.pkg` installer.
- **Ubuntu:** install the `.deb` with `sudo apt install ./export-path-dock-*.deb`.

Restart OBS, then turn the dock on from the **Docks** menu (**Export Path**).

## How it works

OBS keeps the recording folder in your profile's settings and reads it again every time a recording or the replay buffer starts. The dock writes to the same setting that **Settings → Output** uses, so there's nothing extra to configure and nothing changes about how OBS records.

## Building from source

The project is based on the official [OBS plugin template](https://github.com/obsproject/obs-plugintemplate), which downloads OBS, its dependencies and Qt for you.

| Platform | Configure | Build |
|---|---|---|
| Windows | `cmake --preset windows-x64` | `cmake --build --preset windows-x64` |
| macOS | `cmake --preset macos` | `cmake --build --preset macos` |
| Ubuntu | `cmake --preset ubuntu-x86_64` | `cmake --build --preset ubuntu-x86_64` |

You'll need Visual Studio 2022 on Windows, Xcode 16 on macOS, and `build-essential`, `ninja-build`, `pkg-config` and Qt 6 development packages on Ubuntu. See the template's [build system requirements](https://github.com/obsproject/obs-plugintemplate/wiki/Build-System-Requirements) for details.

GitHub Actions builds all three platforms on every push. Pushing a version tag (for example `0.1.0`) creates a draft release with the packages attached.

## License

[GPL-2.0-or-later](LICENSE), the same as OBS Studio.
