#define _USE_MATH_DEFINES
#include "include\freeglut.h"
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <string>
#include <random>

GLsizei windowHeight = 600, windowWidth = 600;
GLfloat angle = 0.0f;
GLfloat firedAngle = 0.0f;
GLfloat astroAngle = 0.0f;
GLfloat bullet;
int isFired;
int showGrid = false;

GLfloat screenCenterX = windowWidth / 2.0f;
GLfloat screenCenterY = windowHeight / 2.0f;
GLfloat spaceshipWidth = 30.0f;
GLfloat spaceshipBaseHeight = 20.0f;
GLfloat spaceshipHeight = 20.0f;
int score = 0;

struct Asteroid {
	GLfloat centerX;
	GLfloat centerY;
	GLfloat radius;
	GLint rotationSpeedMultiplier;
	int isHit = false;
	int numOfFaces;
	GLfloat red = 1.0f;
	GLfloat green = 1.0f;
	GLfloat blue = 1.0f;
};

Asteroid asteroids[5];

void generateAsteroids();

// Callback function called when window resized or moved.
void reshape(GLsizei newWidth, GLsizei newHeight)
{
	// Prevents division by zero errors.
	if (newHeight == 0) {
		newHeight = 1;
	}

	glMatrixMode(GL_PROJECTION);

	// Set Viewport to window dimensions
	glViewport(0, 0, newWidth, newHeight);

	// Reset coordinate system
	glLoadIdentity();

	// Set the aspect ratio of the clipping area to match the viewport
	if (newWidth <= newHeight) {
		gluOrtho2D(0.0f, windowWidth, 0.0f, windowHeight * newHeight / newWidth);
	}
	else {
		gluOrtho2D(0.0f, windowWidth * newWidth / newHeight, 0.0f, windowHeight);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Method to check if two objects are collidded.
bool isCollided(GLfloat centerX1, GLfloat centerY1, GLfloat radius1, GLfloat centerX2, GLfloat centerY2, GLfloat radius2) {
	GLfloat deltaX = centerX1 - centerX2;
	GLfloat deltaY = centerY1 - centerY2;

	GLfloat distance = sqrt(deltaX * deltaX + deltaY * deltaY);

	if (distance < radius1 + radius2) {
		return true;
	}
	return false;
}

// Routine to draw a bitmap character string.
void writeBitmapString(void* font, char* string)
{
	char* c;
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void shootMissile(GLfloat startX, GLfloat startY) {

	GLfloat bulletHeight = 20.0f;

#pragma region Path for the missile. Visual Aid Only.

	//glLoadIdentity();
	//glPushMatrix();

	//glTranslatef(startX, startY, 0.0);
	//glRotatef(firedAngle, 0.0, 0.0, 1.0);
	//glTranslatef(-startX, -startY, 0.0);

	//glBegin(GL_LINES);
	//glLineWidth(1.0);
	//glColor3f(1.0, 1.0, 1.0);

	//glVertex2f(startX, startY);
	//glVertex2f(startX, startY + windowHeight);
	//glEnd();

	//glPopMatrix();
#pragma endregion

#pragma region Actual Missile 

	GLfloat bulletX = screenCenterX + bullet * sin((360 - firedAngle) * (M_PI / 180)); // angle is counter clockwise. Therefore get the clockwise angle.
	GLfloat bulletY = screenCenterY + bullet * cos((360 - firedAngle) * (M_PI / 180));
	//Checks for colissions and updates asteroid states.
	for (int i = 0; i < 5; i++) {
		if (asteroids[i].isHit == false) {
			if (isCollided(asteroids[i].centerX, asteroids[i].centerY, asteroids[i].radius, bulletX, bulletY, bulletHeight)) {
				isFired = false;
				asteroids[i].isHit = true;
			}
		}
	}


	glLoadIdentity();
	glPushMatrix();
	
	//Rotates the missile and translates y to move the missile.
	glTranslatef(startX, startY, 0.0);
	glRotatef(firedAngle, 0.0, 0.0, 1.0);
	glTranslatef(-startX, -startY, 0.0);
	glTranslatef(0.0, bullet, 0.0);

	glBegin(GL_TRIANGLES);
	glVertex2f(startX - (bulletHeight / 2), startY);
	glVertex2f(startX + (bulletHeight / 2), startY);
	glVertex2f(startX, startY + bulletHeight);

	glPopMatrix();
	glEnd();


#pragma endregion

}

void drawSpaceship() {

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//Vertices of the asteroid. Spaceship will be drawn at the center of the screen
	GLfloat Vertices[] = {
		screenCenterX, screenCenterY, 0.0f,
		screenCenterX + spaceshipWidth, screenCenterY - spaceshipBaseHeight, 0.0f,
		screenCenterX, screenCenterY + spaceshipBaseHeight + spaceshipHeight, 0.0f,
		screenCenterX, screenCenterY, 0.0f,
		screenCenterX, screenCenterY + spaceshipBaseHeight + spaceshipHeight, 0.0f,
		screenCenterX - spaceshipWidth, screenCenterY - spaceshipBaseHeight, 0.0f
	};

	//Colors for each vertex of the spaceship
	GLfloat Colors[] = {
		0.0, 0.0, 1.0,
		0.0, 0.0, 0.9,
		0.0, 1.0, 0.0,

		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 0.9,
	};

	glLoadIdentity();
	glPushMatrix();

	glTranslatef(screenCenterX, screenCenterY, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);
	glTranslatef(-screenCenterX, -screenCenterY, 0.0);
	glutPostRedisplay();

	glColorPointer(3, GL_FLOAT, 0, Colors);
	glVertexPointer(3, GL_FLOAT, 0, Vertices);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glPopMatrix();

	glEnd();

	if (isFired == true) {
		shootMissile(screenCenterX, screenCenterY);
	}
}

/*
*Draw the asteroids;
*/
void drawAsteroids(Asteroid asteroid) {	

	glLoadIdentity();
	glPushMatrix();

	glTranslatef(asteroid.centerX, asteroid.centerY, 0);
	glRotatef(astroAngle / asteroid.rotationSpeedMultiplier, 0.0, 0.0, 1.0);
	glTranslatef(-asteroid.centerX, -asteroid.centerY, 0);

	glBegin(GL_POLYGON);
	glColor3f(asteroid.red, asteroid.green, asteroid.blue);
	for (int i = 0; i < asteroid.numOfFaces; i++)
	{
		float theta = 2.0f * M_PI * float(i) / float(asteroid.numOfFaces);

		float x = asteroid.radius * cos(theta);
		float y = asteroid.radius * sin(theta);

		glVertex2f(x + asteroid.centerX, y + asteroid.centerY);//output vertex
	}
	glEnd();

	glPopMatrix();

	glEnd();

}

void timer(int value)
{
	astroAngle += 10;
	if (isFired == true) {
		bullet += 5;
	}

	int hitCount = 0;
	for (int i = 0; i < 5; i++) {
		if (asteroids[i].isHit) {
			hitCount++;
		};
	}
	score = hitCount * 10;

	// Reset the screen when all asteroids are hit
	if (hitCount == 5) {
		generateAsteroids();
	}

	glutTimerFunc(16, timer, 0);
	glutPostRedisplay();
}


void drawPattern() {
	//Code taken from https://www.codeproject.com/Articles/23444/A-Simple-OpenGL-Stipple-Polygon-Example-EP-OpenGL
	GLubyte somePattern[] = {
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, };

	glEnable(GL_POLYGON_STIPPLE);

	glColor3f(0.5, 0.5, 0.5);
	glPolygonStipple(somePattern);
	glRectf(0, 0, windowWidth, windowHeight);
	glDisable(GL_POLYGON_STIPPLE);
}

void drawGrid() {

	//Code inspired from http://opengl.czweb.org/ch06/176-179.html

	GLint factor = 1;
	GLushort pattern = 0x5555;

	glEnable(GL_LINE_STIPPLE);
	for (GLfloat y = 0; y < windowHeight; y += 40.0f) // Gap between the lines
	{
		glLineStipple(factor, pattern);
		glBegin(GL_LINES);
		glVertex2f(0, y);
		glVertex2f(windowWidth, y);
		glEnd();
	}
	for (GLfloat x = 0; x < windowWidth; x += 40.0f)
	{
		glLineStipple(factor, pattern);
		glBegin(GL_LINES);
		glVertex2f(x, 0);
		glVertex2f(x, windowHeight);
		glEnd();
	}
	glDisable(GL_LINE_STIPPLE);
}

// Initiates an asteroid properties randomy.
Asteroid generateAsteroid() {

	//Random generation code copied from https://stackoverflow.com/a/7560564/9689970
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> randFaces(5, 8); // define the range
	std::uniform_int_distribution<> randRadius(10, 50); // define the range
	std::uniform_int_distribution<> randSpeed(-5, 5); // define the range
	int randomSpeed = randSpeed(eng);
	if (randomSpeed == 0) {
		randomSpeed = 1;
	}
	GLfloat red = (rand() % 10 + 1) / 10.0f;
	GLfloat green = (rand() % 10 + 1) / 10.0f;
	GLfloat blue = (rand() % 10 + 1) / 10.0f;

	Asteroid astr = {
		rand() % windowWidth,
		rand() % windowHeight,
		randRadius(eng),
		randomSpeed,
		false,
		randFaces(eng),
		red,
		green,
		blue
	};

	return astr;
}

//Chcecks if a given object falls out of the viewport
bool isOutOfViewPort(GLfloat centerX, GLfloat centerY, GLfloat radius) {
	if (centerX - radius < 0 || centerX + radius > windowWidth || centerY - radius < 0 || centerY + radius > windowHeight) {
		return true;
	}
	return false;
}

// Fills the astroid array with randomly generated asteroids and validates for no colissions 
void generateAsteroids() {
	bool isValid = true;
	for (int i = 0; i < 5; i++) {
		asteroids[i] = generateAsteroid();
		// Check if overlapping spaceship
		GLfloat spaceShipFarthestPoint = spaceshipBaseHeight + spaceshipHeight;
		if (isCollided(screenCenterX, screenCenterY, spaceShipFarthestPoint, asteroids[i].centerX, asteroids[i].centerY, asteroids[i].radius)) {
			i--;
			continue;
		}
		//Checks if the asteroid goes out of viewport boundaries
		if (isOutOfViewPort(asteroids[i].centerX, asteroids[i].centerY, asteroids[i].radius)) {
			i--;
			continue;
		}
		for (int j = 0 ; j < 5 && j < i; j++) {
			//Check if asteroids are overlapping;
			if (isCollided(asteroids[i].centerX, asteroids[i].centerY, asteroids[i].radius, asteroids[j].centerX, asteroids[j].centerY, asteroids[j].radius)) {
				i--;
			}
		}
	}
}

//Method to draw the score on screen.
void drawScore() {
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2f(10, 560);
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, "Score: ");

	glRasterPos2f(70, 560);
	std::string numStr = std::to_string(score); // Convers the score to a char array
	int length = numStr.length();
	char charArr[4];
	strcpy_s(charArr, numStr.c_str());
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, charArr);
}

/* display callback function
called whenever contents of window need to be re-displayed */
//this is the all important drawing method - all drawing code goes in here
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);   /* clear window */

	if (showGrid) {
		drawGrid();
	}
	else {
		drawPattern();
	}	

	drawSpaceship();

	for (int i = 0; i < 5; i++) {		
		if (asteroids[i].isHit == false) {
			drawAsteroids(asteroids[i]);
		}
	}

	drawScore();

	glutSwapBuffers();

	glFlush();     /* execute drawing commands in buffer */
}


void printInstructions() {
	std::cout << "How to Play the Game: Controls and Interaction";
	std::cout << "\r\n";
	std::cout << "\r\n";
	std::cout << "\r\n";
	std::cout << "1> Press the arrow keys to rotate the ship.";
	std::cout << "\r\n";
	std::cout << "\r\n";
	std::cout << "1> Press the space bar to shoot rockets.";
	std::cout << "\r\n";
	std::cout << "\r\n";
	std::cout << "1> Right click for menu options.";
	std::cout << "\r\n";
	std::cout << "\r\n";
}

/* graphics initialisation */
void init(void)
{
	printInstructions();
	generateAsteroids();


	glClearColor(0.0, 0.0, 0.0, 0.0);   /* window will be cleared to black */
}

void keyboardFuncSpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		angle += 10.0;
		if (angle >= 360) {
			angle = 0;
		}
		break;

	case GLUT_KEY_RIGHT:
		angle -= 10.0;
		if (angle <= 0) {
			angle = 360;
		}
		break;
	default:
		break;
	}
}

void keyboardFunc(unsigned char key, int x, int y) {
	switch (key)
	{
	case ' ':
		if (isFired != true) {
			isFired = true;
			firedAngle = angle;
		}
		break;
	default:
		break;
	}
}

void update_objects(void)
{

	if (isFired == true) {
		if (bullet >= windowHeight) {
			bullet = 0;
			isFired = false;
		}
	}
	else {
		bullet = 0;
	}

	glutPostRedisplay();
}

void showMenu(GLint id) {
	switch (id)
	{
	case 1:
		showGrid = false;
		break;
	case 2:
		showGrid = true;
	default:
		break;
	}
}

void initMenu() {
	glutCreateMenu(showMenu);
	glutAddMenuEntry("Pattern on", 1);
	glutAddMenuEntry("Grid on", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//rename this to main(...) and change example 2 to run this main function
int main(int argc, char** argv)
{
	/* window management code ... */
	/* initialises GLUT and processes any command line arguments */
	glutInit(&argc, argv);
	/* use single-buffered window and RGBA colour model */
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowHeight, windowWidth);
	/* window upper left corner at (100, 100) */
	glutInitWindowPosition(100, 100);
	/* creates an OpenGL window with command argument in its title bar */
	glutCreateWindow("Astro Shooter");
	init();
	glutSpecialFunc(keyboardFuncSpecial);
	glutKeyboardFunc(keyboardFunc);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	glutIdleFunc(update_objects);
	initMenu();

	glutMainLoop();

	return 0;
}

