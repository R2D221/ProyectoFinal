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

#define _USE_MATH_DEFINES
#include <math.h>
#include "targa.h"
#include "glm/glm.h"

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
#define ENEMIGO_STEP 0.025

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
int enemigo_vidas[numeroEnemigos];
#define ENEMIGO_VIDAS_INICIAL 5

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

		for (int j = 0; j < numeroEnemigos; j++)
		{
			if (!enemigo_vidas[j]) continue;

			GLfloat distancia = calculateDistance(ammo_initialPosition_x[i], ammo_initialPosition_z[i], enemigo_posicionInicial_X[j], enemigo_posicionInicial_Z[j]);
			if (distancia < 1)
			{
				enemigo_vidas[j]--;
				ammo_isActive[i] = false;
				printf("Colision de bala con enemigo\n");
			}
		}
	}
}

void DetectEnemyCollidingWithPlayer()
{
	for (int j = 0; j < numeroEnemigos; j++)
	{
		if (!enemigo_vidas[j]) continue;

		GLfloat distancia = calculateDistance(position[0], position[2], enemigo_posicionInicial_X[j], enemigo_posicionInicial_Z[j]);
		if (distancia < 1)
		{
			enemigo_vidas[j] = 0;
			jugador_vidas--;
			printf("Colision de enemigo con el jugador\n");
		}
	}
}

void MoveEnemy()
{
	for (int i = 0; i < numeroEnemigos; i++)
	{
		if (!enemigo_vidas[i]) continue;
		if (calculateDistance(enemigo_posicionInicial_X[i], enemigo_posicionInicial_Z[i], position[0], position[2]) > 10.0) continue;

		GLfloat enemigo_angulo =	atan2(
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

void Init()
{
	model = glmReadOBJ("onj_enemigo.obj");
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

	//textura
	glEnable(GL_TEXTURE_2D);
	GLubyte *data;
	GLint x, y, d;

	data = LoadTGA("pasto_1.tga", &x, &y, &d);
	glBindTexture(GL_TEXTURE_2D, 3);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//data = LoadTGA("corazon.tga", &x, &y, &d);
	//glBindTexture(GL_TEXTURE_2D, 4);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	delete data;
}

void ShowHealthBar()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 3);
	glLoadIdentity();
	glBegin(GL_QUADS);
	{
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		GLfloat x1 = 0;
		GLfloat x2 = 0.1;
		GLfloat y1 = 1 - (0.1 * width / height);
		GLfloat y2 = 1;
		glTexCoord2f(1, 1);		glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
		glTexCoord2f(0, 1);		glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
		glTexCoord2f(0, 0);		glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
		glTexCoord2f(1, 0);		glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();
	glBegin(GL_QUADS);
	{
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		GLfloat x1 = 0.1;
		GLfloat x2 = 0.1 + (jugador_vidas * 0.9 / JUGADOR_VIDAS_INICIAL);
		GLfloat y1 = 1 - (0.075 * width / height);
		GLfloat y2 = 0.975;
		glColor3f(0, 0, 1);
		glVertex3f(x2 * 2 - 1, y2 * 2 - 1, 0);
		glVertex3f(x1 * 2 - 1, y2 * 2 - 1, 0);
		glVertex3f(x1 * 2 - 1, y1 * 2 - 1, 0);
		glVertex3f(x2 * 2 - 1, y1 * 2 - 1, 0);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}
void ShowCamera()
{
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
	glBindTexture(GL_TEXTURE_2D, 3);
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
	glTranslatef(10.0, 1.0, 10.0);
	glScaled(20, 15, 1);
	glutSolidCube(1);

	glLoadIdentity();
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
	for (int i = 0; i < numeroEnemigos; i++)
	{
		if (enemigo_vidas[i])
		{
			glLoadIdentity();
			glColor3b(1, 0, 1);
			glTranslated(enemigo_posicionInicial_X[i], 1, enemigo_posicionInicial_Z[i]);
			glScalef(1, 2, 1);
			glutSolidCube(1);
		}
	}
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ShowHealthBar();
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
	DetectEnemyCollidingWithPlayer();
	DetectEnemyCollidingWithAmmo();
	//MoveEnemy();

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