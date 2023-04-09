//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    input handling
//-----------------------------------------------------------------------------

#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>
#include <math.h>
#include <tchar.h>
extern "C"
{
#include <hidsdi.h>
}

#define     NUM_KEYS        256
#define     NUM_BUTTONS     10
#define     NUM_M_BUTTONS   2
#define     SAFE_FREE(p)    { if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }
//#define     CHECK(exp)      { if(!(exp)) goto Error; }

class KeyBoard
{
public:
    bool    bKeys[NUM_KEYS];
    bool    bKeys_state[2][NUM_KEYS];
    int     input_state;

    KeyBoard();
    ~KeyBoard();

    void    update();

    bool    keyPressed(unsigned char key);
    bool    keyReleased(unsigned char key);
    bool    keyDown(unsigned char key);
    bool    keyUp(unsigned char key);
};

class Mouse
{
public:
    bool    bButtons[NUM_M_BUTTONS];
    bool    bButtons_state[2][NUM_M_BUTTONS];
    long    lX;
    long    lX_state[2];
    long    lY;
    long    lY_state[2];
    int     input_state;

    Mouse();
    ~Mouse();

    void    update();

    long    ldX();
    long    ldY();

    bool    buttonPressed();
    bool    buttonReleased();
    bool    buttonDown();
    bool    buttonUp();
};

typedef enum GP_HatDirection
{
    GPH_UP = 0,
    GPH_RIGHTUP = 1,
    GPH_RIGHT = 2,
    GPH_RIGHTDOWN = 3,
    GPH_DOWN = 4,
    GPH_LEFTDOWN = 5,
    GPH_LEFT = 6,
    GPH_LEFTUP = 7
} GP_HatDirection;

typedef enum GP_Button
{
    GPB_A = 0,
    GPB_B = 1,
    GPB_X = 2,
    GPB_Y = 3,
    GPB_LB = 4,
    GPB_RB = 5,
    GPB_SL = 6,
    GPB_ST = 7,
    GPB_LS = 8,
    GPB_RS = 9
} GP_Button;

typedef enum GP_AxisDirection
{
    GPA_LEFT = 1,
    GPA_RIGHT = 2
} GP_AxisDirection;

class GamePad
{
public:
    int     numButtons;

    bool    bButtons[NUM_BUTTONS];
    bool    bButtons_state[2][NUM_BUTTONS];
    long    lAxisX;
    long    lAxisX_state[2];
    long    lAxisY;
    long    lAxisY_state[2];
    long    lAxisZ;
    long    lAxisZ_state[2];
    long    lAxisRz;
    long    lAxisRz_state[2];
    bool    bHat_state[2][8];
    bool    bHat[8];

    int     input_state;

    GamePad();
    ~GamePad();

    void    update();

    bool    buttonPressed(int button);
    bool    buttonReleased(int button);
    bool    buttonDown(int button);
    bool    buttonUp(int button);

    bool    hatPressed(GP_HatDirection direction);
    bool    hatReleased(GP_HatDirection direction);
    bool    hatDown(GP_HatDirection direction);
    bool    hatUp(GP_HatDirection direction);

    bool    lAxisXPressed(GP_AxisDirection direction);
    bool    lAxisXReleased(GP_AxisDirection direction);
    bool    lAxisYPressed(GP_AxisDirection direction);
    bool    lAxisYReleased(GP_AxisDirection direction);

    float   fAxisX();
    float   fAxisY();
    float   fAxisZ();
    float   fAxisRz();
};

class Input
{
public:
    KeyBoard    keyboard;
    Mouse       mouse;
    GamePad     gamepad;

    Input();
    ~Input();

    void        update();
};

void ReadGamePadInput(PRAWINPUT pRawInput, GamePad &pGamePad);

#endif
