#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace L1
{
    // =====================================================
    // GAME VARIABLES
    // =====================================================

    // PLAYER
    float playerX = 0.0f;
    float playerY = -0.80f;
    float playerSpeed = 0.025f;

    int playerLives = 3;
    bool playerAlive = true;

    // held-key movement state (only horizontal: A/D + left/right arrows)
    bool keyLeft = false;
    bool keyRight = false;

    // =====================================================
    // PLAYER BULLET
    // =====================================================

    float bulletX1 = 0.0f;
    float bulletX2 = 0.0f;
    float bulletY = -0.52f;
    float bulletSpeed = 0.04f;

    bool bulletActive = false;

    // =====================================================
    // ENEMY PLANES
    // =====================================================

    float enemyX[6] =
    {
        -0.80f,
        -0.48f,
        -0.16f,
         0.16f,
         0.48f,
         0.80f
    };

    // Each enemy has its own Y position
    float enemyY[6] =
    {
        0.82f,
        0.96f,
        0.74f,
        0.90f,
        0.78f,
        1.02f
    };

    // Each enemy has a different falling speed
    float enemySpeed[6] =
    {
        0.0025f,
        0.0038f,
        0.0030f,
        0.0042f,
        0.0028f,
        0.0036f
    };

    bool enemyAlive[6] =
    {
        true, true, true,
        true, true, true
    };

    // =====================================================
    // ENEMY BULLETS
    // =====================================================

    float enemyBulletX[6] =
    {
        0, 0, 0, 0, 0, 0
    };

    float enemyBulletY[6] =
    {
        0, 0, 0, 0, 0, 0
    };

    bool enemyBulletActive[6] =
    {
        false, false, false,
        false, false, false
    };

    int enemyBulletTimer[6] =
    {
        0, 0, 0,
        0, 0, 0
    };

    float enemyBulletSpeed = 0.025f;

    // =====================================================
    // SCORE
    // =====================================================

    int score = 0;


    // =====================================================
    // RESET LEVEL
    // =====================================================

    void resetLevel()
    {
        // PLAYER
        playerX = 0.0f;
        playerY = -0.80f;

        playerLives = 3;
        playerAlive = true;

        // PLAYER BULLET
        bulletX1 = 0.0f;
        bulletX2 = 0.0f;
        bulletY = -0.52f;
        bulletActive = false;

        // SCORE
        score = 0;

        // =================================================
        // RESET ENEMIES
        // =================================================

        enemyY[0] = 0.82f;
        enemyY[1] = 0.96f;
        enemyY[2] = 0.74f;
        enemyY[3] = 0.90f;
        enemyY[4] = 0.78f;
        enemyY[5] = 1.02f;

        enemySpeed[0] = 0.0025f;
        enemySpeed[1] = 0.0038f;
        enemySpeed[2] = 0.0030f;
        enemySpeed[3] = 0.0042f;
        enemySpeed[4] = 0.0028f;
        enemySpeed[5] = 0.0036f;

        for(int i = 0; i < 6; i++)
        {
            enemyAlive[i] = true;

            enemyBulletX[i] = 0.0f;
            enemyBulletY[i] = 0.0f;
            enemyBulletActive[i] = false;

            enemyBulletTimer[i] = 30 + rand() % 100;
        }
    }


    // =====================================================
    // DRAW ENEMY PLANE
    // =====================================================

    void drawEnemy(float x, float y)
    {
        glPushMatrix();

        glTranslatef(x, y, 0.0f);

        // Enemy points downward
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);

        // Main body
        glColor3f(0.55f, 0.58f, 0.62f);

        glBegin(GL_POLYGON);

            glVertex2f(0.000f, 0.18f);
            glVertex2f(-0.035f, 0.04f);
            glVertex2f(-0.030f, -0.11f);
            glVertex2f(0.030f, -0.11f);
            glVertex2f(0.035f, 0.04f);

        glEnd();


        // Left wing
        glColor3f(0.42f, 0.45f, 0.49f);

        glBegin(GL_TRIANGLES);

            glVertex2f(-0.025f, 0.04f);
            glVertex2f(-0.14f, 0.08f);
            glVertex2f(-0.035f, -0.05f);

        glEnd();


        // Right wing
        glBegin(GL_TRIANGLES);

            glVertex2f(0.025f, 0.04f);
            glVertex2f(0.14f, 0.08f);
            glVertex2f(0.035f, -0.05f);

        glEnd();


        // Cockpit
        glColor3f(0.20f, 0.22f, 0.26f);

        glBegin(GL_QUADS);

            glVertex2f(-0.015f, 0.04f);
            glVertex2f(0.015f, 0.04f);
            glVertex2f(0.015f, 0.09f);
            glVertex2f(-0.015f, 0.09f);

        glEnd();

        glPopMatrix();
    }


    // =====================================================
    // DRAW PLAYER PLANE
    // =====================================================

    void drawPlayer()
    {
        glPushMatrix();

        glTranslatef(playerX, playerY, 0.0f);

        // Main fuselage
        glColor3f(0.82f, 0.85f, 0.88f);

        glBegin(GL_POLYGON);

            glVertex2f(0.000f, 0.180f);
            glVertex2f(-0.035f, 0.040f);
            glVertex2f(-0.030f, -0.110f);
            glVertex2f(0.030f, -0.110f);
            glVertex2f(0.035f, 0.040f);

        glEnd();


        // Center strip
        glColor3f(0.55f, 0.60f, 0.66f);

        glBegin(GL_QUADS);

            glVertex2f(-0.012f, -0.110f);
            glVertex2f(0.012f, -0.110f);
            glVertex2f(0.012f, 0.120f);
            glVertex2f(-0.012f, 0.120f);

        glEnd();


        // Left wing
        glColor3f(0.70f, 0.74f, 0.78f);

        glBegin(GL_TRIANGLES);

            glVertex2f(-0.030f, -0.012f);
            glVertex2f(-0.200f, -0.085f);
            glVertex2f(-0.048f, -0.110f);

        glEnd();


        // Right wing
        glBegin(GL_TRIANGLES);

            glVertex2f(0.030f, -0.012f);
            glVertex2f(0.200f, -0.085f);
            glVertex2f(0.048f, -0.110f);

        glEnd();


        // Cockpit
        glColor3f(0.15f, 0.35f, 0.55f);

        glBegin(GL_TRIANGLES);

            glVertex2f(0.000f, 0.105f);
            glVertex2f(-0.025f, 0.055f);
            glVertex2f(0.025f, 0.055f);

        glEnd();


        // Engine
        glColor3f(0.3f, 0.6f, 1.0f);

        glBegin(GL_TRIANGLES);

            glVertex2f(-0.018f, -0.110f);
            glVertex2f(0.018f, -0.110f);
            glVertex2f(0.000f, -0.160f);

        glEnd();

        glPopMatrix();
    }


    // =====================================================
    // DRAW TEXT
    // =====================================================

    void drawText(float x, float y, const char *text)
    {
        glRasterPos2f(x, y);

        for(const char *c = text; *c != '\0'; c++)
        {
            glutBitmapCharacter(
                GLUT_BITMAP_HELVETICA_18,
                *c
            );
        }
    }


    // =====================================================
    // DRAW BACKGROUND
    // =====================================================

    void drawBackground()
    {
        // Black background
        glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_QUADS);

            glVertex2f(-1.0f, -1.0f);
            glVertex2f( 1.0f, -1.0f);
            glVertex2f( 1.0f,  1.0f);
            glVertex2f(-1.0f,  1.0f);

        glEnd();


        // Rock
        glColor3f(0.58f, 0.42f, 0.38f);

        glBegin(GL_POLYGON);

            glVertex2f(-1.0f, 1.0f);
            glVertex2f(-0.70f, 1.0f);
            glVertex2f(-0.44f, 0.95f);
            glVertex2f(-0.24f, 0.90f);
            glVertex2f(-0.52f, 0.80f);
            glVertex2f(-0.48f, 0.70f);
            glVertex2f(-0.60f, 0.60f);
            glVertex2f(-0.70f, 0.45f);
            glVertex2f(-0.80f, 0.25f);
            glVertex2f(-0.90f, 0.10f);
            glVertex2f(-0.90f, -0.20f);
            glVertex2f(-0.95f, -0.50f);
            glVertex2f(-1.0f, -0.90f);

        glEnd();
    }


    // =====================================================
    // DISPLAY
    // =====================================================

    void display()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glLoadIdentity();

        // Background
        drawBackground();


        // =================================================
        // ENEMY PLANES
        // =================================================

        for(int i = 0; i < 6; i++)
        {
            if(enemyAlive[i])
            {
                drawEnemy(enemyX[i], enemyY[i]);
            }
        }


        // =================================================
        // PLAYER BULLETS
        // =================================================

        if(bulletActive)
        {
            glColor3f(0.2f, 0.8f, 1.0f);

            glLineWidth(4.0f);

            // Left bullet
            glBegin(GL_LINES);

                glVertex2f(
                    bulletX1,
                    bulletY
                );

                glVertex2f(
                    bulletX1,
                    bulletY + 0.14f
                );

            glEnd();


            // Right bullet
            glBegin(GL_LINES);

                glVertex2f(
                    bulletX2,
                    bulletY
                );

                glVertex2f(
                    bulletX2,
                    bulletY + 0.14f
                );

            glEnd();

            glLineWidth(1.0f);
        }


        // =================================================
        // ENEMY BULLETS
        // =================================================

        glColor3f(1.0f, 0.0f, 0.0f);

        glLineWidth(5.0f);

        for(int i = 0; i < 6; i++)
        {
            if(!enemyBulletActive[i])
                continue;

            glBegin(GL_LINES);

                glVertex2f(
                    enemyBulletX[i],
                    enemyBulletY[i]
                );

                glVertex2f(
                    enemyBulletX[i],
                    enemyBulletY[i] - 0.12f
                );

            glEnd();
        }

        glLineWidth(1.0f);


        // =================================================
        // PLAYER
        // =================================================

        if(playerAlive)
        {
            drawPlayer();
        }


        // =================================================
        // SCORE / LIVES
        // =================================================

        glColor3f(1.0f, 1.0f, 1.0f);

        char lifeText[30];
        char scoreText[30];

        sprintf(
            lifeText,
            "LIVES: %d",
            playerLives
        );

        sprintf(
            scoreText,
            "SCORE: %d",
            score
        );

        drawText(
            -0.95f,
            0.90f,
            lifeText
        );

        drawText(
            0.55f,
            0.90f,
            scoreText
        );


        // =================================================
        // WIN
        // =================================================

        bool allDead = true;

        for(int i = 0; i < 6; i++)
        {
            if(enemyAlive[i])
            {
                allDead = false;
                break;
            }
        }

        if(allDead)
        {
            glColor3f(
                0.2f,
                1.0f,
                0.2f
            );

            drawText(
                -0.25f,
                0.0f,
                "ALL ENEMIES DESTROYED!"
            );

            glColor3f(
                1.0f,
                1.0f,
                1.0f
            );

            drawText(
                -0.18f,
                -0.08f,
                "PRESS R TO RESTART"
            );
        }


        // =================================================
        // GAME OVER
        // =================================================

        if(playerLives <= 0)
        {
            glColor3f(
                1.0f,
                0.1f,
                0.1f
            );

            drawText(
                -0.15f,
                0.0f,
                "GAME OVER"
            );

            glColor3f(
                1.0f,
                1.0f,
                1.0f
            );

            drawText(
                -0.18f,
                -0.08f,
                "PRESS R TO RESTART"
            );
        }
    }


    // =====================================================
// UPDATE
// =====================================================

void update(int value)
{
    // =================================================
    // CHECK GAME STATE
    // =================================================

    bool allDead = true;

    for(int i = 0; i < 6; i++)
    {
        if(enemyAlive[i])
        {
            allDead = false;
            break;
        }
    }

    if(allDead || playerLives <= 0)
    {
        bulletActive = false;

        // Make sure no enemy bullets are active
        for(int i = 0; i < 6; i++)
        {
            enemyBulletActive[i] = false;
        }

        glutPostRedisplay();

        // Do NOT call glutTimerFunc here.
        // main.cpp already handles the global timer.
        return;
    }


    // =================================================
    // PLAYER MOVEMENT (HELD KEYS)
    // =================================================

    if(keyLeft)  playerX -= playerSpeed;
    if(keyRight) playerX += playerSpeed;

    if(playerX < -0.80f)
        playerX = -0.80f;

    if(playerX > 0.80f)
        playerX = 0.80f;


    // =================================================
    // MOVE EACH ENEMY INDEPENDENTLY
    // =================================================

    for(int i = 0; i < 6; i++)
    {
        if(enemyAlive[i])
        {
            enemyY[i] -= enemySpeed[i];

            // Enemy reaches player area
            if(enemyY[i] < -0.65f)
            {
                playerLives = 0;
                playerAlive = false;
            }
        }
    }


    // =================================================
    // PLAYER BULLET MOVEMENT
    // =================================================

    if(bulletActive)
    {
        bulletY += bulletSpeed;


        // =================================================
        // BULLET COLLISION
        // =================================================

        for(int i = 0; i < 6; i++)
        {
            if(!enemyAlive[i])
                continue;


            float eX = enemyX[i];
            float eY = enemyY[i];


            bool hit1 =
                bulletX1 >= eX - 0.14f &&
                bulletX1 <= eX + 0.14f &&
                bulletY >= eY - 0.18f &&
                bulletY <= eY + 0.18f;


            bool hit2 =
                bulletX2 >= eX - 0.14f &&
                bulletX2 <= eX + 0.14f &&
                bulletY >= eY - 0.18f &&
                bulletY <= eY + 0.18f;


            if(hit1 || hit2)
            {
                enemyAlive[i] = false;

                bulletActive = false;

                score += 10;

                break;
            }
        }


        // Bullet leaves screen
        if(bulletY > 1.0f)
        {
            bulletActive = false;
        }
    }


    // =================================================
    // ENEMY SHOOTING DISABLED
    // =================================================
    //
    // Enemy bullets are intentionally disabled.
    // Enemies will NEVER fire at the player.
    //

    for(int i = 0; i < 6; i++)
    {
        enemyBulletActive[i] = false;
    }


    // =================================================
    // REDRAW
    // =================================================

    glutPostRedisplay();
}


    // =====================================================
    // KEYBOARD INPUT
    // =====================================================

    void handleKeypress(unsigned char key, int x, int y)
    {
        // =================================================
        // R = RESTART
        // =================================================

        if(key == 'r' || key == 'R')
        {
            resetLevel();

            glutPostRedisplay();

            return;
        }


        // =================================================
        // SPACE = FIRE
        // =================================================

        if(
            key == ' ' &&
            playerAlive &&
            !bulletActive
        )
        {
            // Two bullets
            bulletX1 = playerX - 0.035f;
            bulletX2 = playerX + 0.035f;

            bulletY = playerY + 0.28f;

            bulletActive = true;
        }


        // =================================================
        // A/D = MOVEMENT (HELD)
        // =================================================

        if(key == 'a' || key == 'A') keyLeft = true;
        if(key == 'd' || key == 'D') keyRight = true;


        glutPostRedisplay();
    }


    // =====================================================
    // KEYBOARD UP
    // =====================================================

    void handleKeyUp(unsigned char key, int x, int y)
    {
        if(key == 'a' || key == 'A') keyLeft = false;
        if(key == 'd' || key == 'D') keyRight = false;
    }


    // =====================================================
    // SPECIAL INPUT
    // ARROW KEYS
    // =====================================================

    void SpecialInput(int key, int x, int y)
    {
        if(!playerAlive)
            return;


        switch(key)
        {
            case GLUT_KEY_LEFT:  keyLeft = true;  break;
            case GLUT_KEY_RIGHT: keyRight = true; break;
        }


        glutPostRedisplay();
    }


    // =====================================================
    // SPECIAL UP
    // =====================================================

    void SpecialUp(int key, int x, int y)
    {
        switch(key)
        {
            case GLUT_KEY_LEFT:  keyLeft = false;  break;
            case GLUT_KEY_RIGHT: keyRight = false; break;
        }
    }


    // =====================================================
    // HIGH SCORE SUPPORT
    // =====================================================

    int getScore()
    {
        return score;
    }

    bool isFinished()
    {
        if(playerLives <= 0)
            return true;

        for(int i = 0; i < 6; i++)
            if(enemyAlive[i])
                return false;

        return true; // all enemies destroyed
    }
}
