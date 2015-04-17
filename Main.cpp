#ifdef _WIN32
	#include "glut.h"
#elif __APPLE__
	#include <GLUT/GLUT.h>
#endif

#include <math.h>

GLfloat ang = 0;
GLfloat distance = 5;

GLint width = 300;
GLint height = 300;

void Init();
void Display();
void Reshape();

void Init()
{
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);
    
 

}

void Display()
{
	ang+=0.001;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(40, (float)width / height, 0.1, 50);
	gluLookAt(distance * sin(ang), 1, distance * cos(ang),
		0, 0, 0,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	// Tetera
	glLoadIdentity();
	glColor3f(1, 0, 0);
	glutSolidTeapot(0.5);

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



int main (int artcp, char **argv)
{
	glutInit(&artcp, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Rotating teapot");	// crear una ventana
	glutDisplayFunc(Display);	// callback principal
	glutReshapeFunc(Reshape);	// callback de reshape
	Init();	// Inicializaciones
	glutMainLoop();	// loop del programa
	return 0;

}