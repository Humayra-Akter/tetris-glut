#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

// Constants
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int BLOCK_SIZE = 30; // Size of each block in pixels
const int SHAPES_COUNT = 4; // M, I, A, S

int score = 0; // Keeps track of the player's score
bool isGameOver = false; // Flag to indicate if the game is over

// Tetromino definitions (shapes M, I, A, S)
const int SHAPES[4][4][4] = {
    // H Shape
    {{1, 0, 1}, {1, 1, 1}, {1, 0, 1}},
    // A Shape
    {{0, 1, 0}, {1, 1, 1}, {1, 0, 1}},
    // u Shape
    {{1, 0, 1}, {1, 0, 1}, {1, 1, 1}},
    //m shape
    {{1, 1, 1}, {1, 1, 1}, {1, 0, 1}}
};

// Colors for each shape
const float COLORS[4][3] = {
    {1.0f, 0.0f, 0.0f}, // Red (M)
    {0.0f, 1.0f, 0.0f}, // Green (I)
    {0.0f, 0.0f, 1.0f}, // Blue (A)
    {1.0f, 0.0f, 1.0f},
};

// Game state
int grid[GRID_HEIGHT][GRID_WIDTH] = {0}; // 0: empty, >0: filled with a shape index
int currentShape[4][4];
int currentX, currentY;
int currentShapeIndex;

// Function prototypes
void initGame();
void display();
void timer(int);
void drawGrid();
void drawShape();
void moveShape(int dx, int dy);
void rotateShape(bool clockwise);
bool isValidPosition();
void lockShape();
void clearLines();
bool checkGameOver();
void renderScore();
void gameLoop();

// Initialize the game
void initGame() {
    srand(time(0));
    currentShapeIndex = rand() % SHAPES_COUNT;
    std::copy(&SHAPES[currentShapeIndex][0][0], &SHAPES[currentShapeIndex][0][0] + 16, &currentShape[0][0]);
    currentX = GRID_WIDTH / 2 - 2;
    currentY = 0;
}

// Display callback
// Display callback
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawShape();
    renderScore(); // Render the score

    if (isGameOver) {
        // Display the Game Over message
        glColor3f(1.0f, 1.0f, 1.0f); // Red color for the message
        glRasterPos2f(GRID_WIDTH * BLOCK_SIZE / 4, GRID_HEIGHT * BLOCK_SIZE / 2); // Position for the message

        std::string gameOverText = "Game Over!";
        for (char c : gameOverText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    glutSwapBuffers();
}


// Draw the grid
void drawGrid() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] > 0) {
                glColor3f(COLORS[grid[y][x] - 1][0], COLORS[grid[y][x] - 1][1], COLORS[grid[y][x] - 1][2]);
                glBegin(GL_QUADS);
                glVertex2f(x * BLOCK_SIZE, y * BLOCK_SIZE);
                glVertex2f((x + 1) * BLOCK_SIZE, y * BLOCK_SIZE);
                glVertex2f((x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
                glVertex2f(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
                glEnd();
            }
        }
    }
}

// Draw the current shape
void drawShape() {
    glColor3f(COLORS[currentShapeIndex][0], COLORS[currentShapeIndex][1], COLORS[currentShapeIndex][2]);
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentShape[y][x]) {
                int screenX = (currentX + x) * BLOCK_SIZE;
                int screenY = (currentY + y) * BLOCK_SIZE;

                glBegin(GL_QUADS);
                glVertex2f(screenX, screenY);
                glVertex2f(screenX + BLOCK_SIZE, screenY);
                glVertex2f(screenX + BLOCK_SIZE, screenY + BLOCK_SIZE);
                glVertex2f(screenX, screenY + BLOCK_SIZE);
                glEnd();
            }
        }
    }
}

// Timer callback
void timer(int) {
    if (!isGameOver) {
        moveShape(0, 1); // Move down
        glutPostRedisplay();
        glutTimerFunc(500, timer, 0); // 500 ms for each step
    }
}

// Move the shape
void moveShape(int dx, int dy) {
    currentX += dx;
    currentY += dy;

    if (!isValidPosition()) {
        currentX -= dx;
        currentY -= dy;

        if (dy > 0) { // If moving down fails
            lockShape();
            clearLines();
            if (checkGameOver()) {
                isGameOver = true;
            }
            initGame();
        }
    }
}


// Rotate the shape
void rotateShape(bool clockwise) {
    int rotated[4][4] = {0};
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (clockwise) {
                rotated[x][3 - y] = currentShape[y][x];
            } else {
                rotated[3 - x][y] = currentShape[y][x];
            }
        }
    }

    std::copy(&rotated[0][0], &rotated[0][0] + 16, &currentShape[0][0]);

    if (!isValidPosition()) {
        std::copy(&SHAPES[currentShapeIndex][0][0], &SHAPES[currentShapeIndex][0][0] + 16, &currentShape[0][0]);
    }
}

// Check if the position is valid
bool isValidPosition() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentShape[y][x]) {
                int nx = currentX + x;
                int ny = currentY + y;

                if (nx < 0 || nx >= GRID_WIDTH || ny >= GRID_HEIGHT || (ny >= 0 && grid[ny][nx])) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Lock the shape into the grid
void lockShape() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentShape[y][x]) {
                grid[currentY + y][currentX + x] = currentShapeIndex + 1;
            }
        }
    }
}

// Clear full lines
void clearLines() {
    int linesCleared = 0;

    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        bool fullLine = true;
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == 0) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            linesCleared++;

            for (int row = y; row > 0; row--) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                    grid[row][x] = grid[row - 1][x];
                }
            }

            for (int x = 0; x < GRID_WIDTH; x++) {
                grid[0][x] = 0;
            }

            y++; // Recheck this row
        }
    }

    // Update the score based on rows cleared
    if (linesCleared > 0) {
        score += 100 * linesCleared; // 100 points for each line cleared
    }
}

// Check if the game is over
bool checkGameOver() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        if (grid[0][x] != 0) {
            return true;
        }
    }
    return false;
}

// Render the score
void renderScore() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the score
    glRasterPos2f(10, 20); // Position for the score display at the top-left corner

    std::string scoreText = "Score: " + std::to_string(score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}


// Keyboard callback
void keyboard(int key, int, int) {
    if (isGameOver) return;

    if (key == GLUT_KEY_LEFT) {
        moveShape(-1, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        moveShape(1, 0);
    } else if (key == GLUT_KEY_DOWN) {
        moveShape(0, 1); // Speed up fall
    } else if (key == GLUT_KEY_UP) {
        rotateShape(true); // Rotate clockwise
    }
}

void mouse(int button, int state, int, int) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            rotateShape(true);
        } else if (button == GLUT_RIGHT_BUTTON) {
            rotateShape(false);
        }
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 700);
    glutCreateWindow("Tetris Game");
    glOrtho(0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE, 0, -1, 1);
    initGame();

    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(500, timer, 0);
    glutMainLoop();

    return 0;
}
