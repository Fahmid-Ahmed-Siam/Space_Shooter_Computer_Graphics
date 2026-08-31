#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>


namespace L5
{

// ==========================================
// GAME VARIABLES
// ==========================================

float playerX = 0.0f;

// held-key movement state (level 5 moves left/right)
bool keyLeft = false;
bool keyRight = false;

int score = 0;
int lives = 3;

int bossHealth = 40;
const int maxBossHealth = 40;

bool shieldActive = false;
int shieldTime = 0;


// ==========================================
// PLAYER BULLETS
// ==========================================

float playerBulletX1 = -0.035f;
float playerBulletY1 = -0.6f;

float playerBulletX2 = 0.035f;
float playerBulletY2 = -0.6f;


// ==========================================
// BOSS BULLETS
// ==========================================

float bossBulletX[4] = {-0.28f, -0.14f, 0.14f, 0.28f};
float bossBulletY[4] = {0.20f, 0.20f, 0.20f, 0.20f};


// ==========================================
// ENEMY JETS
// ==========================================

float enemyX[2] = {-0.45f, 0.45f};
float enemyY[2] = {0.70f, 0.95f};

bool enemyAlive[2] = {true, true};


// ==========================================
// POWER UPS
// ==========================================

float shieldX = 0.0f;
float shieldY = -2.0f;
bool shieldDrop = false;

float healthX = 0.0f;
float healthY = -2.0f;
bool healthDrop = false;


// ==========================================
// GAME STATUS
// ==========================================

bool gameOver = false;
bool gameWin = false;


// ==========================================
// TEXT FUNCTION
// ==========================================

void renderBitmapString(float x, float y, float z, void *font, const char *string)
{
    glRasterPos3f(x, y, z);

    for(const char *c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}


// ==========================================
// ENEMY JET
// ==========================================

void drawEnemyJet()
{
    // MAIN BODY

    glColor3f(0.55f, 0.58f, 0.62f);

    glBegin(GL_POLYGON);

    glVertex2f(0.000f, 0.20f);
    glVertex2f(-0.040f, 0.08f);
    glVertex2f(-0.035f, -0.12f);
    glVertex2f(0.035f, -0.12f);
    glVertex2f(0.040f, 0.08f);

    glEnd();


    // LEFT WING

    glColor3f(0.42f, 0.45f, 0.49f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.030f, 0.08f);
    glVertex2f(-0.20f, -0.01f);
    glVertex2f(-0.055f, -0.08f);

    glEnd();


    // RIGHT WING

    glBegin(GL_TRIANGLES);

    glVertex2f(0.030f, 0.08f);
    glVertex2f(0.20f, -0.01f);
    glVertex2f(0.055f, -0.08f);

    glEnd();


    // DARK CENTER

    glColor3f(0.28f, 0.30f, 0.34f);

    glBegin(GL_QUADS);

    glVertex2f(-0.020f, 0.08f);
    glVertex2f(0.020f, 0.08f);
    glVertex2f(0.020f, -0.10f);
    glVertex2f(-0.020f, -0.10f);

    glEnd();


    // COCKPIT

    glColor3f(0.20f, 0.22f, 0.26f);

    glBegin(GL_TRIANGLES);

    glVertex2f(0.000f, 0.12f);
    glVertex2f(-0.025f, 0.06f);
    glVertex2f(0.025f, 0.06f);

    glEnd();


    // ENGINE

    glColor3f(0.25f, 0.30f, 0.35f);

    glBegin(GL_QUADS);

    glVertex2f(-0.025f, -0.10f);
    glVertex2f(0.025f, -0.10f);
    glVertex2f(0.025f, -0.15f);
    glVertex2f(-0.025f, -0.15f);

    glEnd();
}


// ==========================================
// PLAYER JET
// ==========================================

void drawPlayerJet()
{
    // MAIN FUSELAGE

    glColor3f(0.82f, 0.85f, 0.88f);

    glBegin(GL_POLYGON);

    glVertex2f(0.000f, 0.180f);
    glVertex2f(-0.035f, 0.040f);
    glVertex2f(-0.030f, -0.110f);
    glVertex2f(0.030f, -0.110f);
    glVertex2f(0.035f, 0.040f);

    glEnd();


    // CENTER STRIP

    glColor3f(0.55f, 0.60f, 0.66f);

    glBegin(GL_QUADS);

    glVertex2f(-0.012f, -0.110f);
    glVertex2f(0.012f, -0.110f);
    glVertex2f(0.012f, 0.120f);
    glVertex2f(-0.012f, 0.120f);

    glEnd();


    // LEFT WING

    glColor3f(0.70f, 0.74f, 0.78f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.030f, -0.012f);
    glVertex2f(-0.200f, -0.085f);
    glVertex2f(-0.048f, -0.110f);

    glEnd();


    // RIGHT WING

    glBegin(GL_TRIANGLES);

    glVertex2f(0.030f, -0.012f);
    glVertex2f(0.200f, -0.085f);
    glVertex2f(0.048f, -0.110f);

    glEnd();


    // COCKPIT

    glColor3f(0.15f, 0.35f, 0.55f);

    glBegin(GL_TRIANGLES);

    glVertex2f(0.000f, 0.100f);
    glVertex2f(-0.030f, 0.045f);
    glVertex2f(0.030f, 0.045f);

    glEnd();


    // ENGINE

    glColor3f(0.3f, 0.6f, 1.0f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.018f, -0.110f);
    glVertex2f(0.018f, -0.110f);
    glVertex2f(0.000f, -0.160f);

    glEnd();
}


// ==========================================
// BOSS ENEMY JET
// ==========================================

void drawBossEnemyJet()
{
    // LEFT OUTER WING

    glColor3f(0.4706f, 0.4902f, 0.5098f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.08f, 0.15f);
    glVertex2f(-0.38f, -0.12f);
    glVertex2f(-0.15f, -0.18f);

    glEnd();


    // RIGHT OUTER WING

    glBegin(GL_TRIANGLES);

    glVertex2f(0.08f, 0.15f);
    glVertex2f(0.38f, -0.12f);
    glVertex2f(0.15f, -0.18f);

    glEnd();


    // MAIN BODY

    glColor3f(0.7843f, 0.8039f, 0.8235f);

    glBegin(GL_QUADS);

    glVertex2f(-0.10f, 0.22f);
    glVertex2f(0.10f, 0.22f);
    glVertex2f(0.13f, -0.20f);
    glVertex2f(-0.13f, -0.20f);

    glEnd();


    // NOSE

    glColor3f(0.9020f, 0.9216f, 0.9412f);

    glBegin(GL_TRIANGLES);

    glVertex2f(0.0f, 0.48f);
    glVertex2f(-0.10f, 0.20f);
    glVertex2f(0.10f, 0.20f);

    glEnd();


    // DARK CENTER

    glColor3f(0.2549f, 0.2745f, 0.3137f);

    glBegin(GL_QUADS);

    glVertex2f(-0.055f, 0.22f);
    glVertex2f(0.055f, 0.22f);
    glVertex2f(0.075f, -0.15f);
    glVertex2f(-0.075f, -0.15f);

    glEnd();


    // COCKPIT

    glColor3f(0.1569f, 0.1765f, 0.2157f);

    glBegin(GL_TRIANGLES);

    glVertex2f(0.0f, 0.20f);
    glVertex2f(-0.06f, 0.04f);
    glVertex2f(0.06f, 0.04f);

    glEnd();


    // COCKPIT GLASS

    glColor3f(0.3529f, 0.6667f, 0.8627f);

    glBegin(GL_TRIANGLES);

    glVertex2f(0.0f, 0.16f);
    glVertex2f(-0.04f, 0.06f);
    glVertex2f(0.04f, 0.06f);

    glEnd();


    // LEFT ENGINE

    glColor3f(0.2745f, 0.2941f, 0.3137f);

    glBegin(GL_QUADS);

    glVertex2f(-0.12f,-0.15f);
    glVertex2f(-0.04f,-0.15f);
    glVertex2f(-0.04f,-0.28f);
    glVertex2f(-0.12f,-0.28f);

    glEnd();


    // RIGHT ENGINE

    glBegin(GL_QUADS);

    glVertex2f(0.04f,-0.15f);
    glVertex2f(0.12f,-0.15f);
    glVertex2f(0.12f,-0.28f);
    glVertex2f(0.04f,-0.28f);

    glEnd();


    // LEFT ENGINE FIRE

    glColor3f(1.0f, 0.4706f, 0.0784f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.08f,-0.28f);
    glVertex2f(-0.12f,-0.40f);
    glVertex2f(-0.04f,-0.40f);

    glEnd();


    // RIGHT ENGINE FIRE

    glBegin(GL_TRIANGLES);

    glVertex2f(0.08f,-0.28f);
    glVertex2f(0.04f,-0.40f);
    glVertex2f(0.12f,-0.40f);

    glEnd();


    // FOUR SHOOTERS

    glColor3f(0.12f, 0.13f, 0.15f);

    glBegin(GL_QUADS);

    glVertex2f(-0.18f,-0.02f);
    glVertex2f(-0.14f,-0.02f);
    glVertex2f(-0.14f,-0.15f);
    glVertex2f(-0.18f,-0.15f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(-0.29f,-0.08f);
    glVertex2f(-0.25f,-0.08f);
    glVertex2f(-0.25f,-0.21f);
    glVertex2f(-0.29f,-0.21f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(0.14f,-0.02f);
    glVertex2f(0.18f,-0.02f);
    glVertex2f(0.18f,-0.15f);
    glVertex2f(0.14f,-0.15f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(0.25f,-0.08f);
    glVertex2f(0.29f,-0.08f);
    glVertex2f(0.29f,-0.21f);
    glVertex2f(0.25f,-0.21f);

    glEnd();


    // SHOOTER TIPS

    glColor3f(0.65f, 0.67f, 0.70f);

    glBegin(GL_QUADS);

    glVertex2f(-0.175f,-0.15f);
    glVertex2f(-0.145f,-0.15f);
    glVertex2f(-0.145f,-0.19f);
    glVertex2f(-0.175f,-0.19f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(-0.285f,-0.21f);
    glVertex2f(-0.255f,-0.21f);
    glVertex2f(-0.255f,-0.25f);
    glVertex2f(-0.285f,-0.25f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(0.145f,-0.15f);
    glVertex2f(0.175f,-0.15f);
    glVertex2f(0.175f,-0.19f);
    glVertex2f(0.145f,-0.19f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(0.255f,-0.21f);
    glVertex2f(0.285f,-0.21f);
    glVertex2f(0.285f,-0.25f);
    glVertex2f(0.255f,-0.25f);

    glEnd();
}


// ==========================================
// SHIELD POWERUP
// ==========================================

void drawshield()
{
    glColor3f(0.2f, 0.9f, 0.5f);

    glBegin(GL_POLYGON);

    glVertex2f(0.00f, 0.05f);
    glVertex2f(-0.04f, 0.03f);
    glVertex2f(-0.03f, -0.03f);
    glVertex2f(0.00f, -0.05f);
    glVertex2f(0.03f, -0.03f);
    glVertex2f(0.04f, 0.03f);

    glEnd();
}


// ==========================================
// HEALTH POWERUP
// ==========================================

void drawHealth()
{
    glColor3f(1.0f, 0.30f, 0.45f);

    glBegin(GL_QUADS);

    glVertex2f(-0.015f, -0.05f);
    glVertex2f(0.015f, -0.05f);
    glVertex2f(0.015f, 0.05f);
    glVertex2f(-0.015f, 0.05f);

    glEnd();


    glBegin(GL_QUADS);

    glVertex2f(-0.05f, -0.015f);
    glVertex2f(0.05f, -0.015f);
    glVertex2f(0.05f, 0.015f);
    glVertex2f(-0.05f, 0.015f);

    glEnd();
}


// ==========================================
// VOLCANO
// ==========================================

void drawVolcano()
{
    glColor3f(0.55f, 0.32f, 0.20f);

    glBegin(GL_POLYGON);

    glVertex2f(-0.12f, -0.90f);
    glVertex2f(-0.12f, 0.35f);
    glVertex2f(-0.08f, 0.45f);
    glVertex2f(-0.02f, 0.30f);
    glVertex2f(0.04f, 0.25f);
    glVertex2f(0.10f, 0.05f);
    glVertex2f(0.13f, -0.25f);
    glVertex2f(0.12f, -0.90f);

    glEnd();


    glColor3f(0.32f, 0.20f, 0.16f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.08f, 0.30f);
    glVertex2f(0.00f, 0.15f);
    glVertex2f(-0.02f, -0.20f);

    glEnd();


    glColor3f(0.68f, 0.42f, 0.25f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.10f, 0.20f);
    glVertex2f(-0.04f, 0.05f);
    glVertex2f(-0.07f, -0.50f);

    glEnd();


    glBegin(GL_TRIANGLES);

    glVertex2f(0.02f, 0.18f);
    glVertex2f(0.08f, 0.00f);
    glVertex2f(0.08f, -0.55f);

    glEnd();


    glColor3f(1.0f, 0.35f, 0.05f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.045f, 0.34f);
    glVertex2f(-0.015f, 0.29f);
    glVertex2f(-0.04f, -0.30f);

    glEnd();


    glBegin(GL_TRIANGLES);

    glVertex2f(0.025f, 0.27f);
    glVertex2f(0.050f, 0.20f);
    glVertex2f(0.075f, -0.20f);

    glEnd();


    glBegin(GL_TRIANGLES);

    glVertex2f(-0.075f, 0.28f);
    glVertex2f(-0.055f, 0.20f);
    glVertex2f(-0.085f, -0.05f);

    glEnd();
}


// ==========================================
// DISPLAY
// ==========================================

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();


    // LEFT VOLCANO

    glPushMatrix();

    glTranslatef(-0.92f, -0.25f, 0.0f);

    drawVolcano();

    glPopMatrix();


    // RIGHT VOLCANO

    glPushMatrix();

    glTranslatef(0.92f, -0.25f, 0.0f);

    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    drawVolcano();

    glPopMatrix();


    // BOSS

    glPushMatrix();

    glTranslatef(0.0f, 0.48f, 0.0f);

    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);

    drawBossEnemyJet();

    glPopMatrix();


    // BOSS HEALTH BAR BACKGROUND

    glColor3f(0.3f, 0.3f, 0.3f);

    glBegin(GL_QUADS);

    glVertex2f(-0.40f, 0.93f);
    glVertex2f(0.40f, 0.93f);
    glVertex2f(0.40f, 0.88f);
    glVertex2f(-0.40f, 0.88f);

    glEnd();


    // BOSS HEALTH BAR

    float healthWidth = (bossHealth / (float)maxBossHealth) * 0.80f;

    glColor3f(1.0f, 0.1f, 0.1f);

    glBegin(GL_QUADS);

    glVertex2f(-0.40f, 0.93f);
    glVertex2f(-0.40f + healthWidth, 0.93f);
    glVertex2f(-0.40f + healthWidth, 0.88f);
    glVertex2f(-0.40f, 0.88f);

    glEnd();


    // ENEMY JETS

    for(int i = 0; i < 2; i++)
    {
        if(enemyAlive[i])
        {
            glPushMatrix();

            glTranslatef(enemyX[i], enemyY[i], 0.0f);

            glRotatef(180.0f, 0.0f, 0.0f, 1.0f);

            drawEnemyJet();

            glPopMatrix();
        }
    }


    // BOSS BULLETS

    glColor3f(1.0f, 0.35f, 0.05f);

    glLineWidth(5.0f);

    for(int i = 0; i < 4; i++)
    {
        if(bossBulletY[i] > -1.5f)
        {
            glBegin(GL_LINES);

            glVertex2f(bossBulletX[i], bossBulletY[i]);
            glVertex2f(bossBulletX[i], bossBulletY[i] - 0.08f);

            glEnd();
        }
    }


    // PLAYER

    glPushMatrix();

    glTranslatef(playerX, -0.80f, 0.0f);

    drawPlayerJet();


    // SHIELD VISUAL

    if(shieldActive)
    {
        glColor3f(0.0f, 0.8f, 1.0f);

        glLineWidth(3.0f);

        glBegin(GL_LINE_LOOP);

        glVertex2f(-0.25f, 0.15f);
        glVertex2f(0.25f, 0.15f);
        glVertex2f(0.28f, -0.15f);
        glVertex2f(-0.28f, -0.15f);

        glEnd();
    }

    glPopMatrix();


    // PLAYER BULLETS

    glColor3f(0.2f, 0.8f, 1.0f);

    glLineWidth(4.0f);


    glBegin(GL_LINES);

    glVertex2f(playerBulletX1, playerBulletY1);
    glVertex2f(playerBulletX1, playerBulletY1 + 0.10f);

    glEnd();


    glBegin(GL_LINES);

    glVertex2f(playerBulletX2, playerBulletY2);
    glVertex2f(playerBulletX2, playerBulletY2 + 0.10f);

    glEnd();


    // SHIELD POWERUP

    if(shieldDrop)
    {
        glPushMatrix();

        glTranslatef(shieldX, shieldY, 0.0f);

        drawshield();

        glPopMatrix();
    }


    // HEALTH POWERUP

    if(healthDrop)
    {
        glPushMatrix();

        glTranslatef(healthX, healthY, 0.0f);

        drawHealth();

        glPopMatrix();
    }


    // SCORE

    char scoreText[30];

    sprintf(scoreText, "SCORE: %d", score);

    glColor3f(1.0f, 1.0f, 1.0f);

    renderBitmapString(
        -0.95f,
        0.80f,
        0.0f,
        GLUT_BITMAP_HELVETICA_18,
        scoreText
    );


    // LIVES

    char lifeText[30];

    sprintf(lifeText, "LIVES: %d", lives);

    renderBitmapString(
        -0.95f,
        0.72f,
        0.0f,
        GLUT_BITMAP_HELVETICA_18,
        lifeText
    );


    // SHIELD TIMER

    if(shieldActive)
    {
        char shieldText[40];

        sprintf(shieldText, "SHIELD: %d", shieldTime / 10);

        glColor3f(0.0f, 0.8f, 1.0f);

        renderBitmapString(
            0.60f,
            0.80f,
            0.0f,
            GLUT_BITMAP_HELVETICA_18,
            shieldText
        );
    }


    // GAME OVER

    if(gameOver)
    {
        glColor3f(1.0f, 0.0f, 0.0f);

        renderBitmapString(
            -0.20f,
            0.0f,
            0.0f,
            GLUT_BITMAP_TIMES_ROMAN_24,
            "GAME OVER"
        );
    }


    // WIN

    if(gameWin)
    {
        glColor3f(0.0f, 1.0f, 0.3f);

        renderBitmapString(
            -0.20f,
            0.10f,
            0.0f,
            GLUT_BITMAP_TIMES_ROMAN_24,
            "YOU WIN!"
        );

        renderBitmapString(
            -0.25f,
            0.0f,
            0.0f,
            GLUT_BITMAP_TIMES_ROMAN_24,
            "GALAXY SAVED!"
        );
    }


    glFlush();
}


// ==========================================
// ARROW KEY CONTROL
// ==========================================

void SpecialInput(int key, int x, int y)
{
    if(gameOver || gameWin)
        return;

    if(key == GLUT_KEY_LEFT)
        keyLeft = true;

    if(key == GLUT_KEY_RIGHT)
        keyRight = true;
}


// ==========================================
// SPECIAL UP
// ==========================================

void SpecialUp(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT)
        keyLeft = false;

    if(key == GLUT_KEY_RIGHT)
        keyRight = false;
}


// ==========================================
// KEYBOARD INPUT (A / D)
// ==========================================

void handleKeypress(unsigned char key, int x, int y)
{
    if(gameOver || gameWin)
        return;

    if(key == 'a' || key == 'A')
        keyLeft = true;

    if(key == 'd' || key == 'D')
        keyRight = true;
}


// ==========================================
// KEYBOARD UP
// ==========================================

void handleKeyUp(unsigned char key, int x, int y)
{
    if(key == 'a' || key == 'A')
        keyLeft = false;

    if(key == 'd' || key == 'D')
        keyRight = false;
}

// ==========================================
// GAME UPDATE
// ==========================================

void update(int value)
{
    static int frame = 0;
    frame++;

    if(frame % 6 != 0)
    {
        return;
    }

    if(!gameOver && !gameWin)
    {
        // =================================
        // PLAYER MOVEMENT (HELD KEYS)
        // =================================

        if(keyLeft)
        {
            playerX -= 0.05f;

            if(playerX < -0.75f)
                playerX = -0.75f;
        }

        if(keyRight)
        {
            playerX += 0.05f;

            if(playerX > 0.75f)
                playerX = 0.75f;
        }

        // =================================
        // PLAYER BULLETS
        // =================================

        playerBulletX1 = playerX - 0.035f;
        playerBulletX2 = playerX + 0.035f;

        playerBulletY1 += 0.035f;
        playerBulletY2 += 0.035f;


        if(playerBulletY1 > 1.0f)
        {
            playerBulletY1 = -0.55f;
        }


        if(playerBulletY2 > 1.0f)
        {
            playerBulletY2 = -0.55f;
        }


        // =================================
        // BOSS BULLETS
        // SLOW SPEED
        // =================================

        for(int i = 0; i < 4; i++)
        {
            bossBulletY[i] -= 0.008f;

            if(bossBulletY[i] < -1.0f)
            {
                bossBulletY[i] = 0.30f;
            }
        }


        // =================================
        // ENEMY MOVEMENT
        // =================================

        for(int i = 0; i < 2; i++)
        {
            if(enemyAlive[i])
            {
                enemyY[i] -= 0.012f;

                if(enemyY[i] < -1.1f)
                {
                    enemyY[i] = 1.0f;

                    enemyX[i] =
                        ((rand() % 140) - 70) / 100.0f;
                }
            }
        }


        // =================================
        // POWER UP MOVEMENT
        // =================================

        if(shieldDrop)
        {
            shieldY -= 0.015f;

            if(shieldY < -1.0f)
            {
                shieldDrop = false;
            }
        }


        if(healthDrop)
        {
            healthY -= 0.015f;

            if(healthY < -1.0f)
            {
                healthDrop = false;
            }
        }


        // =================================
        // BOSS HIT
        // =================================

        if(playerBulletY1 > 0.20f &&
           playerBulletY1 < 0.75f &&
           playerBulletX1 > -0.38f &&
           playerBulletX1 < 0.38f)
        {
            bossHealth--;
            score++;

            playerBulletY1 = -0.55f;
        }


        if(playerBulletY2 > 0.20f &&
           playerBulletY2 < 0.75f &&
           playerBulletX2 > -0.38f &&
           playerBulletX2 < 0.38f)
        {
            bossHealth--;
            score++;

            playerBulletY2 = -0.55f;
        }


        // =================================
        // BOSS BULLET VS PLAYER BULLET
        // =================================

        for(int i = 0; i < 4; i++)
        {
            if(playerBulletY1 > bossBulletY[i] - 0.08f &&
               playerBulletY1 < bossBulletY[i] + 0.08f &&
               playerBulletX1 > bossBulletX[i] - 0.05f &&
               playerBulletX1 < bossBulletX[i] + 0.05f)
            {
                bossBulletY[i] = -2.0f;
                playerBulletY1 = -0.55f;
            }


            if(playerBulletY2 > bossBulletY[i] - 0.08f &&
               playerBulletY2 < bossBulletY[i] + 0.08f &&
               playerBulletX2 > bossBulletX[i] - 0.05f &&
               playerBulletX2 < bossBulletX[i] + 0.05f)
            {
                bossBulletY[i] = -2.0f;
                playerBulletY2 = -0.55f;
            }
        }


        // =================================
        // ENEMY HIT BY PLAYER BULLET
        // =================================

        for(int i = 0; i < 2; i++)
        {
            if(enemyAlive[i])
            {
                if(playerBulletY1 > enemyY[i] - 0.15f &&
                   playerBulletY1 < enemyY[i] + 0.15f &&
                   playerBulletX1 > enemyX[i] - 0.20f &&
                   playerBulletX1 < enemyX[i] + 0.20f)
                {
                    enemyAlive[i] = false;

                    score += 5;

                    playerBulletY1 = -0.55f;


                    int randomDrop = rand() % 4;

                    if(randomDrop == 0)
                    {
                        shieldDrop = true;

                        shieldX = enemyX[i];
                        shieldY = enemyY[i];
                    }

                    if(randomDrop == 1)
                    {
                        healthDrop = true;

                        healthX = enemyX[i];
                        healthY = enemyY[i];
                    }
                }


                if(playerBulletY2 > enemyY[i] - 0.15f &&
                   playerBulletY2 < enemyY[i] + 0.15f &&
                   playerBulletX2 > enemyX[i] - 0.20f &&
                   playerBulletX2 < enemyX[i] + 0.20f)
                {
                    enemyAlive[i] = false;

                    score += 5;

                    playerBulletY2 = -0.55f;


                    int randomDrop = rand() % 4;

                    if(randomDrop == 0)
                    {
                        shieldDrop = true;

                        shieldX = enemyX[i];
                        shieldY = enemyY[i];
                    }

                    if(randomDrop == 1)
                    {
                        healthDrop = true;

                        healthX = enemyX[i];
                        healthY = enemyY[i];
                    }
                }
            }
        }


        // =================================
        // RESPAWN ENEMIES
        // =================================

        for(int i = 0; i < 2; i++)
        {
            if(!enemyAlive[i])
            {
                enemyAlive[i] = true;

                enemyY[i] = 1.0f + (rand() % 30) / 100.0f;

                enemyX[i] =
                    ((rand() % 140) - 70) / 100.0f;
            }
        }


        // =================================
        // PLAYER HIT BY BOSS BULLET
        // =================================

        if(!shieldActive)
        {
            for(int i = 0; i < 4; i++)
            {
                if(bossBulletY[i] < -0.60f &&
                   bossBulletY[i] > -0.95f &&
                   bossBulletX[i] > playerX - 0.18f &&
                   bossBulletX[i] < playerX + 0.18f)
                {
                    lives--;

                    bossBulletY[i] = -2.0f;
                }
            }
        }


        // =================================
        // PLAYER HIT BY ENEMY
        // =================================

        if(!shieldActive)
        {
            for(int i = 0; i < 2; i++)
            {
                if(enemyY[i] < -0.60f &&
                   enemyY[i] > -0.95f &&
                   enemyX[i] > playerX - 0.20f &&
                   enemyX[i] < playerX + 0.20f)
                {
                    lives--;

                    enemyY[i] = 1.0f;
                }
            }
        }


        // =================================
        // COLLECT SHIELD
        // =================================

        if(shieldDrop)
        {
            if(shieldY < -0.65f &&
               shieldY > -0.95f &&
               shieldX > playerX - 0.20f &&
               shieldX < playerX + 0.20f)
            {
                shieldActive = true;

                shieldTime = 100;

                shieldDrop = false;
            }
        }


        // =================================
        // COLLECT HEALTH
        // =================================

        if(healthDrop)
        {
            if(healthY < -0.65f &&
               healthY > -0.95f &&
               healthX > playerX - 0.20f &&
               healthX < playerX + 0.20f)
            {
                if(lives < 3)
                {
                    lives++;
                }

                healthDrop = false;
            }
        }


        // =================================
        // SHIELD TIMER
        // =================================

        if(shieldActive)
        {
            shieldTime--;

            if(shieldTime <= 0)
            {
                shieldActive = false;
            }
        }


        // =================================
        // GAME OVER
        // =================================

        if(lives <= 0)
        {
            gameOver = true;
        }


        // =================================
        // BOSS DEFEATED
        // =================================

        if(bossHealth <= 0)
        {
            bossHealth = 0;

            gameWin = true;
        }
    }


    glutPostRedisplay();
}


// ==========================================
// RESET LEVEL
// ==========================================

void resetLevel()
{
    playerX = 0.0f;

    score = 0;

    lives = 3;

    bossHealth = 40;

    shieldActive = false;

    shieldTime = 0;

    playerBulletX1 = -0.035f;

    playerBulletY1 = -0.6f;

    playerBulletX2 = 0.035f;

    playerBulletY2 = -0.6f;

    bossBulletX[0] = -0.28f;
    bossBulletX[1] = -0.14f;
    bossBulletX[2] = 0.14f;
    bossBulletX[3] = 0.28f;

    bossBulletY[0] = 0.20f;
    bossBulletY[1] = 0.20f;
    bossBulletY[2] = 0.20f;
    bossBulletY[3] = 0.20f;

    enemyX[0] = -0.45f;
    enemyX[1] = 0.45f;

    enemyY[0] = 0.70f;
    enemyY[1] = 0.95f;

    enemyAlive[0] = true;
    enemyAlive[1] = true;

    shieldX = 0.0f;
    shieldY = -2.0f;
    shieldDrop = false;

    healthX = 0.0f;
    healthY = -2.0f;
    healthDrop = false;

    gameOver = false;
    gameWin = false;
}


// ==========================================
// HIGH SCORE SUPPORT
// ==========================================

int getScore()
{
    return score;
}

bool isFinished()
{
    return gameOver || gameWin;
}

} // namespace L5
