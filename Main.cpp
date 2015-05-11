/*** Tercera entrega proyecto final gráficas computacionales ***\
| Arturo Torres Sánchez         A01212763
| Erika K. Ponce Ocampo         A01126220
| Jorge Luis Torres Esquivel    A01213890
\************************************************************/

//youlose.wav: https://www.freesound.org/people/noirenex/sounds/159408/
//youwin.wav: https://www.freesound.org/people/Tuudurt/sounds/258142/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include "glut.h"
#elif __APPLE__
#include <GLUT/GLUT.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include "targa.h"
#include "glm/glm.h"
#include <irrKlang.h>

using namespace irrklang;
ISoundEngine* engine;

GLfloat ang = 0;
GLfloat position[3];
GLfloat distance = 5;

GLint width = 300;
GLint height = 300;
GLfloat caminar;

#define AMMO_MAX_TIMEALIVE 100
#define AMMO_STEP 1
#define AMMO_COUNT 100

#define ENEMIGO_CUENTA_INICIAL 10
#define ENEMIGO_STEP 0.025
#define ENEMIGO_VIDAS_INICIAL 5

int enemigosVivos = ENEMIGO_CUENTA_INICIAL;

bool ammo_isActive[AMMO_COUNT];
GLfloat ammo_initialPosition_x[AMMO_COUNT];
GLfloat ammo_initialPosition_y[AMMO_COUNT];
GLfloat ammo_initialPosition_z[AMMO_COUNT];
GLfloat ammo_angle[AMMO_COUNT];
int ammo_timeAlive[AMMO_COUNT];
void ammo_increaseTimeAlive(int index)
{
    ammo_timeAlive[index]++;
    if (ammo_timeAlive[index] == AMMO_MAX_TIMEALIVE)
    {
        ammo_timeAlive[index] = 0;
        ammo_isActive[index] = false;
    }
}


GLfloat enemigo_posicionInicial_X[ENEMIGO_CUENTA_INICIAL];
GLfloat enemigo_posicionInicial_Y[ENEMIGO_CUENTA_INICIAL];
GLfloat enemigo_posicionInicial_Z[ENEMIGO_CUENTA_INICIAL];
int enemigo_vidas[ENEMIGO_CUENTA_INICIAL];

GLMmodel *model;

bool w;
bool a;
bool s;
bool d;

#define JUGADOR_VIDAS_INICIAL 5
int jugador_vidas = JUGADOR_VIDAS_INICIAL;

GLfloat calculateDistance(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

bool isCollidingWithWalls(GLfloat * position)
{
    if (position[0] >= -1.0&& position[0] <= 21.0&& position[2] <= 11.5 && position[2] >= 8.5)
        return true;

    if (position[0] <= 1.0&& position[0] >= -21.0&& position[2] >= -11.5 && position[2] <= -8.5)
        return true;

    return false;
}

void DetectEnemyCollidingWithAmmo()
{
    for (int i = 0; i < AMMO_COUNT; i++)
    {
        if (!ammo_isActive[i]) continue;

        for (int j = 0; j < ENEMIGO_CUENTA_INICIAL; j++)
        {
            if (!enemigo_vidas[j]) continue;

            GLfloat distancia = calculateDistance(ammo_initialPosition_x[i], ammo_initialPosition_z[i], enemigo_posicionInicial_X[j], enemigo_posicionInicial_Z[j]);
            if (distancia < 1)
            {
                enemigo_vidas[j]--;
                if (!enemigo_vidas[j])
                {
                    enemigosVivos--;
                }
                ammo_isActive[i] = false;
                engine->play2D("hit_enemy.wav", false);     //https://www.freesound.org/people/TheyCallMeCaudex/sounds/266148/
                printf("Colision de bala con enemigo\n");
            }
        }
    }
}

void DetectEnemyCollidingWithPlayer()
{
    for (int j = 0; j < ENEMIGO_CUENTA_INICIAL; j++)
    {
        if (!enemigo_vidas[j]) continue;

        GLfloat distancia = calculateDistance(position[0], position[2], enemigo_posicionInicial_X[j], enemigo_posicionInicial_Z[j]);
        if (distancia < 1)
        {
            enemigo_vidas[j] = 0;
            enemigosVivos--;
            jugador_vidas--;
            engine->play2D("hit_player.wav", false);    //https://www.freesound.org/people/LittleRobotSoundFactory/sounds/270311/
            printf("Colision de enemigo con el jugador\n");
        }
    }
}

void MoveEnemy()
{
    for (int i = 0; i < ENEMIGO_CUENTA_INICIAL; i++)
    {
        if (!enemigo_vidas[i]) continue;
        if (calculateDistance(enemigo_posicionInicial_X[i], enemigo_posicionInicial_Z[i], position[0], position[2]) > 10.0) continue;

        GLfloat enemigo_angulo =    atan2(
                                        (enemigo_posicionInicial_Z[i] - position[2]),
                                        (enemigo_posicionInicial_X[i] - position[0])
                                    );
        enemigo_posicionInicial_X[i] -= cos(enemigo_angulo) * ENEMIGO_STEP;
        enemigo_posicionInicial_Z[i] -= sin(enemigo_angulo) * ENEMIGO_STEP;
    }
}

void Init();
void Display();
void Reshape();

void PlayMusic()
{
    //Music by Eric Skiff
    //http://ericskiff.com/music/
    engine->play2D("music.wav", true);
}

#define TEXTURE_GRASS       3
#define TEXTURE_HEART       4
#define TEXTURE_LIFE_5      6
#define TEXTURE_LIFE_4      7
#define TEXTURE_LIFE_3      8
#define TEXTURE_LIFE_2      9
#define TEXTURE_LIFE_1      10
#define TEXTURE_NUMBER_0    11
#define TEXTURE_NUMBER_1    12
#define TEXTURE_NUMBER_2    13
#define TEXTURE_NUMBER_3    14
#define TEXTURE_NUMBER_4    15
#define TEXTURE_NUMBER_5    16
#define TEXTURE_NUMBER_6    17
#define TEXTURE_NUMBER_7    18
#define TEXTURE_NUMBER_8    19
#define TEXTURE_NUMBER_9    20
#define TEXTURE_ENEMIES     21

void LoadTextures()
{
    glEnable(GL_TEXTURE_2D);
    GLubyte *data;
    GLint x, y, d;

    data = LoadTGA("pasto_1.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_GRASS);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("Corazon.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_HEART);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("vida_5.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_5);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("vida_4.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("vida_3.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_3);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("vida_2.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_2);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("vida_1.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("0.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("1.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("2.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_2);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("3.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_3);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("4.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("5.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_5);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("6.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_6);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("7.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_7);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("8.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_8);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("9.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_9);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    data = LoadTGA("enemigos.tga", &x, &y, &d);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_ENEMIES);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    delete data;
}

void Init()
{
    engine = createIrrKlangDevice();
    model = glmReadOBJ("onj_enemigo.obj");

    PlayMusic();

    for (int i = 0; i < AMMO_COUNT; i++)
    {
        ammo_isActive[i] = false;
        ammo_initialPosition_x[i] = 0;
        ammo_initialPosition_y[i] = 0;
        ammo_initialPosition_z[i] = 0;
        ammo_angle[i] = 0;
        ammo_timeAlive[i] = 0;
    }

    int i;

    for(i = 0; i < ENEMIGO_CUENTA_INICIAL; i++)
    {
        GLfloat posicionEnemigo[3];

        do
        {
            enemigo_posicionInicial_X[i] = rand() % 20 -10;
            enemigo_posicionInicial_Z[i] = rand() % 20 -10;
            enemigo_vidas[i] = ENEMIGO_VIDAS_INICIAL;

            posicionEnemigo[0] = enemigo_posicionInicial_X[i];
            posicionEnemigo[1] = enemigo_posicionInicial_Y[i];
            posicionEnemigo[2] = enemigo_posicionInicial_Z[i];
        }
        while(isCollidingWithWalls(posicionEnemigo));

    }

    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    position[0] = 0.0;
    position[1] = 1.60;
    position[2] = 0.0;

    LoadTextures();
}

void ShowStatusBar()
{
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TEXTURE_HEART);
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        int width = glutGet(GLUT_WINDOW_WIDTH);
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        GLfloat x1 = 0;
        GLfloat x2 = 0.1;
        GLfloat y1 = 1 - (0.1 * width / height);
        GLfloat y2 = 1;
        glTexCoord2f(1, 1);         glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 1);         glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 0);         glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
        glTexCoord2f(1, 0);         glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_5 + (JUGADOR_VIDAS_INICIAL - jugador_vidas > 0 ? jugador_vidas : 1));
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        GLfloat x1 = 0.1;
        GLfloat x2 = 0.4025641025641;
        GLfloat y1 = 1 - (0.1 * width / height);
        GLfloat y2 = 1;
        glTexCoord2f(1, 1);         glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 1);         glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 0);         glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
        glTexCoord2f(1, 0);         glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_0 + enemigosVivos / 10);
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        GLfloat x1 = 0.9;
        GLfloat x2 = 0.95;
        GLfloat y1 = 1 - (0.05 * width / height);
        GLfloat y2 = 1;
        glTexCoord2f(1, 1);         glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 1);         glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 0);         glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
        glTexCoord2f(1, 0);         glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, TEXTURE_NUMBER_0 + enemigosVivos % 10);
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        GLfloat x1 = 0.95;
        GLfloat x2 = 1;
        GLfloat y1 = 1 - (0.05 * width / height);
        GLfloat y2 = 1;
        glTexCoord2f(1, 1);         glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 1);         glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 0);         glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
        glTexCoord2f(1, 0);         glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, TEXTURE_ENEMIES);
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        GLfloat x1 = 0.9 - 0.21392405063291;
        GLfloat x2 = 0.9;
        GLfloat y1 = 1 - (0.05 * width / height);
        GLfloat y2 = 1;
        glTexCoord2f(1, 1);         glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 1);         glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
        glTexCoord2f(0, 0);         glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
        glTexCoord2f(1, 0);         glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}
void ShowCamera()
{
    glLoadIdentity();
    gluPerspective(40, (float)width / height, 0.1, 50);
    gluLookAt(
        position[0], position[1], position[2],
        position[0] + distance * cos(ang), position[1], position[2] + distance * sin(ang),
        0, 1, 0);

    glMatrixMode(GL_MODELVIEW);
}
void ShowLights()
{
    GLfloat light_position[] = { 0.0, 20, 0.0, 1.0 };
    GLfloat light_color[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
}
void ShowFloor()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TEXTURE_GRASS);
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        glTexCoord2f(100, 100);
        glVertex3f(100, 0, 100);
        glTexCoord2f(-100, 100);
        glVertex3f(-100, 0, 100);
        glTexCoord2f(-100, -100);
        glVertex3f(-100, 0, -100);
        glTexCoord2f(100, -100);
        glVertex3f(100, 0, -100);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void ShowWalls()
{
    glLoadIdentity();
    glColor3f(0, 0.5, 0.5);
    glTranslatef(10.0, 1.0, 10.0);
    glScaled(20, 15, 1);
    glutSolidCube(1);

    glLoadIdentity();
    glColor3f(0, 0.5, 0.5);
    glTranslatef(-10.0, 1.0, -10.0);
    glScaled(20, 15, 1);
    glutSolidCube(1);
}
void ShowTeapotForDebugging()
{
    glLoadIdentity();
    glColor3f(1, 0, 0);
    glutSolidTeapot(0.5);
}
void ShowAmmo()
{
    for (int i = 0; i < AMMO_COUNT; i++)
    {
        if (ammo_isActive[i])
        {
            glLoadIdentity();
            glColor3f(1, 0, 0);

            glTranslatef(ammo_initialPosition_x[i], ammo_initialPosition_y[i], ammo_initialPosition_z[i]);
            glutSolidTeapot(0.1);
        }
    }
}
void ShowEnemies()
{
    for (int i = 0; i < ENEMIGO_CUENTA_INICIAL; i++)
    {
        if (enemigo_vidas[i])
        {
            glLoadIdentity();
            glColor3f(1, 0, 1);
            glTranslated(enemigo_posicionInicial_X[i], 1, enemigo_posicionInicial_Z[i]);
            glScalef(1, 2, 1);
            glutSolidCube(1);

            glScalef(1, 0.5, 1);

            glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, TEXTURE_LIFE_5 + (ENEMIGO_VIDAS_INICIAL - enemigo_vidas[i]));
            glBegin(GL_QUADS);
            {
                GLfloat x1 = 0.25;
                GLfloat x2 = 0.75;//0.4025641025641;
                GLfloat y1 = 1;// - (0.1 * width / height);
                GLfloat y2 = 1.16525423728814;
                glTexCoord2f(1, 1);         glVertex3f((x2 * 2 - 1) * cos(ang + M_PI_2), y2 * 2 - 1, (x2 * 2 - 1) * sin(ang + M_PI_2));
                glTexCoord2f(0, 1);         glVertex3f((x1 * 2 - 1) * cos(ang + M_PI_2), y2 * 2 - 1, (x1 * 2 - 1) * sin(ang + M_PI_2));
                glTexCoord2f(0, 0);         glVertex3f((x1 * 2 - 1) * cos(ang + M_PI_2), y1 * 2 - 1, (x1 * 2 - 1) * sin(ang + M_PI_2));
                glTexCoord2f(1, 0);         glVertex3f((x2 * 2 - 1) * cos(ang + M_PI_2), y1 * 2 - 1, (x2 * 2 - 1) * sin(ang + M_PI_2));
            }
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
        }
    }
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    ShowStatusBar();
    ShowCamera();
    ShowLights();
    ShowFloor();
    ShowWalls();
    ShowTeapotForDebugging();
    ShowAmmo();
    ShowEnemies();

    //glLoadIdentity();
    //glTranslatef(5, 0, 0);
    //glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);

    glLoadIdentity();
    glutSwapBuffers();
    glutPostRedisplay();
}

void Reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);     //actualizar la ventana
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -10, 10);
}

void Keyboard(unsigned char     key, int x, int y)
{
    switch (key)
    {
        case 'w':   w = true;
                    break;

        case 'a':   a = true;
                    break;

        case 's':   s = true;
                    break;

        case 'd':   d = true;
                    break;

        case ' ':   for (int i = 0; i < AMMO_COUNT; i++)
                    {
                        if (!ammo_isActive[i])
                        {
                            ammo_isActive[i] = true;
                            ammo_initialPosition_x[i] = position[0];
                            ammo_initialPosition_y[i] = position[1];
                            ammo_initialPosition_z[i] = position[2];
                            ammo_angle[i] = ang;
                            ammo_timeAlive[i] = 0;
                            engine->play2D("shoot.wav", false);     //https://www.freesound.org/people/Quonux/sounds/166418/
                            break;
                        }
                    }

        default: break;
    }
}

void KeyboardUP(unsigned char   key, int x, int y)
{
    switch (key)
    {
        case 'w':   w = false;
                    break;

        case 'a':   a = false;
                    break;

        case 's':   s = false;
                    break;

        case 'd':   d = false;
                    break;

        default: break;
    }
}

void Movimiento(int _i)
{
    if (w)
    {
        float step = 0.1;
        float x = step * cos(ang);
        float z = step * sin(ang);
        caminar += 1;
        float y = (1.6 + sin(caminar)*0.05) - position[1];
        position[0] += x;
        position[1] += y;
        position[2] += z;
        if (isCollidingWithWalls(position))
        {
            position[0] -= x;
            position[1] -= y;
            position[2] -= z;
        }
    }
    if (a)
    {
        ang -= 0.1;
    }
    if (s)
    {
        float step = 0.1;
        float x = step * cos(ang);
        float z = step * sin(ang);
        caminar -= 1;
        float y = (1.6 + sin(caminar)*0.05) - position[1];
        position[0] -= x;
        position[1] += y;
        position[2] -= z;
        if (isCollidingWithWalls(position))
        {
            position[0] += x;
            position[1] -= y;
            position[2] += z;
        }
    }
    if (d)
    {
        ang += 0.1;
    }
    for (int i = 0; i < AMMO_COUNT; i++)
    {
        if (ammo_isActive[i])
        {
            ammo_initialPosition_x[i] += AMMO_STEP * cos(ammo_angle[i]);
            ammo_initialPosition_y[i] += 0;
            ammo_initialPosition_z[i] += AMMO_STEP * sin(ammo_angle[i]);
            ammo_increaseTimeAlive(i);
        }
    }
    DetectEnemyCollidingWithPlayer();
    DetectEnemyCollidingWithAmmo();
    //MoveEnemy();

    glutTimerFunc(33, Movimiento, 0);
}

int main(int artcp, char **argv)
{
    glutInit(&artcp, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Teteras asesinas!");  // crear una ventana
    glutDisplayFunc(Display);               // callback principal
    glutReshapeFunc(Reshape);               // callback de reshape
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUP);
    glutTimerFunc(33, Movimiento, 0);

    Init();             // Inicializaciones
    glutMainLoop();     // loop del programa
    return 0;
}