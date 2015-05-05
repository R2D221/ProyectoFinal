/*** Tercera entrega proyecto final gráficas computacionales ***\
| Arturo Torres Sánchez	A01212763
| Erika K. Ponce Ocampo	A01126220
| Jorge Luis Torres Esquivel	A01213890
\************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include "glut.h"
#elif __APPLE__
#include <GLUT/GLUT.h>
#endif

#include <math.h>
#include "targa.h"

GLfloat ang = 0;
GLfloat position[3];
GLfloat distance = 5;

GLint width = 300;
GLint height = 300;
GLfloat caminar;

#define AMMO_MAX_TIMEALIVE 100
#define AMMO_STEP 1
#define AMMO_COUNT 100

#define numeroEnemigos 10


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

GLfloat enemigo_posicionInicial_X[numeroEnemigos];
GLfloat enemigo_posicionInicial_Y[numeroEnemigos];
GLfloat enemigo_posicionInicial_Z[numeroEnemigos];
bool enemigo_activo[numeroEnemigos];


bool w;
bool a;
bool s;
bool d;

bool isCollidingWithWalls(GLfloat * position)
{
    if (position[0] >= -1.0&& position[0] <= 21.0&& position[2] <= 11.5 && position[2] >= 8.5)
        return true;

    if (position[0] <= 1.0&& position[0] >= -21.0&& position[2] >= -11.5 && position[2] <= -8.5)
        return true;

    return false;
}

void enemyIsCollidingWithAmmo()
{
	for (int i = 0; i < AMMO_COUNT; i++)
	{
		if (!ammo_isActive[i]) continue;

		for (int j = 0; j < numeroEnemigos; j++)
		{
			GLfloat distancia =	sqrt(
					pow(enemigo_posicionInicial_X[j] - ammo_initialPosition_x[i], 2)
					+
					pow(enemigo_posicionInicial_Z[j] - ammo_initialPosition_z[i], 2)
				);
			if (distancia < 0.5)
			{
				enemigo_activo[j] = false;
				ammo_isActive[i] = false;
			}
		}
	}
}



void Init();
void Display();
void Reshape();


void Init()
{
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

    for(i = 0; i < numeroEnemigos; i++){

        GLfloat posicionEnemigo[3];

        do{
            enemigo_posicionInicial_X[i] = rand() % 20 -10;
            enemigo_posicionInicial_Z[i] = rand() % 20 -10;
            enemigo_activo[i] = true;

            posicionEnemigo[0] = enemigo_posicionInicial_X[i];
            posicionEnemigo[1] = enemigo_posicionInicial_Y[i];
            posicionEnemigo[2] = enemigo_posicionInicial_Z[i];

        }while(isCollidingWithWalls(posicionEnemigo));

    }

    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    position[0] = 0.0;
    position[1] = 1.60;
    position[2] = 0.0;

    //textura
    glEnable(GL_TEXTURE_2D);
    GLubyte *data;
    GLint x,y,d;
    data = LoadTGA("pasto_1.tga",&x,&y,&d);

    glBindTexture(GL_TEXTURE_2D,1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,x,y,0, GL_RGB,GL_UNSIGNED_BYTE,data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    //glBindTexture(GL_TEXTURE_2D,1);
    //glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,x,y,0,GL_RGB,GL_UNSIGNED_BYTE,data);
}


void Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(40, (float)width / height, 0.1, 50);
    gluLookAt(
              position[0], position[1], position[2],
              position[0] + distance * cos(ang), position[1], position[2] + distance * sin(ang),
              0, 1, 0);

    glMatrixMode(GL_MODELVIEW);

    //luces
    GLfloat light_position[] = { 0.0, 20, 0.0, 1.0 };
    GLfloat light_color[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //Plano
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        glColor3f(0, 0, 1);
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

    //muros
    glLoadIdentity();
    glTranslatef(10.0, 1.0, 10.0);
    glScaled(20, 15, 1);
    glutSolidCube(1);

    glLoadIdentity();
    glTranslatef(-10.0, 1.0, -10.0);
    glScaled(20, 15, 1);
    glutSolidCube(1);

    // Tetera
    glLoadIdentity();
    glColor3f(1, 0, 0);
    glutSolidTeapot(0.5);

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


	for (int i = 0; i < numeroEnemigos; i++)
	{
		if (enemigo_activo[i])
		{
			glLoadIdentity();
			glColor3b(1, 0, 1);
			glTranslated(enemigo_posicionInicial_X[i], 1, enemigo_posicionInicial_Z[i]);
			glScalef(1, 2, 1);
			glutSolidCube(1);
		}
	}

    glutSwapBuffers();
    glutPostRedisplay();
}

void Reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);	//actualizar la ventana
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -10, 10);
}

void Keyboard(unsigned char  key, int x, int y)
{
    switch (key) {
        case 'w':	w = true;
            break;

        case 'a':	a = true;
            break;

        case 's':	s = true;
            break;

        case 'd':	d = true;
            break;

        case ' ':	for (int i = 0; i < AMMO_COUNT; i++)
        {
            if (!ammo_isActive[i])
            {
                ammo_isActive[i] = true;
                ammo_initialPosition_x[i] = position[0];
                ammo_initialPosition_y[i] = position[1];
                ammo_initialPosition_z[i] = position[2];
                ammo_angle[i] = ang;
                ammo_timeAlive[i] = 0;
                break;
            }
        }

        default: break;
    }
}
void KeyboardUP(unsigned char  key, int x, int y)
{
    switch (key)
    {
        case 'w':	w = false;
            break;

        case 'a':	a = false;
            break;

        case 's':	s = false;
            break;

        case 'd':	d = false;
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
	enemyIsCollidingWithAmmo();

    glutTimerFunc(33, Movimiento, 0);
}

int main(int artcp, char **argv)
{
    glutInit(&artcp, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Teteras asesinas!");	// crear una ventana
    glutDisplayFunc(Display);	// callback principal
    glutReshapeFunc(Reshape);	// callback de reshape
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUP);
    glutTimerFunc(33, Movimiento, 0);

    Init();	// Inicializaciones
    glutMainLoop();	// loop del programa
    return 0;
}