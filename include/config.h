#ifndef CONFIG_H
#define CONFIG_H

#include <utility>

inline constexpr int TARGET_FPS = 60;
inline constexpr int WIDTH = 1920;
inline constexpr int HEIGHT = 1080;

inline constexpr float SCREEN_CENTER_X = WIDTH / 2.0f;
inline constexpr float SCREEN_CENTER_Y = WIDTH / 2.0f;

inline constexpr float CAMERA_ZOOM_SCALE = 0.05f;
inline constexpr float CAMERA_ZOOM_MIN = 0.1f;
inline constexpr float CAMERA_ZOOM_MAX = 3.0f;
inline constexpr int CAMERA_MOVE_SPEED = 8;

// UI SPECIFIC CONFIGURATION

inline constexpr std::pair<int, int> DEFAULT_ROOM_SIZE{ 560, 400 };
inline constexpr std::pair<int, int> DEFAULT_PLANK_SIZE{ 130, 25 };

// config sliders
inline constexpr std::pair<int, int> SLIDER_ROOM_RANGE{ 200, 801 };
inline constexpr std::pair<int, int> SLIDER_PLANK_X_RANGE{ 60, 301 };
inline constexpr std::pair<int, int> SLIDER_PLANK_Y_RANGE{ 10, 100 };

#endif
