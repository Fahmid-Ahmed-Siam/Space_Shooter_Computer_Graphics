#include <windows.h>
#include <GL/glut.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace L1
{
    void display();
    void update(int value);
    void resetLevel();
    void handleKeypress(unsigned char key, int x, int y);
    void handleKeyUp(unsigned char key, int x, int y);
    void SpecialInput(int key, int x, int y);
    void SpecialUp(int key, int x, int y);
    int getScore();
    bool isFinished();
}

namespace L2 {
    void display();
    void update(int value);
    void resetLevel();
    void handleKeypress(unsigned char key, int x, int y);
    void handleKeyUp(unsigned char key, int x, int y);
    void SpecialInput(int key, int x, int y);
    void SpecialUp(int key, int x, int y);
    int getScore();
    bool isFinished();
}

namespace L3 {
    void display();
    void update(int value);
    void resetLevel();
    void handleKeypress(unsigned char key, int x, int y);
    void handleKeyUp(unsigned char key, int x, int y);
    void SpecialInput(int key, int x, int y);
    void SpecialUp(int key, int x, int y);
    int getScore();
    bool isFinished();
}

namespace L4 {
    void display();
    void update(int value);
    void resetGame();
    void handleKeypress(unsigned char key, int x, int y);
    void handleKeyUp(unsigned char key, int x, int y);
    void SpecialInput(int key, int x, int y);
    void SpecialUp(int key, int x, int y);
    int getScore();
    bool isFinished();
}

namespace L5 {
    void display();
    void update(int value);
    void resetLevel();
    void handleKeypress(unsigned char key, int x, int y);
    void handleKeyUp(unsigned char key, int x, int y);
    void SpecialInput(int key, int x, int y);
    void SpecialUp(int key, int x, int y);
    int getScore();
    bool isFinished();
}

enum Scene { MENU = 0, LEVEL_1 = 1, LEVEL_2 = 2, LEVEL_3 = 3, LEVEL_4 = 4, LEVEL_5 = 5, LOADING = 10, SETTINGS = 20, INSTRUCTIONS = 30, HIGHSCORES = 40 };

int currentScene = LOADING;

const int NUM_LEVELS = 5;
const float BTN_W = 0.5f;
const float BTN_H = 0.09f;
const float btnCenters[NUM_LEVELS] = { 0.30f, 0.14f, -0.02f, -0.18f, -0.34f };

// extra home buttons (settings / instructions / high scores)
const float SETTINGS_BTN_CY = -0.50f;
const float INSTRUCTIONS_BTN_CY = -0.62f;
const float HIGHSCORES_BTN_CY = -0.74f;

// music control
bool muted = false;

void toggleMusic(bool on);

// ---- high scores (stored in highscores.txt next to the exe) ----
int highScores[NUM_LEVELS][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
bool recorded[NUM_LEVELS] = { false, false, false, false, false };

void loadHighScores();
void saveHighScores();
void recordScore(int level, int score);

// loading screen texture
GLuint loadingTexture = 0;
int loadingTexW = 0;
int loadingTexH = 0;

void renderText(float x, float y, void *font, const char *s) {
    glRasterPos2f(x, y);
    for (const char *c = s; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

// load a 32-bit (BGRA, non-padded) .bmp file into an OpenGL texture
int loadBMPTexture(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54) { fclose(f); return 0; }
    if (header[0] != 'B' || header[1] != 'M') { fclose(f); return 0; }

    int dataOffset = header[10] | (header[11] << 8) | (header[12] << 16) | (header[13] << 24);
    int width  = header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
    int height = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
    int bpp = header[28] | (header[29] << 8);

    if (bpp != 32) { fclose(f); return 0; }

    int imgBytes = width * height * 4;
    unsigned char *pixels = new unsigned char[imgBytes];
    fseek(f, dataOffset, SEEK_SET);
    if (fread(pixels, 1, imgBytes, f) != (size_t)imgBytes) {
        delete[] pixels; fclose(f); return 0;
    }
    fclose(f);

    // BMP is bottom-up: flip rows for OpenGL. Also convert BGRA -> RGBA.
    unsigned char *flipped = new unsigned char[imgBytes];
    int rowBytes = width * 4;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src = ((height - 1 - y) * width + x) * 4;
            int dst = (y * width + x) * 4;
            flipped[dst + 0] = pixels[src + 2];
            flipped[dst + 1] = pixels[src + 1];
            flipped[dst + 2] = pixels[src + 0];
            flipped[dst + 3] = pixels[src + 3];
        }
    }

    glGenTextures(1, &loadingTexture);
    glBindTexture(GL_TEXTURE_2D, loadingTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped);

    loadingTexW = width;
    loadingTexH = height;

    delete[] pixels;
    delete[] flipped;
    return 1;
}

void drawLoading() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (loadingTexture && loadingTexW > 0 && loadingTexH > 0) {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        // fit the (portrait) image to the full window height, centered,
        // preserving aspect ratio. Account for the window's own aspect
        // ratio so it is not stretched on screen.
        float imgAspect = (float)loadingTexH / (float)loadingTexW;
        float halfH = 1.0f;
        float halfW = (1.0f / imgAspect) * ((float)h / (float)w);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, loadingTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfW, -halfH);
            glTexCoord2f(1.0f, 1.0f); glVertex2f( halfW, -halfH);
            glTexCoord2f(1.0f, 0.0f); glVertex2f( halfW,  halfH);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfW,  halfH);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.3f, 0.3f, 0.3f);
        renderText(-0.10f, 0.0f, GLUT_BITMAP_HELVETICA_18, "LOADING...");
    }

    glFlush();
}

void loadingDone(int value) {
    if (currentScene == LOADING) {
        currentScene = MENU;
        glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
        glutSetWindowTitle("Space Shooter - Home");
        glutPostRedisplay();
    }
}

void drawMenu() {
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.9f, 0.9f, 0.2f);
    renderText(-0.24f, 0.80f, GLUT_BITMAP_TIMES_ROMAN_24, "SPACE SHOOTER");

    glColor3f(0.85f, 0.85f, 0.85f);
    renderText(-0.15f, 0.58f, GLUT_BITMAP_HELVETICA_18, "SELECT LEVEL");

    float colors[NUM_LEVELS][3] = {
        { 1.0f, 0.20f, 0.20f },
        { 1.0f, 0.60f, 0.10f },
        { 0.90f, 0.90f, 0.20f },
        { 0.20f, 0.60f, 1.00f },
        { 0.70f, 0.30f, 1.00f }
    };

    for (int i = 0; i < NUM_LEVELS; i++) {
        float cy = btnCenters[i];

        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glBegin(GL_QUADS);
        glVertex2f(-BTN_W / 2, cy - BTN_H / 2);
        glVertex2f( BTN_W / 2, cy - BTN_H / 2);
        glVertex2f( BTN_W / 2, cy + BTN_H / 2);
        glVertex2f(-BTN_W / 2, cy + BTN_H / 2);
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
        char label[16];
        sprintf(label, "LEVEL %d", i + 1);
        renderText(-0.06f, cy - 0.03f, GLUT_BITMAP_HELVETICA_18, label);
    }

    // SETTINGS button
    glColor3f(0.40f, 0.40f, 0.42f);
    glBegin(GL_QUADS);
        glVertex2f(-BTN_W / 2, SETTINGS_BTN_CY - BTN_H / 2);
        glVertex2f( BTN_W / 2, SETTINGS_BTN_CY - BTN_H / 2);
        glVertex2f( BTN_W / 2, SETTINGS_BTN_CY + BTN_H / 2);
        glVertex2f(-BTN_W / 2, SETTINGS_BTN_CY + BTN_H / 2);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(-0.07f, SETTINGS_BTN_CY - 0.03f, GLUT_BITMAP_HELVETICA_18, "SETTINGS");

    // INSTRUCTIONS button
    glColor3f(0.40f, 0.40f, 0.42f);
    glBegin(GL_QUADS);
        glVertex2f(-BTN_W / 2, INSTRUCTIONS_BTN_CY - BTN_H / 2);
        glVertex2f( BTN_W / 2, INSTRUCTIONS_BTN_CY - BTN_H / 2);
        glVertex2f( BTN_W / 2, INSTRUCTIONS_BTN_CY + BTN_H / 2);
        glVertex2f(-BTN_W / 2, INSTRUCTIONS_BTN_CY + BTN_H / 2);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(-0.13f, INSTRUCTIONS_BTN_CY - 0.03f, GLUT_BITMAP_HELVETICA_18, "INSTRUCTIONS");

    // HIGH SCORES button
    glColor3f(0.40f, 0.40f, 0.42f);
    glBegin(GL_QUADS);
        glVertex2f(-BTN_W / 2, HIGHSCORES_BTN_CY - BTN_H / 2);
        glVertex2f( BTN_W / 2, HIGHSCORES_BTN_CY - BTN_H / 2);
        glVertex2f( BTN_W / 2, HIGHSCORES_BTN_CY + BTN_H / 2);
        glVertex2f(-BTN_W / 2, HIGHSCORES_BTN_CY + BTN_H / 2);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(-0.13f, HIGHSCORES_BTN_CY - 0.03f, GLUT_BITMAP_HELVETICA_18, "HIGH SCORES");

    glColor3f(0.7f, 0.7f, 0.7f);
    renderText(-0.58f, -0.86f, GLUT_BITMAP_HELVETICA_18, "CLICK A BUTTON OR PRESS 1-5     ESC RETURNS TO MENU");

    glFlush();
}

void drawSettings() {
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.9f, 0.9f, 0.2f);
    renderText(-0.15f, 0.80f, GLUT_BITMAP_TIMES_ROMAN_24, "SETTINGS");

    glColor3f(0.85f, 0.85f, 0.85f);
    renderText(-0.22f, 0.55f, GLUT_BITMAP_HELVETICA_18, "MUSIC");

    // large mute / unmute button
    float w = 0.7f;
    float h = 0.35f;
    float cy = 0.0f;
    float cx = 0.0f;

    if (muted)
        glColor3f(0.75f, 0.20f, 0.20f);
    else
        glColor3f(0.20f, 0.70f, 0.30f);

    glBegin(GL_QUADS);
        glVertex2f(cx - w / 2, cy - h / 2);
        glVertex2f(cx + w / 2, cy - h / 2);
        glVertex2f(cx + w / 2, cy + h / 2);
        glVertex2f(cx - w / 2, cy + h / 2);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    if (muted)
        renderText(cx - 0.16f, cy - 0.04f, GLUT_BITMAP_HELVETICA_18, "UNMUTE");
    else
        renderText(cx - 0.12f, cy - 0.04f, GLUT_BITMAP_HELVETICA_18, "MUTE");

    glColor3f(0.7f, 0.7f, 0.7f);
    renderText(-0.35f, -0.45f, GLUT_BITMAP_HELVETICA_18, "CLICK TO MUTE / UNMUTE MUSIC");
    renderText(-0.38f, -0.70f, GLUT_BITMAP_HELVETICA_18, "ESC RETURNS TO MENU");

    glFlush();
}

void drawInstructions() {
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.9f, 0.9f, 0.2f);
    renderText(-0.28f, 0.88f, GLUT_BITMAP_TIMES_ROMAN_24, "INSTRUCTIONS");

    glColor3f(0.7f, 0.9f, 0.9f);
    renderText(-0.90f, 0.76f, GLUT_BITMAP_HELVETICA_18,
               "ALL LEVELS: MOVE WITH A/D OR ARROW KEYS.   ESC RETURNS TO MENU");

    const char *lines[][4] = {
        { "LEVEL 1", "Basic shooting practice. Enemies never fire.", "Press SPACE to shoot. R to restart." },
        { "LEVEL 2", "Survive falling enemies and asteroids.", "Press SPACE to fire two lasers. Destroy 20 to win." },
        { "LEVEL 3", "Enemies drop bombs; shoot them for power-ups.", "Press SPACE to fire. Collect green shield / red cross." },
        { "LEVEL 4", "Enemy formation drifts, dives and drops bombs.", "HOLD SPACE for auto-fire. Avoid asteroids. 3 lives." },
        { "LEVEL 5", "Final boss battle - your ship AUTO-SHOOTS.", "Dodge projectiles and jets. Destroy the boss (40 HP)." }
    };

    float y = 0.60f;
    for (int i = 0; i < 5; i++) {
        glColor3f(0.3f, 0.9f, 0.6f);
        renderText(-0.90f, y, GLUT_BITMAP_HELVETICA_18, lines[i][0]);

        y -= 0.07f;
        glColor3f(0.9f, 0.9f, 0.9f);
        renderText(-0.90f, y, GLUT_BITMAP_HELVETICA_18, lines[i][1]);
        y -= 0.055f;
        renderText(-0.90f, y, GLUT_BITMAP_HELVETICA_18, lines[i][2]);
        y -= 0.065f;
    }

    glFlush();
}

void drawHighScores() {
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.9f, 0.9f, 0.2f);
    renderText(-0.32f, 0.88f, GLUT_BITMAP_TIMES_ROMAN_24, "HIGH SCORES");

    glColor3f(0.7f, 0.9f, 0.9f);
    renderText(-0.90f, 0.78f, GLUT_BITMAP_HELVETICA_18,
               "TOP 3 SCORES PER LEVEL.   ESC RETURNS TO MENU");

    // header
    glColor3f(0.9f, 0.9f, 0.9f);
    renderText(-0.90f, 0.66f, GLUT_BITMAP_HELVETICA_18, "LEVEL");
    renderText(0.30f, 0.66f, GLUT_BITMAP_HELVETICA_18, "#1");
    renderText(0.55f, 0.66f, GLUT_BITMAP_HELVETICA_18, "#2");
    renderText(0.80f, 0.66f, GLUT_BITMAP_HELVETICA_18, "#3");

    float y = 0.54f;
    for (int i = 0; i < NUM_LEVELS; i++) {
        glColor3f(0.3f, 0.9f, 0.6f);
        char lvl[16];
        sprintf(lvl, "LEVEL %d", i + 1);
        renderText(-0.90f, y, GLUT_BITMAP_HELVETICA_18, lvl);

        glColor3f(1.0f, 1.0f, 1.0f);
        for (int r = 0; r < 3; r++) {
            char s[24];
            int sc = highScores[i][r];
            if (sc > 0)
                sprintf(s, "%d", sc);
            else
                sprintf(s, "-");
            renderText(0.30f + r * 0.25f, y, GLUT_BITMAP_HELVETICA_18, s);
        }
        y -= 0.12f;
    }

    glFlush();
}

void startLevel(int n) {
    currentScene = n;
    if (n >= LEVEL_1 && n <= LEVEL_5)
        recorded[n - LEVEL_1] = false;
    switch (n) {
    case LEVEL_1:
        L1::resetLevel();
        glutSetWindowTitle("Level 1 - Space Shooter");
        break;
    case LEVEL_2:
        L2::resetLevel();
        glutSetWindowTitle("Level 2 - Space Shooter Mechanics");
        break;
    case LEVEL_3:
        L3::resetLevel();
        glutSetWindowTitle("Level 3 - Space Shooter Mechanics");
        break;
    case LEVEL_4:
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        L4::resetGame();
        glutSetWindowTitle("Level 4 - Space Shooter");
        break;
    case LEVEL_5:
        L5::resetLevel();
        glutSetWindowTitle("Level 5 - Space Shooter Mechanics");
        break;
    }
    glutPostRedisplay();
}

void goToMenu() {
    currentScene = MENU;
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glutSetWindowTitle("Space Shooter - Home");
    glutPostRedisplay();
}

void display() {
    switch (currentScene) {
    case LOADING:      drawLoading();     break;
    case MENU:         drawMenu();        break;
    case SETTINGS:     drawSettings();    break;
    case INSTRUCTIONS: drawInstructions();break;
    case HIGHSCORES:   drawHighScores();  break;
    case LEVEL_1: L1::display();  break;
    case LEVEL_2: L2::display();  break;
    case LEVEL_3: L3::display();  break;
    case LEVEL_4: L4::display();  break;
    case LEVEL_5: L5::display();  break;
    }
    glutSwapBuffers();
}

void update(int value) {
    if (currentScene == MENU || currentScene == SETTINGS || currentScene == INSTRUCTIONS || currentScene == LOADING || currentScene == HIGHSCORES) {
        glutPostRedisplay();
    } else {
        int lvl = 0;
        bool finished = false;
        int score = 0;
        switch (currentScene) {
        case LEVEL_1: L1::update(value); finished = L1::isFinished(); score = L1::getScore(); lvl = 1; break;
        case LEVEL_2: L2::update(value); finished = L2::isFinished(); score = L2::getScore(); lvl = 2; break;
        case LEVEL_3: L3::update(value); finished = L3::isFinished(); score = L3::getScore(); lvl = 3; break;
        case LEVEL_4: L4::update(value); finished = L4::isFinished(); score = L4::getScore(); lvl = 4; break;
        case LEVEL_5: L5::update(value); finished = L5::isFinished(); score = L5::getScore(); lvl = 5; break;
        }

        if (finished && !recorded[lvl - 1]) {
            recorded[lvl - 1] = true;
            recordScore(lvl, score);
        }
    }
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (currentScene == MENU)
    {
        if (key >= '1' && key <= '5')
            startLevel(key - '0');

        return;
    }

    // ESC = return to menu
    if (key == 27)
    {
        goToMenu();
        return;
    }

    switch (currentScene)
    {
    case LEVEL_1:
        L1::handleKeypress(key, x, y);
        break;

    case LEVEL_2:
        L2::handleKeypress(key, x, y);
        break;

    case LEVEL_3:
        L3::handleKeypress(key, x, y);
        break;

    case LEVEL_4:
        L4::handleKeypress(key, x, y);
        break;

    case LEVEL_5:
        L5::handleKeypress(key, x, y);
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (currentScene)
    {
    case LEVEL_1: L1::handleKeyUp(key, x, y); break;
    case LEVEL_2: L2::handleKeyUp(key, x, y); break;
    case LEVEL_3: L3::handleKeyUp(key, x, y); break;
    case LEVEL_4: L4::handleKeyUp(key, x, y); break;
    case LEVEL_5: L5::handleKeyUp(key, x, y); break;
    }
}

void special(int key, int x, int y)
{
    if (currentScene == MENU || currentScene == SETTINGS || currentScene == INSTRUCTIONS)
        return;

    switch (currentScene)
    {
    case LEVEL_1:
        L1::SpecialInput(key, x, y);
        break;

    case LEVEL_2:
        L2::SpecialInput(key, x, y);
        break;

    case LEVEL_3:
        L3::SpecialInput(key, x, y);
        break;

    case LEVEL_4:
        L4::SpecialInput(key, x, y);
        break;

    case LEVEL_5:
        L5::SpecialInput(key, x, y);
        break;
    }
}
void specialUp(int key, int x, int y) {
    switch (currentScene)
    {
    case LEVEL_1: L1::SpecialUp(key, x, y); break;
    case LEVEL_2: L2::SpecialUp(key, x, y); break;
    case LEVEL_3: L3::SpecialUp(key, x, y); break;
    case LEVEL_4: L4::SpecialUp(key, x, y); break;
    case LEVEL_5: L5::SpecialUp(key, x, y); break;
    }
}

void mouse(int button, int state, int mx, int my) {
    if (currentScene == LOADING) return;

    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float ndcX = (2.0f * mx) / w - 1.0f;
    float ndcY = 1.0f - (2.0f * my) / h;

    if (currentScene == SETTINGS) {
        // large mute button centered at (0,0)
        if (ndcX >= -0.35f && ndcX <= 0.35f && ndcY >= -0.175f && ndcY <= 0.175f) {
            if (muted) {
                muted = false;
                toggleMusic(true);
            } else {
                muted = true;
                toggleMusic(false);
            }
            glutPostRedisplay();
        }
        return;
    }

    if (currentScene != MENU) return;

    for (int i = 0; i < NUM_LEVELS; i++) {
        float cy = btnCenters[i];
        if (ndcX >= -BTN_W / 2 && ndcX <= BTN_W / 2 &&
            ndcY >= cy - BTN_H / 2 && ndcY <= cy + BTN_H / 2) {
            startLevel(i + 1);
            return;
        }
    }

    if (ndcX >= -BTN_W / 2 && ndcX <= BTN_W / 2 &&
        ndcY >= SETTINGS_BTN_CY - BTN_H / 2 && ndcY <= SETTINGS_BTN_CY + BTN_H / 2) {
        currentScene = SETTINGS;
        glutSetWindowTitle("Space Shooter - Settings");
        glutPostRedisplay();
        return;
    }

    if (ndcX >= -BTN_W / 2 && ndcX <= BTN_W / 2 &&
        ndcY >= INSTRUCTIONS_BTN_CY - BTN_H / 2 && ndcY <= INSTRUCTIONS_BTN_CY + BTN_H / 2) {
        currentScene = INSTRUCTIONS;
        glutSetWindowTitle("Space Shooter - Instructions");
        glutPostRedisplay();
        return;
    }

    if (ndcX >= -BTN_W / 2 && ndcX <= BTN_W / 2 &&
        ndcY >= HIGHSCORES_BTN_CY - BTN_H / 2 && ndcY <= HIGHSCORES_BTN_CY + BTN_H / 2) {
        loadHighScores();
        currentScene = HIGHSCORES;
        glutSetWindowTitle("Space Shooter - High Scores");
        glutPostRedisplay();
        return;
    }
}

void toggleMusic(bool on)
{
    if (on && !muted)
    {
        // resolve a.wav from the executable's own directory so it works
        // regardless of the current working directory
        char dir[MAX_PATH];
        GetModuleFileName(NULL, dir, MAX_PATH);
        char *slash = strrchr(dir, '\\');
        if (slash) *(slash + 1) = '\0';

        char wav[MAX_PATH];
        sprintf(wav, "%sa.wav", dir);
        PlaySound(wav, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
    }
    else
    {
        PlaySound(NULL, NULL, 0);
    }
}

void sound()
{
    if (!muted)
        toggleMusic(true);
}

// resolve a file path relative to the executable's directory
void getGamePath(const char *name, char *out) {
    char dir[MAX_PATH];
    GetModuleFileName(NULL, dir, MAX_PATH);
    char *slash = strrchr(dir, '\\');
    if (slash) *(slash + 1) = '\0';
    sprintf(out, "%s%s", dir, name);
}

void loadHighScores() {
    char path[MAX_PATH];
    getGamePath("highscores.txt", path);

    FILE *f = fopen(path, "r");
    if (!f) {
        // no file yet - start empty
        for (int i = 0; i < NUM_LEVELS; i++)
            for (int r = 0; r < 3; r++)
                highScores[i][r] = 0;
        return;
    }

    for (int i = 0; i < NUM_LEVELS; i++) {
        fscanf(f, "%d %d %d", &highScores[i][0], &highScores[i][1], &highScores[i][2]);
    }
    fclose(f);
}

void saveHighScores() {
    char path[MAX_PATH];
    getGamePath("highscores.txt", path);

    FILE *f = fopen(path, "w");
    if (!f) return;
    for (int i = 0; i < NUM_LEVELS; i++) {
        fprintf(f, "%d %d %d\n", highScores[i][0], highScores[i][1], highScores[i][2]);
    }
    fclose(f);
}

void recordScore(int level, int score) {
    if (level < 1 || level > NUM_LEVELS) return;
    int idx = level - 1;

    // insert into position
    if (score > highScores[idx][2]) {
        highScores[idx][2] = score;
        // bubble to keep descending order
        for (int r = 2; r > 0; r--) {
            if (highScores[idx][r] > highScores[idx][r - 1]) {
                int t = highScores[idx][r];
                highScores[idx][r] = highScores[idx][r - 1];
                highScores[idx][r - 1] = t;
            }
        }
        saveHighScores();
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));
    loadHighScores();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 600);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Space Shooter - Home");
    glutSetWindowTitle("Space Shooter - Home");

    // load the loading image texture (from the exe dir, with relative fallback)
    char loadingPath[MAX_PATH];
    char altLoadingPath[MAX_PATH];
    getGamePath("loading.bmp", loadingPath);
    getGamePath("..\\..\\loading.bmp", altLoadingPath);
    if (!loadBMPTexture(loadingPath)) {
        if (!loadBMPTexture(altLoadingPath)) {
            loadBMPTexture("loading.bmp");
        }
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    glutTimerFunc(10000, loadingDone, 0);

    sound();

    glutMainLoop();
    return 0;
}
