#ifdef _WIN32
	#include "glut.h"
#elif __APPLE__
	#include <GLUT/GLUT.h>
#endif

#include <math.h>

GLfloat ang = 0;
GLfloat position [3];
GLfloat distance = 5;

GLint width = 300;
GLint height = 300;

bool w;
bool a;
bool s;
bool d;

void Init();
void Display();
void Reshape();

void Init()
{
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);
    position[0]= 0.0;
    position[1]=1.60;
    position[2]=0.0;
  
}

void Display()
{

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

    gluPerspective(40, (float)width / height, 0.1, 50);
	gluLookAt(
		position[0], position[1], position[2],
		distance * cos(ang), position[1], distance * sin(ang),
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
    
    //luces
    
    
    GLfloat light_position[]={0.0,20,0.0,1.0};
    GLfloat light_color[]={1,1,1,1};
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light_color);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    
	
    //Plano
    glLoadIdentity();
    glBegin(GL_QUADS);
    glColor3f(0, 0, 1);
    glVertex3f(100, 0, 100);
    glVertex3f(-100,0,100);
    glVertex3f(-100,0,-100);
    glVertex3f(100,0,-100);
    glEnd();
    
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

void Keyboard(unsigned char  key, int x, int y)
{
    switch (key) {
        case'w':
            w=true;
            break;
        case 'a':
            a= true;
            break;
        case 's':
            s= true;
            break;
        case 'd':
            d=true;
            break;
        default:
            break;
    }
}
void KeyboardUP(unsigned char  key, int x, int y)
{
    switch (key)
	{
        case'w':
            w=false;
            break;
        case 'a':
            a= false;
            break;
        case 's':
            s= false;
            break;
        case 'd':
            d=false;
            break;
        default:
            break;
    }
}
void idle(int _i)
{
	if (w)
	{
		float step = 0.1;
		float x = 0.1 * cos(ang);
		float z = 0.1 * sin(ang);
		position[0] += x;
		position[2] += z;
	}
	if (a)
	{
		ang-=0.1;
	}
	if (s)
	{
		float step = 0.1;
		float x = 0.1 * cos(ang);
		float z = 0.1 * sin(ang);
		position[0] -= x;
		position[2] -= z;
	}
	if (d)
	{
		ang+=0.1;
	}

	glutTimerFunc(33, idle, 0);
}

int main (int artcp, char **argv)
{
	glutInit(&artcp, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Rotating teapot");	// crear una ventana
	glutDisplayFunc(Display);	// callback principal
	glutReshapeFunc(Reshape);	// callback de reshape
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUP);
	glutTimerFunc(33, idle, 0);
    
	Init();	// Inicializaciones
	glutMainLoop();	// loop del programa
	return 0;

}