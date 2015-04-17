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
	gluLookAt(distance * cos(ang),position[1], distance * sin(ang),
		position[0], position[1], position[2],
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
void idle()
{
	if (w)
	{
		//TODO
	}
	if (a)
	{
		//TODO
	}
	if (s)
	{
		//TODO
	}
	if (d)
	{
		//TODO
	}
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
    glutIdleFunc(idle);
    
	Init();	// Inicializaciones
	glutMainLoop();	// loop del programa
	return 0;

}