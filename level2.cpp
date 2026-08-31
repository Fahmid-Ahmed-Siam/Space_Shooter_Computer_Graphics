#include <windows.h>
#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>

namespace L2 {

const float PI = 3.14159265f;
const int MAX_ENEMIES = 8;
const int MAX_LASERS = 10;
const int MAX_BOMBS = 0;
const int MAX_ASTEROIDS = 8;

// Game State
int score = 0;
int lives = 3;
int killCount = 0;
bool levelComplete = false;
float playerX = 0.0f;
float playerY = -0.7f;
float playerSpeed = 0.025f;

// held-key movement state (only horizontal: A/D + left/right arrows)
bool keyLeft = false;
bool keyRight = false;

struct Entity {
    float x, y;
    bool active;
};

Entity enemies[MAX_ENEMIES];
Entity lasers[MAX_LASERS];
Entity bombs[MAX_BOMBS];
Entity asteroids[MAX_ASTEROIDS];


// HELPER FUNCTIONS

bool checkCollision(float x1, float y1, float w1, float h1,
                    float x2, float y2, float w2, float h2) {
    return (fabsf(x1 - x2) * 2 < (w1 + w2)) &&
           (fabsf(y1 - y2) * 2 < (h1 + h2));
}

void renderBitmapString(float x, float y, float z,
                        void *font, const char *string) {
    glRasterPos3f(x, y, z);

    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(cx, cy);

    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * PI * float(i) / float(num_segments);

        float x = r * cosf(theta);
        float y = r * sinf(theta);

        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}


// DRAW ENEMY JET

void drawEnemyJet(float x, float y) {

    glPushMatrix();

    glTranslatef(x, y, 0.0f);
    glScalef(0.60f, 0.60f, 1.0f);

    glColor3f(0.55f, 0.58f, 0.62f);

    glBegin(GL_POLYGON);

    glVertex2f(0.000f, -0.100f);
    glVertex2f(-0.025f, -0.010f);
    glVertex2f(-0.018f, 0.060f);
    glVertex2f(0.018f, 0.060f);
    glVertex2f(0.025f, -0.010f);

    glEnd();

    glColor3f(0.42f, 0.45f, 0.49f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.018f, 0.012f);
    glVertex2f(-0.110f, 0.050f);
    glVertex2f(-0.018f, -0.030f);

    glVertex2f(0.018f, 0.012f);
    glVertex2f(0.110f, 0.050f);
    glVertex2f(0.018f, -0.030f);

    glEnd();

    glColor3f(0.35f, 0.37f, 0.40f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.018f, 0.055f);
    glVertex2f(-0.040f, 0.090f);
    glVertex2f(-0.006f, 0.060f);

    glVertex2f(0.018f, 0.055f);
    glVertex2f(0.040f, 0.090f);
    glVertex2f(0.006f, 0.060f);

    glEnd();

    glColor3f(0.20f, 0.22f, 0.26f);

    glBegin(GL_POLYGON);

    glVertex2f(-0.009f, 0.020f);
    glVertex2f(-0.009f, 0.045f);
    glVertex2f(0.009f, 0.045f);
    glVertex2f(0.009f, 0.020f);

    glEnd();

    glPopMatrix();
}


// DRAW ENEMY BOMB

void drawRedOrb(float x, float y) {

    glColor3f(1.0f, 0.15f, 0.10f);
    drawCircle(x, y, 0.032f, 16);

    glColor3f(1.0f, 0.85f, 0.70f);
    drawCircle(x, y, 0.016f, 12);
}


// DRAW PLAYER JET

void drawPlayerJet(float x, float y) {

    glPushMatrix();

    glTranslatef(x, y, 0.0f);
    glScalef(1.1f, 1.1f, 1.0f);

    glColor3f(0.85f, 0.88f, 0.92f);

    glBegin(GL_POLYGON);

    glVertex2f(0.000f, 0.120f);
    glVertex2f(-0.025f, 0.020f);
    glVertex2f(-0.020f, -0.080f);
    glVertex2f(0.020f, -0.080f);
    glVertex2f(0.025f, 0.020f);

    glEnd();

    glColor3f(0.15f, 0.45f, 0.85f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.020f, 0.010f);
    glVertex2f(-0.110f, -0.070f);
    glVertex2f(-0.020f, -0.050f);

    glVertex2f(0.020f, 0.010f);
    glVertex2f(0.110f, -0.070f);
    glVertex2f(0.020f, -0.050f);

    glEnd();

    glColor3f(0.70f, 0.75f, 0.82f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.020f, -0.050f);
    glVertex2f(-0.110f, -0.070f);
    glVertex2f(-0.020f, -0.085f);

    glVertex2f(0.020f, -0.050f);
    glVertex2f(0.110f, -0.070f);
    glVertex2f(0.020f, -0.085f);

    glEnd();

    glColor3f(0.10f, 0.60f, 0.90f);

    glBegin(GL_POLYGON);

    glVertex2f(0.000f, 0.080f);
    glVertex2f(-0.010f, 0.030f);
    glVertex2f(0.000f, 0.010f);
    glVertex2f(0.010f, 0.030f);

    glEnd();

    glPopMatrix();
}


// DRAW LASER

void drawLaser(float x, float y) {

    glLineWidth(3.0f);
    glColor3f(0.2f, 0.7f, 1.0f);

    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x, y + 0.18f);
    glEnd();

    glLineWidth(1.5f);
    glColor3f(0.9f, 0.98f, 1.0f);

    glBegin(GL_LINES);
    glVertex2f(x, y + 0.01f);
    glVertex2f(x, y + 0.17f);
    glEnd();

    glLineWidth(1.0f);
}


// DRAW ASTEROID

void drawAsteroid(float x, float y) {

    glPushMatrix();

    glTranslatef(x, y, 0.0f);

    glColor3f(0.42f, 0.32f, 0.22f);

    glBegin(GL_POLYGON);

    glVertex2f(0.000f, 0.110f);
    glVertex2f(0.061f, 0.088f);
    glVertex2f(0.094f, 0.022f);
    glVertex2f(0.094f, -0.039f);
    glVertex2f(0.028f, -0.094f);
    glVertex2f(-0.061f, -0.094f);
    glVertex2f(-0.105f, -0.039f);
    glVertex2f(-0.061f, 0.039f);

    glEnd();

    glPopMatrix();
}


// SPAWN ENEMY

void spawnEnemy() {

    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {

            enemies[i].x = ((rand() % 160) - 80) / 100.0f;
            enemies[i].y = 1.1f;
            enemies[i].active = true;

            break;
        }
    }
}


// SPAWN ASTEROID

void spawnAsteroid() {

    for (int i = 0; i < MAX_ASTEROIDS; i++) {

        if (!asteroids[i].active) {

            asteroids[i].x = ((rand() % 160) - 80) / 100.0f;
            asteroids[i].y = 1.2f;
            asteroids[i].active = true;

            break;
        }
    }
}


// UPDATE GAME

void update(int value) {

    if (lives <= 0 || levelComplete) {
        glutPostRedisplay();
        return;
    }

    if (killCount >= 20) {
        levelComplete = true;
        glutPostRedisplay();
        return;
    }


    // Player movement (held keys)
    if (keyLeft)  playerX -= playerSpeed;
    if (keyRight) playerX += playerSpeed;

    if (playerX > 0.9f)  playerX = 0.9f;
    if (playerX < -0.9f) playerX = -0.9f;


    // Move Lasers
    for (int i = 0; i < MAX_LASERS; i++) {

        if (lasers[i].active) {

            lasers[i].y += 0.05f;

            if (lasers[i].y > 1.0f)
                lasers[i].active = false;
        }
    }


    // Move Bombs
    for (int i = 0; i < MAX_BOMBS; i++) {

        if (bombs[i].active) {

            bombs[i].y -= 0.02f;

            if (bombs[i].y < -1.0f)
                bombs[i].active = false;

            if (bombs[i].active &&
                checkCollision(bombs[i].x, bombs[i].y,
                               0.06f, 0.06f,
                               playerX, playerY,
                               0.1f, 0.1f)) {

                lives--;
                bombs[i].active = false;
            }
        }
    }


    // Spawn Asteroids
    if (rand() % 38 == 0) {
    spawnAsteroid();
}


    // Move Asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++) {

        if (asteroids[i].active) {

            asteroids[i].y -= 0.015f;

            if (asteroids[i].y < -1.0f)
                asteroids[i].active = false;

            if (asteroids[i].active &&
                checkCollision(asteroids[i].x, asteroids[i].y,
                               0.15f, 0.15f,
                               playerX, playerY,
                               0.1f, 0.1f)) {

                lives--;
                asteroids[i].active = false;
            }
        }
    }


    // Spawn Enemies
    if (rand() % 40 == 0) {
        spawnEnemy();
    }


    // Move Enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (enemies[i].active) {

            enemies[i].y -= 0.008f;

            if (enemies[i].y < -1.0f) {
                enemies[i].active = false;
                continue;
            }


            // Enemy shoots
            if (rand() % 80 == 0) {

                for (int j = 0; j < MAX_BOMBS; j++) {

                    if (!bombs[j].active) {

                        bombs[j].x = enemies[i].x;
                        bombs[j].y = enemies[i].y - 0.1f;
                        bombs[j].active = true;

                        break;
                    }
                }
            }


            // Enemy hits player
            if (checkCollision(enemies[i].x, enemies[i].y,
                               0.15f, 0.15f,
                               playerX, playerY,
                               0.1f, 0.1f)) {

                lives--;
                enemies[i].active = false;

                continue;
            }


            // Laser hits enemy
            for (int j = 0; j < MAX_LASERS; j++) {

                if (lasers[j].active &&
                    checkCollision(lasers[j].x, lasers[j].y,
                                   0.02f, 0.18f,
                                   enemies[i].x, enemies[i].y,
                                   0.15f, 0.15f)) {

                    score += 100;
                    killCount++;

                    enemies[i].active = false;
                    lasers[j].active = false;

                    break;
                }
            }
        }
    }

    glutPostRedisplay();
}


// RESET LEVEL

void resetLevel() {

    score = 0;
    lives = 3;
    killCount = 0;

    levelComplete = false;

    playerX = 0.0f;
    playerY = -0.7f;

    for (int i = 0; i < MAX_ENEMIES; i++)
        enemies[i].active = false;

    for (int i = 0; i < MAX_LASERS; i++)
        lasers[i].active = false;

    for (int i = 0; i < MAX_BOMBS; i++)
        bombs[i].active = false;

    for (int i = 0; i < MAX_ASTEROIDS; i++)
        asteroids[i].active = false;
}


// KEYBOARD INPUT

void handleKeypress(unsigned char key, int x, int y) {

    if (lives <= 0 || levelComplete)
        return;

    if (key == 32) {

        for (int i = 0; i < MAX_LASERS; i += 2) {

            if (!lasers[i].active && !lasers[i + 1].active) {

                lasers[i].x = playerX - 0.03f;
                lasers[i].y = playerY + 0.1f;
                lasers[i].active = true;

                lasers[i + 1].x = playerX + 0.11f;
                lasers[i + 1].y = playerY + 0.1f;
                lasers[i + 1].active = true;

                break;
            }
        }
    }

    // A/D movement (held)
    if (key == 'a' || key == 'A') keyLeft = true;
    if (key == 'd' || key == 'D') keyRight = true;
}


// KEYBOARD UP

void handleKeyUp(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') keyLeft = false;
    if (key == 'd' || key == 'D') keyRight = false;
}


// ARROW KEY MOVEMENT

void SpecialInput(int key, int x, int y) {

    if (lives <= 0 || levelComplete)
        return;

    switch (key) {
    case GLUT_KEY_LEFT:  keyLeft = true;  break;
    case GLUT_KEY_RIGHT: keyRight = true; break;
    }
}


// SPECIAL UP

void SpecialUp(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:  keyLeft = false;  break;
    case GLUT_KEY_RIGHT: keyRight = false; break;
    }
}


// DISPLAY

void display() {

    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    char scoreStr[64];

    sprintf(scoreStr,
            "SCORE: %06d | KILLS: %d/20",
            score,
            killCount);


    char livesStr[64] = "LIVES: ";

    for (int i = 0; i < 3; i++) {

        if (i < lives)
            strcat(livesStr, "[<3]");
        else
            strcat(livesStr, "[  ]");
    }


    glColor3f(1.0f, 1.0f, 1.0f);

    renderBitmapString(-0.92f, 0.88f, 0.0f,
                       GLUT_BITMAP_HELVETICA_18,
                       scoreStr);

    renderBitmapString(-0.92f, 0.78f, 0.0f,
                       GLUT_BITMAP_HELVETICA_18,
                       livesStr);

    renderBitmapString(0.60f, 0.88f, 0.0f,
                       GLUT_BITMAP_HELVETICA_18,
                       "LEVEL 2");


    if (lives <= 0) {

        glColor3f(1.0f, 0.2f, 0.2f);

        renderBitmapString(-0.15f, 0.0f, 0.0f,
                           GLUT_BITMAP_HELVETICA_18,
                           "GAME OVER");

        return;
    }


    if (levelComplete) {

        glColor3f(0.2f, 1.0f, 0.2f);

        renderBitmapString(-0.25f, 0.0f, 0.0f,
                           GLUT_BITMAP_HELVETICA_18,
                           "LEVEL 2 COMPLETE!");

        return;
    }


    // Asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active)
            drawAsteroid(asteroids[i].x, asteroids[i].y);
    }


    // Enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active)
            drawEnemyJet(enemies[i].x, enemies[i].y);
    }


    // Bombs
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active)
            drawRedOrb(bombs[i].x, bombs[i].y);
    }


    // Lasers
    for (int i = 0; i < MAX_LASERS; i++) {
        if (lasers[i].active)
            drawLaser(lasers[i].x, lasers[i].y);
    }


    // Player
    drawPlayerJet(playerX, playerY);
}


// HIGH SCORE SUPPORT

int getScore() {
    return score;
}

bool isFinished() {
    if (lives <= 0 || levelComplete)
        return true;
    return false;
}

} // namespace L2
