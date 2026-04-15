//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    input handling
//-----------------------------------------------------------------------------

#include "input.h"

#include <cstring>

KeyBoard::KeyBoard()
{
    input_state = 0;
    std::memset(bKeys, 0, sizeof(bKeys));
    std::memset(bKeys_state, 0, sizeof(bKeys_state));
}

KeyBoard::~KeyBoard() {}

void KeyBoard::update()
{
    input_state ^= 1;
    std::memcpy(bKeys_state[input_state], bKeys, sizeof(bKeys));
}

bool KeyBoard::keyPressed(SDL_Scancode key)
{
    return bKeys_state[input_state][key] && !bKeys_state[input_state ^ 1][key];
}

bool KeyBoard::keyReleased(SDL_Scancode key)
{
    return !bKeys_state[input_state][key] && bKeys_state[input_state ^ 1][key];
}

bool KeyBoard::keyDown(SDL_Scancode key) { return bKeys_state[input_state][key]; }

bool KeyBoard::keyUp(SDL_Scancode key) { return !bKeys_state[input_state][key]; }

Mouse::Mouse()
{
    input_state = 0;
    lX = 0;
    lY = 0;
    std::memset(bButtons, 0, sizeof(bButtons));
    std::memset(bButtons_state, 0, sizeof(bButtons_state));
    std::memset(lX_state, 0, sizeof(lX_state));
    std::memset(lY_state, 0, sizeof(lY_state));
}

Mouse::~Mouse() {}

void Mouse::update()
{
    input_state ^= 1;
    std::memcpy(bButtons_state[input_state], bButtons, sizeof(bButtons));
    lX_state[input_state] = lX;
    lY_state[input_state] = lY;
}

long Mouse::ldX() { return (lX_state[input_state] - lX_state[input_state ^ 1]); }

long Mouse::ldY() { return (lY_state[input_state] - lY_state[input_state ^ 1]); }

bool Mouse::buttonPressed()
{
    return bButtons_state[input_state][0] && !bButtons_state[input_state ^ 1][0];
}

bool Mouse::buttonReleased()
{
    return !bButtons_state[input_state][0] && bButtons_state[input_state ^ 1][0];
}

bool Mouse::buttonDown() { return bButtons_state[input_state][0]; }

bool Mouse::buttonUp() { return !bButtons_state[input_state][0]; }

GamePad::GamePad()
{
    numButtons = NUM_BUTTONS;
    std::memset(bButtons, 0, sizeof(bButtons));
    std::memset(bButtons_state, 0, sizeof(bButtons_state));
    std::memset(bHat, 0, sizeof(bHat));
    std::memset(bHat_state, 0, sizeof(bHat_state));
    lAxisX = 0;
    lAxisY = 0;
    lAxisZ = 0;
    lAxisRz = 0;
    lAxisX_state[0] = lAxisX_state[1] = 0;
    lAxisY_state[0] = lAxisY_state[1] = 0;
    lAxisZ_state[0] = lAxisZ_state[1] = 0;
    lAxisRz_state[0] = lAxisRz_state[1] = 0;
    input_state = 0;
}

GamePad::~GamePad() {}

void GamePad::update()
{
    input_state ^= 1;
    std::memcpy(bButtons_state[input_state], bButtons, sizeof(bButtons));
    std::memcpy(bHat_state[input_state], bHat, sizeof(bHat));
    lAxisX_state[input_state] = lAxisX;
    lAxisY_state[input_state] = lAxisY;
    lAxisZ_state[input_state] = lAxisZ;
    lAxisRz_state[input_state] = lAxisRz;
}

bool GamePad::buttonPressed(int button)
{
    return bButtons_state[input_state][button] &&
           !bButtons_state[input_state ^ 1][button];
}

bool GamePad::buttonReleased(int button)
{
    return !bButtons_state[input_state][button] &&
           bButtons_state[input_state ^ 1][button];
}

bool GamePad::buttonDown(int button) { return bButtons_state[input_state][button]; }

bool GamePad::buttonUp(int button) { return !bButtons_state[input_state][button]; }

bool GamePad::hatPressed(GP_HatDirection direction)
{
    return (bHat_state[input_state][direction]) &&
           !(bHat_state[input_state ^ 1][direction]);
}

bool GamePad::hatReleased(GP_HatDirection direction)
{
    return !(bHat_state[input_state][direction]) &&
           (bHat_state[input_state ^ 1][direction]);
}

bool GamePad::hatDown(GP_HatDirection direction)
{
    return bHat_state[input_state][direction];
}

bool GamePad::hatUp(GP_HatDirection direction)
{
    return !bHat_state[input_state][direction];
}

bool GamePad::lAxisXPressed(GP_AxisDirection direction)
{
    return direction == GPA_LEFT ? (lAxisX_state[input_state] < 0 &&
                                    lAxisX_state[input_state ^ 1] >= 0)
                                 : (lAxisX_state[input_state] > 0 &&
                                    lAxisX_state[input_state ^ 1] <= 0);
}

bool GamePad::lAxisXReleased(GP_AxisDirection direction)
{
    return direction == GPA_LEFT ? (lAxisX_state[input_state] >= 0 &&
                                    lAxisX_state[input_state ^ 1] < 0)
                                 : (lAxisX_state[input_state] <= 0 &&
                                    lAxisX_state[input_state ^ 1] > 0);
}

bool GamePad::lAxisYPressed(GP_AxisDirection direction)
{
    return direction == GPA_LEFT ? (lAxisY_state[input_state] < 0 &&
                                    lAxisY_state[input_state ^ 1] >= 0)
                                 : (lAxisY_state[input_state] > 0 &&
                                    lAxisY_state[input_state ^ 1] <= 0);
}

bool GamePad::lAxisYReleased(GP_AxisDirection direction)
{
    return direction == GPA_LEFT ? (lAxisY_state[input_state] >= 0 &&
                                    lAxisY_state[input_state ^ 1] < 0)
                                 : (lAxisY_state[input_state] <= 0 &&
                                    lAxisY_state[input_state ^ 1] > 0);
}

float GamePad::fAxisX() { return (static_cast<float>(lAxisX) / 32767.0f); }

float GamePad::fAxisY() { return (static_cast<float>(lAxisY) / 32767.0f); }

float GamePad::fAxisZ() { return (static_cast<float>(lAxisZ) / 32767.0f); }

float GamePad::fAxisRz() { return (static_cast<float>(lAxisRz) / 32767.0f); }

Input::Input() {}

Input::~Input() {}

void Input::update()
{
    keyboard.update();
    mouse.update();
    gamepad.update();
}

