#include <windows.h>
#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace L3 {

const float PI = 3.14159265f;
const int MAX_ENEMIES = 8;
const int MAX_LASERS = 10;
const int MAX_BOMBS = 10;
const int MAX_POWERUPS = 3;

// Game State
int score = 0;
int lives = 3;
float playerX = 0.0f;
float playerY = -0.7f;
float playerSpeed = 0.025f;
bool shieldActive = false;
int shieldTimer = 0;

// held-key movement state (only horizontal: A/D + left/right arrows)
bool keyLeft = false;
bool keyRight = false;

struct Entity {
    float x, y;
    bool active;
    int type; // For powerups: 1 = Shield, 2 = Health
    int timer;
};

Entity enemies[MAX_ENEMIES];
Entity lasers[MAX_LASERS];
Entity bombs[MAX_BOMBS];
Entity powerups[MAX_POWERUPS];

// Helper Functions
bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (fabsf(x1 - x2) * 2 < (w1 + w2)) && (fabsf(y1 - y2) * 2 < (h1 + h2));
}

void renderBitmapString(float x, float y, float z, void *font, const char *string) {
    glRasterPos3f(x, y, z);
    for (const char *c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
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

// User-Provided Drawing Functions
void drawMountains() {
    glBegin(GL_TRIANGLES);
        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(-1.0f, -1.0f);
        glColor3f(0.9f, 0.2f, 0.5f);
        glVertex2f(-0.88f, 0.60f);
        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(-0.72f, -1.0f);

        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(-0.90f, -1.0f);
        glColor3f(0.6f, 0.1f, 0.7f);
        glVertex2f(-0.78f, 0.52f);
        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(-0.62f, -1.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(0.72f, -1.0f);
        glColor3f(0.9f, 0.2f, 0.5f);
        glVertex2f(0.82f, 0.60f);
        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(1.0f, -1.0f);

        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(0.62f, -1.0f);
        glColor3f(0.6f, 0.1f, 0.7f);
        glVertex2f(0.76f, 0.52f);
        glColor3f(0.1f, 0.1f, 0.3f);
        glVertex2f(0.90f, -1.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0.05f, 0.05f, 0.15f);
        glVertex2f(-1.0f, -1.0f);
        glColor3f(0.4f, 0.1f, 0.4f);
        glVertex2f(-0.92f, 0.55f);
        glColor3f(0.05f, 0.05f, 0.15f);
        glVertex2f(-0.82f, -1.0f);

        glColor3f(0.05f, 0.05f, 0.15f);
        glVertex2f(0.82f, -1.0f);
        glColor3f(0.4f, 0.1f, 0.4f);
        glVertex2f(0.92f, 0.55f);
        glColor3f(0.05f, 0.05f, 0.15f);
        glVertex2f(1.0f, -1.0f);
    glEnd();
}

void drawEnemyJet(float x, float y) {
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(0.60f, 0.60f, 1.0f);
        glColor3f(0.55f, 0.58f, 0.62f);
        glBegin(GL_POLYGON);
            glVertex2f( 0.000f, -0.100f);
            glVertex2f(-0.025f, -0.010f);
            glVertex2f(-0.018f,  0.060f);
            glVertex2f( 0.018f,  0.060f);
            glVertex2f( 0.025f, -0.010f);
        glEnd();

        glColor3f(0.42f, 0.45f, 0.49f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.018f,  0.012f);
            glVertex2f(-0.110f,  0.050f);
            glVertex2f(-0.018f, -0.030f);
            glVertex2f( 0.018f,  0.012f);
            glVertex2f( 0.110f,  0.050f);
            glVertex2f( 0.018f, -0.030f);
        glEnd();

        glColor3f(0.35f, 0.37f, 0.40f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.018f, 0.055f);
            glVertex2f(-0.040f, 0.090f);
            glVertex2f(-0.006f, 0.060f);
            glVertex2f( 0.018f, 0.055f);
            glVertex2f( 0.040f, 0.090f);
            glVertex2f( 0.006f, 0.060f);
        glEnd();

        glColor3f(0.20f, 0.22f, 0.26f);
        glBegin(GL_POLYGON);
            glVertex2f(-0.009f, 0.020f);
            glVertex2f(-0.009f, 0.045f);
            glVertex2f( 0.009f, 0.045f);
            glVertex2f( 0.009f, 0.020f);
        glEnd();
    glPopMatrix();
}

void drawRedOrb(float x, float y) {
    glColor3f(1.0f, 0.15f, 0.10f);
    drawCircle(x, y, 0.032f, 16);
    glColor3f(1.0f, 0.85f, 0.70f);
    drawCircle(x, y, 0.016f, 12);
}

void drawPlayerJet(float x, float y) {
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(1.1f, 1.1f, 1.0f);
        if (shieldActive) {
            glColor3f(0.2f, 0.9f, 0.5f);
            drawCircle(0.0f, 0.02f, 0.15f, 20); // Shield Aura
        }
        glColor3f(0.85f, 0.88f, 0.92f);
        glBegin(GL_POLYGON);
            glVertex2f( 0.000f,  0.120f);
             glVertex2f(-0.025f,  0.020f);
            glVertex2f(-0.020f, -0.080f);
            glVertex2f( 0.020f, -0.080f);
            glVertex2f( 0.025f,  0.020f);
        glEnd();

        glColor3f(0.15f, 0.45f, 0.85f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.020f,  0.010f);
            glVertex2f(-0.110f, -0.070f);
            glVertex2f(-0.020f, -0.050f);
            glVertex2f( 0.020f,  0.010f);
            glVertex2f( 0.110f, -0.070f);
            glVertex2f( 0.020f, -0.050f);
        glEnd();

        glColor3f(0.70f, 0.75f, 0.82f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.020f, -0.050f);
            glVertex2f(-0.110f, -0.070f);
            glVertex2f(-0.020f, -0.085f);
            glVertex2f( 0.020f, -0.050f);
            glVertex2f( 0.110f, -0.070f);
            glVertex2f( 0.020f, -0.085f);
        glEnd();

        glColor3f(0.10f, 0.60f, 0.90f);
        glBegin(GL_POLYGON);
            glVertex2f( 0.000f,  0.080f);
            glVertex2f(-0.010f,  0.030f);
            glVertex2f( 0.000f,  0.010f);
            glVertex2f( 0.010f,  0.030f);
        glEnd();
    glPopMatrix();
}

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

void drawShieldPowerUp(float x, float y) {
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glColor3f(0.2f, 0.9f, 0.5f);
        glBegin(GL_POLYGON);
            glVertex2f( 0.00f,  0.05f);
            glVertex2f(-0.04f,  0.03f);
            glVertex2f(-0.03f, -0.03f);
            glVertex2f( 0.00f, -0.05f);
            glVertex2f( 0.03f, -0.03f); glVertex2f( 0.04f,  0.03f);
        glEnd();
    glPopMatrix();
}

void drawHealthPowerUp(float x, float y) {
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glColor3f(0.95f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
            glVertex2f(-0.010f, -0.035f);
            glVertex2f( 0.010f, -0.035f);
            glVertex2f( 0.010f,  0.035f);
            glVertex2f(-0.010f,  0.035f);
        glEnd();
        glBegin(GL_QUADS);
            glVertex2f(-0.035f, -0.010f);
            glVertex2f( 0.035f, -0.010f);
            glVertex2f( 0.035f,  0.010f);
            glVertex2f(-0.035f,  0.010f);
        glEnd();
    glPopMatrix();
}

// Game Mechanics Update
void spawnEnemy() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x = ((rand() % 160) - 80) / 100.0f; // Range -0.8 to 0.8
            enemies[i].y = 1.1f;
            enemies[i].active = true;
            break;
        }
    }
}

void spawnPowerUp(float x, float y) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].active = true;
            powerups[i].type = (rand() % 2) + 1; // 1 = Shield, 2 = Health
            powerups[i].timer = 200; // Lasts ~3.3 seconds at 60fps
            break;
        }
    }
}

void update(int value) {
    if (lives <= 0) return; // Game over freeze

    // Shield Logic
    if (shieldActive) {
        shieldTimer--;
        if (shieldTimer <= 0) shieldActive = false;
    }

    // Player movement (held keys)
    if (keyLeft)  playerX -= playerSpeed;
    if (keyRight) playerX += playerSpeed;

    if (playerX > 0.9f)  playerX = 0.9f;
    if (playerX < -0.9f) playerX = -0.9f;

    // Move Lasers (Player)
    for (int i = 0; i < MAX_LASERS; i++) {
        if (lasers[i].active) {
            lasers[i].y += 0.05f;
            if (lasers[i].y > 1.0f) lasers[i].active = false;
        }
    }

    // Move Bombs (Enemies)
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active) {
            bombs[i].y -= 0.02f;
            if (bombs[i].y < -1.0f) bombs[i].active = false;

            // Collision: Bomb vs Player
            if (!shieldActive && checkCollision(bombs[i].x, bombs[i].y, 0.06f, 0.06f, playerX, playerY, 0.1f, 0.1f)) {
                lives--;
                bombs[i].active = false;
            }
        }
    }

    // Move Enemies & Enemy Shooting
    if (rand() % 30 == 0) spawnEnemy();

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].y -= 0.008f;

            if (enemies[i].y < -1.0f) {
                enemies[i].active = false;
                continue;
            }

            // Enemy randomly shoots bomb
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

            // Collision: Enemy vs Player
            if (!shieldActive && checkCollision(enemies[i].x, enemies[i].y, 0.15f, 0.15f, playerX, playerY, 0.1f, 0.1f)) {
                lives--;
                enemies[i].active = false;
            }

            // Collision: Laser vs Enemy
            for (int j = 0; j < MAX_LASERS; j++) {
                if (lasers[j].active && checkCollision(lasers[j].x, lasers[j].y, 0.02f, 0.18f, enemies[i].x, enemies[i].y, 0.15f, 0.15f)) {
                    score += 100;
                    enemies[i].active = false;
                    lasers[j].active = false;

                    // Chance to drop power-up
                    if (rand() % 5 == 0) spawnPowerUp(enemies[i].x, enemies[i].y);
                }
            }
        }
    }

    // Move Power-Ups
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            powerups[i].y -= 0.005f;
            powerups[i].timer--;

            if (powerups[i].timer <= 0 || powerups[i].y < -1.0f) {
                powerups[i].active = false;
            }

            // Collision: Player collects Power-Up
            if (checkCollision(powerups[i].x, powerups[i].y, 0.1f, 0.1f, playerX, playerY, 0.15f, 0.15f)) {
                if (powerups[i].type == 1) { // Shield
                    shieldActive = true;
                    shieldTimer = 300; // 5 seconds
                } else if (powerups[i].type == 2) { // Health
                    if (lives < 3) lives++;
                }
                powerups[i].active = false;
            }
        }
    }

    glutPostRedisplay();
}

void resetLevel() {
    score = 0;
    lives = 3;
    playerX = 0.0f;
    playerY = -0.7f;
    shieldActive = false;
    shieldTimer = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)  enemies[i].active = false;
    for (int i = 0; i < MAX_LASERS; i++)   lasers[i].active = false;
    for (int i = 0; i < MAX_BOMBS; i++)    bombs[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
}

// Input Handling
void handleKeypress(unsigned char key, int x, int y) {
    if (lives <= 0) return;

    // Spacebar to shoot
    if (key == 32) {
        for (int i = 0; i < MAX_LASERS; i+=2) {
            if (!lasers[i].active && !lasers[i+1].active) {
                lasers[i].x = playerX - 0.03f;
                lasers[i].y = playerY + 0.1f;
                lasers[i].active = true;

                lasers[i+1].x = playerX + 0.11f;
                lasers[i+1].y = playerY + 0.1f;
                lasers[i+1].active = true;
                break;
            }
        }
    }

    // A/D movement (held)
    if (key == 'a' || key == 'A') keyLeft = true;
    if (key == 'd' || key == 'D') keyRight = true;
}

void handleKeyUp(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') keyLeft = false;
    if (key == 'd' || key == 'D') keyRight = false;
}

void SpecialInput(int key, int x, int y) {
    if (lives <= 0) return;
    switch(key) {
        case GLUT_KEY_LEFT:  keyLeft = true;  break;
        case GLUT_KEY_RIGHT: keyRight = true; break;
    }
}

void SpecialUp(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:  keyLeft = false;  break;
        case GLUT_KEY_RIGHT: keyRight = false; break;
    }
}

void display() {
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawMountains();

    // UI
    char scoreStr[32];
    sprintf(scoreStr, "SCORE: %06d", score);

    char livesStr[32] = "LIVES: ";
    for (int i = 0; i < 3; i++) {
        if (i < lives) strcat(livesStr, "[<3]");
        else strcat(livesStr, "[  ]");
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-0.92f, 0.88f, 0.0f, GLUT_BITMAP_HELVETICA_18, scoreStr);
    renderBitmapString(-0.92f, 0.78f, 0.0f, GLUT_BITMAP_HELVETICA_18, livesStr);
    renderBitmapString(-0.32f, 0.88f, 0.0f, GLUT_BITMAP_HELVETICA_18, "LEVEL 3");

    if (lives <= 0) {
        renderBitmapString(-0.15f, 0.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, "GAME OVER");
        glFlush();
        return;
    }

    // Render Game Objects
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            if (powerups[i].type == 1) drawShieldPowerUp(powerups[i].x, powerups[i].y);
            else drawHealthPowerUp(powerups[i].x, powerups[i].y);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) drawEnemyJet(enemies[i].x, enemies[i].y);
    }

    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active) drawRedOrb(bombs[i].x, bombs[i].y);
    }

    for (int i = 0; i < MAX_LASERS; i++) {
        if (lasers[i].active) drawLaser(lasers[i].x, lasers[i].y);
    }

    drawPlayerJet(playerX, playerY);
    glFlush();
}

// HIGH SCORE SUPPORT

int getScore() {
    return score;
}

bool isFinished() {
    return lives <= 0;
}

} // namespace L3
