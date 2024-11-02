#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
#include <vector>
#include "list.h"

using namespace std;

static bool allowMove = false;
Color green = {91, 135, 49, 255};
Color darkGreen = {43, 51, 24, 255};
Color start = {112, 179, 81, 255};

int cellSize = 64;
int cellCount = 13;
int offset = 64;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    std::vector<Texture2D> headTextures;
    std::vector<Texture2D> bodyTextures;
    std::vector<Texture2D> tailTextures;
    Rectangle headSourceRect = {0, 0, 64, 64};

    Snake()
    {
        headTextures.push_back(LoadTexture("images\\cap_dreapta.png"));
        headTextures.push_back(LoadTexture("images\\cap_jos.png"));
        headTextures.push_back(LoadTexture("images\\cap_stanga.png"));
        headTextures.push_back(LoadTexture("images\\cap_sus.png"));

        bodyTextures.push_back(LoadTexture("images\\corp_dreapta.png"));
        bodyTextures.push_back(LoadTexture("images\\corp_jos.png"));
        bodyTextures.push_back(LoadTexture("images\\corp_stanga.png"));
        bodyTextures.push_back(LoadTexture("images\\corp_sus.png"));

        tailTextures.push_back(LoadTexture("images\\coada_dreapta.png"));
        tailTextures.push_back(LoadTexture("images\\coada_sus.png"));
        tailTextures.push_back(LoadTexture("images\\coada_stanga.png"));
        tailTextures.push_back(LoadTexture("images\\coada_jos.png"));
    }

    ~Snake()
    {
        for (auto &texture : headTextures) {
            UnloadTexture(texture);
        }

        for (auto &texture : bodyTextures) {
            UnloadTexture(texture);
        }

        for (auto &texture : tailTextures) {
            UnloadTexture(texture);
        }
    }

    void Draw()
    {
        int directionIndex = 0; // Default to right-facing texture
        if (direction.x == -1) directionIndex = 2; // Left
        else if (direction.y == 1) directionIndex = 1; // Down
        else if (direction.y == -1) directionIndex = 3; // Up

        // Draw head
        DrawTexturePro(headTextures[directionIndex], headSourceRect, Rectangle{offset + body[0].x * cellSize, offset + body[0].y * cellSize, 64, 64}, Vector2{0}, 0.0f, WHITE);

        // Draw body + tail
        int bodyDirectionIndex = 0;  // Initialize to default value
        for (size_t i = 1; i < body.size(); i++)
        {
            Vector2 currentSegment = body[i];
            Vector2 previousSegment = (i == 1) ? body[0] : body[i - 1];

            // Determine the direction of the current segment (based on the previous segment)
            if (currentSegment.x > previousSegment.x) bodyDirectionIndex = 2; // Left
            else if (currentSegment.x < previousSegment.x) bodyDirectionIndex = 0; // Right
            else if (currentSegment.y > previousSegment.y) bodyDirectionIndex = 1; // Down
            else if (currentSegment.y < previousSegment.y) bodyDirectionIndex = 3; // Up

            Texture2D textureToUse = (i < body.size() - 1) ? bodyTextures[bodyDirectionIndex] : tailTextures[bodyDirectionIndex];

            DrawTextureEx(textureToUse, Vector2{offset + currentSegment.x * cellSize, offset + currentSegment.y * cellSize}, 0, 1.0, WHITE);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{
public:
    Vector2 position;
    Texture2D texture, texture2;
    bool isGolden = false;

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("images\\apple.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        Image image2 = LoadImage("images\\bolat_cola.png");
        texture2 = LoadTextureFromImage(image2);
        UnloadImage(image2);

        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
        UnloadTexture(texture2);
    }

    void Draw()
    {
        if (isGolden == true)
        {
            DrawTexture(texture2, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        }
        else
        {
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        }
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    enum GameState { MAIN_MENU, RUNNING, GAME_OVER, PAUSE, QUESTION, HELP, CREDITS };
    GameState state = MAIN_MENU;

    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    int goldenapple = 0;
    Texture2D background;
    Sound eatSound;
    Sound wallSound;
    std::string question;
    std::vector<std::string> answers;
    int correctAnswerIndex;
    struct node* currentQuestion;
    double questionStartTime = 0; // Timer for questions

    Game()
    {
        InitAudioDevice();
        Image bgImage = LoadImage("images\\bg.png");
        background = LoadTextureFromImage(bgImage);
        UnloadImage(bgImage);

        eatSound = LoadSound("Sounds\\eat.mp3");
        wallSound = LoadSound("Sounds\\bonk.mp3");

        // Load initial question
        LoadRandomQuestion();
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void LoadRandomQuestion()
    {
        if (head == NULL) {
            // Handle the case where the linked list is empty
            // For example, print an error message or return
            return;
        }

        int totalQuestions = 0;
        struct node* temp = head;
        while (temp != NULL) {
            totalQuestions++;
            temp = temp->next;
        }

        if (totalQuestions == 0) {
            // Handle the case where the linked list is empty
            // For example, print an error message or return
            return;
        }

        int randomIndex = GetRandomValue(0, totalQuestions - 1);
        temp = head;
        for (int i = 0; i < randomIndex; i++) {
            if (temp == NULL) {
                // Handle the case where the index is out of range
                // For example, print an error message or return
                return;
            }
            temp = temp->next;
        }

        if (temp == NULL) {
            // Handle the case where the index is out of range
            // For example, print an error message or return
            return;
        }

        currentQuestion = temp;
        question = temp->grila[0];
        answers.clear();
        for (int i = 1; i <= 4; i++) {
            answers.push_back(temp->grila[i]);
        }
        correctAnswerIndex = -1;
        for (int i = 0; i < 4; i++) {
            if (temp->rasp[i] == 1) {
                correctAnswerIndex = i;
                break;
            }
        }
    }

void Draw()
    {
        switch (state)
        {
            case MAIN_MENU:
                DrawMainMenu();
                break;
            case RUNNING:
                food.Draw();
                snake.Draw();
                break;
            case GAME_OVER:
                ClearBackground(RED);
                DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 60) / 2, GetScreenHeight() / 2 - 40, 60, BLACK);
                DrawText("Press SPACE to restart", GetScreenWidth() / 2 - MeasureText("Press SPACE to restart", 20) / 2, GetScreenHeight() / 2 + 10, 20, BLACK);
                break;
            case PAUSE:
                DrawPauseScreen();
                break;
            case QUESTION:
                DrawQuestionScreen();
                break;
            case HELP:
                DrawHelpScreen();
                break;
            case CREDITS:
                DrawCreditsScreen();
                break;
        }
    }

void DrawMainMenu()
{
    ClearBackground(start);

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonX = screenWidth / 2 - buttonWidth / 2;
    int buttonSpacing = 20;

    int totalButtonsHeight = 5 * buttonHeight + 4 * buttonSpacing;
    int startY = (screenHeight - totalButtonsHeight) / 2;

    // Start Button
    int startButtonY = startY;
    DrawRectangle(buttonX, startButtonY, buttonWidth, buttonHeight, GREEN);
    DrawText("START", buttonX + 60, startButtonY + 15, 25, BLACK);

    // Module Button
    int moduleButtonY = startButtonY + buttonHeight + buttonSpacing;
    DrawRectangle(buttonX, moduleButtonY, buttonWidth, buttonHeight, BLUE);
    DrawText("MODULE", buttonX + 50, moduleButtonY + 15, 25, BLACK);

    // Help Button
    int helpButtonY = moduleButtonY + buttonHeight + buttonSpacing;
    DrawRectangle(buttonX, helpButtonY, buttonWidth, buttonHeight, YELLOW);
    DrawText("HELP", buttonX + 73, helpButtonY + 15, 25, BLACK);

    // Credits Button
    int creditsButtonY = helpButtonY + buttonHeight + buttonSpacing;
    DrawRectangle(buttonX, creditsButtonY, buttonWidth, buttonHeight, ORANGE);
    DrawText("CREDITS", buttonX + 55, creditsButtonY + 15, 25, BLACK);

    // Quit Button
    int quitButtonY = creditsButtonY + buttonHeight + buttonSpacing;
    DrawRectangle(buttonX, quitButtonY, buttonWidth, buttonHeight, RED);
    DrawText("QUIT", buttonX + 73, quitButtonY + 15, 25, BLACK);


    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)startButtonY, (float)buttonWidth, (float)buttonHeight }))
        {
            state = RUNNING;
        }

        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)moduleButtonY, (float)buttonWidth, (float)buttonHeight }))
        {
            system("start images\\module.pdf");
        }

        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)helpButtonY, (float)buttonWidth, (float)buttonHeight }))
        {
            state = HELP;
        }

        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)creditsButtonY, (float)buttonWidth, (float)buttonHeight }))
        {
            state = CREDITS;
        }

        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)quitButtonY, (float)buttonWidth, (float)buttonHeight }))
        {
            CloseWindow();
        }
    }
}

void DrawCreditsScreen()
{
    ClearBackground(LIGHTGRAY);
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    std::string title = "Project title: Gender, Digitalization, Green: Ensuring a Sustainable Future for all in Europe";
    std::string projectNo = "Project No: 2023-1-RO01- KA220-HED-000154433";
    std::string students[] = {
        "Students:",
        "Avram Sorin, sorin.avram@stud.acs.upb.ro",
        "Besoi David, david.besoi@stud.acs.upb.ro",
        "Bolat Tayfun, tayfun.bolat@stud.acs.upb.ro",
        "Simion Eduard, eduard.simion@stud.acs.upb.ro",
        "Timoc Mario Ovidiu, mario.timoc@stud.acs.upb.ro"
    };
    std::string teachers = "Teachers: Prof M. Caramihai & D Chis";


    int fontSize = 20;
    int lineSpacing = 10;
    Color textColor = BLACK;
    Color backgroundColor = GRAY;

    // Calculate text positions
    int titleFontSize = 18; // Adjusted for the long title
    int titleWidth = MeasureText(title.c_str(), titleFontSize);
    int projectNoWidth = MeasureText(projectNo.c_str(), fontSize);
    int teachersWidth = MeasureText(teachers.c_str(), fontSize);

    int textX = screenWidth / 2 - titleWidth / 2;
    int currentY = 150;  // Adjusted Y position to move text lower

    // Draw the background rectangles and text
    int rectPadding = 5;

    DrawRectangle(screenWidth / 2 - titleWidth / 2 - rectPadding, currentY - rectPadding, titleWidth + 2 * rectPadding, titleFontSize + 2 * rectPadding, backgroundColor);
    DrawText(title.c_str(), textX, currentY, titleFontSize, textColor);
    currentY += titleFontSize + lineSpacing * 2;

    textX = screenWidth / 2 - projectNoWidth / 2;
    DrawRectangle(screenWidth / 2 - projectNoWidth / 2 - rectPadding, currentY - rectPadding, projectNoWidth + 2 * rectPadding, fontSize + 2 * rectPadding, backgroundColor);
    DrawText(projectNo.c_str(), textX, currentY, fontSize, textColor);
    currentY += fontSize + lineSpacing * 2;

    for (int i = 0; i < 6; i++) {
        int lineWidth = MeasureText(students[i].c_str(), fontSize);
        textX = screenWidth / 2 - lineWidth / 2;
        DrawRectangle(screenWidth / 2 - lineWidth / 2 - rectPadding, currentY - rectPadding, lineWidth + 2 * rectPadding, fontSize + 2 * rectPadding, backgroundColor);
        DrawText(students[i].c_str(), textX, currentY, fontSize, textColor);
        currentY += fontSize + lineSpacing;
    }

    textX = screenWidth / 2 - teachersWidth / 2;
    DrawRectangle(screenWidth / 2 - teachersWidth / 2 - rectPadding, currentY - rectPadding, teachersWidth + 2 * rectPadding, fontSize + 2 * rectPadding, backgroundColor);
    DrawText(teachers.c_str(), textX, currentY, fontSize, textColor);

    // Draw back button
    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonX = screenWidth / 2 - buttonWidth / 2;
    int buttonY = screenHeight - 100;
    DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, GREEN);
    DrawText("BACK", buttonX + 70, buttonY + 15, 25, BLACK);

    // Check if the back button is pressed
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight }))
        {
            state = MAIN_MENU;
        }
    }
}

    void DrawPauseScreen()
    {
        ClearBackground(DARKGRAY);
        if (IsKeyPressed(KEY_ENTER))
        {
            state = RUNNING;
        }
    }

    void DrawQuestionScreen()
    {
        ClearBackground(DARKGRAY);

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Define the positions for the text
        int bolatTextY = screenHeight / 2 - 120;
        int instructionTextY = screenHeight / 2 - 80;
        int questionTextY = screenHeight / 2 - 40;
        int answersTextStartY = screenHeight / 2;
        int remainingTimeTextY = screenHeight / 2 + 120;

        // Draw gray rectangles and text for each line

        // BOLAT COLA appeared!
        int bolatTextWidth = MeasureText("BOLAT COLA appeared!", 20);
        int bolatRectX = screenWidth / 2 - bolatTextWidth / 2 - 10;
        int bolatRectY = bolatTextY - 5;
        DrawRectangle(bolatRectX, bolatRectY, bolatTextWidth + 20, 30, LIGHTGRAY);
        DrawText("BOLAT COLA appeared!", bolatRectX + 10, bolatTextY, 20, BLACK);

        // Answer this question for a power-up, but if you answer wrong...
        int instructionTextWidth = MeasureText("Answer this question for a power-up, but if you answer wrong...", 20);
        int instructionRectX = screenWidth / 2 - instructionTextWidth / 2 - 10;
        int instructionRectY = instructionTextY - 5;
        DrawRectangle(instructionRectX, instructionRectY, instructionTextWidth + 20, 30, LIGHTGRAY);
        DrawText("Answer this question for a power-up, but if you answer wrong...", instructionRectX + 10, instructionTextY, 20, BLACK);

        // Question
        int questionTextWidth = MeasureText(question.c_str(), 20);
        int questionRectX = screenWidth / 2 - questionTextWidth / 2 - 10;
        int questionRectY = questionTextY - 5;
        DrawRectangle(questionRectX, questionRectY, questionTextWidth + 20, 30, LIGHTGRAY);
        DrawText(question.c_str(), questionRectX + 10, questionTextY, 20, BLACK);

        // Answers
        for (size_t i = 0; i < answers.size(); i++)
        {
            int answerTextWidth = MeasureText(answers[i].c_str(), 19);
            int answerRectX = screenWidth / 2 - answerTextWidth / 2 - 10;
            int answerRectY = answersTextStartY + i * 30 - 5;
            DrawRectangle(answerRectX, answerRectY, answerTextWidth + 20, 30, LIGHTGRAY);
            DrawText(answers[i].c_str(), answerRectX + 10, answersTextStartY + i * 30, 19, BLACK);
        }

        // Remaining time
        double elapsed = GetTime() - questionStartTime;
        double remaining = 120.0 - elapsed; // 2 minutes timer
        if (remaining < 0) remaining = 0;
        int remainingTimeTextWidth = MeasureText(TextFormat("Time left: %.0f seconds", remaining), 20);
        int remainingTimeRectX = screenWidth / 2 - remainingTimeTextWidth / 2 - 10;
        int remainingTimeRectY = remainingTimeTextY - 5;
        DrawRectangle(remainingTimeRectX, remainingTimeRectY, remainingTimeTextWidth + 20, 30, LIGHTGRAY);
        DrawText(TextFormat("Time left: %.0f seconds", remaining), remainingTimeRectX + 10, remainingTimeTextY, 20, BLACK);
    }

    void DrawHelpScreen()
{
    ClearBackground(LIGHTGRAY);
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    // Draw green square
    int squareX = offset;
    int squareY = offset;
    int squareWidth = cellSize * cellCount;
    int squareHeight = cellSize * cellCount;
    DrawRectangle(squareX, squareY, squareWidth, squareHeight, green);
    // Title
    DrawText("Snake ECO", squareX + 20, squareY + 20, 20, BLACK);
    // Game Controls
    DrawText("Game Controls", squareX + 20, squareY + 60, 20, BLACK);
    DrawText("Arrow keys or WASD keys: Control the direction of the snake.", squareX + 40, squareY + 90, 20, BLACK);
    DrawText("Up Arrow or W: Move the snake up.", squareX + 40, squareY + 110, 20, BLACK);
    DrawText("Down Arrow or S: Move the snake down.", squareX + 40, squareY + 130, 20, BLACK);
    DrawText("Left Arrow or A: Move the snake left.", squareX + 40, squareY + 150, 20, BLACK);
    DrawText("Right Arrow or D: Move the snake right.", squareX + 40, squareY + 170, 20, BLACK);
    // Answering Questions
    DrawText("Answering Questions", squareX + 20, squareY + 210, 20, BLACK);
    DrawText("When a golden apple appears, a question will pop up.", squareX + 40, squareY + 240, 20, BLACK);
    DrawText("Use 1, 2, 3, and 4 keys to select the corresponding answer.", squareX + 40, squareY + 260, 20, BLACK);
    DrawText("You have two minutes to answer the question.", squareX + 40, squareY + 280, 20, BLACK);
    // Special Features
    DrawText("Special Features", squareX + 20, squareY + 320, 20, BLACK);
    DrawText("Golden Apple: Eating a golden apple triggers an environmental question.", squareX + 40, squareY + 350, 20, BLACK);
    DrawText("Module: Click the MODULE button on the main menu to access educational materials related to climate change.", squareX + 40, squareY + 370, 20, BLACK);
    // Game States
    DrawText("Game States", squareX + 20, squareY + 410, 20, BLACK);
    DrawText("RUNNING: In this state, the game is actively running.", squareX + 40, squareY + 440, 20, BLACK);
    DrawText("GAME OVER: The game ends when the snake collides with the walls or its own tail.", squareX + 40, squareY + 460, 20, BLACK);
    DrawText("QUESTION: When a golden apple is eaten, this state triggers a question.", squareX + 40, squareY + 480, 20, BLACK);
    // Scoring
    DrawText("Scoring", squareX + 20, squareY + 520, 20, BLACK);
    DrawText("Gain points by eating regular apples and answering questions correctly.", squareX + 40, squareY + 550, 20, BLACK);
    DrawText("Every third apple is a golden apple, triggering a question.", squareX + 40, squareY + 570, 20, BLACK);
    // Back Button
    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonX = screenWidth / 2 - buttonWidth / 2;
    int buttonY = screenHeight - 100;
    DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, GREEN);
    DrawText("BACK", buttonX + 70, buttonY + 15, 25, BLACK);
    // Check if the back button is pressed
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, Rectangle{ (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight }))
        {
            state = MAIN_MENU;
        }
    }
}

    void Update()
    {
        if (state == RUNNING)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
        else if (state == GAME_OVER && IsKeyPressed(KEY_SPACE))
        {
            RestartGame();
        }
        else if (state == QUESTION)
        {
            UpdateQuestionScreen();
        }
    }

    void UpdateQuestionScreen()
    {
        double elapsed = GetTime() - questionStartTime;
        if (elapsed >= 120.0) // 2 minutes timer
        {
            state = GAME_OVER;
        }
        if (IsKeyPressed(KEY_ONE))
        {
            CheckAnswer(0);
        }
        else if (IsKeyPressed(KEY_TWO))
        {
            CheckAnswer(1);
        }
        else if (IsKeyPressed(KEY_THREE))
        {
            CheckAnswer(2);
        }
        else if (IsKeyPressed(KEY_FOUR))
        {
            CheckAnswer(3);
        }
    }

    void CheckAnswer(size_t answerIndex)
    {
        if (answerIndex == static_cast<size_t>(correctAnswerIndex))
        {
            state = RUNNING; // Correct answer
            score++;
        }
        else
        {
            state = GAME_OVER; // Incorrect answer
        }
    }

    void RestartGame()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        score = 0;
        goldenapple = 0;
        state = RUNNING;
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            snake.addSegment = true;
            score++;
            goldenapple++;

            if (food.isGolden)
            {
                LoadRandomQuestion();
                state = QUESTION; // Pause the game if it's a golden apple
                questionStartTime = GetTime(); // Start the question timer
            }

            if (goldenapple % 3 == 0 && goldenapple != 0)
            {
                food.isGolden = true; // Every third apple is golden
            }
            else
            {
                food.isGolden = false;
            }

            food.position = food.GenerateRandomPos(snake.body);
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
        state = GAME_OVER;
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    readFile();
    printList();
    cout << "Starting the game..." << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake ECO");
    SetTargetFPS(144);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (EventTriggered(0.2) && game.state != Game::GameState::QUESTION)
        {
            allowMove = true;
            game.Update();
        }
        else if (game.state == Game::GameState::QUESTION)
        {
            game.UpdateQuestionScreen();
        }

        if (game.state == Game::GameState::RUNNING && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game.snake.direction.y != 1 && allowMove)
        {
            game.snake.direction = {0, -1};
            game.running = true;
            allowMove = false;
        }
        if (game.state == Game::GameState::RUNNING && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game.snake.direction.y != -1 && allowMove)
        {
            game.snake.direction = {0, 1};
            game.running = true;
            allowMove = false;
        }
        if (game.state == Game::GameState::RUNNING && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game.snake.direction.x != 1 && allowMove)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
            allowMove = false;
        }
        if (game.state == Game::GameState::RUNNING && (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game.snake.direction.x != -1 && allowMove)
        {
            game.snake.direction = {1, 0};
            game.running = true;
            allowMove = false;
        }

        if (IsKeyPressed(KEY_SPACE) && game.state == Game::GameState::GAME_OVER)
        {
            game.RestartGame();
        }

        int textWidth = MeasureText("Snake ECO", 40);
        int screenWidth = GetScreenWidth();
        int textX = (screenWidth - textWidth) / 2;

        // Drawing
        DrawTexture(game.background, offset, offset, WHITE);
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
        DrawText("Snake ECO", textX, 20, 40, darkGreen);
        DrawText(TextFormat("Score: %i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}