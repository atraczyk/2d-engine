//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	input handling
//-----------------------------------------------------------------------------

#include "input.h"

KeyBoard::KeyBoard()
{
    input_state = 0;
    ZeroMemory(bKeys, sizeof(bKeys));
}

KeyBoard::~KeyBoard()
{
}

void KeyBoard::update()
{
    input_state ^= 1;
    memcpy(bKeys_state[input_state], bKeys, NUM_KEYS);
}

bool KeyBoard::keyPressed(unsigned char key)
{
    return bKeys_state[input_state][key] &&
        !bKeys_state[input_state ^ 1][key];
}

bool KeyBoard::keyReleased(unsigned char key)
{
    return !bKeys_state[input_state][key] &&
        bKeys_state[input_state ^ 1][key];
}

bool KeyBoard::keyDown(unsigned char key)
{
    return bKeys_state[input_state][key];
}

bool KeyBoard::keyUp(unsigned char key)
{
    return !bKeys_state[input_state][key];
}

Mouse::Mouse()
{
    input_state = 0;
    lX = 0;
    lY = 0;
    ZeroMemory(bButtons, sizeof(bButtons));
}

Mouse::~Mouse()
{
}

void Mouse::update()
{
    input_state ^= 1;
    memcpy(bButtons_state[input_state], bButtons_state, NUM_M_BUTTONS);
    lX_state[input_state] = lX;
    lY_state[input_state] = lY;
}

long Mouse::ldX()
{
    return (lX_state[input_state] - lX_state[input_state ^ 1]);
}

long Mouse::ldY()
{
    return (lY_state[input_state] - lY_state[input_state ^ 1]);
}

GamePad::GamePad()
{
    ZeroMemory(bButtons, sizeof(bButtons));
    lAxisX = 0;
    lAxisY = 0;
    lAxisZ = 0;
    lAxisRz = 0;
    ZeroMemory(bHat, sizeof(bHat));
}

GamePad::~GamePad()
{
}

void GamePad::update()
{
    input_state ^= 1;
    memcpy(bButtons_state[input_state], bButtons, NUM_BUTTONS);
    memcpy(bHat_state[input_state], bHat, 8);
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

bool GamePad::buttonDown(int button)
{
    return bButtons_state[input_state][button];
}

bool GamePad::buttonUp(int button)
{
    return !bButtons_state[input_state][button];
}

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
    return false;
}

bool GamePad::lAxisXReleased(GP_AxisDirection direction)
{
    return false;
}

bool GamePad::lAxisYPressed(GP_AxisDirection direction)
{
    return false;
}

bool GamePad::lAxisYReleased(GP_AxisDirection direction)
{
    return false;
}

float GamePad::fAxisX()
{
    return ((float)lAxisX / 32767.0f);
}

float GamePad::fAxisY()
{
    return ((float)lAxisY / 32767.0f);
}

float GamePad::fAxisZ()
{
    return ((float)lAxisZ / 32767.0f);
}

float GamePad::fAxisRz()
{
    return ((float)lAxisRz / 32767.0f);
}

Input::Input()
{
}

Input::~Input()
{
}

void Input::update()
{
    keyboard.update();
    mouse.update();
    gamepad.update();
}

void safe_free(PHIDP_PREPARSED_DATA pPreparsedData, PHIDP_BUTTON_CAPS pButtonCaps, PHIDP_VALUE_CAPS pValueCaps, HANDLE hHeap){
    SAFE_FREE(pPreparsedData);
    SAFE_FREE(pButtonCaps);
    SAFE_FREE(pValueCaps);
}

void ReadGamePadInput(PRAWINPUT pRawInput, GamePad &pGamePad)
{
    PHIDP_PREPARSED_DATA pPreparsedData;
    HIDP_CAPS            Caps;
    PHIDP_BUTTON_CAPS    pButtonCaps;
    PHIDP_VALUE_CAPS     pValueCaps;
    USHORT               capsLength;
    UINT                 bufferSize;
    HANDLE               hHeap;
    USAGE                usage[NUM_BUTTONS];
    ULONG                i, usageLength, value;

    pPreparsedData = NULL;
    pButtonCaps = NULL;
    pValueCaps = NULL;
    hHeap = GetProcessHeap();

    if(GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) != 0){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }
    pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, bufferSize);

    if(!pPreparsedData){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }

    if((int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) < 0){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }

    if(HidP_GetCaps(pPreparsedData, &Caps) != HIDP_STATUS_SUCCESS){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }

    pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps);

    if(!pButtonCaps){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }

    capsLength = Caps.NumberInputButtonCaps;
    if(HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }


    pGamePad.numButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;
    pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps);

    if(!pValueCaps){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }

    capsLength = Caps.NumberInputValueCaps;

    if(HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS){
        safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
        return;
    }

    usageLength = pGamePad.numButtons;
    if(HidP_GetUsages(HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
            (PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS){
            safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
            return;
    }

    ZeroMemory(pGamePad.bButtons, sizeof(pGamePad.bButtons));
    for (i = 0; i < usageLength; i++)
        pGamePad.bButtons[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;

    for (i = 0; i < Caps.NumberInputValueCaps; i++)
    {
        if(HidP_GetUsageValue(HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
                (PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid)!= HIDP_STATUS_SUCCESS){
            safe_free(pPreparsedData, pButtonCaps, pValueCaps, hHeap);
            return;
        }

        switch (pValueCaps[i].Range.UsageMin)
        {
        case 0x30:
            pGamePad.lAxisX = (long)((value - 32768));
            break;
        case 0x31:
            pGamePad.lAxisY = -1 * ((long)((value - 32767)));
            break;
        case 0x32:
            pGamePad.lAxisZ = (long)((value - 32768));
            break;
        case 0x35:
            pGamePad.lAxisRz = (long)((value - 32768));
            break;
        case 0x39:
            ZeroMemory(pGamePad.bHat, sizeof(pGamePad.bHat));
            pGamePad.bHat[value - 1] = 1;
            int i = value - 1;
            if (i == GPH_LEFTDOWN)
            {
                pGamePad.bHat[GPH_LEFT] = 1;
                pGamePad.bHat[GPH_DOWN] = 1;
            }
            else if (i == GPH_RIGHTDOWN)
            {
                pGamePad.bHat[GPH_RIGHT] = 1;
                pGamePad.bHat[GPH_DOWN] = 1;
            }
            else if (i == GPH_LEFTUP)
            {
                pGamePad.bHat[GPH_LEFT] = 1;
                pGamePad.bHat[GPH_UP] = 1;
            }
            else if (i == GPH_RIGHTUP)
            {
                pGamePad.bHat[GPH_RIGHT] = 1;
                pGamePad.bHat[GPH_UP] = 1;
            }
            break;
        }
    }

    long joyDeadZone = 8192;
    if (pGamePad.lAxisX < joyDeadZone && pGamePad.lAxisX > -joyDeadZone)
        pGamePad.lAxisX = 0;
    if (pGamePad.lAxisY < joyDeadZone && pGamePad.lAxisY > -joyDeadZone)
        pGamePad.lAxisY = 0;
    if (pGamePad.lAxisZ < joyDeadZone && pGamePad.lAxisZ > -joyDeadZone)
        pGamePad.lAxisZ = 0;
    if (pGamePad.lAxisRz < joyDeadZone && pGamePad.lAxisRz > -joyDeadZone)
        pGamePad.lAxisRz = 0;

}
