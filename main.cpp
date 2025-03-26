// Created by: Nilfred Baez

#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>


using namespace std;


// Colors
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};


// Cell size and count
int cellSize = 20;
int cellCount = 25;
int offset = 75;
const int screenWidth  = cellSize * cellCount + 2 * offset;
const int screenHeight = cellSize * cellCount + offset + 40;

double lastUpdateTime = 0;
int selectedOption = 0; // 0: Play, 1: Exit



bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for(unsigned int i = 0; i < deque.size(); i++) {
        if(Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}


//Snake class
class Snake {
    public:
        deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        Vector2 direction = {1, 0};
        bool addSegment = false;

        void Draw() {
            for(unsigned int i = 0; i < body.size(); i++) {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{offset+ x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.5, 6, darkGreen);
            }
        }

        void Update() {

            body.push_front(Vector2Add(body[0], direction));
            if (addSegment == true) 
            {
                addSegment = false;
            }else
            {
                body.pop_back();
                
            }

        }

        void Reset() {
            body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
            direction = {1, 0};
        }

    
};

//Food class
class Food {
    public:
        Vector2 position;
        Texture2D texture;

        Food(deque<Vector2> snakeBody)
        {
            Image image = LoadImage("resources/apple.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = GetRandomPosition(snakeBody);
        }

        ~Food()
        {
            UnloadTexture(texture);
        }
        void Draw() {
            Vector2 drawPos = {offset + position.x * cellSize, offset + position.y * cellSize};
            float scale = (float)cellSize / texture.width;
            DrawTextureEx(texture, drawPos, 0.0f, scale, WHITE);
        }

        Vector2 GetRandomCell() {
            float x = {GetRandomValue(0, cellCount - 1)};
            float y = {GetRandomValue(0, cellCount - 1)};
            return Vector2{x, y};
        }

        Vector2 GetRandomPosition(deque<Vector2> snakeBody) {
            Vector2 position = GetRandomCell();
            while(ElementInDeque(position, snakeBody)) {
                position = GetRandomCell();
            }
            return position;
            
        }
};



class Game {
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;
        Sound eatSound ;
        Sound wallSound ;

        Game() {
            InitAudioDevice();
            eatSound = LoadSound("resources/apple.mp3");
            wallSound = LoadSound("resources/wall.mp3");
        }

        ~Game() {
            UnloadSound(eatSound);
            UnloadSound(wallSound);
            CloseAudioDevice();
        }

        void Update() {

            if(running){
                snake.Update();
                CheckColissionWithFood();
                CheckColissionWithWalls();
                CheckColissionWithTail();
            }
        }

        void Draw() {
            ClearBackground(green);
            food.Draw();
            snake.Draw();
        }

        void CheckColissionWithFood() {
            if (Vector2Equals(snake.body[0], food.position)) {
                food.position = food.GetRandomPosition(snake.body);
                snake.addSegment = true;
                score++;
                PlaySound(eatSound);
            }
        }

        void CheckColissionWithWalls() {
            if(snake.body[0].x == cellCount || snake.body[0].x == -1) {
                GameOver();
            }

            if(snake.body[0].y == cellCount || snake.body[0].y == -1) {
                GameOver();
            }
        }

        void GameOver() {
            snake.Reset();
            food.position = food.GetRandomPosition(snake.body);
            running = false;
            score = 0;
            PlaySound(wallSound);
        }

        void CheckColissionWithTail(){
            deque<Vector2> headlessBody = snake.body;
            headlessBody.pop_front();
            if(ElementInDeque(snake.body[0], headlessBody)) {
                GameOver();
            }
        }
};
//Main function

enum GameScreen {
    MENU,
    GAME,
    GAMEPLAY

};

GameScreen currentScreen = MENU;



int main () {

    cout << "Starting Game..." << endl;
    InitWindow(screenWidth, screenHeight, "Retro Snake");
    SetTargetFPS(60);

    Game game = Game();

    while (!WindowShouldClose())
{
    BeginDrawing();
    ClearBackground(green);

    if (currentScreen == MENU)
    {
        // Navegación del menú
        if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 2;
        if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption + 1) % 2;

        // Acción con ENTER
        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedOption == 0) {
                currentScreen = GAMEPLAY;
                game.running = true;
            } else if (selectedOption == 1) {
                break; // Exit game
            }
        }

        // Dibujar menú
        DrawText("Retro Snake", screenWidth / 2 - MeasureText("Retro Snake", 40) / 2, 100, 40, darkGreen);

        //Selector

        Color exitColor = (selectedOption == 0) ? DARKGRAY : darkGreen;
        Color startColor  = (selectedOption == 1) ? DARKGRAY : darkGreen;

        DrawText("1. Start", screenWidth / 2 - 60, 200, 30, startColor);
        DrawText("2. Exit",  screenWidth / 2 - 60, 250, 30, exitColor);
    }
    else if (currentScreen == GAMEPLAY)
    {
        // Actualizar juego si ha pasado el tiempo
        if (eventTriggered(0.1)) {
            game.Update();
        }

        // Dirección de la serpiente
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y == 0) {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y == 0) {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x == 0) {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x == 0) {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        // Dibujar elementos del juego
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize * cellCount+10, (float)cellSize * cellCount+10}, 5, darkGreen);
        DrawText("Retro Snake", offset -5, 20, 40, darkGreen);
        DrawText(TextFormat("Score: %i", game.score), offset + cellSize * cellCount - 100, 20, 20, darkGreen);
        game.Draw();
    }

    EndDrawing();
    }

    CloseWindow();
    return 0;

}
