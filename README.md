# Motorbike HUD

Firmware workspace scaffold for the LilyGO T-Display S3 AMOLED Plus using PlatformIO (ESP32-S3, Arduino framework).

## Getting started
- Install VS Code + PlatformIO extension.
- Connect the board via USB-C; on first flash Windows may install the driver automatically.
- Open this folder in VS Code, let PlatformIO parse platformio.ini, then click the checkmark (build) or right-arrow (upload).

## Notes
- The environment is named `lilygo_t_display_s3_amoled_plus` and currently targets the esp32-s3-devkitc-1 board definition (16MB flash, PSRAM, USB-CDC enabled). Adjust in platformio.ini if your module differs.
- src/main.cpp only logs over USB-CDC; add your display/touch drivers under lib_deps as needed.
- If upload/monitor fails, set the correct COM port under PlatformIO > Devices or add upload_port / monitor_port to platformio.ini.

## Layout
- platformio.ini — PlatformIO environment and build flags.
- src/main.cpp — minimal firmware entry point.

