/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "GameSettings.h"
#include "GameCommon.h"

#include "StageManager.h"
#include "TitleManager.h"

#if 0
#  define FHD
#  define FULLSCREEN
#endif
//#define SKIP_MENU


#ifdef FHD
#define WINDOW_WIDTH   FHD_WIDTH
#define WINDOW_HEIGHT  FHD_HEIGHT
#define LOGICAL_WIDTH  FHD_WIDTH
#define LOGICAL_HEIGHT FHD_HEIGHT
#else
#define WINDOW_WIDTH   HD_WIDTH
#define WINDOW_HEIGHT  HD_HEIGHT
#define LOGICAL_WIDTH  FHD_WIDTH
#define LOGICAL_HEIGHT FHD_HEIGHT
#endif

enum class GameState
{
    MAIN_MENU, STAGE
};

int main(int argc, char *argv[])
{
    // Initialise les librairies du jeu
    const Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER;
    const Uint32 imgFlags = IMG_INIT_PNG;
    const Uint32 mixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
    const int channelCount = 16;
    int exitStatus = Game_Init(sdlFlags, imgFlags, mixFlags, channelCount);
    if (exitStatus == EXIT_FAILURE)
    {
        assert(false);
        abort();
    }

    exitStatus = Mix_MasterVolume((int)(0.5f * MIX_MAX_VOLUME));
    exitStatus = Mix_VolumeMusic((int)(0.5f * MIX_MAX_VOLUME));

    srand((unsigned int)time(nullptr));

    // Crée la fenêtre
    Uint32 windowFlags = 0;
#ifdef FULLSCREEN
    windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

    g_window = SDL_CreateWindow(
        u8"Super Pixel Melee", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, windowFlags
    );
    if (!g_window)
    {
        printf("ERROR - Create window %s\n", SDL_GetError());
        assert(false); abort();
    }

    // Crée le moteur de rendu
    g_renderer = SDL_CreateRenderer(
        g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!g_renderer)
    {
        printf("ERROR - Create renderer %s\n", SDL_GetError());
        assert(false); abort();
    }

    SDL_RenderSetLogicalSize(g_renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    // Input manager
    SceneManager *sceneManger = nullptr;
    bool quitGame = false;
    GameState state = GameState::MAIN_MENU;

    InputManager *inputManager = new InputManager();
    InitInputConfig(inputManager);
    ApplicationInput *appInput = ApplicationInput::GetFromManager(inputManager);

    inputManager->SetKeyboardIsPlayer(true);

    // Config du jeu par défaut

    int playerCount = 1;
    std::array<PlayerConfig, MAX_PLAYER_COUNT> configs;
    PlayerConfig playerConfig;
    playerConfig.type = PlayerConfig::Type::FIRE_WARRIOR;
    playerConfig.SetTeamID(0);
    playerConfig.playerID = 0;
    playerConfig.skinID = 0;
    playerConfig.isCPU = false;
    configs[0] = playerConfig;

    playerConfig.type = PlayerConfig::Type::LIGHTNING_WARRIOR;
    playerConfig.SetTeamID(1);
    playerConfig.playerID = 1;
    playerConfig.skinID = 0;
    playerConfig.isCPU = false;
    configs[1] = playerConfig;

    playerConfig.type = PlayerConfig::Type::FIRE_WARRIOR;
    playerConfig.SetTeamID(2);
    playerConfig.playerID = 2;
    playerConfig.skinID = 0;
    playerConfig.isCPU = true;
    playerConfig.enabled = false;
    configs[2] = playerConfig;

    playerConfig.type = PlayerConfig::Type::FIRE_WARRIOR;
    playerConfig.SetTeamID(3);
    playerConfig.playerID = 3;
    playerConfig.skinID = 0;
    playerConfig.isCPU = true;
    playerConfig.enabled = false;
    configs[3] = playerConfig;

    StageConfig stageConfig;
    stageConfig.mode = StageConfig::Mode::LIMITED_LIVES;
    stageConfig.lifeCount = 2;

#ifdef SKIP_MENU
    state = GameState::STAGE;
#endif

    // Boucle de jeu
    while (quitGame == false)
    {
        // Construction de la scène
        switch (state)
        {
        case GameState::STAGE:
            sceneManger = new StageManager(inputManager, configs, stageConfig);
            break;

        case GameState::MAIN_MENU:
        default:
            sceneManger = new TitleManager(inputManager, configs, stageConfig, playerCount);
            break;
        }

        // Boucle de rendu
        while (true)
        {
            // Met à jour la scène
            Scene *scene = sceneManger->GetScene();
            scene->Update();

            if (appInput->quitPressed)
            {
                quitGame = true;
                break;
            }

            if (sceneManger->ShouldQuitScene())
                break;

            // Efface le rendu précédent
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            // Dessine la scène
            scene->Render();

            // Affiche le nouveau rendu
            SDL_RenderPresent(g_renderer);
        }

        if (sceneManger->ShouldQuitGame())
        {
            quitGame = true;
        }

        switch (state)
        {
        case GameState::STAGE:
            state = GameState::MAIN_MENU;
            break;

        case GameState::MAIN_MENU:
        default:
            state = GameState::STAGE;
            break;
        }

        if (sceneManger)
        {
            delete sceneManger;
            sceneManger = nullptr;
        }
    }


    delete inputManager; inputManager = nullptr;
    SDL_DestroyRenderer(g_renderer); g_renderer = nullptr;
    SDL_DestroyWindow(g_window); g_window = nullptr;
    Game_Quit();

    return 0;
}
