#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace L4 {

const float FIELD_L = -0.6f;
const float FIELD_R =  0.6f;
const float PI = 3.14159265f;

// array sizes
const int MAX_ENEMIES   = 11;
const int MAX_BOMBS     = 30;
const int MAX_LASERS    = 40;
const int MAX_ASTEROIDS = 20;
const int MAX_POWERUPS  = 6;
const int MAX_EXPLOSIONS= 20;

// enemy jets
GLfloat enemyBaseX[MAX_ENEMIES], enemyBaseY[MAX_ENEMIES];
GLfloat enemyX[MAX_ENEMIES],     enemyY[MAX_ENEMIES];
bool    enemyAlive[MAX_ENEMIES];
int     enemyCooldown[MAX_ENEMIES];
bool    enemyDiving[MAX_ENEMIES];

// enemy bombs
GLfloat bombX[MAX_BOMBS], bombY[MAX_BOMBS];
bool    bombAlive[MAX_BOMBS];

// player lasers
GLfloat laserX[MAX_LASERS], laserY[MAX_LASERS];
bool    laserAlive[MAX_LASERS];

// asteroids
GLfloat astX[MAX_ASTEROIDS], astY[MAX_ASTEROIDS];
GLfloat astRot[MAX_ASTEROIDS], astRotSpeed[MAX_ASTEROIDS];
GLfloat astScale[MAX_ASTEROIDS], astSpeed[MAX_ASTEROIDS];
bool    astAlive[MAX_ASTEROIDS];

// power-ups (0 = shield, 1 = health)
GLfloat puX[MAX_POWERUPS], puY[MAX_POWERUPS], puSpeed[MAX_POWERUPS];
int     puType[MAX_POWERUPS];
bool    puAlive[MAX_POWERUPS];

// explosions
GLfloat exX[MAX_EXPLOSIONS], exY[MAX_EXPLOSIONS];
int     exLife[MAX_EXPLOSIONS];

// player
GLfloat playerX = 0.0f;
const GLfloat playerY = -0.75f;
GLfloat playerSpeed = 0.016f;
GLfloat laserSpeed  = 0.025f;

int  score = 0;
int  lives = 3;
bool shieldActive   = false;
float shieldTimeLeft = 0.0f;
bool gameOver = false;

int frameCount    = 0;
int startDelay    = 150;  // ~2.5s safe warm-up so the level starts calmly
float formationTime = 0.0f;
int  fireCooldown = 0;
const int FIRE_COOLDOWN_FRAMES = 10;

const int   BOMB_SPAWN_INTERVAL     = 110;
const int   ASTEROID_SPAWN_INTERVAL = 130;
const int   POWERUP_SPAWN_INTERVAL  = 300;
const int   DIVE_ATTACK_INTERVAL    = 300;
const float FORMATION_SPEED         = 0.014f;
const float BOMB_SPEED              = 0.006f;
const float ASTEROID_SPEED_MIN      = 0.003f;
const float ASTEROID_SPEED_MAX      = 0.005f;
const float POWERUP_SPEED           = 0.008f;

// held-key state
bool moveLeft  = false;
bool moveRight = false;
bool spaceHeld = false;

void renderBitmapString(float x, float y, float z, void *font, const char *string)
{
    glRasterPos3f(x, y, z);
    for (const char *c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

// places the 11 enemy jets back into formation
void spawnEnemyFormation()
{
    float coords[MAX_ENEMIES][2] = {
        {-0.30f, 0.88f}, {-0.12f, 0.88f}, {0.12f, 0.88f}, {0.30f, 0.88f},
        {-0.20f, 0.70f}, { 0.00f, 0.70f}, {0.20f, 0.70f},
        {-0.08f, 0.52f}, { 0.08f, 0.52f},
        { 0.22f, 0.34f}, { 0.20f, 0.18f}
    };
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemyBaseX[i] = coords[i][0];
        enemyBaseY[i] = coords[i][1];
        enemyX[i] = enemyBaseX[i];
        enemyY[i] = enemyBaseY[i];
        enemyAlive[i] = true;
        enemyCooldown[i] = BOMB_SPAWN_INTERVAL + rand() % 40;
        enemyDiving[i] = false;
    }
}

void resetGame()
{
    score = 0;
    lives = 3;
    shieldActive = false;
    shieldTimeLeft = 0.0f;
    gameOver = false;
    frameCount = 0;
    startDelay = 150;
    formationTime = 0.0f;
    playerX = 0.0f;

    for (int i = 0; i < MAX_BOMBS; i++)      bombAlive[i] = false;
    for (int i = 0; i < MAX_LASERS; i++)     laserAlive[i] = false;
    for (int i = 0; i < MAX_ASTEROIDS; i++)  astAlive[i] = false;
    for (int i = 0; i < MAX_POWERUPS; i++)   puAlive[i] = false;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) exLife[i] = 0;

    spawnEnemyFormation();
}

// simple circle-distance collision check
bool hit(float x1, float y1, float x2, float y2, float r)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return (dx * dx + dy * dy) < (r * r);
}

void addExplosion(float x, float y)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (exLife[i] <= 0) {
            exX[i] = x; exY[i] = y; exLife[i] = 15;
            return;
        }
    }
}

// fires two lasers from the player jet, gated by a cooldown
void fireLasers()
{
    if (fireCooldown > 0 || gameOver) return;

    int slots = 0;
    for (int i = 0; i < MAX_LASERS && slots < 2; i++) {
        if (!laserAlive[i]) {
            laserAlive[i] = true;
            laserX[i] = playerX + (slots == 0 ? -0.07f : 0.07f);
            laserY[i] = playerY + 0.10f;
            slots++;
        }
    }
    fireCooldown = FIRE_COOLDOWN_FRAMES;
}

void update(int value)
{
    frameCount++;

    if (gameOver) {
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }

    if (startDelay > 0) startDelay--;

    // player movement
    if (moveLeft)  playerX -= playerSpeed;
    if (moveRight) playerX += playerSpeed;
    if (playerX < FIELD_L + 0.05f) playerX = FIELD_L + 0.05f;
    if (playerX > FIELD_R - 0.05f) playerX = FIELD_R - 0.05f;

    if (spaceHeld) fireLasers();

    // enemy formation drift
    formationTime += FORMATION_SPEED;
    float offsetX = 0.15f * sinf(formationTime);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemyAlive[i]) continue;

        if (!enemyDiving[i]) {
            enemyX[i] = enemyBaseX[i] + offsetX;
            enemyY[i] = enemyBaseY[i];
        } else {
            enemyY[i] -= 0.008f;
            if (enemyY[i] < -0.9f) {
                enemyDiving[i] = false;
                enemyX[i] = enemyBaseX[i];
                enemyY[i] = enemyBaseY[i];
            }
        }

        if (startDelay <= 0) {
            enemyCooldown[i]--;
            if (enemyCooldown[i] <= 0) {
                for (int b = 0; b < MAX_BOMBS; b++) {
                    if (!bombAlive[b]) {
                        bombAlive[b] = true;
                        bombX[b] = enemyX[i];
                        bombY[b] = enemyY[i] - 0.10f;
                        break;
                    }
                }
                enemyCooldown[i] = BOMB_SPAWN_INTERVAL + rand() % 40;
            }
        }
    }

    // random dive attack (disabled during start warm-up)
    if (startDelay <= 0 && frameCount % DIVE_ATTACK_INTERVAL == 0) {
        int candidate = -1, count = 0;
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemyAlive[i] && !enemyDiving[i]) {
                count++;
                if (rand() % count == 0) candidate = i;
            }
        }
        if (candidate != -1) enemyDiving[candidate] = true;
    }

    // enemy bombs
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!bombAlive[i]) continue;
        bombY[i] -= BOMB_SPEED;
        if (bombY[i] < -1.05f) { bombAlive[i] = false; continue; }

        if (hit(bombX[i], bombY[i], playerX, playerY, 0.10f)) {
            bombAlive[i] = false;
            addExplosion(playerX, playerY);
            if (shieldActive) shieldActive = false;
            else lives--;
        }
    }

    // player lasers vs enemies / asteroids
    for (int i = 0; i < MAX_LASERS; i++) {
        if (!laserAlive[i]) continue;
        laserY[i] += laserSpeed;
        if (laserY[i] > 1.05f) { laserAlive[i] = false; continue; }

        bool consumed = false;
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemyAlive[j]) continue;
            if (hit(laserX[i], laserY[i], enemyX[j], enemyY[j], 0.09f)) {
                laserAlive[i] = false;
                enemyAlive[j] = false;
                score += 2;
                addExplosion(enemyX[j], enemyY[j]);
                consumed = true;
                break;
            }
        }
        if (consumed) continue;

        for (int k = 0; k < MAX_ASTEROIDS; k++) {
            if (!astAlive[k]) continue;
            float r = 0.28f * astScale[k];
            if (hit(laserX[i], laserY[i], astX[k], astY[k], r)) {
                laserAlive[i] = false;
                astAlive[k] = false;
                score += 1  ;
                addExplosion(astX[k], astY[k]);
                break;
            }
        }
    }

    // respawn destroyed jets after a short delay
    if (frameCount % 200 == 0) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemyAlive[i]) {
                enemyAlive[i] = true;
                enemyX[i] = enemyBaseX[i];
                enemyY[i] = enemyBaseY[i];
                enemyDiving[i] = false;
                enemyCooldown[i] = BOMB_SPAWN_INTERVAL + rand() % 40;
                break;
            }
        }
    }

    // asteroids
    if (startDelay <= 0 && frameCount % ASTEROID_SPAWN_INTERVAL == 0) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (!astAlive[i]) {
                astX[i] = FIELD_L + 0.05f + static_cast<float>(rand() % 100) / 100.0f * (FIELD_R - FIELD_L - 0.10f);
                astY[i] = 1.1f;
                astRot[i] = 0.0f;
                astRotSpeed[i] = 1.0f + static_cast<float>(rand() % 300) / 100.0f;
                astScale[i] = 0.08f + static_cast<float>(rand() % 100) / 100.0f * 0.07f;
                astSpeed[i] = ASTEROID_SPEED_MIN + static_cast<float>(rand() % 100) / 100.0f * (ASTEROID_SPEED_MAX - ASTEROID_SPEED_MIN);
                astAlive[i] = true;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!astAlive[i]) continue;
        astY[i] -= astSpeed[i];
        astRot[i] += astRotSpeed[i];
        if (astY[i] < -1.15f) { astAlive[i] = false; continue; }

        float r = 0.28f * astScale[i];
        if (hit(astX[i], astY[i], playerX, playerY, r + 0.08f)) {
            astAlive[i] = false;
            addExplosion(playerX, playerY);
            if (shieldActive) shieldActive = false;
            else lives--;
        }
    }

    // power-ups
    if (startDelay <= 0 && frameCount % POWERUP_SPAWN_INTERVAL == 0) {
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (!puAlive[i]) {
                puX[i] = FIELD_L + 0.05f + static_cast<float>(rand() % 100) / 100.0f * (FIELD_R - FIELD_L - 0.10f);
                puY[i] = 1.05f;
                puSpeed[i] = POWERUP_SPEED;
                puType[i] = rand() % 2;
                puAlive[i] = true;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!puAlive[i]) continue;
        puY[i] -= puSpeed[i];
        if (puY[i] < -1.05f) { puAlive[i] = false; continue; }

        if (hit(puX[i], puY[i], playerX, playerY, 0.10f)) {
            puAlive[i] = false;
            if (puType[i] == 0) {
                // shield gain
                shieldActive = true;
                shieldTimeLeft = 300.0f;
            } else {
                if (lives < 5) lives++;
            }
        }
    }

    if (shieldActive) {
        shieldTimeLeft -= 1.0f;
        if (shieldTimeLeft <= 0.0f) shieldActive = false;
    }

    for (int i = 0; i < MAX_EXPLOSIONS; i++)
        if (exLife[i] > 0) exLife[i]--;

    if (fireCooldown > 0) fireCooldown--;

    if (lives <= 0) {
        lives = 0;
        gameOver = true;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a':
    case 'A':
        moveLeft = true;
        break;
    case 'd':
    case 'D':
        moveRight = true;
        break;
    case ' ':
        spaceHeld = true;
        break;
    case 'r':
    case 'R':
        if (gameOver) resetGame();
        break;
    }
    glutPostRedisplay();
}

void handleKeyUp(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a':
    case 'A':
        moveLeft = false;
        break;
    case 'd':
    case 'D':
        moveRight = false;
        break;
    case ' ':
        spaceHeld = false;
        break;
    }
}

void SpecialInput(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        moveLeft = true;
        break;
    case GLUT_KEY_RIGHT:
        moveRight = true;
        break;
    case GLUT_KEY_UP:
        playerSpeed += 0.005f;
        break;
    case GLUT_KEY_DOWN:
        if (playerSpeed > 0.01f) playerSpeed -= 0.005f;
        break;
    }
    glutPostRedisplay();
}

void SpecialUp(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        moveLeft = false;
        break;
    case GLUT_KEY_RIGHT:
        moveRight = false;
        break;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // ===== MOUNTAINS (outside the black playfield) =====
    glColor3f(0.10f, 0.10f, 0.13f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-0.88f, 0.60f);
        glVertex2f(-0.72f, -1.0f);

        glVertex2f(-0.90f, -1.0f);
        glVertex2f(-0.78f, 0.52f);
        glVertex2f(-0.62f, -1.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(0.72f, -1.0f);
        glVertex2f(0.82f, 0.60f);
        glVertex2f(1.0f, -1.0f);

        glVertex2f(0.62f, -1.0f);
        glVertex2f(0.76f, 0.52f);
        glVertex2f(0.90f, -1.0f);
    glEnd();

    glColor3f(0.06f, 0.06f, 0.08f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-0.92f, 0.55f);
        glVertex2f(-0.82f, -1.0f);

        glVertex2f(0.82f, -1.0f);
        glVertex2f(0.92f, 0.55f);
        glVertex2f(1.0f, -1.0f);
    glEnd();

    // ===== PLAYFIELD (black center area) =====
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(FIELD_L, -1.0f);
        glVertex2f(FIELD_R, -1.0f);
        glVertex2f(FIELD_R,  1.0f);
        glVertex2f(FIELD_L,  1.0f);
    glEnd();

    // ===== HUD / UI TEXT =====
    char buf[64];
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(buf, "SCORE: %06d", score);
    renderBitmapString(-0.92f, 0.88f, 0.0f, GLUT_BITMAP_HELVETICA_18, buf);

    char livesBuf[32] = "LIVES: ";
    for (int i = 0; i < lives; i++) strcat(livesBuf, "[<3]");
    renderBitmapString(-0.92f, 0.78f, 0.0f, GLUT_BITMAP_HELVETICA_18, livesBuf);

    renderBitmapString(-0.32f, 0.88f, 0.0f, GLUT_BITMAP_HELVETICA_18, "LEVEL 4");

    if (shieldActive)
        renderBitmapString(-0.92f, 0.68f, 0.0f, GLUT_BITMAP_HELVETICA_18, "SHIELD ACTIVE");

    if (gameOver) {
        glColor3f(1.0f, 0.2f, 0.2f);
        renderBitmapString(-0.15f, 0.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, "GAME OVER - press R to restart");
    }

    // ===== ENEMY JETS =====
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemyAlive[i]) continue;

        glPushMatrix();
            glTranslatef(enemyX[i], enemyY[i], 0.0f);
            glScalef(0.60f, 0.60f, 1.0f);

            // Main body
            glColor3f(0.55f, 0.58f, 0.62f);
            glBegin(GL_POLYGON);
                glVertex2f( 0.000f, -0.100f);
                glVertex2f(-0.025f, -0.010f);
                glVertex2f(-0.018f,  0.060f);
                glVertex2f( 0.018f,  0.060f);
                glVertex2f( 0.025f, -0.010f);
            glEnd();

            // Wings
            glColor3f(0.42f, 0.45f, 0.49f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.018f,  0.012f);
                glVertex2f(-0.110f,  0.050f);
                glVertex2f(-0.018f, -0.030f);

                glVertex2f( 0.018f,  0.012f);
                glVertex2f( 0.110f,  0.050f);
                glVertex2f( 0.018f, -0.030f);
            glEnd();

            // Tail
            glColor3f(0.35f, 0.37f, 0.40f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.018f, 0.055f);
                glVertex2f(-0.040f, 0.090f);
                glVertex2f(-0.006f, 0.060f);

                glVertex2f( 0.018f, 0.055f);
                glVertex2f( 0.040f, 0.090f);
                glVertex2f( 0.006f, 0.060f);
            glEnd();

            // Cockpit
            glColor3f(0.20f, 0.22f, 0.26f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.009f, -0.075f);
                glVertex2f(-0.009f, -0.045f);
                glVertex2f( 0.009f, -0.045f);
                glVertex2f( 0.009f, -0.075f);
            glEnd();
        glPopMatrix();
    }

    // ===== ENEMY BOMBS (RED ORBS) =====
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!bombAlive[i]) continue;

        // Outer glowing aura
        glColor3f(1.0f, 0.15f, 0.10f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(bombX[i], bombY[i]);
            for (int s = 0; s <= 16; s++) {
                float theta = 2.0f * PI * float(s) / 16.0f;
                glVertex2f(bombX[i] + 0.032f * cosf(theta), bombY[i] + 0.032f * sinf(theta));
            }
        glEnd();

        // Inner bright core
        glColor3f(1.0f, 0.85f, 0.70f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(bombX[i], bombY[i]);
            for (int s = 0; s <= 12; s++) {
                float theta = 2.0f * PI * float(s) / 12.0f;
                glVertex2f(bombX[i] + 0.016f * cosf(theta), bombY[i] + 0.016f * sinf(theta));
            }
        glEnd();
    }

    // ===== ASTEROIDS =====
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!astAlive[i]) continue;

        glPushMatrix();
            glTranslatef(astX[i], astY[i], 0.0f);
            glScalef(astScale[i], astScale[i], 1.0f);
            glRotatef(astRot[i], 0.0f, 0.0f, 1.0f);

            // Main outer body
            glColor3f(0.32f, 0.24f, 0.16f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.05f,  0.55f);
                glVertex2f( 0.35f,  0.50f);
                glVertex2f( 0.58f,  0.20f);
                glVertex2f( 0.50f, -0.25f);
                glVertex2f( 0.20f, -0.55f);
                glVertex2f(-0.25f, -0.50f);
                glVertex2f(-0.55f, -0.20f);
                glVertex2f(-0.50f,  0.20f);
                glVertex2f(-0.30f,  0.48f);
            glEnd();

            // Rocky facet 1
            glColor3f(0.43f, 0.32f, 0.21f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.30f,  0.42f);
                glVertex2f( 0.02f,  0.55f);
                glVertex2f( 0.18f,  0.25f);
                glVertex2f(-0.05f,  0.10f);
            glEnd();

            // Rocky facet 2
            glColor3f(0.22f, 0.17f, 0.12f);
            glBegin(GL_POLYGON);
                glVertex2f(0.18f,  0.25f);
                glVertex2f(0.55f,  0.20f);
                glVertex2f(0.40f, -0.15f);
                glVertex2f(0.05f, -0.05f);
            glEnd();

            // Rocky facet 3
            glColor3f(0.38f, 0.28f, 0.18f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.05f,  0.10f);
                glVertex2f(0.05f, -0.05f);
                glVertex2f(-0.10f, -0.45f);
                glVertex2f(-0.40f, -0.20f);
            glEnd();
        glPopMatrix();
    }

    // ===== POWER-UPS (0 = shield hex, 1 = health cross) =====
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!puAlive[i]) continue;

        glPushMatrix();
            glTranslatef(puX[i], puY[i], 0.0f);
            if (puType[i] == 0) {
                // Green shield
                glColor3f(0.2f, 0.9f, 0.5f);
                glBegin(GL_POLYGON);
                    glVertex2f( 0.00f,  0.05f);
                    glVertex2f(-0.04f,  0.03f);
                    glVertex2f(-0.03f, -0.03f);
                    glVertex2f( 0.00f, -0.05f);
                    glVertex2f( 0.03f, -0.03f);
                    glVertex2f( 0.04f,  0.03f);
                glEnd();
            } else {
                // Pink health cross
                glColor3f(1.0f, 0.30f, 0.45f);
                glBegin(GL_QUADS);
                    glVertex2f(-0.015f, -0.05f);
                    glVertex2f( 0.015f, -0.05f);
                    glVertex2f( 0.015f,  0.05f);
                    glVertex2f(-0.015f,  0.05f);
                glEnd();
                glBegin(GL_QUADS);
                    glVertex2f(-0.05f, -0.015f);
                    glVertex2f( 0.05f, -0.015f);
                    glVertex2f( 0.05f,  0.015f);
                    glVertex2f(-0.05f,  0.015f);
                glEnd();
            }
        glPopMatrix();
    }

    // ===== PLAYER SHIELD + PLAYER JET =====
    if (!gameOver) {
        if (shieldActive) {
            glPushMatrix();
                glTranslatef(playerX, playerY, 0.0f);
                glColor3f(0.25f, 0.75f, 1.0f);
                glLineWidth(3.0f);
                glBegin(GL_LINE_LOOP);
                    for (int s = 0; s < 80; s++) {
                        float theta = 2.0f * PI * s / 80.0f;
                        glVertex2f(0.15f * cosf(theta), 0.15f * sinf(theta));
                    }
                glEnd();
                glLineWidth(1.0f);
            glPopMatrix();
        }

        glPushMatrix();
            glTranslatef(playerX, playerY, 0.0f);
            glScalef(1.1f, 1.1f, 1.0f);

            // Main body
            glColor3f(0.85f, 0.88f, 0.92f);
            glBegin(GL_POLYGON);
                glVertex2f( 0.000f,  0.120f);
                glVertex2f(-0.025f,  0.020f);
                glVertex2f(-0.020f, -0.080f);
                glVertex2f( 0.020f, -0.080f);
                glVertex2f( 0.025f,  0.020f);
            glEnd();

            // Wings with blue details
            glColor3f(0.15f, 0.45f, 0.85f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.020f,  0.010f);
                glVertex2f(-0.110f, -0.070f);
                glVertex2f(-0.020f, -0.050f);

                glVertex2f( 0.020f,  0.010f);
                glVertex2f( 0.110f, -0.070f);
                glVertex2f( 0.020f, -0.050f);
            glEnd();

            // Wing trims
            glColor3f(0.70f, 0.75f, 0.82f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.020f, -0.050f);
                glVertex2f(-0.110f, -0.070f);
                glVertex2f(-0.020f, -0.085f);

                glVertex2f( 0.020f, -0.050f);
                glVertex2f( 0.110f, -0.070f);
                glVertex2f( 0.020f, -0.085f);
            glEnd();

            // Cockpit glass
            glColor3f(0.10f, 0.60f, 0.90f);
            glBegin(GL_POLYGON);
                glVertex2f( 0.000f,  0.080f);
                glVertex2f(-0.010f,  0.030f);
                glVertex2f( 0.000f,  0.010f);
                glVertex2f( 0.010f,  0.030f);
            glEnd();
        glPopMatrix();
    }

    // ===== PLAYER LASERS =====
    for (int i = 0; i < MAX_LASERS; i++) {
        if (!laserAlive[i]) continue;

        glLineWidth(3.0f);
        glColor3f(0.2f, 0.7f, 1.0f);
        glBegin(GL_LINES);
            glVertex2f(laserX[i], laserY[i]);
            glVertex2f(laserX[i], laserY[i] + 0.18f);
        glEnd();

        // Bright core
        glLineWidth(1.5f);
        glColor3f(0.9f, 0.98f, 1.0f);
        glBegin(GL_LINES);
            glVertex2f(laserX[i], laserY[i] + 0.01f);
            glVertex2f(laserX[i], laserY[i] + 0.17f);
        glEnd();
        glLineWidth(1.0f);
    }

    // ===== EXPLOSIONS =====
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (exLife[i] <= 0) continue;
        float t = exLife[i] / 15.0f;
        float scale = 0.6f + 0.6f * t;

        glPushMatrix();
            glTranslatef(exX[i], exY[i], 0.0f);
            glScalef(scale, scale, 1.0f);

            // Outer spikes
            glColor3f(0.9f, 0.4f, 0.1f);
            glBegin(GL_TRIANGLES);
                for (int k = 0; k < 8; k++) {
                    float a1 = k * (PI / 4.0f);
                    float a2 = a1 + 0.25f;
                    glVertex2f(0.0f, 0.0f);
                    glVertex2f(cosf(a1) * 0.11f, sinf(a1) * 0.11f);
                    glVertex2f(cosf(a2) * 0.06f, sinf(a2) * 0.06f);
                }
            glEnd();

            // Inner flame core
            glColor3f(1.0f, 0.85f, 0.2f);
            glBegin(GL_TRIANGLE_FAN);
                glVertex2f(0.0f, 0.0f);
                for (int s = 0; s <= 12; s++) {
                    float theta = 2.0f * PI * float(s) / 12.0f;
                    glVertex2f(0.05f * cosf(theta), 0.05f * sinf(theta));
                }
            glEnd();
        glPopMatrix();
    }

    glFlush();
}

int getScore()
{
    return score;
}

bool isFinished()
{
    return gameOver;
}

} // namespace L4
