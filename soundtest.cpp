//MCC Nestor Gomez
//
//"Hola Tetera"
#include <stdlib.h>
#include "glut.h"
#include <irrKlang.h>

using namespace irrklang;

GLfloat ang=0;

GLfloat escala=1.0f;

ISoundEngine* engine;


void Init();
void Display();
void Reshape(int w, int h);
void keyboard ( unsigned char key, int x, int y );

void Init()
{
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);
    
    
    engine = createIrrKlangDevice();
    
}

void Display()
{

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidTeapot(0.5);
	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard ( unsigned char key, int x, int y ){
	if(key=='q')
		exit(0);
    if(key==' ')
        engine->play2D("bell.wav", false);
	
}

void Reshape(int w, int h)
{
	glViewport(0, 0, 300, 300);   //actualizar la ventana  
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();             
	glOrtho(-1, 1, -1, 1, -10, 10);
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Rotating teapot"); //crear una ventana
	glutDisplayFunc(Display);  //callback principal
	glutReshapeFunc(Reshape); //callback de reshape
	glutKeyboardFunc(keyboard);
	Init();			// Inicializaciones
	glutMainLoop();   //loop del programa
	return 0;        
	
}
