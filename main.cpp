#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

// Constants
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int BLOCK_SIZE = 30;

// Current shape and its position
int shape[4][4];
int shapeX = 3, shapeY = 0;

// Game grid
int grid[GRID_HEIGHT][GRID_WIDTH] = {0};

// Shapes for H, U, M, A
vector<vector<vector<int>>> shapes = {
    // H
    {{1, 0, 1},
     {1, 1, 1},
     {0, 0, 0}},
    // U
    {{1, 0, 1},
     {1, 0, 1},
     {1, 1, 1}},
    // M
    {{1, 0, 0, 1},
     {1, 1, 1, 1}},
    // A
    {{0, 1, 0},
     {1, 0, 1},
     {1, 1, 1}}
};

// Function to draw a block
void drawBlock(int x, int y) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + BLOCK_SIZE, y);
    glVertex2f(x + BLOCK_SIZE, y + BLOCK_SIZE);
    glVertex2f(x, y + BLOCK_SIZE);
    glEnd();
}

// Initialize a random shape
void generateShape() {
    int randomIndex = rand() % shapes.size();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shape[i][j] = shapes[randomIndex][i][j];
        }
    }
    shapeX = 3;
    shapeY = 0;
}

// Check collision
bool checkCollision(int dx, int dy) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[i][j]) {
                int newX = shapeX + j + dx;
                int newY = shapeY + i + dy;
                if (newX < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT || grid[newY][newX]) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Lock the shape into the grid
void lockShape() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[i][j]) {
                grid[shapeY + i][shapeX + j] = 1;
            }
        }
    }
}

// Move the shape down
void dropShape() {
    if (!checkCollision(0, 1)) {
        shapeY++;
    } else {
        lockShape();
        generateShape();
    }
}

// Rotate the shape clockwise
void rotateShapeClockwise() {
    int temp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j][3 - i] = shape[i][j];
        }
    }
    memcpy(shape, temp, sizeof(temp));
    if (checkCollision(0, 0)) {
        rotateShapeClockwise(); // Undo rotation if it collides
    }
}

// Rotate the shape counterclockwise
void rotateShapeCounterClockwise() {
    int temp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[3 - j][i] = shape[i][j];
        }
    }
    memcpy(shape, temp, sizeof(temp));
    if (checkCollision(0, 0)) {
        rotateShapeCounterClockwise(); // Undo rotation if it collides
    }
}

// Render the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw grid
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x]) {
                glColor3f(0.5, 0.5, 0.5);
                drawBlock(x * BLOCK_SIZE, y * BLOCK_SIZE);
            }
        }
    }

    // Draw current shape
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[i][j]) {
                drawBlock((shapeX + j) * BLOCK_SIZE, (shapeY + i) * BLOCK_SIZE);
            }
        }
    }

    glutSwapBuffers();
}

// Handle keyboard input
void keyboard(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT && !checkCollision(-1, 0)) {
        shapeX--;
    } else if (key == GLUT_KEY_RIGHT && !checkCollision(1, 0)) {
        shapeX++;
    }
    glutPostRedisplay();
}

// Handle mouse input
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            rotateShapeClockwise();
        } else if (button == GLUT_RIGHT_BUTTON) {
            rotateShapeCounterClockwise();
        }
        glutPostRedisplay();
    }
}

// Timer for dropping the shape
void timer(int) {
    dropShape();
    glutPostRedisplay();
    glutTimerFunc(500, timer, 0);
}

// Initialize OpenGL
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glOrtho(0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE, 0, -1, 1);
    generateShape();
    srand(time(0));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE);
    glutCreateWindow("Tetris Game with H, U, M, A");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(500, timer, 0);
    glutMainLoop();
    return 0;
}
