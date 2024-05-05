#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
// YAAANNNN
//  Global variables for texture, position, and size
// using my code here Hussnain
bool stoppac;
GLuint pacmanTextureID;
GLuint PrevpacmanTextureID;
GLuint backgroundTextureID;

float x = 60.0f;
float y = 60.0f;
float side = 30.0f;                                      // Adjusted the size for clarity
const char *pacmanTexturePath = "imgs/pacman/right.png"; // Changed from char* to const char*
const char *backgroundTexturePath = "imgs/map/map.png";  // Path to the background texture

void *gameEngineThread(void *arg);
void *userInterfaceThread(void *arg);

char keypressed[10];
char delayKey[10];
char prevkeypressed[10];

int delayTurn = 50;
int delayTimer = 0;

// Function to load texture from file
void loadTexture(const char *filename, GLuint *textureID)
{
    // Load image texture using SOIL
    *textureID = SOIL_load_OGL_texture(
        filename,           // Image file path
        SOIL_LOAD_AUTO,     // Automatically choose format
        SOIL_CREATE_NEW_ID, // Generate a new texture ID
        SOIL_FLAG_INVERT_Y  // Invert Y-axis (OpenGL's origin is at the bottom-left corner)
    );

    if (*textureID == 0)
    {
        printf("Failed to load image: %s %s\n", SOIL_last_result(), filename);
        exit(1);
    }
}

// Function to display the scene
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw the background map
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(600, 0);
    glTexCoord2f(1, 1);
    glVertex2f(600, 800);
    glTexCoord2f(0, 1);
    glVertex2f(0, 800);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Draw Pacman
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pacmanTextureID);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x, y);
    glTexCoord2f(1, 0);
    glVertex2f(x + side, y);
    glTexCoord2f(1, 1);
    glVertex2f(x + side, y + (side * 1.0f)); // Adjusted the height of the quad
    glTexCoord2f(0, 1);
    glVertex2f(x, y + (side * 1.0f)); // Adjusted the height of the quad
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

void printPosition()
{
    printf("position: (%.2f, %.2f)\n", x, y);
}

bool isWallCollide(bool moveAxis, float xx, float yy) // 0 for x axis movement
{
    bool errFlag = true;
    if (moveAxis == 0) // Horizontal
    {
        int xPath[] = {60, 128, 685, 531, 596, 685, 531, 128, 395, 262, 195, 128, 128};
        int toFrom[] = {20, 555, 20, 125, 20, 254, 20, 125, 20, 555, 319, 555, 447, 555, 447, 555, 10, 165, 300, 555, 125, 447, 200, 283, 317, 350};
        int numElements = sizeof(xPath) / sizeof(xPath[0]);
        int numElementsToFrom = sizeof(toFrom) / sizeof(toFrom[0]);
        for (int i = 0; i < numElements; i++)
            if (yy == xPath[i])
            {
                printf("NOT COLLIIIDDE %d\n", numElementsToFrom);
                int j;
                if (i == 0)
                {
                    j = 0;
                }
                else
                {
                    j = i * 2;
                }

                if (xx >= toFrom[j] && xx <= toFrom[j + 1])
                {
                    return false;
                }
            }
        if (errFlag)
        {
            // strcpy(prevkeypressed, keypressed);
            return true;
        }
    }
    else // Vertical
    {
        int YPath[] = {20, 61, 125, 20, 254, 555, 319, 447, 555, 509, 253, 317};
        int toFrom[] = {60, 128, 128, 195, 128, 685, 531, 685, 596, 685, 531, 685, 596, 685, 128, 685, 60, 128, 128, 195, 60, 128, 60, 128};
        int numElements = sizeof(YPath) / sizeof(YPath[0]);
        int numElementsToFrom = sizeof(toFrom) / sizeof(toFrom[0]);
        for (int i = 0; i < numElements; i++)
            if (xx == YPath[i])
            {
                int j;
                if (i == 0)
                {
                    j = 0;
                }
                else
                {
                    j = i * 2;
                }

                if (yy >= toFrom[j] && yy <= toFrom[j + 1])
                {
                    return false;
                }
            }
        if (errFlag)
        {
            // strcpy(prevkeypressed, keypressed);
            return true;
        }
    }
}

// Function to handle keyboard input
void keyboard(int key, float xx, float yy)
{
    float newX = x;
    float newY = y;
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        strcpy(prevkeypressed, keypressed);
        strcpy(keypressed, "right");
        pacmanTexturePath = "imgs/pacman/right.png";

        break;
    case GLUT_KEY_LEFT:
        strcpy(prevkeypressed, keypressed);
        pacmanTexturePath = "imgs/pacman/left.png";
        strcpy(keypressed, "left");

        break;
    case GLUT_KEY_UP:
        strcpy(prevkeypressed, keypressed);
        pacmanTexturePath = "imgs/pacman/down.png";
        strcpy(keypressed, "up");

        break;
    case GLUT_KEY_DOWN:
        strcpy(prevkeypressed, keypressed);
        pacmanTexturePath = "imgs/pacman/up.png";
        strcpy(keypressed, "down");

        break;
    }
    // Reload the texture with the new filename
    loadTexture(pacmanTexturePath, &pacmanTextureID);
    glutPostRedisplay();
}

// Function to initialize OpenGL settings
void initOpenGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear color with alpha set to 0
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 600, 800, 0);
    glMatrixMode(GL_MODELVIEW);

    // Enable blending for transparent textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load the image textures
    loadTexture(pacmanTexturePath, &pacmanTextureID);
    loadTexture(backgroundTexturePath, &backgroundTextureID);
}

int main(int argc, char **argv)
{
    pthread_t EngineThread, playerThread;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pacman Game");
    initOpenGL();

    pthread_create(&EngineThread, NULL, gameEngineThread, NULL);
    pthread_create(&playerThread, NULL, userInterfaceThread, NULL);

    glutMainLoop();

    return 0;
}
bool delayFlag = false;
void *gameEngineThread(void *arg)
{
    glutSpecialFunc(keyboard);

    // Initialize game engine
    while (1)
    {
        if (delayFlag)
        {
            delayTimer++;
            if (delayTimer > delayTurn)
                delayTimer = 0;
            delayFlag = false;
        }
        if (!stoppac)
        {
            float newX = x;
            float newY = y;

            if (strcmp(delayKey, "down") == 0)
            {
            }

            if (strcmp(keypressed, "down") == 0 || (delayFlag && (strcmp(delayKey, "down") == 0)))
            {
                newY += 0.5;
                if (isWallCollide(1, newX, newY))
                {
                    if (!delayFlag)
                    {
                        delayTimer = 0;
                        delayFlag = true;
                        strcpy(delayKey, keypressed);
                    }

                    strcpy(keypressed, prevkeypressed);
                }
                else
                {

                    y += 0.5;
                }
            }
            else if (strcmp(keypressed, "up") == 0)
            {
                newY -= 0.5;
                if (isWallCollide(1, newX, newY))
                {
                    strcpy(keypressed, prevkeypressed);
                }
                else
                {
                    y -= 0.5;
                }
            }
            else if (strcmp(keypressed, "left") == 0)
            {
                newX -= 0.5;
                if (isWallCollide(0, newX, newY))
                {
                    strcpy(keypressed, prevkeypressed);
                }
                else
                {
                    x -= 0.5;
                }
            }
            else if (strcmp(keypressed, "right") == 0)
            {
                newX += 0.5;
                if (isWallCollide(0, newX, newY))
                {
                    strcpy(keypressed, prevkeypressed);
                }
                else
                {
                    x += 0.5;
                }
            }

            printPosition();
            glutDisplayFunc(display);
            glutPostRedisplay(); // Request redisplay
        }
        usleep(5000);
    }
    pthread_exit(NULL);
}

// User Interface Thread
void *userInterfaceThread(void *arg)
{
    // Initialize user interface
    while (1)
    {
    }
    pthread_exit(NULL);
}