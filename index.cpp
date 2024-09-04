#include <bits/stdc++.h>
#include <conio.h>
#include <windows.h>

using namespace std;

// Directions
const char DIR_UP = 'U';
const char DIR_DOWN = 'D';
const char DIR_LEFT = 'L';
const char DIR_RIGHT = 'R';

int consoleWidth, consoleHeight;

void createScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

// Global gotoxy function
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

struct Point {
    int xAxis;
    int yAxis;
    Point() {}
    Point(int x, int y) {
        xAxis = x;
        yAxis = y;
    }
};

class Snake {
    int length;
    char direction;
public:
    Point body[1000];
    Snake(int x, int y) {
        length = 1;
        body[0] = Point(x, y);
        direction = DIR_RIGHT;
    }

    int getLength() {
        return length;
    }

    void changeDirection(char newDirection) {
        if(newDirection == DIR_UP && direction != DIR_DOWN)
            direction = newDirection;
        else if(newDirection == DIR_DOWN && direction != DIR_UP)
            direction = newDirection;
        else if(newDirection == DIR_LEFT && direction != DIR_RIGHT)
            direction = newDirection;
        else if(newDirection == DIR_RIGHT && direction != DIR_LEFT)
            direction = newDirection;
    }

    bool move(Point& food) {
        Point prevTail = body[length - 1]; // Store previous tail position

        for(int i = length - 1; i > 0; i--) {
            body[i] = body[i - 1];
        }

        switch(direction) {
            case DIR_UP:
                body[0].yAxis -= 1;
                break;
            case DIR_DOWN:
                body[0].yAxis += 1;
                break;
            case DIR_RIGHT:
                body[0].xAxis += 1;
                break;
            case DIR_LEFT:
                body[0].xAxis -= 1;
                break;
        }

        // Check for collision with boundary
        if(body[0].xAxis < 1 || body[0].xAxis >= consoleWidth - 1 || body[0].yAxis < 1 || body[0].yAxis >= consoleHeight - 1) {
            return false;
        }

        // Check for self-collision
        for(int i = 1; i < length; i++) {
            if(body[0].xAxis == body[i].xAxis && body[0].yAxis == body[i].yAxis) {
                return false;
            }
        }

        // Check if snake eats food
        if(food.xAxis == body[0].xAxis && food.yAxis == body[0].yAxis) {
            body[length] = prevTail;
            length++;
            return true;
        }

        // Clear the previous tail position
        gotoxy(prevTail.xAxis, prevTail.yAxis);
        cout <<" "; // Clear the tail from the screen

        return true;
    }
};

class Board {
    Snake *snake;
    const char SNAKE_BODY = 'O';
    const char FOOD = '*';
    const char BOUNDARY = '#';
    Point food;
    int score;
    int level;
    int speed;
public:
    Board() {
        spawnFood();
        snake = new Snake(consoleWidth / 2, consoleHeight / 2);
        score = 0;
        level = 1;
        speed = 100;
    }

    ~Board() {
        delete snake;
    }

    int getScore() {
        return score;
    }

    void spawnFood() {
        food.xAxis = 1 + rand() % (consoleWidth - 2);
        food.yAxis = 1 + rand() % (consoleHeight - 2);
    }

    void displayCurrentScore() {
        gotoxy(0, 0);
        cout << "Score: " << score << "  Level: " << level << "  Speed: " << speed;
    }

    void drawBoundary() {
        // Draw top and bottom boundaries
        for(int i = 0; i < consoleWidth; i++) {
            gotoxy(i, 0);
            cout << BOUNDARY;
            gotoxy(i, consoleHeight - 1);
            cout << BOUNDARY;
        }
        // Draw left and right boundaries
        for(int i = 1; i < consoleHeight - 1; i++) {
            gotoxy(0, i);
            cout << BOUNDARY;
            gotoxy(consoleWidth - 1, i);
            cout << BOUNDARY;
        }
    }

    void draw() {
        // Draw the snake's body
        for(int i = 0; i < snake->getLength(); i++) {
            gotoxy(snake->body[i].xAxis, snake->body[i].yAxis);
            cout << SNAKE_BODY;
        }

        // Draw the food
        gotoxy(food.xAxis, food.yAxis);
        cout << FOOD;

        displayCurrentScore();
    }

    bool update() {
        bool isAlive = snake->move(food);
        if(!isAlive) {
            return false;
        }

        if(food.xAxis == snake->body[0].xAxis && food.yAxis == snake->body[0].yAxis) {
            score++;
            if(score % 5 == 0) { // Increase level every 5 points
                level++;
                if(speed > 50) { // Increase speed (decrease delay) but cap it
                    speed -= 10;
                }
            }
            spawnFood();
        }
        return true;
    }

    void getInput() {
        if(kbhit()) {
            int key = getch();
            if(key == 'w' || key == 'W') {
                snake->changeDirection(DIR_UP);
            }
            else if(key == 'a' || key == 'A') {
                snake->changeDirection(DIR_LEFT);
            }
            else if(key == 's' || key == 'S') {
                snake->changeDirection(DIR_DOWN);
            }
            else if(key == 'd' || key == 'D') {
                snake->changeDirection(DIR_RIGHT);
            }
        }
    }

    void endGame() {
        system("cls");
        cout << "Game Over!" << endl;
        cout << "Final Score: " << score << endl;
        cout << "Do you want to play again? (y/n): ";
    }

    bool playAgain() {
        char choice;
        cin >> choice;
        return (choice == 'y' || choice == 'Y');
    }

    int getSpeed() {
        return speed;
    }
};

int main() {
    srand(time(0));
    createScreen();

    Board *board = new Board();
    board->drawBoundary(); // Draw boundary only once
    do {
        board = new Board();
        board->drawBoundary(); // Ensure boundary is drawn before starting the game
        while(board->update()) {
            board->getInput();
            board->draw();
            Sleep(board->getSpeed());
        }
        board->endGame();
        delete board;
    } while(board->playAgain());

    return 0;
}
