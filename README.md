# Export Path Dock

[![Build](https://github.com/sememtac/obs-export-path-dock/actions/workflows/push.yaml/badge.svg)](https://github.com/sememtac/obs-export-path-dock/actions/workflows/push.yaml)

An OBS Studio dock for changing where your recordings are saved.

Normally, changing the recording folder means opening **Settings → Output** and finding the right field for your output mode. Export Path Dock puts that folder in a dock next to your scenes, so you can switch folders between takes without leaving the main window.

## Features

- Shows the folder OBS will save the next recording to
- Type a path and press Enter, or click **Browse…** to pick a folder
- The folder box drops down to the last 10 folders you used
- **Open Folder** opens the current folder in your file manager
- Offers to create a folder that doesn't exist yet, and won't accept a folder OBS can't write to
- Works in Simple and Advanced output mode, including Advanced mode's Custom Output (FFmpeg)
- Picks up changes made in **Settings → Output** and follows you when you switch profiles

## Requirements

- OBS Studio 31.1 or newer
- Windows 10 or 11 (64-bit), macOS 12 or newer, or Ubuntu 24.04 (x86_64)

## Installation

Download the file for your system from the [latest release](https://github.com/sememtac/obs-export-path-dock/releases/latest), close OBS, then follow the steps for your system.

### Windows

1. Extract `export-path-dock-<version>-windows-x64.zip` into `C:\ProgramData\obs-studio\plugins\`.
2. Check that this file exists: `C:\ProgramData\obs-studio\plugins\export-path-dock\bin\64bit\export-path-dock.dll`.

**Portable OBS:** portable installs don't load plugins from `ProgramData`. Copy the files into your OBS folder instead:

| From the zip | To your OBS folder |
|---|---|
| `export-path-dock\bin\64bit\export-path-dock.dll` | `obs-plugins\64bit\` |
| `export-path-dock\data\` (its contents) | `data\obs-plugins\export-path-dock\` |

### macOS

Open `export-path-dock-<version>-macos-universal.pkg` and follow the installer.

If macOS says the package is from an unidentified developer, Control-click the file, choose **Open**, then confirm.

### Ubuntu

```sh
sudo apt install ./export-path-dock-<version>-x86_64-linux-gnu.deb
```

The package works with OBS installed from the [official PPA](https://obsproject.com/kb/linux-installation). The Flatpak and Snap versions of OBS can't load it.

## Usage

1. Start OBS and choose **Docks → Export Path**. The dock opens as a floating window; drag it onto the main window to dock it.
2. Change the folder in any of these ways:
   - Click **Browse…** and pick a folder.
   - Type or paste a path into the folder box and press Enter.
   - Open the folder box's dropdown and pick a recent folder.

The next recording goes to the new folder. Below the buttons, the dock warns you when:

- a recording or the replay buffer is running (the new folder applies the next time it starts)
- the current folder no longer exists (for example, an unplugged drive)
- Custom Output is sending to a URL rather than a file, so there's no folder to change

To clear the history, open the dropdown and choose **Clear Recent Folders**.

## How it works

OBS stores the recording folder in each profile's settings, in a different field for each output mode, and reads it again every time a recording or the replay buffer starts. The dock edits the field for the output mode you're currently using, which is the same field **Settings → Output** edits. Your other recording settings stay as they are.

The replay buffer saves to the recording folder, so it follows the dock too.

The recent-folders list is saved separately in the plugin's own settings, so it's shared by all your profiles.

## Known limitations

- If **Settings** is open while you change the folder in the dock, clicking **OK** or **Apply** in Settings puts back the folder Settings had when you opened it. The dock updates to show whichever folder OBS ends up using.
- OBS has no separate folder for the replay buffer, so the dock can't set one.

## Building from source

The project is based on the official [OBS plugin template](https://github.com/obsproject/obs-plugintemplate). The first configure downloads the OBS sources, the prebuilt OBS dependencies and Qt.

| Platform | Configure | Build |
|---|---|---|
| Windows | `cmake --preset windows-x64` | `cmake --build --preset windows-x64` |
| macOS | `cmake --preset macos` | `cmake --build --preset macos` |
| Ubuntu | `cmake --preset ubuntu-x86_64` | `cmake --build --preset ubuntu-x86_64` |

Each platform needs:

- **Windows:** Visual Studio 2022 with the C++ workload, and Windows SDK 10.0.22621 (the version the preset selects)
- **macOS:** Xcode 16
- **Ubuntu:** `build-essential`, `cmake`, `ninja-build`, `pkg-config`, `qt6-base-dev` and `qt6-base-private-dev`

See the template's [build system requirements](https://github.com/obsproject/obs-plugintemplate/wiki/Build-System-Requirements) for the full list.

To try a Windows build, run `cmake --install build_x64 --prefix release --config RelWithDebInfo` and copy the contents of `release\` into `C:\ProgramData\obs-studio\plugins\`.

### Source layout

| File | Purpose |
|---|---|
| `src/plugin-main.cpp` | Loads the plugin and registers the dock with OBS |
| `src/export-path-dock.cpp` | The dock's interface and behavior |
| `src/recording-path.cpp` | Reads and writes the recording folder for each output mode |
| `src/recent-folders.cpp` | Saves and loads the recent-folders list |
| `data/locale/en-US.ini` | Interface text |

### Releases

GitHub Actions builds all three platforms on every push to `main` and checks formatting with clang-format 19.1.1 and gersemi. Pushing a version tag such as `0.1.0` creates a draft release with the packages attached.

## Contributing

Bug reports and pull requests are welcome on the [issue tracker](https://github.com/sememtac/obs-export-path-dock/issues). When reporting a bug, include your OBS version, operating system, and the OBS log file (**Help → Log Files → Upload Current Log File**).

## License

Export Path Dock is licensed under the [GNU General Public License v2.0 or later](LICENSE), the same license as OBS Studio.
