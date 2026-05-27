#pragma once

#include <memory>
#include <functional>
#include "math/Math.h"

struct SDL_Window;
union SDL_Event;

namespace library
{

enum class Key
{
    Unknown = 0,
    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,
    Number1 = 30,
    Number2 = 31,
    Number3 = 32,
    Number4 = 33,
    Number5 = 34,
    Number6 = 35,
    Number7 = 36,
    Number8 = 37,
    Number9 = 38,
    Number0 = 39,
    Return = 40,
    Escape = 41,
    Backspace = 42,
    Tab = 43,
    Space = 44,
    Minus = 45,
    Equals = 46,
    Leftbracket = 47,
    RightBracket = 48,
    Backslash = 49,
    NonusHash = 50,
    Semicolon = 51,
    Apostrophe = 52,
    Grave = 53,
    Comma = 54,
    Period = 55,
    Slash = 56,
    CapsLock = 57,
    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,
    PrintScreen = 70,
    ScrollLock = 71,
    Pause = 72,
    Insert = 73,
    Home = 74,
    PageUp = 75,
    Delete = 76,
    End = 77,
    PageDown = 78,
    Right = 79,
    Left = 80,
    Down = 81,
    Up = 82,
    NumLockClear = 83,
    KeypadDivide = 84,
    KeypadMultiply = 85,
    KeypadMinus = 86,
    KeypadPlus = 87,
    KeypadEnter = 88,
    Keypad1 = 89,
    Keypad2 = 90,
    Keypad3 = 91,
    Keypad4 = 92,
    Keypad5 = 93,
    Keypad6 = 94,
    Keypad7 = 95,
    Keypad8 = 96,
    Keypad9 = 97,
    Keypad0 = 98,
    KeypadPeriod = 99,
    NonusBackSlash = 100,
    Application = 101,
    Power = 102,
    KeypadEquals = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    Execute = 116,
    Help = 117,
    Menu = 118,
    Select = 119,
    Stop = 120,
    Again = 121,
    Undo = 122,
    Cut = 123,
    Copy = 124,
    Paste = 125,
    Find = 126,
    Mute = 127,
    VolumeUp = 128,
    VolumeDown = 129,
    KeypadComma = 133,
    KeypadEqualSas400 = 134,
    International1 = 135,
    International2 = 136,
    International3 = 137,
    International4 = 138,
    International5 = 139,
    International6 = 140,
    International7 = 141,
    International8 = 142,
    International9 = 143,
    Lang1 = 144,
    Lang2 = 145,
    Lang3 = 146,
    Lang4 = 147,
    Lang5 = 148,
    Lang6 = 149,
    Lang7 = 150,
    Lang8 = 151,
    Lang9 = 152,
    AltErase = 153,
    SysReq = 154,
    Cancel = 155,
    Clear = 156,
    Prior = 157,
    Return2 = 158,
    Separator = 159,
    Out = 160,
    Oper = 161,
    ClearAgain = 162,
    CrSel = 163,
    ExSel = 164,
    Keypad00 = 176,
    Keypad000 = 177,
    ThousandsSeparator = 178,
    DecimalSeparator = 179,
    CurrencyUnit = 180,
    CurrencySubunit = 181,
    KeypadLeftParen = 182,
    KeypadRightParen = 183,
    KeypadLeftBrace = 184,
    KeypadRightBrace = 185,
    KeypadTab = 186,
    KeypadBackSpace = 187,
    KeypadA = 188,
    KeypadB = 189,
    KeypadC = 190,
    KeypadD = 191,
    KeypadE = 192,
    KeypadF = 193,
    KeypadXor = 194,
    KeypadPower = 195,
    KeypadPercent = 196,
    KeypadLess = 197,
    KeypadGreater = 198,
    KeypadAmpersand = 199,
    KeypadDblAmpersand = 200,
    KeypadVerticalBar = 201,
    KeypadDblVerticalBar = 202,
    KeypadColon = 203,
    KeypadHash = 204,
    KeypadSpace = 205,
    KeypadAt = 206,
    KeypadExclam = 207,
    KeypadMemStore = 208,
    KeypadMemRecall = 209,
    KeypadMemClear = 210,
    KeypadMemAdd = 211,
    KeypadMemSubtract = 212,
    KeypadMemMultiply = 213,
    KeypadMemDivide = 214,
    KeypadPlusMinus = 215,
    KeypadClear = 216,
    KeypadClearEntry = 217,
    KeypadBinary = 218,
    KeypadOctal = 219,
    KeypadDecimal = 220,
    KeypadHexadecimal = 221,
    LeftCtrl = 224,
    LeftShift = 225,
    LeftAlt = 226,
    LeftGui = 227,
    RightCtrl = 228,
    RightShift = 229,
    RightAlt = 230,
    RightGui = 231,
    Mode = 257,
    AudioNext = 258,
    AudioPrevious = 259,
    AudioStop = 260,
    AudioPlay = 261,
    AudioMute = 262,
    MediaSelect = 263,
    WWW = 264,
    Mail = 265,
    Calculator = 266,
    Computer = 267,
    ACSearch = 268,
    ACHome = 269,
    ACBack = 270,
    ACForward = 271,
    ACStop = 272,
    ACRefresh = 273,
    ACBookmarks = 274,
    BrightnessDown = 275,
    BrightnessUp = 276,
    DisplaySwitch = 277,
    KeyboardIllumToggle = 278,
    KeyboardIllumDown = 279,
    KeyboardIllumUp = 280,
    Eject = 281,
    Sleep = 282,
    App1 = 283,
    App2 = 284,
    AudioRewind = 285,
    AudioFastForward = 286
};

enum class Button
{
    Left = 0,
    Right = 1,
    Middle = 2
};

enum class WindowMode
{
    Windowed,
    FullScreen,
    FullScreenNative
};

class Window
{
public:
    Window();
    virtual ~Window();

    void setTitle(const char *title);
    void setSize(int width, int height);
    void centerScreen();
    void swapBuffers();

    void handleEvents();
    std::function<void(const SDL_Event *)> eventCallback;
    std::function<void()> closeEvent;
    std::function<void(int, int)> resizeEvent;
    std::function<void(int, int)> positionEvent;

    bool isKeyDown(Key key);
    bool isKeyPressed(Key key);
    bool isKeyShiftDown();
    bool isKeyCtrlDown();
    bool isKeyAltDown();
    bool isKeySuperDown();
    const bool *getKeysDown();
    const char *getTextInput();

    ivec2 getMousePosition();
    ivec2 getMouseChange();
    int getMouseWheelChange();
    bool isMouseButtonDown(Button button);

    SDL_Window *getSDLWindow();
    void *getGLContext();
    void *getWindowHandle();
    void *getDisplay();
    int getWidth();
    int getHeight();
    ivec2 getSize();
    void setWindowMode(const WindowMode &mode);
	void showMouse(bool show = true);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}