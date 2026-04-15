//-----------------------------------------------------------------------------
//  2d platform game engine and level editor
//    email: 
//
//  DESCRIPTION:    main entry point
//-----------------------------------------------------------------------------

#include "game.h"
#include "game_time.h"

#include <cstring>

bool bdebugcollisions = false;
bool bdebugtext = false;
bool bdrawhitboxes = false;
bool btogglefullscreen = false;
bool bmodechange = false;
bool bdoublebuffer = false;
bool bbilinear = false;
bool bcurvature = false;
bool bpause = false;
bool bstarted = false;
bool btransitioning = false;
bool bdrawspritelayer = true;
bool bdrawbglayer01 = true;
bool bdrawbglayer02 = true;
bool bdrawbglayer03 = true;
bool bdrawbglayer04 = true;
bool bdrawbglayer05 = true;
bool bplayercontrol = true;
bool blighting = true;
bool bfixedaspect = true;
bool bsnapscale = true;

Game game;
GLWindow window;
GameTime gametime;
Font font;
Camera camera;
World world;
Player player;
Input input;
Sound sound;
SDL_GameController* g_controller = nullptr;

static void requestQuit()
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}

static void pollControllerState()
{
    std::memset(input.gamepad.bButtons, 0, sizeof(input.gamepad.bButtons));
    std::memset(input.gamepad.bHat, 0, sizeof(input.gamepad.bHat));
    input.gamepad.lAxisX = 0;
    input.gamepad.lAxisY = 0;
    input.gamepad.lAxisZ = 0;
    input.gamepad.lAxisRz = 0;

    if (!g_controller)
        return;

    input.gamepad.bButtons[GPB_A] =
        SDL_GameControllerGetButton(g_controller, SDL_CONTROLLER_BUTTON_A) != 0;
    input.gamepad.bButtons[GPB_B] =
        SDL_GameControllerGetButton(g_controller, SDL_CONTROLLER_BUTTON_B) != 0;
    input.gamepad.bButtons[GPB_X] =
        SDL_GameControllerGetButton(g_controller, SDL_CONTROLLER_BUTTON_X) != 0;
    input.gamepad.bButtons[GPB_Y] =
        SDL_GameControllerGetButton(g_controller, SDL_CONTROLLER_BUTTON_Y) != 0;
    input.gamepad.bButtons[GPB_LB] = SDL_GameControllerGetButton(
                                         g_controller,
                                         SDL_CONTROLLER_BUTTON_LEFTSHOULDER) != 0;
    input.gamepad.bButtons[GPB_RB] = SDL_GameControllerGetButton(
                                         g_controller,
                                         SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) != 0;
    input.gamepad.bButtons[GPB_SL] = SDL_GameControllerGetButton(
                                         g_controller, SDL_CONTROLLER_BUTTON_BACK) != 0;
    input.gamepad.bButtons[GPB_ST] = SDL_GameControllerGetButton(
                                         g_controller, SDL_CONTROLLER_BUTTON_START) != 0;
    input.gamepad.bButtons[GPB_LS] = SDL_GameControllerGetButton(
                                         g_controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) != 0;
    input.gamepad.bButtons[GPB_RS] = SDL_GameControllerGetButton(
                                         g_controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK) != 0;

    const bool dpadUp =
        SDL_GameControllerGetButton(g_controller, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0;
    const bool dpadDown = SDL_GameControllerGetButton(
                              g_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0;
    const bool dpadLeft = SDL_GameControllerGetButton(
                              g_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0;
    const bool dpadRight = SDL_GameControllerGetButton(
                               g_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0;

    input.gamepad.bHat[GPH_UP] = dpadUp;
    input.gamepad.bHat[GPH_DOWN] = dpadDown;
    input.gamepad.bHat[GPH_LEFT] = dpadLeft;
    input.gamepad.bHat[GPH_RIGHT] = dpadRight;
    input.gamepad.bHat[GPH_LEFTUP] = dpadLeft && dpadUp;
    input.gamepad.bHat[GPH_RIGHTUP] = dpadRight && dpadUp;
    input.gamepad.bHat[GPH_LEFTDOWN] = dpadLeft && dpadDown;
    input.gamepad.bHat[GPH_RIGHTDOWN] = dpadRight && dpadDown;

    input.gamepad.lAxisX =
        SDL_GameControllerGetAxis(g_controller, SDL_CONTROLLER_AXIS_LEFTX);
    input.gamepad.lAxisY =
        -SDL_GameControllerGetAxis(g_controller, SDL_CONTROLLER_AXIS_LEFTY);
    input.gamepad.lAxisZ =
        SDL_GameControllerGetAxis(g_controller, SDL_CONTROLLER_AXIS_RIGHTX);
    input.gamepad.lAxisRz =
        -SDL_GameControllerGetAxis(g_controller, SDL_CONTROLLER_AXIS_RIGHTY);

    const long joyDeadZone = 8192;
    if (input.gamepad.lAxisX < joyDeadZone &&
        input.gamepad.lAxisX > -joyDeadZone)
        input.gamepad.lAxisX = 0;
    if (input.gamepad.lAxisY < joyDeadZone &&
        input.gamepad.lAxisY > -joyDeadZone)
        input.gamepad.lAxisY = 0;
    if (input.gamepad.lAxisZ < joyDeadZone &&
        input.gamepad.lAxisZ > -joyDeadZone)
        input.gamepad.lAxisZ = 0;
    if (input.gamepad.lAxisRz < joyDeadZone &&
        input.gamepad.lAxisRz > -joyDeadZone)
        input.gamepad.lAxisRz = 0;
}

Game::Game()
{
    game.menu.sliderCaptured = false;
    game.menu.current = NULL;
    game.menu.start.name = "Game Title";
    game.menu.pause.name = "Pause";
    game.menu.options.name = "Options";
    game.menu.video.name = "Video";
    game.menu.sound.name = "Sound";
    game.shaders.blurValue = 10.0f;
    game.shaders.curvValue = 0.0f;
    game.menu.menuDarken = 0.0f;
}

void G_initializeGame()
{
    game.loadConfig("config.ini");

    window.initialize();

    game.clientRect =
        ScreenRect(0, 0, window.currentWidth(), window.currentHeight());
    game.aspectRatio = game.clientRect.w / game.clientRect.h;
    game.sceneRect = ScreenRect(0, 0, 1920, 1080);

    player.initialize(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                      &game.atlases.sprites);
    player.frames = &game.atlases.test;

    initializeTileAnimations();

    sound.load();

    G_initializeEvents();

    U_Load_demo();

    font.initialize(16, 16, 8, 8, 1.0f, &camera, &game.clientRect,
                    &game.atlases.font);

    world.gravity = Vector2(0.0f, GRAVITY);

    G_initializeMenus();

    bpause = true;

    camera.pTarget = (MovingObject*)&player;

    gametime.initialize();
}

void G_loadLevel(const char* file)
{
    // TODO
    camera.initialize(64, 0, game.sceneRect.w, game.sceneRect.h);
    camera.isTargeting = true;

    world.loadLevel(file, &camera);
    world.constraints = WorldRect(
        world.level.layer[2].w_tileSize.x, 0,
        world.level.layer[2].t_mapSize.x * world.level.layer[2].w_tileSize.x -
            2 * world.level.layer[2].w_tileSize.x,
        world.level.layer[2].t_mapSize.y * world.level.layer[2].w_tileSize.y);
    world.enemies.clear();
    for (int i = 0; i < world.level.enemySpawns.size(); i++)
    {
        float top_padding = (world.level.enemySpawns[i].type == 2 ||
                             world.level.enemySpawns[i].type == 1)
                                ? 72.0f
                                : 32.0f;
        world.enemies.push_back(
            Enemy((ENEMY_TYPE)world.level.enemySpawns[i].type,
                  world.level.enemySpawns[i].position.x,
                  world.level.enemySpawns[i].position.y, 64.0f, 128.0f,
                  world.level.enemySpawns[i].direction * ENEMY1_X_SPEED, 0.0f,
                  8.0f, 8.0f, top_padding, 8.0f, &game.atlases.sprites));
    }

    player.initialize(world.level.playerSpawn.position.x,
                      world.level.playerSpawn.position.y, 64.0f, 128.0f, 8.0f,
                      8.0f, 16.0f, 8.0f, &game.atlases.sprites);
    player.direction = world.level.playerSpawn.direction;
    player.hit = 0;
    player.hitEnemy = 0;
    player.health = 2;

    sound.setReverb(world.level.reverb);
    sound.StopAll();
    if (world.level.musicID > -1)
        sound.PlayMusic((SOUND_ID)world.level.musicID);

    gametime.currentFrame = 0;
}

void U_Load_demo()
{
    game.menu.savedVolume = sound.sfxVolume;

    game.clips.demo = Timeline("game.clips.demo", 1000);

    game.clips.demo.AddEvent(0,
                             []()
                             {
                                 G_loadLevel("levels/demo.lvl");
                                 sound.changeSfxVolume(-1.0f);
                             });

    game.clips.demo.AddEvent(0, []() { game.menu.menuDarken = 1.0f; });
    game.clips.demo.AddEvent(1, []() { G_playEvent("fadefromblack", false); });
    game.clips.demo.AddEvent(100, []() { camera.toMove.x = 1; });
    game.clips.demo.AddEvent(60, []() { player.toMove.x = 1; });
    game.clips.demo.AddEvent(140, []() { player.toMove.y = 1; });
    game.clips.demo.AddEvent(150, []() { player.toMove.y = 0; });
    game.clips.demo.AddEvent(180, []() { player.toMove.y = 1; });
    game.clips.demo.AddEvent(180, []() { player.isRunning = true; });
    game.clips.demo.AddEvent(245, []() { player.toMove.y = 1; });
    game.clips.demo.AddEvent(370, []() { player.toMove.y = 1; });
    game.clips.demo.AddEvent(420, []() { player.isRunning = false; });
    game.clips.demo.AddEvent(440, []() { player.toMove.x = 0; });
    game.clips.demo.AddEvent(500, []() { camera.toMove.x = -1; });
    game.clips.demo.AddEvent(800, []() { G_playEvent("fadetoblack", false); });
    game.clips.demo.AddEvent(860, []() { game.menu.menuDarken = 1.0f; });

    game.clips.demo.Play(true);
}

void I_resetControls()
{
    camera.toMove.x = camera.toMove.y = player.toMove.x = player.toMove.y = 0;
}

void I_zeroGameInput()
{
    std::memset(input.gamepad.bButtons_state[0], 0,
                sizeof(input.gamepad.bButtons));
    std::memset(input.keyboard.bKeys_state[0], 0, sizeof(input.keyboard.bKeys));
    std::memset(input.gamepad.bButtons_state[1], 0,
                sizeof(input.gamepad.bButtons));
    std::memset(input.keyboard.bKeys_state[1], 0, sizeof(input.keyboard.bKeys));
    std::memset(input.gamepad.bHat_state[0], 0, sizeof(input.gamepad.bHat));
    std::memset(input.gamepad.bHat_state[1], 0, sizeof(input.gamepad.bHat));
}

bool G_playEvent(std::string name, bool loop)
{
    for (int i = 0; i < game.events.size(); i++)
    {
        if (!name.compare(game.events[i].name))
        {
            game.events[i].Play(loop);
            return true;
        }
    }
    return false;
}

bool G_stopEvent(std::string name)
{
    for (int i = 0; i < game.events.size(); i++)
    {
        if (!name.compare(game.events[i].name))
        {
            game.events[i].Stop();
            return true;
        }
    }
    return false;
}

void G_initializeEvents()
{
    game.clips.fadeIn = Timeline("fadetoblack", 50);
    game.clips.fadeIn.AddEvent(0, []() {});
    for (int i = 1; i < 25; i++)
        game.clips.fadeIn.AddEvent(2 * i,
                                   []() { game.menu.menuDarken += 0.04f; });
    game.clips.fadeIn.AddEvent(49, []() { game.menu.menuDarken = 1.0f; });

    game.clips.fadeOut = Timeline("fadefromblack", 50);
    game.clips.fadeOut.AddEvent(1, []() { game.menu.menuDarken = 1.0f; });
    for (int i = 1; i < 25; i++)
        game.clips.fadeOut.AddEvent(2 * i,
                                    []() { game.menu.menuDarken -= 0.04f; });
    game.clips.fadeOut.AddEvent(49, []() { game.menu.menuDarken = 0.0f; });

    game.clips.gameStart = Timeline("start", 100);
    game.clips.gameStart.AddEvent(0,
                                  []()
                                  {
                                      btransitioning = true;
                                      P_pause(false);
                                      game.menu.current = &game.menu.pause;
                                      bplayercontrol = false;
                                      G_playEvent("fadetoblack", false);
                                  });
    game.clips.gameStart.AddEvent(50,
                                  []()
                                  {
                                      game.clips.demo.Stop();
                                      G_loadLevel("levels/stage1.lvl");
                                      sound.sfxVolume = game.menu.savedVolume;
                                      G_playEvent("fadefromblack", false);
                                  });
    game.clips.gameStart.AddEvent(99,
                                  []()
                                  {
                                      btransitioning = false;
                                      bstarted = true;
                                      bplayercontrol = true;
                                      camera.isTargeting = true;
                                      I_zeroGameInput();
                                      I_resetControls();
                                  });

    game.clips.gameQuit = Timeline("quit", 50);
    game.clips.gameQuit.AddEvent(0,
                                 []()
                                 {
                                     btransitioning = true;
                                     P_pause(false);
                                     game.menu.current = &game.menu.start;
                                     bstarted = false;
                                     bplayercontrol = false;
                                     G_playEvent("fadetoblack", false);
                                 });
    game.clips.gameQuit.AddEvent(49,
                                 []()
                                 {
                                     btransitioning = false;
                                     P_pause(true);
                                     U_Load_demo();
                                 });

    game.events.push_back(game.clips.fadeIn);
    game.events.push_back(game.clips.fadeOut);
    game.events.push_back(game.clips.gameStart);
    game.events.push_back(game.clips.gameQuit);
}

void GL_initializeTextures() { game.initializeTextures(); }

void Game::initializeTextures()
{
    atlases.test.initialize(&textures.sprites);
    atlases.test.loadFromFile(resolveResourcePath("sprites/smw.spr").c_str());

    textures.sprites =
        loadBMPTexture_ARGB(resolveResourcePath(atlasInfo.sprites.file).c_str());
    atlases.sprites.initialize(&textures.sprites, atlasInfo.sprites.framesWide,
                               atlasInfo.sprites.framesHigh,
                               atlasInfo.sprites.frameWidth,
                               atlasInfo.sprites.frameHeight);
    textures.tiles =
        loadBMPTexture_ARGB(resolveResourcePath(atlasInfo.tiles.file).c_str());
    atlases.tiles.initialize(
        &textures.tiles, atlasInfo.tiles.framesWide, atlasInfo.tiles.framesHigh,
        atlasInfo.tiles.frameWidth, atlasInfo.tiles.frameHeight);
    textures.bgs =
        loadBMPTexture_ARGB(resolveResourcePath(atlasInfo.bgs.file).c_str());
    atlases.bgs.initialize(&textures.bgs, atlasInfo.bgs.framesWide,
                           atlasInfo.bgs.framesHigh, atlasInfo.bgs.frameWidth,
                           atlasInfo.bgs.frameHeight);

    textures.font =
        loadBMPTexture_ARGB(resolveResourcePath("textures/font_8x8.bmp").c_str());
    atlases.font.initialize(&game.textures.font, 16, 16, 8, 8);

    textures.light =
        loadBMPTexture_ARGB(resolveResourcePath("textures/light.bmp").c_str());
    textures.cursor =
        loadBMPTexture_ARGB(resolveResourcePath("textures/cursor.bmp").c_str());
    textures.slider =
        loadBMPTexture_ARGB(resolveResourcePath("textures/slider.bmp").c_str());
};

void Game::loadConfig(const char* filename)
{
    string line;
    ifstream file(resolveResourcePath(filename));
    istringstream ssf;
    AtlasInfo* info;
    if (file.is_open())
    {
        getline(file, line);
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        ssf = istringstream(line);
        info = &atlasInfo.sprites;
        ssf >> info->file >> info->framesWide >> info->framesHigh >>
            info->frameWidth >> info->frameHeight;
        getline(file, line);
        ssf = istringstream(line);
        info = &atlasInfo.tiles;
        ssf >> info->file >> info->framesWide >> info->framesHigh >>
            info->frameWidth >> info->frameHeight;
        getline(file, line);
        ssf = istringstream(line);
        info = &atlasInfo.bgs;
        ssf >> info->file >> info->framesWide >> info->framesHigh >>
            info->frameWidth >> info->frameHeight;
        file.close();
    }
};

void P_pause(bool pause)
{
    bpause = pause;
    if (pause)
    {
        sound.changeMasterVolume(-0.75f);
        sound.cacheReverb(true);
        game.menu.menuDarken = 0.5f;
    }
    else
    {
        if (!btransitioning)
        {
            I_zeroGameInput();
            I_resetControls();
        }
        if (bstarted)
        {
            sound.sfxVolume = game.menu.savedVolume;
            sound.cacheReverb(false);
        }
        sound.changeMasterVolume(0.75f);
        game.menu.menuDarken = 0.0f;
    }
}

void G_initializeMenus()
{
    float ty = 0, dth = 32.0f, tx = 0;
    game.menu.start.addItem(tx, ty, false, "Start",
                            _L(G_playEvent("start", false);));
    ty += dth;
    game.menu.start.addItem(tx, ty, false, "Options",
                            []()
                            {
                                game.menu.current = &game.menu.options;
                                game.menu.currentItem = 0;
                            });
    ty += dth;
    game.menu.start.addItem(tx, ty, false, "Exit",
                            []() { requestQuit(); });

    ty = 0;
    game.menu.pause.addItem(tx, ty, false, "Resume", []() { P_pause(false); });
    ty += dth;
    game.menu.pause.addItem(tx, ty, false, "Options",
                            []()
                            {
                                game.menu.current = &game.menu.options;
                                game.menu.currentItem = 0;
                            });
    ty += dth;
    game.menu.pause.addItem(tx, ty, false, "Quit",
                            []() { G_playEvent("quit", false); });

    ty = 0;
    game.menu.options.addItem(tx, ty, false, "Video",
                              []()
                              {
                                  game.menu.current = &game.menu.video;
                                  game.menu.currentItem = 0;
                              });
    ty += dth;
    game.menu.options.addItem(tx, ty, false, "Sound",
                              []()
                              {
                                  game.menu.current = &game.menu.sound;
                                  game.menu.currentItem = 4;
                              });
    ty += dth;
    game.menu.options.addItem(tx, ty, false, "Debug",
                              []() { bdebugtext = !bdebugtext; });
    ty += dth;
    game.menu.options.addItem(tx, ty, false, "Back",
                              []()
                              {
                                  game.menu.current = bstarted
                                                          ? &game.menu.pause
                                                          : &game.menu.start;
                                  game.menu.currentItem = 0;
                              });
    game.menu.options.items[2].boolValue = &bdebugtext;

    ty = 0;
    game.menu.video.addItem(tx, ty, false, "Fullscreen",
                            []() { btogglefullscreen = true; });
    ty += dth;
    game.menu.video.addItem(tx, ty, false, "DoubleBuffer",
                            []() {
                                bdoublebuffer = window.isFullscreen
                                                    ? bdoublebuffer
                                                    : !bdoublebuffer;
                            });
    ty += dth;
    game.menu.video.addItem(tx, ty, false, "Bilinear",
                            []() { bbilinear = !bbilinear; });
    ty += dth;
    game.menu.video.addItem(tx, ty, false, "Curvature",
                            []() { bcurvature = !bcurvature; });
    ty += dth;
    game.menu.video.addItem(tx, ty, true, "slider", []() {});
    ty += dth;
    game.menu.video.addItem(tx, ty, false, "Fixed Aspect",
                            []() { bfixedaspect = !bfixedaspect; });
    ty += dth;
    game.menu.video.addItem(tx, ty, false, "Snap Scale",
                            []() { bsnapscale = !bsnapscale; });
    ty += dth;
    game.menu.video.addItem(tx, ty, false, "Back",
                            []()
                            {
                                game.menu.current = &game.menu.options;
                                game.menu.currentItem = 3;
                            });
    game.menu.video.items[0].boolValue = &window.isFullscreen;
    game.menu.video.items[1].boolValue = &bdoublebuffer;
    game.menu.video.items[2].boolValue = &bbilinear;
    game.menu.video.items[3].boolValue = &bcurvature;
    game.menu.video.items[4].floatValue = &game.shaders.curvValue;
    game.menu.video.items[5].boolValue = &bfixedaspect;
    game.menu.video.items[6].boolValue = &bsnapscale;

    ty = 0;
    game.menu.sound.addItem(tx, ty, false, "Music Volume", NULL);
    ty += dth;
    game.menu.sound.addItem(tx, ty, true, "slider", []() {});
    ty += dth;
    game.menu.sound.addItem(tx, ty, false, "Sound Volume", NULL);
    ty += dth;
    game.menu.sound.addItem(tx, ty, true, "slider", []() {});
    ty += dth;
    game.menu.sound.addItem(tx, ty, false, "Back",
                            []()
                            {
                                game.menu.current = &game.menu.options;
                                game.menu.currentItem = 3;
                            });
    game.menu.sound.items[1].floatValue = &sound.musicVolume;
    game.menu.sound.items[3].floatValue = &game.menu.savedVolume;

    game.menu.current = &game.menu.start;
    game.menu.currentItem = 0;
}

void GL_initializeGLObjects()
{
    // GLEW is now initialized in GLWindow::initGL
    GL_initializeTextures();
    GL_initializeFrameBuffers();
    GL_initializeShaders();
}

void P_computeFPS()
{
    gametime.fpsFrame++;
    gametime.fpsTime = gametime.getCurrentMilliseconds();

    if (gametime.fpsTime - gametime.lastfpsTime > gametime.fpsPollingTime)
    {
        gametime.fps = gametime.fpsFrame * 1000.0 /
                       (gametime.fpsTime - gametime.lastfpsTime);
        gametime.lastfpsTime = gametime.fpsTime;
        gametime.fpsFrame = 0;
    }
}

void G_update(double elapsed_time)
{
    sound.update();
    input.update();

    for (int i = 0; i < game.events.size(); i++)
        game.events[i].update();

    I_updateProgramInput(&input);

    if (bpause || btransitioning) // PAUSED
    {
        gametime.pauseTime += elapsed_time;

        if (!btransitioning)
            I_updateMenuInput(&input);

        if (!bstarted) // DEMO/STARTSCREEN
        {
            game.clips.demo.update();
            if (player.hit < 60)
            {
                world.level.layer[3].update();
                world.level.layer[2].update();
                world.level.layer[1].update();
                world.level.layer[0].update();
                world.level.dynTileMaps.update(gametime.currentFrame);
                player.update(
                    &world.constraints, &world.level.layer[2],
                    &world.level.layer[3], &(world.level.dynTileMaps.tileMaps),
                    world.gravity, MS_PER_FRAME, &camera, &game.clientRect);
                for (unsigned int i = 0; i < world.enemies.size(); i++)
                {
                    world.enemies[i].update(
                        &world.constraints, &world.level.layer[2],
                        &(world.level.dynTileMaps.tileMaps), world.gravity,
                        MS_PER_FRAME, &camera, &game.clientRect, i);
                }
                camera.update(&world.constraints);
            }
            else
            {
                player.hit--;
            }
        }
    }
    else // IN GAME
    {
        if (player.hit < 60)
        {
            gametime.gameTime += elapsed_time;
            gametime.currentFrame++;

            if (bplayercontrol)
                I_updateGameInput(&input);

            world.level.layer[3].update();
            world.level.layer[2].update();
            world.level.layer[1].update();
            world.level.layer[0].update();
            world.level.dynTileMaps.update(gametime.currentFrame);
            player.update(&world.constraints, &world.level.layer[2],
                          &world.level.layer[3],
                          &(world.level.dynTileMaps.tileMaps), world.gravity,
                          MS_PER_FRAME, &camera, &game.clientRect);
            for (unsigned int i = 0; i < world.enemies.size(); i++)
            {
                world.enemies[i].update(
                    &world.constraints, &world.level.layer[2],
                    &(world.level.dynTileMaps.tileMaps), world.gravity,
                    MS_PER_FRAME, &camera, &game.clientRect, i);
                if (world.enemies[i].toRemove)
                    remove(world.enemies, i);
            }

            if (player.position.y <= world.constraints.y - player.size.y ||
                !player.health)
            {
                player.reinitialize(240.0f, 120.0f);
                camera.initialize(64, 0, camera.size.x, camera.size.y);
            }
            camera.update(&world.constraints);
        }
        else
        {
            player.hit--;
            if (player.hit == 59)
            {
                I_resetControls();
                I_zeroGameInput();
            }
        }
    }
}

void R_updateUniforms()
{
    glUseProgramObjectARB(game.shaders.curvature);
    glUniform1f(game.shaders.uniformCurvature, game.shaders.curvValue);
    glUseProgramObjectARB(game.shaders.horizontalBlur);
    glUniform1f(game.shaders.uniformHBlurSize,
                game.shaders.blurValue / game.clientRect.w);
    glUseProgramObjectARB(game.shaders.verticalBlur);
    glUniform1f(game.shaders.uniformVBlurSize,
                game.shaders.blurValue / game.clientRect.h);
    glUseProgramObjectARB(game.shaders.bilinear);
    glUniform1f(game.shaders.uniformHBilinearSize, 2.0f / game.sceneRect.w);
    glUniform1f(game.shaders.uniformVBilinearSize, 2.0f / game.sceneRect.h);
    glUseProgramObjectARB(NULL);
}

void I_updateGameInput(Input* pinput)
{
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_KP_1))
        G_loadLevel("levels/stage1.lvl");
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_KP_2))
        G_loadLevel("levels/stage2.lvl");
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_KP_3))
        G_loadLevel("levels/stage3.lvl");

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_T))
        camera.isTargeting = !camera.isTargeting;
    if (pinput->keyboard.keyPressed(SDL_SCANCODE_RIGHT))
        camera.toMove.x = 1;
    if (pinput->keyboard.keyPressed(SDL_SCANCODE_LEFT))
        camera.toMove.x = -1;
    if (pinput->keyboard.keyPressed(SDL_SCANCODE_UP))
        camera.toMove.y = 1;
    if (pinput->keyboard.keyPressed(SDL_SCANCODE_DOWN))
        camera.toMove.y = -1;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_RIGHT))
        camera.toMove.x == 1 ? camera.toMove.x = 0 : NULL;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_LEFT))
        camera.toMove.x == -1 ? camera.toMove.x = 0 : NULL;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_UP))
        camera.toMove.y == 1 ? camera.toMove.y = 0 : NULL;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_DOWN))
        camera.toMove.y == -1 ? camera.toMove.y = 0 : NULL;

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_DOWN))
        camera.toMove.y == -1 ? camera.toMove.y = 0 : NULL;

    if (pinput->keyboard.keyPressed(SDL_SCANCODE_W) ||
        pinput->gamepad.hatPressed(GPH_UP))
    {
        player.lookingUp = true;
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_W) ||
        pinput->gamepad.hatReleased(GPH_UP))
    {
        player.lookingUp = false;
    }

    if (pinput->keyboard.keyPressed(SDL_SCANCODE_A) ||
        pinput->gamepad.hatPressed(GPH_LEFT))
    {
        player.toMove.x = -1;
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_A) ||
        pinput->gamepad.hatReleased(GPH_LEFT))
    {
        if (player.toMove.x == -1)
            player.toMove.x = 0;
    }

    if (pinput->keyboard.keyPressed(SDL_SCANCODE_D) ||
        pinput->gamepad.hatPressed(GPH_RIGHT))
    {
        player.toMove.x = 1;
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_D) ||
        pinput->gamepad.hatReleased(GPH_RIGHT))
    {
        if (player.toMove.x == 1)
            player.toMove.x = 0;
    }

    if (pinput->keyboard.keyPressed(SDL_SCANCODE_S) ||
        pinput->gamepad.hatPressed(GPH_DOWN))
    {
        player.toMove.y = -1;
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_S) ||
        pinput->gamepad.hatReleased(GPH_DOWN))
    {
        if (player.toMove.y == -1)
            player.toMove.y = 0;
    }

    if (pinput->keyboard.keyDown(SDL_SCANCODE_S) ||
        pinput->gamepad.hatDown(GPH_DOWN))
    {
        if (!player.crouching && !player.holding &&
            (player.currentState != CS_JUMPING &&
             player.currentState != CS_FALLING))
        {
            player.crouching = true;
            if (player.health > 1)
                player.topPadding += (player.size.y / 2 - player.bottomPadding);
        }
    }
    else if (pinput->keyboard.keyUp(SDL_SCANCODE_S) ||
             pinput->gamepad.hatUp(GPH_DOWN))
    {
        if (player.crouching && (player.currentState != CS_JUMPING &&
                                 player.currentState != CS_FALLING))
        {
            player.crouching = false;
            if (player.health > 1)
                player.topPadding = player.storeTopPadding;
        }
    }

    if (pinput->keyboard.keyPressed(SDL_SCANCODE_N) ||
        pinput->gamepad.buttonPressed(GPB_A))
    {
        if (!player.swimming && player.canJump && !player.isSquished &&
            (player.currentState != CS_FALLING &&
             player.currentState != CS_JUMPING))
        {
            player.canJump = false;
            sound.PlaySfx(SOUND_JUMP);
            player.toMove.y = 1;
        }
        else if (player.swimming && !player.cantSwim)
        {
            player.jumpCountDown = JUMPFRAMES * 0.5f;
            if (player.cantSwim < 5)
                sound.PlaySfx(SOUND_SWIM);
            player.toMove.y = 1;
            player.cantSwim = 10;
        }
    }
    if (pinput->keyboard.keyDown(SDL_SCANCODE_M) ||
        pinput->gamepad.buttonDown(GPB_X))
    {
        if ((player.ycol || player.xycol) && !player.swimming)
            player.isRunning = true;
        player.tryingToHold = true;
    }

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_N) ||
        pinput->gamepad.buttonReleased(GPB_A))
    {
        player.canJump = true;
        if (player.toMove.y == 1)
            player.toMove.y = 0;
    }

    if (pinput->keyboard.keyDown(SDL_SCANCODE_Y) ||
        pinput->gamepad.buttonDown(GPB_Y))
    {
        if (gametime.currentFrame % 4 == 0)
        {
            if (!player.lookingUp)
            {
                world.enemies.push_back(
                    Enemy(ET_SHELL,
                          player.position.x +
                              ((player.direction + 1) / 2) * player.size.x -
                              (1 - (player.direction + 1) / 2) * 64.0f,
                          player.position.y, 64.0f, 128.0f,
                          ENEMY2_X_SPEED * player.direction, 10.0f, 12.0f,
                          12.0f, 72.0f, 8.0f, &game.atlases.sprites));
            }
            else
            {
                world.enemies.push_back(
                    Enemy(ET_SHELL,
                          player.position.x +
                              ((player.direction + 1) / 2) * player.size.x -
                              (1 - (player.direction + 1) / 2) * 64.0f,
                          player.position.y, 64.0f, 128.0f, 0.0f,
                          MAX_SHELL_Y_VELOCITY, 12.0f, 12.0f, 72.0f, 8.0f,
                          &game.atlases.sprites));
            }
        }
    }

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_M) ||
        pinput->gamepad.buttonReleased(GPB_X))
    {
        if (player.isRunning == true)
            player.isRunning = false;
        player.tryingToHold = false;
        if (player.holding)
        {
            sound.PlaySfx(SOUND_STOMP);
            player.kicking = 20;
            for (int i = 0; i < world.enemies.size(); i++)
            {
                if (world.enemies[i].beingHeld)
                {
                    world.enemies[i].beingHeld = false;
                    world.enemies[i].pHolder = NULL;
                    world.enemies[i].cantHitPlayer = CANTHITPLAYERFRAMES;
                    if (player.lookingUp)
                    {
                        world.enemies[i].direction = player.direction;
                        world.enemies[i].velocity.y = 36.0f;
                        world.enemies[i].velocity.x = 0;
                    }
                    else if (player.direction == 1)
                    {
                        world.enemies[i].direction = 1;
                        world.enemies[i].velocity.y = 10.0f;
                        world.enemies[i].position.x -=
                            world.enemies[i].size.x / 4;
                        world.enemies[i].velocity.x = ENEMY2_X_SPEED;
                    }
                    else
                    {
                        world.enemies[i].direction = -1;
                        world.enemies[i].velocity.y = 10.0f;
                        world.enemies[i].position.x +=
                            world.enemies[i].size.x / 4;
                        world.enemies[i].velocity.x = -ENEMY2_X_SPEED;
                    }
                    continue;
                }
            }
            player.holding = 0;
        }
    }
}

void I_updateProgramInput(Input* pinput)
{
    if (pinput->gamepad.buttonDown(GPB_LB))
        gametime.frameMultiplier = 8.0;
    else if (pinput->gamepad.buttonDown(GPB_RB))
        gametime.frameMultiplier = 0.25;
    else if (pinput->gamepad.buttonUp(GPB_LB))
        gametime.frameMultiplier = 1.0;
    else if (pinput->gamepad.buttonUp(GPB_RB))
        gametime.frameMultiplier = 1.0;
    // options
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_1))
        bdrawspritelayer = !bdrawspritelayer;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_2))
        bdrawbglayer01 = !bdrawbglayer01;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_3))
        bdrawbglayer02 = !bdrawbglayer02;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_4))
        bdrawbglayer03 = !bdrawbglayer03;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_5))
        bdrawbglayer04 = !bdrawbglayer04;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_6))
        bdrawbglayer05 = !bdrawbglayer05;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_7))
        blighting = !blighting;

    if (pinput->keyboard.keyDown(SDL_SCANCODE_8))
        game.shaders.curvValue -= 0.01f;
    if (pinput->keyboard.keyDown(SDL_SCANCODE_9))
        game.shaders.curvValue = 0.0f;
    if (pinput->keyboard.keyDown(SDL_SCANCODE_0))
        game.shaders.curvValue += 0.01f;

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_J))
    {
        if (!window.isFullscreen && window.windowedMode < NUM_MODES - 1)
        {
            bmodechange = true;
            window.windowedMode =
                CLAMP(window.windowedMode + 1, 0, NUM_MODES - 1);
        }
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_K))
    {
        if (!window.isFullscreen && window.windowedMode > 0)
        {
            bmodechange = true;
            window.windowedMode =
                CLAMP(window.windowedMode - 1, 0, NUM_MODES - 1);
        }
    }

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F1))
        btogglefullscreen = true;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F2))
    {
        bdebugtext = !bdebugtext;
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F3))
        bdebugcollisions = !bdebugcollisions;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F4))
        bdrawhitboxes = !bdrawhitboxes;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F6))
        bdoublebuffer = window.isFullscreen ? bdoublebuffer : !bdoublebuffer;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F7))
        bbilinear = !bbilinear;
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_F8))
        bcurvature = !bcurvature;

    if (pinput->keyboard.keyDown(SDL_SCANCODE_O))
        game.shaders.blurValue =
            CLAMP(game.shaders.blurValue - 0.1f, 0.0f, 40.0f);
    if (pinput->keyboard.keyDown(SDL_SCANCODE_P))
        game.shaders.blurValue =
            CLAMP(game.shaders.blurValue + 0.1f, 0.0f, 40.0f);

    if (pinput->keyboard.keyPressed(SDL_SCANCODE_ESCAPE) ||
        pinput->gamepad.buttonPressed(GPB_SL))
        requestQuit();

    if (pinput->keyboard.keyReleased(SDL_SCANCODE_SPACE) ||
        pinput->gamepad.buttonReleased(GPB_ST))
    {
        if (bstarted)
        {
            sound.PlayGUISfx(SOUND_SELECT3);
            P_pause(!bpause);
        }
    }
}

void I_updateMenuInput(Input* pinput)
{
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_S) ||
        pinput->gamepad.hatReleased(GPH_DOWN))
    {
        bool select = false;
        int current_item = game.menu.currentItem;
        while (!select)
        {
            game.menu.currentItem++;
            if (game.menu.currentItem > game.menu.current->items.size() - 1)
                game.menu.currentItem = 0;
            select = game.menu.current->items[game.menu.currentItem].function;
        }
        if (game.menu.currentItem != current_item)
            sound.PlayGUISfx(SOUND_SELECT2);
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_W) ||
        pinput->gamepad.hatReleased(GPH_UP))
    {
        bool select = false;
        int current_item = game.menu.currentItem;
        while (!select)
        {
            game.menu.currentItem--;
            if (game.menu.currentItem < 0)
                game.menu.currentItem =
                    static_cast<int>(game.menu.current->items.size() - 1);
            select =
                (bool)game.menu.current->items[game.menu.currentItem].function;
        }
        if (game.menu.currentItem != current_item)
            sound.PlayGUISfx(SOUND_SELECT2);
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_A) ||
        pinput->gamepad.hatReleased(GPH_LEFT))
    {
        if (game.menu.current->items[game.menu.currentItem].isSlider)
        {
            *game.menu.current->items[game.menu.currentItem].floatValue = CLAMP(
                *game.menu.current->items[game.menu.currentItem].floatValue -
                    0.1f,
                0.0f, 1.0f);
        }
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_D) ||
        pinput->gamepad.hatReleased(GPH_RIGHT))
    {
        if (game.menu.current->items[game.menu.currentItem].isSlider)
        {
            *game.menu.current->items[game.menu.currentItem].floatValue = CLAMP(
                *game.menu.current->items[game.menu.currentItem].floatValue +
                    0.1f,
                0.0f, 1.0f);
        }
    }
    if (pinput->keyboard.keyReleased(SDL_SCANCODE_RETURN) ||
        pinput->gamepad.buttonReleased(GPB_A))
    {
        if (game.menu.current->items[game.menu.currentItem].function != NULL)
        {
            sound.PlayGUISfx(SOUND_SELECT3);
            game.menu.current->items[game.menu.currentItem].function();
        }
    }
}

int main(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError()
                  << std::endl;
        return 1;
    }

    G_initializeGame();
    window.createGLWindow("2D Platformer");
    GL_initializeGLObjects();

    gametime.lastGameTime = gametime.getCurrentTime();

    for (int joystick_index = 0;
         joystick_index < SDL_NumJoysticks() && !g_controller;
         ++joystick_index)
    {
        if (SDL_IsGameController(joystick_index))
            g_controller = SDL_GameControllerOpen(joystick_index);
    }

    SDL_Event event;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        R_resize(event.window.data1, event.window.data2);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode < NUM_KEYS)
                        input.keyboard.bKeys[event.key.keysym.scancode] = true;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.scancode < NUM_KEYS)
                        input.keyboard.bKeys[event.key.keysym.scancode] = false;
                    break;
                case SDL_MOUSEMOTION:
                    input.mouse.lX = event.motion.x;
                    input.mouse.lY = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        input.mouse.bButtons[0] = true;
                        if (bpause)
                        {
                            float sc = (window.scaleValue / 64.0f);
                            int mix =
                                (game.clientRect.w -
                                 sc * game.renderTargets.menuPanel.width) /
                                    2 +
                                sc * 64.0f +
                                game.menu.current->items[game.menu.currentItem]
                                    .x;
                            int miy =
                                (game.clientRect.h -
                                 sc * game.renderTargets.menuPanel.height) /
                                    2 +
                                sc * game.renderTargets.menuPanel.height / 2 +
                                sc * game.menu.current
                                         ->items[game.menu.currentItem]
                                         .y;
                            int miw = sc * 320.0f;
                            int mih = sc * 32.0f;
                            if (input.mouse.lX > mix &&
                                input.mouse.lX < mix + miw &&
                                input.mouse.lY > miy &&
                                input.mouse.lY < miy + mih)
                            {
                                if (game.menu.current
                                        ->items[game.menu.currentItem]
                                        .isSlider)
                                {
                                    game.menu.capturePoint =
                                        Point(input.mouse.lX, input.mouse.lY);
                                    int sx = (game.clientRect.w -
                                              sc * game.renderTargets.menuPanel
                                                       .width) /
                                                 2 +
                                             sc * 96.0f +
                                             game.menu.current
                                                 ->items[game.menu.currentItem]
                                                 .x;
                                    *game.menu.current
                                         ->items[game.menu.currentItem]
                                         .floatValue =
                                        CLAMP((game.menu.capturePoint.x - sx) /
                                                  (sc * 225.0f),
                                              0.0f, 1.0f);
                                    game.menu.sliderCaptured = true;
                                }
                                else
                                {
                                    game.menu.current
                                        ->items[game.menu.currentItem]
                                        .function();
                                    sound.PlayGUISfx(SOUND_SELECT2);
                                }
                            }
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        input.mouse.bButtons[0] = false;
                        if (bpause)
                        {
                            game.menu.sliderCaptured = false;
                        }
                    }
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    if (!g_controller &&
                        SDL_IsGameController(event.cdevice.which))
                    {
                        g_controller =
                            SDL_GameControllerOpen(event.cdevice.which);
                    }
                    break;
                case SDL_CONTROLLERDEVICEREMOVED:
                    if (g_controller)
                    {
                        SDL_Joystick* joystick =
                            SDL_GameControllerGetJoystick(g_controller);
                        if (joystick &&
                            SDL_JoystickInstanceID(joystick) ==
                                event.cdevice.which)
                        {
                            SDL_GameControllerClose(g_controller);
                            g_controller = nullptr;
                        }
                    }
                    break;
            }
        }

        pollControllerState();

        P_run();
    }

    P_cleanup();
    if (g_controller)
    {
        SDL_GameControllerClose(g_controller);
        g_controller = nullptr;
    }
    window.destroyGLWindow();
    SDL_Quit();

    return 0;
}

int GL_initializeFrameBuffers()
{
    game.renderTargets.lighting.Create(game.clientRect.w, game.clientRect.h,
                                       false);
    game.renderTargets.temp0.Create(game.clientRect.w, game.clientRect.h,
                                    false);
    game.renderTargets.temp1.Create(game.clientRect.w, game.clientRect.h,
                                    false);
    game.renderTargets.temp2.Create(1, game.sceneRect.h / 4, false);
    game.renderTargets.menu.Create(game.clientRect.w, game.clientRect.h, false);
    game.renderTargets.menu.Clear(0.0f, 0.0f, 0.0f, 1.0f);
    game.renderTargets.menuPanel.Create(480, 128 + 416, false);
    game.renderTargets.scene.Create(game.sceneRect.w, game.sceneRect.h, false);
    game.renderTargets.back.fbo = 0;

    return 1;
}

void GL_loadShader(unsigned int& window, const char* vsfile, const char* fsfile)
{
    char *vs = NULL, *fs = NULL;
    const char* vv = NULL;
    const char* ff = NULL;
    game.shaders.vertex = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    game.shaders.fragment = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    vs = textFileRead(vsfile);
    fs = textFileRead(fsfile);
    vv = vs;
    ff = fs;
    glShaderSourceARB(game.shaders.vertex, 1, &vv, NULL);
    glShaderSourceARB(game.shaders.fragment, 1, &ff, NULL);
    free(vs);
    free(fs);
    glCompileShaderARB(game.shaders.vertex);
    glCompileShaderARB(game.shaders.fragment);
    window = glCreateProgramObjectARB();
    glAttachObjectARB(window, game.shaders.vertex);
    glAttachObjectARB(window, game.shaders.fragment);
    glLinkProgramARB(window);
}

void GL_initializeShaders()
{
    GL_loadShader(game.shaders.horizontalBlur, "shaders/post_GHB.v.glsl",
                  "shaders/post_GHB.f.glsl");
    GL_loadShader(game.shaders.verticalBlur, "shaders/post_GVB.v.glsl",
                  "shaders/post_GVB.f.glsl");
    GL_loadShader(game.shaders.wave, "shaders/post.v.glsl",
                  "shaders/post_VW.f.glsl");
    GL_loadShader(game.shaders.bilinear, "shaders/post.v.glsl",
                  "shaders/post_CB.f.glsl");
    GL_loadShader(game.shaders.sky, "shaders/none.v.glsl",
                  "shaders/sky.f.glsl");
    GL_loadShader(game.shaders.curvature, "shaders/post.v.glsl",
                  "shaders/post_CURV.f.glsl");

    game.shaders.uniformHBlurSize =
        glGetUniformLocation(game.shaders.horizontalBlur, "blurSize");
    game.shaders.uniformVBlurSize =
        glGetUniformLocation(game.shaders.verticalBlur, "blurSize");
    game.shaders.uniformOffset =
        glGetUniformLocation(game.shaders.wave, "offset");
    game.shaders.uniformHBilinearSize =
        glGetUniformLocation(game.shaders.bilinear, "xSize");
    game.shaders.uniformVBilinearSize =
        glGetUniformLocation(game.shaders.bilinear, "ySize");
    game.shaders.uniformCurvature =
        glGetUniformLocation(game.shaders.curvature, "curvature");

    glUseProgramObjectARB(game.shaders.sky);
    glUniform2f(glGetUniformLocationARB(game.shaders.sky, "resolution"),
                game.sceneRect.w / 4, game.sceneRect.h / 4);
}

void P_cleanup()
{
    game.renderTargets.menu.Delete();
    game.renderTargets.menuPanel.Delete();
    game.renderTargets.lighting.Delete();
    game.renderTargets.temp0.Delete();
    game.renderTargets.temp1.Delete();
    game.renderTargets.temp2.Delete();
    game.renderTargets.scene.Delete();

    glDeleteTextures(1, &game.textures.sprites);
    glDeleteTextures(1, &game.textures.tiles);
    glDeleteTextures(1, &game.textures.bgs);
    glDeleteTextures(1, &game.textures.font);
    glDeleteTextures(1, &game.textures.light);
    glDeleteTextures(1, &game.textures.cursor);
    glDeleteTextures(1, &game.textures.slider);

    glDeleteProgram(game.shaders.horizontalBlur);
    glDeleteProgram(game.shaders.verticalBlur);
    glDeleteProgram(game.shaders.wave);
    glDeleteProgram(game.shaders.bilinear);
    glDeleteProgram(game.shaders.sky);
}

void R_drawBox(float x, float y, float w, float h, Vector4 color)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.x, color.y, color.z, color.w);
    glBegin(GL_QUADS);
    {
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void R_drawScreenDebug()
{
    glUseProgramObjectARB(NULL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int x = 10;
    int y = 10;
    int w = 340;
    int h = 200;
    Vector4 bgcolor(0.1f, 0.1f, 0.1f, 0.75f);
    R_drawBox(x, y, w, h, bgcolor);

    SDL_DisplayMode nativeMode;
    SDL_GetCurrentDisplayMode(0, &nativeMode);

    char buffer[128];
    Vector4 color;

    int ty = y + 8;
    int dy = font.charsXYWH.h * font.scaleValue;

    color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    sprintf(buffer, "shader version   = %s",
            glGetString(GL_SHADING_LANGUAGE_VERSION));
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "gametime.fps   = %0.2f", gametime.fps);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "gametime.currentFrame = %d", gametime.currentFrame);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "ttime = %0.4f",
            (gametime.getCurrentMilliseconds() / 1000.0f) - gametime.startTime);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "gtime = %0.4f", gametime.gameTime);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "ptime = %0.4f", gametime.pauseTime);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "mstime = %0.4f", gametime.moveSizeTime);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    color = Vector4(1.0f, 0.25f, 0.0f, 1.0f);

    sprintf(buffer, "mastervol  = %0.2f", sound.masterVolume);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "music  = %0.2f", sound.musicVolume);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "sfx  = %0.2f", sound.sfxVolume);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "cantsfx  = %d", sound.cantPlaySfxSound);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "cantguisfx  = %d", sound.cantPlayGUISound);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    color = Vector4(1.0f, 0.75f, 0.25f, 1.0f);

    sprintf(buffer, "native = %dx%dx%dbit %dHz", nativeMode.w, nativeMode.h,
            SDL_BITSPERPIXEL(nativeMode.format), nativeMode.refresh_rate);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "game.clientRect = %d x %d", (int)game.clientRect.w,
            (int)game.clientRect.h);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "game.sceneRect = %d x %d", (int)game.sceneRect.w,
            (int)game.sceneRect.h);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "camera = (%d,%d,%d,%d)", (int)camera.position.x,
            (int)camera.position.y, (int)camera.size.x, (int)camera.size.y);
    ty += dy;
    font.displayScreenText(x + 8, ty, color.x, color.y, color.z, buffer);

    sprintf(buffer, "layers = (%s,%s,%s,%s,%s)", BOOLTOSTRING(bdrawbglayer01),
            BOOLTOSTRING(bdrawbglayer02), BOOLTOSTRING(bdrawbglayer03),
            BOOLTOSTRING(bdrawbglayer04), BOOLTOSTRING(bdrawbglayer05));
    ty += dy;
    font.displayScreenText(x + 8, ty, 1.0, 1.0, 0.0, buffer);
}

void R_drawWorldDebug()
{
    camera.setOrthoMatrix();
    glUseProgramObjectARB(NULL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glDisable(GL_TEXTURE_2D);
    float posx, posy, pw, ph, padl, padb, padr, padt;
    if (bdebugcollisions)
    {
        for (int i = 0; i < player.collidableTiles.tileObjects.size(); i++)
        {
            posx = player.collidableTiles.tileObjects[i].position.x;
            posy = player.collidableTiles.tileObjects[i].position.y;
            pw = player.collidableTiles.tileObjects[i].size.x;
            ph = player.collidableTiles.tileObjects[i].size.x;
            glColor4f(0.75f, 0.2f, 0.1f, 1.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);
            {
                glVertex2f(posx, posy);
                glVertex2f(posx + pw, posy);
                glVertex2f(posx + pw, posy + ph);
                glVertex2f(posx, posy + ph);
                glVertex2f(posx, posy);
            }
            glEnd();
        }
        if (world.enemies.size())
        {
            for (int e = 0; e < world.enemies.size(); e++)
            {
                for (int i = 0;
                     i < world.enemies[e].collidableTiles.tileObjects.size();
                     i++)
                {
                    posx = world.enemies[e]
                               .collidableTiles.tileObjects[i]
                               .position.x;
                    posy = world.enemies[e]
                               .collidableTiles.tileObjects[i]
                               .position.y;
                    pw = world.enemies[e].collidableTiles.tileObjects[i].size.x;
                    ph = world.enemies[e].collidableTiles.tileObjects[i].size.x;
                    glColor4f(0.75f, 0.2f * e, 0.1f * e, 1.0f);
                    glLineWidth(2.0f);
                    glBegin(GL_LINE_STRIP);
                    {
                        glVertex2f(posx, posy);
                        glVertex2f(posx + pw, posy);
                        glVertex2f(posx + pw, posy + ph);
                        glVertex2f(posx, posy + ph);
                        glVertex2f(posx, posy);
                    }
                    glEnd();
                }
                if (bdrawhitboxes)
                {
                    posx = world.enemies[e].position.x;
                    posy = world.enemies[e].position.y;
                    pw = world.enemies[e].size.x;
                    ph = world.enemies[e].size.y;
                    padl = world.enemies[e].leftPadding;
                    padb = world.enemies[e].bottomPadding;
                    padr = world.enemies[e].rightPadding;
                    padt = world.enemies[e].topPadding;

                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glLineWidth(2.0f);
                    glBegin(GL_LINE_STRIP);
                    glVertex2f(posx, posy);
                    glVertex2f(posx + pw, posy);
                    glVertex2f(posx + pw, posy + ph);
                    glVertex2f(posx, posy + ph);
                    glVertex2f(posx, posy);
                    glEnd();
                    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                    glBegin(GL_LINE_STRIP);
                    glVertex2f(posx + padl, posy + padb);
                    glVertex2f(posx + pw - padr, posy + padb);
                    glVertex2f(posx + pw - padr, posy + ph - padt);
                    glVertex2f(posx + padl, posy + ph - padt);
                    glVertex2f(posx + padl, posy + padb);
                    glEnd();
                    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                    glBegin(GL_LINES);
                    glVertex2f(posx + pw * 0.5f, posy + padb);
                    glVertex2f(posx + pw * 0.5f, posy + ph - padt);
                    glEnd();
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
        }
    }

    if (bdrawhitboxes)
    {
        posx = player.position.x;
        posy = player.position.y;
        pw = player.size.x;
        ph = player.size.y;
        padl = player.leftPadding;
        padb = player.bottomPadding;
        padr = player.rightPadding;
        padt = player.topPadding;

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(posx, posy);
        glVertex2f(posx + pw, posy);
        glVertex2f(posx + pw, posy + ph);
        glVertex2f(posx, posy + ph);
        glVertex2f(posx, posy);
        glEnd();
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(posx + padl, posy + padb);
        glVertex2f(posx + pw - padr, posy + padb);
        glVertex2f(posx + pw - padr, posy + ph - padt);
        glVertex2f(posx + padl, posy + ph - padt);
        glVertex2f(posx + padl, posy + padb);
        glEnd();
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(posx + pw * 0.5f, posy + padb);
        glVertex2f(posx + pw * 0.5f, posy + ph - padt);
        glEnd();
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void R_setPPOrthoMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
}

void R_setClientOrthoMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, game.clientRect.w, game.clientRect.h, 0, -1, 1);
}

void R_setSceneOrthoMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, game.renderTargets.scene.width, game.renderTargets.scene.height,
            0, -1.0f, 1.0f);
}

void R_setMenuOrthoMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, game.renderTargets.menuPanel.width,
            game.renderTargets.menuPanel.height, 0, -1.0f, 1.0f);
}

void R_drawSky(RenderTarget target)
{
    glUseProgramObjectARB(game.shaders.sky);
    glUniform1f(glGetUniformLocationARB(game.shaders.sky, "gametime"),
                bstarted
                    ? ((float)6 + gametime.currentFrame * S_PER_FRAME)
                    : ((float)6 + game.clips.demo.currentFrame * S_PER_FRAME));
    DrawFullscreenQuad(target, game.shaders.sky);
}

void R_drawSceneLights(TileMap* ptilemap, float ls)
{
    ScreenRect dstRect;
    float bounce;
    for (int i = 0; i < ptilemap->animatedTiles.size(); i++)
    {
        float lx =
            ptilemap->animatedTiles[i].position.x * ptilemap->w_tileSize.x -
            ls / 2 + ptilemap->w_tileSize.x / 2;
        float ly =
            ptilemap->animatedTiles[i].position.y * ptilemap->w_tileSize.y -
            ls / 2 + ptilemap->w_tileSize.y / 2;
        if (lx < camera.position.x + camera.size.x &&
            lx + ls > camera.position.x &&
            ly < camera.position.y + camera.size.y &&
            ly + ls > camera.position.y)
        {
            bounce = ptilemap
                         ->imageLayer[ptilemap->animatedTiles[i].position.x]
                                     [ptilemap->animatedTiles[i].position.y]
                         .bounce;
            dstRect = ScreenRect(lx, ly + bounce, ls, ls);
            if (ptilemap
                    ->imageLayer[ptilemap->animatedTiles[i].position.x]
                                [ptilemap->animatedTiles[i].position.y]
                    .atileIndex == 245)
                glColor4f(1.0f, 0.0f, 0.0f, 0.75f);
            else if (ptilemap
                         ->imageLayer[ptilemap->animatedTiles[i].position.x]
                                     [ptilemap->animatedTiles[i].position.y]
                         .atileIndex == 246)
                glColor4f(0.1f, 0.1f, 1.0f, 0.15f);
            else
                glColor4f(1.0f, 1.0f, 0.2f, 0.25f);
            glTexCoord2d(0, 0);
            glVertex2f(dstRect.fLeft(), dstRect.fTop());
            glTexCoord2d(1, 0);
            glVertex2f(dstRect.fRight(), dstRect.fTop());
            glTexCoord2d(1, 1);
            glVertex2f(dstRect.fRight(), dstRect.fBottom());
            glTexCoord2d(0, 1);
            glVertex2f(dstRect.fLeft(), dstRect.fBottom());
        }
    }
}

void R_render()
{
    // Update window size if needed
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window.window, &windowWidth, &windowHeight);
    if (windowWidth != game.clientRect.w || windowHeight != game.clientRect.h)
    {
        game.clientRect.w = windowWidth;
        game.clientRect.h = windowHeight;
        game.aspectRatio = (float)windowWidth / windowHeight;
        R_resize(windowWidth, windowHeight);
    }

    glGetUniformLocation(NULL, NULL);

    if (!bdrawbglayer05)
        game.renderTargets.scene.Clear(0.75f, 0.56f, 0.46f, 1.0f);
    else
        game.renderTargets.scene.Clear(0.0f, 0.0f, 0.0f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    float screen_width;
    float screen_height;
    float targetAspectRatio = _16x9;
    float width;
    float height;
    ScreenRect boxRect;

    if (bsnapscale && bfixedaspect)
    {
        screen_width = window.virtualModes[window.scaleMode].width;
        screen_height = window.virtualModes[window.scaleMode].height;
    }
    else if (bfixedaspect)
    {
        screen_width = window.currentWidth();
        screen_height = window.currentHeight();
    }

    width = screen_width;
    height = (width / targetAspectRatio);

    if (bfixedaspect)
    {
        if (height > screen_height)
        {
            height = screen_height;
            width = (height * targetAspectRatio);
        }
        float boxwidth = 2 * (width / game.clientRect.w);
        float boxheight = 2 * (height / game.clientRect.h);

        boxRect = ScreenRect(-1 + (1 - boxwidth * 0.5f),
                             -1 + (1 - boxheight * 0.5f), boxwidth, boxheight);
    }
    else
    {
        boxRect = ScreenRect(-1, -1, 2, 2);
    }

    ScreenRect dstRect(-1, -1, 2, 2);
    ScreenRect srcRect(0, 0, 1, 1);

    if (boxRect.w < 2 || boxRect.h < 2)
        game.renderTargets.back.Clear(0.0f, 0.0f, 0.0f, 1.0f);

    float lighting_value = blighting ? world.level.ambientLighting : 1.0f;
    glColor4f(lighting_value, lighting_value, lighting_value, 1.0f);

    if (true)
    {
        glViewport(0, 0, game.renderTargets.temp0.width,
                   game.renderTargets.temp0.height);
        if (bdrawbglayer05)
        {
            R_setPPOrthoMatrix();
            R_drawSky(game.renderTargets.temp2);
            if (bbilinear || bcurvature)
                DrawFullscreenQuad(game.renderTargets.temp0,
                                   game.renderTargets.temp2, NULL);
            else
                DrawQuadD(game.renderTargets.back, game.renderTargets.temp2,
                          &boxRect, NULL);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, game.renderTargets.scene.fbo);
        glViewport(0, 0, game.renderTargets.scene.width,
                   game.renderTargets.scene.height);
        camera.setOrthoMatrix();

        if (bdrawbglayer04)
            world.level.layer[0].draw(&camera);
        if (bdrawbglayer03)
            world.level.layer[1].draw(&camera);
        if (bdrawspritelayer && !player.inFront &&
            (!player.hit || (player.hit && player.hit % 2 == 0)))
        {
            player.draw(&camera);
        }
        if (bdrawbglayer02)
        {
            world.level.layer[2].draw(&camera);
            for (int i = 0; i < world.level.dynTileMaps.count; i++)
                world.level.dynTileMaps.tileMaps[i].draw(&camera);
        }
        if (bdrawspritelayer)
        {
            for (unsigned int i = 0; i < world.enemies.size(); i++)
                world.enemies[i].draw(&camera);
            if (player.inFront &&
                (!player.hit || (player.hit && player.hit % 2 == 0)))
                player.draw(&camera);
        }
        if (bdrawbglayer01)
            world.level.layer[3].draw(&camera);

        R_drawWorldDebug();

        if (blighting)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            glBindTexture(GL_TEXTURE_2D, game.textures.light);
            float ls = 192;
            glBegin(GL_QUADS);
            {
                R_drawSceneLights(&world.level.layer[2], ls);
                R_drawSceneLights(&world.level.layer[3], ls);
            }
            glEnd();
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    glViewport(0, 0, game.clientRect.w, game.clientRect.h);

    R_setPPOrthoMatrix();

    lighting_value = 1.0f;

    if (true)
    {
        if (bcurvature)
        {
            DrawFullscreenQuadS(game.renderTargets.temp0,
                                game.renderTargets.scene, &srcRect,
                                game.shaders.curvature);

            DrawFullscreenQuadS(game.renderTargets.temp1,
                                game.renderTargets.temp0, &srcRect,
                                bbilinear ? game.shaders.bilinear : NULL);

            glColor4f(lighting_value, lighting_value, lighting_value, 1.0f);
            DrawQuadD(game.renderTargets.back, game.renderTargets.temp1,
                      &boxRect, NULL);
        }
        else if (bbilinear)
        {
            DrawFullscreenQuadS(game.renderTargets.temp0,
                                game.renderTargets.scene, &srcRect, NULL);

            DrawFullscreenQuadS(game.renderTargets.temp1,
                                game.renderTargets.temp0, &srcRect,
                                game.shaders.bilinear);

            glColor4f(lighting_value, lighting_value, lighting_value, 1.0f);
            DrawQuadD(game.renderTargets.back, game.renderTargets.temp1,
                      &boxRect, NULL);
        }
        else
        {
            glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(lighting_value, lighting_value, lighting_value, 1.0f);
            DrawQuadD(game.renderTargets.back, game.renderTargets.scene,
                      &boxRect, NULL);
        }
    }

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (bpause && !btransitioning)
    {
        R_setPPOrthoMatrix();
        glColor4f(1.0f, 1.0f, 1.0f, game.menu.menuDarken);
        DrawFullscreenQuad(game.renderTargets.back, game.renderTargets.menu,
                           NULL);

        glViewport(0, 0, game.renderTargets.menuPanel.width,
                   game.renderTargets.menuPanel.height);
        glBindFramebuffer(GL_FRAMEBUFFER, game.renderTargets.menuPanel.fbo);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        R_setMenuOrthoMatrix();
        R_drawMenus();
        glViewport(0, 0, game.clientRect.w, game.clientRect.h);
        R_setPPOrthoMatrix();
        float w = (window.scaleValue / 32.0f) *
                  (float)game.renderTargets.menuPanel.width / game.clientRect.w;
        float h = (window.scaleValue / 32.0f) *
                  (float)game.renderTargets.menuPanel.height /
                  game.clientRect.h;
        game.menu.position.x = -w / 2;
        game.menu.position.y = -h / 2;
        dstRect = ScreenRect(game.menu.position.x, game.menu.position.y, w, h);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        DrawQuadD(game.renderTargets.back, game.renderTargets.menuPanel,
                  &dstRect, NULL);
        R_setClientOrthoMatrix();
        font.displayScreenText(9, game.clientRect.h - 10, 0, 0, 0,
                               " 2014 v0.99");
        font.displayScreenText(10, game.clientRect.h - 11, 1, 1, 1,
                               " 2014 v0.99");
        // cursor
        dstRect = ScreenRect(input.mouse.lX, input.mouse.lY,
                             window.scaleValue / 2, window.scaleValue / 2);
        DrawQuad(game.renderTargets.back, game.textures.cursor, &srcRect,
                 &dstRect, NULL);
    }
    else if (!bstarted)
    {
        R_setPPOrthoMatrix();
        glColor4f(1.0f, 1.0f, 1.0f, game.menu.menuDarken);
        DrawFullscreenQuad(game.renderTargets.back, game.renderTargets.menu,
                           NULL);
    }

    if (bdebugtext)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, game.renderTargets.back.fbo);
        R_setClientOrthoMatrix();
        R_drawScreenDebug();
    }

    glFlush();
    // Swap buffers using SDL
    SDL_GL_SwapWindow(window.window);
}

void R_drawMenus()
{
    float tw = 32.0f;
    float th = -tw;
    float tx = game.renderTargets.menuPanel.width / 2 - 5 * tw;
    float ty = game.renderTargets.menuPanel.height / 2 - th;

    int lw = 8;

    float bw = game.renderTargets.menuPanel.width - lw;
    float bh = game.renderTargets.menuPanel.height - lw;
    float bx = 0;
    float by = 0;

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    {
        glColor4f(0.0f, 0.0f, 0.25f, 1.0f);
        glVertex2f(bx, by);
        glVertex2f(bx + bw, by);
        glColor4f(0.0f, 0.0f, 0.25f, 0.70f);
        glVertex2f(bx + bw, by + bh);
        glVertex2f(bx, by + bh);
    }
    glEnd();

    glBegin(GL_QUADS);
    {
        glColor4f(0.5f, 0.5f, 1.0f, 1.0f);

        glVertex2i(bx, by);
        glVertex2i(bx + bw + lw, by);
        glVertex2i(bx + bw + lw, by + lw);
        glVertex2i(bx, by + lw);

        glVertex2i(bx + bw, by);
        glVertex2i(bx + bw + lw, by);
        glVertex2i(bx + bw + lw, by + bh + lw);
        glVertex2i(bx + bw, by + bh);

        glVertex2i(bx, by + bh);
        glVertex2i(bx + bw + lw, by + bh);
        glVertex2i(bx + bw + lw, by + bh + lw);
        glVertex2i(bx, by + bh + lw);

        glVertex2i(bx, by);
        glVertex2i(bx + lw, by);
        glVertex2i(bx + lw, by + bh + lw);
        glVertex2i(bx, by + bh);
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);

    if (game.menu.current->name != NULL)
        font.displayText(tx + 64.0f, by - 2 * th, tw, th, 1.0f, 1.0f, 1.0f,
                         game.menu.current->name);
    for (int i = 0; i < game.menu.current->items.size(); i++)
    {
        if (i != game.menu.currentItem)
        {
            if (game.menu.current->items[i].isSlider)
            {
                glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
                // start
                ScreenRect srcRect = ScreenRect(0, 0, 0.166f, 1);
                ScreenRect dstRect =
                    ScreenRect(tx + game.menu.current->items[i].x,
                               ty + game.menu.current->items[i].y, 12.0f, th);
                DrawQuad(game.renderTargets.menuPanel, game.textures.slider,
                         &srcRect, &dstRect, NULL);
                // middle
                srcRect = ScreenRect(0.166f, 0, 0.166f, 1);
                dstRect =
                    ScreenRect(tx + game.menu.current->items[i].x + 12.0f,
                               ty + game.menu.current->items[i].y, 232.0f, th);
                DrawQuad(game.renderTargets.menuPanel, game.textures.slider,
                         &srcRect, &dstRect, NULL);
                // end
                srcRect = ScreenRect(0.333f, 0, 0.166f, 1);
                dstRect =
                    ScreenRect(tx + game.menu.current->items[i].x + 244.0f,
                               ty + game.menu.current->items[i].y, 12.0f, th);
                DrawQuad(game.renderTargets.menuPanel, game.textures.slider,
                         &srcRect, &dstRect, NULL);
                // handle
                srcRect = ScreenRect(0.5f, 0, 0.5f, 1);
                dstRect = ScreenRect(
                    tx + game.menu.current->items[i].x +
                        *game.menu.current->items[i].floatValue * 225.0f,
                    ty + game.menu.current->items[i].y, tw, th);
                DrawQuad(game.renderTargets.menuPanel, game.textures.slider,
                         &srcRect, &dstRect, NULL);

                char buffer[8];
                sprintf(buffer, "%d",
                        (int)(*game.menu.current->items[i].floatValue * 100.0f +
                              0.5f));
                font.displayText(tx + game.menu.current->items[i].x + 260.0f,
                                 ty + game.menu.current->items[i].y, tw, th,
                                 0.5f, 0.5f, 0.5f, buffer);
            }
            else
            {
                font.displayText(tx + game.menu.current->items[i].x,
                                 ty + game.menu.current->items[i].y, tw, th,
                                 0.5f, 0.5f, 0.5f,
                                 game.menu.current->items[i].name);
                if (game.menu.current->items[i].boolValue != NULL)
                {
                    font.displayText(
                        tx + game.menu.current->items[i].x + 256.0f,
                        ty + game.menu.current->items[i].y, tw, th, 0.5f, 0.5f,
                        0.5f,
                        BOOLTOSTRING(*(game.menu.current->items[i].boolValue)));
                }
            }
        }
    }
    R_drawSelectedMenuItem(game.menu.current->items[game.menu.currentItem], tx,
                           ty, tw, th);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void R_drawSelectedMenuItem(MenuItem item, float tx, float ty, float tw,
                            float th)
{
    if (item.isSlider)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ScreenRect srcRect = ScreenRect(0, 0, 0.166f, 1);
        ScreenRect dstRect = ScreenRect(tx + item.x, ty + item.y, 12.0f, th);
        DrawQuad(game.renderTargets.menuPanel, game.textures.slider, &srcRect,
                 &dstRect, NULL);
        // middle
        srcRect = ScreenRect(0.166f, 0, 0.166f, 1);
        dstRect = ScreenRect(tx + item.x + 12.0f, ty + item.y, 232.0f, th);
        DrawQuad(game.renderTargets.menuPanel, game.textures.slider, &srcRect,
                 &dstRect, NULL);
        // end
        srcRect = ScreenRect(0.333f, 0, 0.166f, 1);
        dstRect = ScreenRect(tx + item.x + 244.0f, ty + item.y, 12.0f, th);
        DrawQuad(game.renderTargets.menuPanel, game.textures.slider, &srcRect,
                 &dstRect, NULL);
        // handle
        srcRect = ScreenRect(0.5f, 0, 0.5f, 1);
        dstRect = ScreenRect(tx + item.x + *item.floatValue * 225.0f,
                             ty + item.y, tw, th);
        DrawQuad(game.renderTargets.menuPanel, game.textures.slider, &srcRect,
                 &dstRect, NULL);

        char buffer[8];
        sprintf(buffer, "%d", (int)(*item.floatValue * 100.0f + 0.5f));
        font.displayText(tx + item.x + 260.0f, ty + item.y, tw, th, 1.0f, 1.0f,
                         1.0f, buffer);
    }
    else
    {
        font.displayText(tx + item.x - 4.0f, ty + item.y, tw, th, 1.0f, 1.0f,
                         1.0f, item.name);
        if (item.boolValue != NULL)
            font.displayText(tx + item.x + 256.0f, ty + item.y, tw, th, 1.0f,
                             1.0f, 1.0f, BOOLTOSTRING(*item.boolValue));
    }
}

void R_resize(int w, int h)
{
    game.clientRect.w = (float)w;
    game.clientRect.h = (float)h;
    game.aspectRatio = game.clientRect.w / game.clientRect.h;

    window.computeScale();
    game.sceneRect = ScreenRect(0, 0, 1920, 1080);

    game.renderTargets.temp2.Resize(1, game.sceneRect.h / 4);
    game.renderTargets.scene.Resize(game.sceneRect.w, game.sceneRect.h);
    game.renderTargets.menu.Resize(game.clientRect.w, game.clientRect.h);
    game.renderTargets.menu.Clear(0.0f, 0.0f, 0.0f, 1.0f);
    game.renderTargets.lighting.Resize(game.clientRect.w, game.clientRect.h);
    game.renderTargets.temp0.Resize(game.clientRect.w, game.clientRect.h);
    game.renderTargets.temp1.Resize(game.clientRect.w, game.clientRect.h);
}

void P_run()
{
    if (window.isActive)
    {
        double elapsed_time;
        double current_time = gametime.getCurrentTime();
        if (gametime.moveSizeTime < 0.05f)
            gametime.moveSizeTime = 0.0f;
        elapsed_time =
            (current_time - gametime.lastGameTime - gametime.moveSizeTime) +
            gametime.remaining;
        gametime.moveSizeTime = 0.0f;
        while (elapsed_time >= S_PER_GAME_UPDATE * gametime.frameMultiplier)
        {
            G_update(elapsed_time);
            elapsed_time -= S_PER_GAME_UPDATE * gametime.frameMultiplier;
        }
        gametime.remaining = elapsed_time;
        gametime.lastGameTime = current_time;

        P_computeFPS();
        R_updateUniforms();
        R_render(); // Remove hDC parameter since we're using SDL

        if (btogglefullscreen || bmodechange)
        {
            gametime.lastMoveSizeTime = gametime.getCurrentTime();
            if (btogglefullscreen)
            {
                P_cleanup();
                window.createGLWindow(true);
                btogglefullscreen = false;
                GL_initializeGLObjects();
            }
            else
            {
                window.createGLWindow(false);
                bmodechange = false;
            }
            gametime.moveSizeTime =
                gametime.getCurrentTime() - gametime.lastMoveSizeTime;
        }
    }
}
