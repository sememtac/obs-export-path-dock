# Export Path Dock

[![Build](https://github.com/sememtac/obs-export-path-dock/actions/workflows/push.yaml/badge.svg)](https://github.com/sememtac/obs-export-path-dock/actions/workflows/push.yaml)

<img width="365" height="116" alt="image" src="https://github.com/user-attachments/assets/655bce9b-5e79-481a-a038-9a2044d00d3d" />

Export Path Dock is an OBS Studio dock that sets the folder where OBS saves your recordings.

Without the dock, you must open **Settings → Output** to change the recording folder. Then you must find the correct field for your output mode. The dock shows the folder in the main OBS window, next to your scenes. You can change the folder between recordings without opening Settings.

## Features

- Shows the folder where OBS saves the next recording.
- Changes the folder when you type a path and press Enter, or when you click **Browse…**.
- Keeps a list of the last 10 folders that you used. The list is in the dropdown of the folder box.
- Opens the current folder in your file manager when you click **Open Folder**.
- Asks if you want to create a folder that does not exist.
- Does not accept a folder where OBS cannot write files.
- Works in Simple output mode and in Advanced output mode, including Custom Output (FFmpeg).
- Shows changes that you make in **Settings → Output**, and shows the correct folder when you change to a different profile.

## Requirements

- OBS Studio 31.1 or newer
- Windows 10 or 11 (64-bit), macOS 12 or newer, or Ubuntu 24.04 (x86_64)

## Installation

1. Close OBS.
2. Download the file for your operating system from the [latest release](https://github.com/sememtac/obs-export-path-dock/releases/latest).
3. Do the steps for your operating system below.

### Windows

1. Extract `export-path-dock-<version>-windows-x64.zip` into `C:\ProgramData\obs-studio\plugins\`.
2. Make sure that this file exists: `C:\ProgramData\obs-studio\plugins\export-path-dock\bin\64bit\export-path-dock.dll`.

**Portable OBS:** A portable installation of OBS does not load plugins from `ProgramData`. Copy the files from the zip into your OBS folder:

| File or folder in the zip | Destination in your OBS folder |
|---|---|
| `export-path-dock\bin\64bit\export-path-dock.dll` | `obs-plugins\64bit\` |
| The contents of `export-path-dock\data\` | `data\obs-plugins\export-path-dock\` |

### macOS

1. Open `export-path-dock-<version>-macos-universal.pkg`.
2. Do the steps in the installer. Do not change the install location.

The installer puts the plugin in `~/Library/Application Support/obs-studio/plugins/`.

The package does not have an Apple signature. Thus, macOS can stop the installer. On macOS 15, the message contains the words "free of malware". If you see this message, do these steps:

1. Click **Done**. Do not click **Move to Trash**.
2. Open **System Settings → Privacy & Security**. On macOS 12, open **System Preferences → Security & Privacy**.
3. Find the message about the package in the **Security** section. Click **Open Anyway**.
4. If macOS asks for your password, type it. The installer starts.
5. If the installer does not start, open the package again.

**If OBS does not load the plugin:** A browser marks the files that you download. macOS does not let OBS load a marked plugin. This problem can occur if you copy `export-path-dock.plugin` yourself and do not use the package.

When OBS starts, macOS shows "“export-path-dock.plugin” Not Opened". Then OBS shows "The following OBS plugins failed to load". **Allow Anyway** in **Privacy & Security** does not solve this problem. Do these steps:

1. Quit OBS.
2. In Terminal, run this command:

   ```sh
   xattr -dr com.apple.quarantine ~/Library/Application\ Support/obs-studio/plugins/export-path-dock.plugin
   ```

3. Start OBS.

### Ubuntu

```sh
sudo apt install ./export-path-dock-<version>-x86_64-linux-gnu.deb
```

The package works with OBS from the [official PPA](https://obsproject.com/kb/linux-installation). The Flatpak and Snap versions of OBS cannot load it.

## Usage

1. Start OBS.
2. Select **Docks → Export Path**. The dock opens as a floating window.
3. Optional: Drag the dock onto the main window to attach it.

To change the folder, use one of these methods:

- Click **Browse…** and select a folder.
- Type or paste a path into the folder box, then press Enter.
- Open the dropdown of the folder box and select a recent folder.

OBS saves the next recording in the new folder.

The dock shows a message below the buttons in these conditions:

- A recording or the replay buffer is running. The new folder applies when the next recording or replay buffer starts.
- The current folder does not exist. For example, the folder is on a drive that you disconnected.
- Custom Output sends to a URL, not to a file. In this mode, there is no folder to change.

To clear the list of recent folders, open the dropdown and select **Clear Recent Folders**.

## How it works

OBS keeps the recording folder in the settings of each profile. Each output mode has a different setting for the folder. OBS reads this setting each time a recording or the replay buffer starts.

The dock changes the setting for your current output mode. **Settings → Output** changes the same setting. The dock does not change your other recording settings.

The replay buffer saves to the recording folder. When you change the folder in the dock, the replay buffer also uses the new folder.

The plugin keeps the list of recent folders in its own settings file. All your profiles use the same list.

## Known limitations

- The Settings window reads the folder when it opens. Do not change the folder in the dock while Settings is open. If you do, **OK** and **Apply** in Settings set the old folder again. The dock then shows the old folder.
- OBS does not have a separate folder setting for the replay buffer. Thus, the dock cannot set a different folder for the replay buffer.

## Building from source

This project uses the official [OBS plugin template](https://github.com/obsproject/obs-plugintemplate). When you configure the project for the first time, CMake downloads the OBS source code, the prebuilt OBS dependencies, and Qt.

| Platform | Configure | Build |
|---|---|---|
| Windows | `cmake --preset windows-x64` | `cmake --build --preset windows-x64` |
| macOS | `cmake --preset macos` | `cmake --build --preset macos` |
| Ubuntu | `cmake --preset ubuntu-x86_64` | `cmake --build --preset ubuntu-x86_64` |

Each platform needs these tools:

- **Windows:** Visual Studio 2022 with the C++ workload, and Windows SDK 10.0.22621. The CMake preset selects this SDK version.
- **macOS:** Xcode 16 or newer, and CMake 3.28 or newer. The Command Line Tools alone are not sufficient. Install the full Xcode app. Then run these commands:

  ```sh
  sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
  sudo xcodebuild -license accept
  ```

  To install CMake with Homebrew, run `brew install cmake`.
- **Ubuntu:** `build-essential`, `cmake`, `ninja-build`, `pkg-config`, `qt6-base-dev`, and `qt6-base-private-dev`.

For the full list, see the [build system requirements](https://github.com/obsproject/obs-plugintemplate/wiki/Build-System-Requirements) of the template.

To test a Windows build:

1. Run `cmake --install build_x64 --prefix release --config RelWithDebInfo`.
2. Copy the contents of `release\` into `C:\ProgramData\obs-studio\plugins\`.

To test a macOS build:

1. Run `cmake --install build_macos --config RelWithDebInfo --prefix release/RelWithDebInfo`. This command also makes `release/RelWithDebInfo/export-path-dock.pkg`.
2. Open the package. Or copy `release/RelWithDebInfo/export-path-dock.plugin` into `~/Library/Application Support/obs-studio/plugins/`.

### Source layout

| File | Purpose |
|---|---|
| `src/plugin-main.cpp` | Loads the plugin and adds the dock to OBS |
| `src/export-path-dock.cpp` | Shows the dock and handles user actions |
| `src/recording-path.cpp` | Reads and writes the recording folder for each output mode |
| `src/recent-folders.cpp` | Saves and loads the list of recent folders |
| `data/locale/en-US.ini` | Contains the interface text |

### Releases

GitHub Actions builds the plugin for all three platforms when you push to `main`. It also checks the formatting with clang-format 19.1.1 and gersemi.

When you push a version tag, for example `0.1.0`, GitHub Actions creates a draft release. The draft release contains the packages for all three platforms.

## Contributing

To report a bug or to request a change, open an issue in the [issue tracker](https://github.com/sememtac/obs-export-path-dock/issues). Pull requests are welcome.

When you report a bug, include this information:

- Your OBS version
- Your operating system
- The URL of your OBS log file. To get the URL, select **Help → Log Files → Upload Current Log File**.

## License

Export Path Dock uses the [GNU General Public License v2.0 or later](LICENSE), the same license as OBS Studio.
