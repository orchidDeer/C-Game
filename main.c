#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WINVER 0x0500
#pragma comment (lib, "User32.lib")
#include <windows.h>
#include <conio.h>

#define VK_W 0x57
#define VK_S 0x53
#define VK_A 0x41
#define VK_D 0x44
#define VK_Num1 0x61
#define VK_Num2 0x62
#define VK_Num3 0x63
#define VK_Num4 0x64
#define VK_Num5 0x65
#define VK_Num6 0x66
#define VK_Num7 0x67
#define VK_Num8 0x68
#define VK_Num9 0x69

#define Blue BACKGROUND_BLUE | BACKGROUND_INTENSITY
#define LiteBlue 50
#define Green BACKGROUND_GREEN
#define LiteGreen BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define LiteGray BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED
#define Gray 140
#define Orange 100
#define Black 0

struct Map {
    short int SizeX;
    short int SizeY;
    char Matrix[255 * 255 + 12];
};

struct Player {
    short int x;
    short int y;
    short int AnimationStepp;
    int Hp;
    int Schaden;
    int watchDistanceX;
    int watchDistanceY;
    char ViewMatrix[8 * 5 * 8 * 8 * 5 * 8];
};

struct Game {
    clock_t time;
    clock_t oldtime;
    int physiktime;
    int deltaTime;
    int DestroyingTime;
    ///Console Handles
    HANDLE hOut;
};

struct Cube {
    char Symbol;
    unsigned short colour[9];
    char Texture[64];
    int durationToDestroyBlock;
};

///Player
struct Player Player1;
///Map
struct Map map;
///Game
struct Game game;
///Cube
struct Cube cubes[5];
///Enemy
struct Player Zomby;

int isButtonPresst = 0;

int waitingTime = 200;
int waitingTimePhysik = 300;

void createMap(char *air, char *block);
void drawMap(struct Map *map, struct Cube *cubes, struct Game *game, struct Player *player);
void ReDrawPosition(int *x, int *y, unsigned short *colour, struct Game *game, char *Text);

///Controls for Player
void playerControls();

///Player GUI
void initPlayerGUI();

///Gives back the Y Position of the PLayer
int spawnPlayer(char *CGrass, char *CPlayer, struct Player *player);

int delayA = 0;
int delayD = 0;
int delayW = 0;

int RandSeed;

int main()
{
    ///Init Console
    game.hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    HWND wh = GetConsoleWindow();
    MoveWindow(wh, 100, 100, 1500, 750, TRUE);

    ///Cubes
    ///Color B(Blue), L(Lite Blue), G(Green), H(Lite Green), T(Gray), Z(Lite Gray), O(Orange), B(Black)
    ///Sky
    cubes[0].Symbol = ' ';
    ///                      |       8       8       8       8       8       8       8       8|
    strcpy(cubes[0].Texture, "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
    ///Grass
    cubes[1].Symbol = 'X';
    cubes[1].durationToDestroyBlock = 1000;
    ///                      |       8       8       8       8       8       8       8       8|
    strcpy(cubes[1].Texture, "GGGGGGGGGHHHHHHGGHHHHHHGGHHHHHHGGHHHHHHGGHHHHHHGGHHHHHHGGGGGGGGG");

    ///Stone
    cubes[2].Symbol = 'W';
    cubes[2].durationToDestroyBlock = 3000;
    ///                      |       8       8       8       8       8       8       8       8|
    strcpy(cubes[2].Texture, "TTTTTTTTTZZZZZZTTZZZZZZTTZZZZZZTTZZZZZZTTZZZZZZTTZZZZZZTTTTTTTTT");

    ///Dirt
    cubes[3].Symbol = 'D';
    cubes[3].durationToDestroyBlock = 1000;
    ///                      |       8       8       8       8       8       8       8       8|
    strcpy(cubes[3].Texture, "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");




    ///Player
    Player1.x = 11;
    Player1.y = spawnPlayer(&cubes[1].Symbol, &cubes[3].Symbol, &Player1);
    Player1.watchDistanceX = 10;
    Player1.watchDistanceY = 5;
    Player1.AnimationStepp = 0;
    cubes[3].Symbol = 'P';
    ///                      |       8       8       8       8       8       8       8       8|
    strcpy(cubes[3].Texture, "LLLOOLLLLLLOOLLLLLLOOLLLLLLOOLLLLLLOOLLLLLLOOLLLLLLOOLLLLLLOOLLL");



    ///Zomby
    Zomby.x = 8;
    Zomby.y = spawnPlayer(&cubes[1].Symbol, &cubes[4].Symbol, &Zomby);
    cubes[4].Symbol = 'Z';
    ///                      |       8       8       8       8       8       8       8       8|
    strcpy(cubes[4].Texture, "BBBBBBBBBBBBBBBBBBBLLBBBBBBLLBBBBBBLLBBBBBBLLBBBBBBLLBBBBBBLLBBB");

    ///Hide Cursor
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(game.hOut, &info);

    map.SizeX = 255;
    map.SizeY = 255;
    map.Matrix[0] = '\0';

    createMap(&cubes[0].Symbol, &cubes[1].Symbol);

    drawMap(&map, cubes, &game, &Player1);

    initPlayerGUI();

    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        game.oldtime = clock();
        drawMap(&map, cubes, &game, &Player1);
        playerControls();
        game.time = clock();
        game.deltaTime = game.time - game.oldtime;
    }
    return 0;
}

///Player
void playerControls()
{
    boolean Moved = FALSE;

    ///Go Left
    delayA = delayA + game.deltaTime;
    if(GetAsyncKeyState(VK_A))
    {
        if(delayA >= waitingTime && Player1.x > 0 && map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x - 1)] == ' ')
        {
            map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x)] = ' ';
            map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x - 1)] = 'P';

            Player1.AnimationStepp = Player1.AnimationStepp - 1;

            if(Player1.AnimationStepp = -5){
                Player1.x -= 1;
                Player1.AnimationStepp = 5;
            }
            Moved = TRUE;
            delayA = 0;
        }
    }
    ///Go Right
    delayD = delayD + game.deltaTime;
     if(GetAsyncKeyState(VK_D))
    {
        if(delayD >= waitingTime && Player1.x < map.SizeX - 1 && map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x + 1)] == ' ')
        {
            map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x)] = ' ';
            map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x + 1)] = 'P';

            Player1.x = Player1.x + 1;
            delayD = 0;
            Moved = TRUE;
        }
    }
    ///Jump
    delayW = delayW + game.deltaTime;
    if(GetAsyncKeyState(VK_W))
    {
        if(delayW >= 500 && Player1.y > 0 && map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x)] == ' ')
        {
            map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x)] = ' ';
            map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x)] = 'P';

            Player1.y = Player1.y - 1;
            Moved = TRUE;
            delayW = 0;
            game.physiktime = game.time;
        }
    }

    ///Destroy
    if(GetAsyncKeyState(VK_Num1))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x - 1)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x - 1)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num2))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num3))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x + 1)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x + 1)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num4))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x - 1)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x - 1)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num6))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x + 1)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x + 1)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num7))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x - 1)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x - 1)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num8))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else if(GetAsyncKeyState(VK_Num9))
    {
        int b = 0;
        while(sizeof(cubes) != b){
            if(cubes[b].Symbol == map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x + 1)])
            {
                if(game.time - game.DestroyingTime > cubes[b].durationToDestroyBlock){
                    map.Matrix[getPosition(map.SizeX, Player1.y - 1, Player1.x + 1)] = ' ';
                    game.DestroyingTime = game.time;
                }
            }
            ++b;
        }
    }else {
        game.DestroyingTime = game.time;
    }

    if(Moved == TRUE)
        game.oldtime = game.time;

    ///Gravitation
    if(map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x)] == ' ')
    {
        if(game.time - game.physiktime >= waitingTimePhysik)
        {
            map.Matrix[getPosition(map.SizeX, Player1.y, Player1.x)] = ' ';
            map.Matrix[getPosition(map.SizeX, Player1.y + 1, Player1.x)] = 'P';

            Player1.y = Player1.y + 1;

            waitingTimePhysik = waitingTimePhysik * 0.3;
            game.physiktime = game.time;
        }
    }else{
        game.physiktime = game.time;
        waitingTimePhysik = 300;
    }
}

int spawnPlayer(char *CGrass, char *CPlayer, struct Player *player)
{
    int y = 0;
    while(y != map.SizeY)
    {
        if(map.Matrix[getPosition(map.SizeX, y + 1, player->x)] == CGrass)
        {
            map.Matrix[getPosition(map.SizeX, y, player->x)] = CPlayer;
            break;
        }
        ++y;
    }
    return y;
}

void initPlayerGUI()
{
    int y = map.SizeY;
    while(y != map.SizeY + 3)
    {
        int x = 0;
        while(x != map.SizeX)
        {
            ///Hp bar
            if(y - map.SizeY == 1 && x > 2 && x < 13){
                ReDrawPosition(&x, (y * 8) - 7, BACKGROUND_RED, &game, ' ');
            }
            ///Hp Number
            else if(y - map.SizeY == 1 && x == 14){
                ReDrawPosition(&x, (y * 8) - 7, BACKGROUND_INTENSITY | BACKGROUND_INTENSITY, &game, '1');
            }else if(y - map.SizeY == 1 && x == 15){
                ReDrawPosition(&x, (y * 8) - 7, BACKGROUND_INTENSITY | BACKGROUND_INTENSITY, &game, '0');
            }
            ++x;
        }
        ++y;
    }
}

///Map
void createMap(char *air, char *block)
{
    srand(time(NULL));

    int numberOfSpaces = random_number(2, 10);
    int positionOfSpaces[numberOfSpaces];
    int SizeOfSpaces[numberOfSpaces];

    ///create Map
    int y = 0;
    while(y != map.SizeY)
    {
        ///Create random Numbers
        int k = 0;
        numberOfSpaces = random_number(2, 10);
        while (k != numberOfSpaces)
        {
            if(numberOfSpaces == 0)
                break;

            SizeOfSpaces[k] = random_number(0, 50);
            positionOfSpaces[k] = random_number(-50 , map.SizeX * 2);

            ++k;
        }

        ///Puts random numbers in char
        int x = 0;
        while(x != map.SizeX)
        {
            short int space = 0;
            int l = 0;
            while (l != numberOfSpaces)
            {
                if(positionOfSpaces[l] - (SizeOfSpaces[l] / 2) < x && positionOfSpaces[l] + (SizeOfSpaces[l] / 2) > x)
                {
                    strcat(map.Matrix, air);

                    space = 1;
                    break;
                }
                ++l;
            }
            if(space != 1)
            {
                strcat(map.Matrix, block);
            }
            ++x;
        }
        strcat(map.Matrix, "\n");
        ++y;
    }

    ///Gravity all blocks fall to build the Map
    short int donsth = 1;
    while(donsth != 0)
    {
        donsth = 0;
        y = 0;
        while(y != map.SizeY)
        {
            int x = 0;
            while(x != map.SizeX)
            {
                if(map.Matrix[getPosition(map.SizeX, y + 1, x)] == air[0] && map.Matrix[getPosition(map.SizeX, y, x)] == block[0])
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = air[0];
                    map.Matrix[getPosition(map.SizeX, y + 1, x)] = block[0];
                    donsth = 1;
                }
                ++x;
            }

            ++y;
        }
    }

    ///Editing after the Map is Finished             WIP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    short int testifKp = 1;
    while(testifKp)
    {
        testifKp = 0;
        y = 0;
        while(y != map.SizeY - 1)
        {
            int x = 0;
            while(x != map.SizeX)
            {
                if(map.Matrix[getPosition(map.SizeX, y, x + 1)] == air[0] && map.Matrix[getPosition(map.SizeX, y, x - 1)] == air[0] && map.Matrix[getPosition(map.SizeX, y, x)] == block[0])
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = air[0];
                    testifKp = 1;
                }
                else if(map.Matrix[getPosition(map.SizeX, y, x + 1)] == block[0] && map.Matrix[getPosition(map.SizeX, y, x - 1)] == block[0] && map.Matrix[getPosition(map.SizeX, y, x)] == air[0])
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = block[0];
                    testifKp = 1;
                }
                else if((map.Matrix[getPosition(map.SizeX, y, x + 1)] == block[0] && map.Matrix[getPosition(map.SizeX, y - 1, x + 1)] == block[0]) && map.Matrix[getPosition(map.SizeX, y, x)] == air[0])
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = block[0];
                    testifKp = 1;
                }
                else if((map.Matrix[getPosition(map.SizeX, y, x - 1)] == block[0] && map.Matrix[getPosition(map.SizeX, y - 1, x - 1)] == block[0]) && map.Matrix[getPosition(map.SizeX, y, x)] == air[0])
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = block[0];
                    testifKp = 1;
                }
                ++x;
            }
            ++y;
        }
    }


    ///Spawn Stone usw.
    testifKp = 1;
    while(testifKp)
    {
        testifKp = 0;
        y = 0;
        while(y != map.SizeY - 1)
        {
            int x = 0;
            while(x != map.SizeX)
            {
                if(map.Matrix[getPosition(map.SizeX, y, x)] == block[0] && map.Matrix[getPosition(map.SizeX, y - 1, x)] == block[0])
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = cubes[2].Symbol;
                    testifKp = 1;
                }else if(map.Matrix[getPosition(map.SizeX, y, x)] == block[0] && map.Matrix[getPosition(map.SizeX, y - 1, x)] == cubes[2].Symbol)
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = cubes[2].Symbol;
                    testifKp = 1;
                }

                if(map.Matrix[getPosition(map.SizeX, y, x)] == block[2] && map.Matrix[getPosition(map.SizeX, y - 1, x)] == cubes[1].Symbol)
                {
                    map.Matrix[getPosition(map.SizeX, y, x)] = cubes[3].Symbol;
                    testifKp = 1;
                }
                ++x;
            }
            ++y;
        }
    }
}

void drawMap(struct Map *map, struct Cube *cubes, struct Game *game, struct Player *player)
{
    int y = 0;
    while(player->watchDistanceY * 2 != y)
    {
        int x = 0;
        while(player->watchDistanceX * 2 != x)
        {
            int b = 0;
            while(5 != b){
                if(map->Matrix[getPosition(map->SizeX, player->y - player->watchDistanceY + y, player->x - player->watchDistanceX + x)] == cubes[b].Symbol){
                    int k = 0;
                    while (k != 8) {
                        int i = 0;
                        while (i != 8) {
                                ///Color B(Blue), L(Lite Blue), G(Green), H(Lite Green), T(Gray), Z(Lite Gray), O(Orange), A(Black)
                                if(cubes[b].Texture[k * 8 + i] == 'B' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'B'){      /// 6 = player.watchDistance * 2
                                    ReDrawPosition(x * 8 + i, y * 8 + k, Blue, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'B';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'L'  && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'L'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, LiteBlue, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'L';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'G' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'G'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, Green, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'G';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'H' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'H'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, LiteGreen, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'H';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'T' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'T'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, Gray, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'T';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'Z' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'Z'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, LiteGray, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'Z';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'O' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'O'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, Orange, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'O';
                                }
                                else if(cubes[b].Texture[k * 8 + i] == 'A' && player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] != 'A'){
                                    ReDrawPosition(x * 8 + i, y * 8 + k, Black, game, ' ');
                                    player->ViewMatrix[getPosition(8 * player->watchDistanceX * 2, y * 8 + k, x * 8 + i)] = 'A';
                                }

                            ++i;
                        }
                        ++k;
                    }
                    break;
                }
                ++b;
            }
            ++x;
        }
        ++y;
    }
}

///Random
int random_number(int min_num, int max_num)
{
    srand(time(NULL) * RandSeed);

    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; /// include max_num in output
    } else {
        low_num = max_num + 1; /// include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    RandSeed = RandSeed + hi_num - low_num;
    return result;
}

///Button Released
int buttonReleased(int VK_Button)
{
    if(GetAsyncKeyState(VK_Button) && isButtonPresst == 0)
    {
        isButtonPresst = 1;
        return 0;
    }else if(!GetAsyncKeyState(VK_Button) && isButtonPresst == 1)
    {
        isButtonPresst = 0;
        return 1;
    }
}

int getPosition(int sizeX, int Y, int X)
{
    int OneLine = sizeX + 1;
    return ((OneLine * Y) + X);
}

void ReDrawPosition(int *x, int *y, unsigned short *colour, struct Game *game, char *Text)
{
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(game->hOut, coord);
    SetConsoleTextAttribute(game->hOut, colour);
    printf("%c", Text);
}
