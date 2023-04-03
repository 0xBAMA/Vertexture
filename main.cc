//******************************************************************************
//  Program: Interactive Computer Graphics Project 1
//
//  Author: Jon Baker
//  Email: jb239812@ohio.edu
//
//  Description: This file contains GLUT specific code to open a window and
//       allow interaction. It instantiates model classes declared in model.h
//
//  Date: 20 September 2019
//******************************************************************************

#include "resources/model.h"
#include <stdio.h>

int animation_time = 0;

//the model
GroundModel*       ground;
DudesAndTreesModel* datmodel;
WaterModel*        water;
SkirtModel*        skirts;

//parameters for the game
int num_good_guys;
int num_bad_guys;
int num_trees;
int num_boxes_initial;

//should you draw the models?
bool drawground = true;
bool drawwater = true;

bool big_radius = false;
bool drawdudes = true;

bool rotate = true;
int temp_time = animation_time;

int scroll = 0; // 0 - no scrolling, 1 - slow, 2 - faster

//DEBUG STUFF

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
	if ( severity != GL_DEBUG_SEVERITY_NOTIFICATION )
		fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
}

void init() {
	cout << "initializing ground model" << endl;
	ground = new GroundModel();
	cout << "initializing dudesandtrees model" << endl;
	datmodel = new DudesAndTreesModel(num_good_guys, num_bad_guys, num_trees, num_boxes_initial);
	cout << "initializing water model" << endl;
	water = new WaterModel();
	cout << "initializing skirt model" << endl;
	skirts = new SkirtModel();

	GLfloat left = -1.366f;
	GLfloat right = 1.366f;
	GLfloat top = -0.768f;
	GLfloat bottom = 0.768f;
	GLfloat zNear = 1.2f;
	GLfloat zFar = -1.0f;

	glm::mat4 proj = glm::ortho(left, right, top, bottom, zNear, zFar);

	ground->set_proj(proj);
	water->set_proj(proj);
	datmodel->set_proj(proj);
	skirts->set_proj(proj);

	ground->set_scroll(scroll);
	datmodel->set_scroll(scroll);
	skirts->set_scroll(scroll);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LINE_SMOOTH);

	//DEBUG

	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );

	cout << endl << endl << " GL_DEBUG_OUTPUT ENABLED " << endl;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------
void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(rotate) {
		animation_time++;
		ground->set_time(animation_time);
		datmodel->set_time(animation_time);
		water->set_time(animation_time);
		skirts->set_time(animation_time);
	}

	//DRAW THE GROUND
	if(drawground)
		ground->display();
	else
		ground->display(true);


	//DRAW THE WATER
	if(drawwater)
		water->display();

	//DRAW THE DUDES AND TREES
	if(drawdudes)
		datmodel->display();

	//DRAW THE SKIRTS
	skirts->display();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 033:
			exit(EXIT_SUCCESS);
			break;

		case 'f':
			glutFullScreen();
			break;

		case 'a':
			ground->scale_up();
			skirts->scale_up();
			break;
		case 's':
			ground->scale_down();
			skirts->scale_down();
			break;


		case 'd':
			//toggle drawing of ground
			drawwater = !drawwater;
			break;

		case 'g':
			big_radius = !big_radius;
			break;

		case 'h':
			drawdudes = !drawdudes;
			break;

		case 'z':
			//toggle drawing of ground
			drawground = !drawground;
			break;

		case 'x': //cycle speeds
			switch(scroll) {
				case 0:
				scroll = 1;
				break;
				case 1:
				scroll = 2;
				break;
				case 2:
				scroll = 0;
				break;
			}
			ground->set_scroll(scroll);
			skirts->set_scroll(scroll);
			break;

		case 'v':
			//stop or start the rotation;
			temp_time = animation_time;
			rotate = !rotate;
			break;

		case 'b':
			ground->toggle_normals();
			break;

		case 'n':
			datmodel->toggle_cursor_draw();
			break;
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int x, int y ) {
	if ( state == GLUT_DOWN ) {
		switch( button ) {
			case GLUT_LEFT_BUTTON:    cout << "left" << endl;   break;
			case GLUT_MIDDLE_BUTTON:  cout << "middle" << endl; break;
			case GLUT_RIGHT_BUTTON:   cout << "right" << endl;  break;
		}

		if(button == GLUT_LEFT_BUTTON) {

			//clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render with selection colors
			ground->display(true);

			//read out the pixel

			double phi = 1.618;
			y = glutGet( GLUT_WINDOW_HEIGHT ) - y;

			unsigned char pixel[4];
			glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

			float calcx = (-phi + ((2*phi)/(205))*((int)pixel[0]-25));
			float calcy = (-phi + ((2*phi)/(205))*((int)pixel[1]-25));

			// cout << "R: " << (int)pixel[0] << " which is x: " << calcx << endl;
			// cout << "G: " << (int)pixel[1] << " which is y: " << calcy  << endl;
			// cout << "B: " << (int)pixel[2];
			//
			// if((int) pixel[2] > 0)
			//   cout << " which is ... UNDERWATER" << endl;

			//do whatever you need to do

			//the r value ranges from 25 to 230 -> map to the range (-phi to phi)
			//the g value ranges from 25 to 230 -> map to the range (-phi to phi)
			//the b is usaully either 0 or 255  -> >0 tells you it's in the water

			datmodel->set_pos(glm::vec3(calcx/2, calcy/2, 0.0), glm::vec3(1,0,(float)pixel[2]));

			if(pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
				datmodel->handle_click(glm::vec3(0,0,0));
			else
				datmodel->handle_click(glm::vec3(calcx/2, calcy/2, pixel[2]/255.0));

			cout << endl;

			//clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glutPostRedisplay();
		}
		cout << endl << "Your current score is " << datmodel->get_score() << endl;
	}
}

//----------------------------------------------------------------------------

void timer(int) {
	datmodel->big_radius = big_radius;
	if(rotate)
		datmodel->update_sim();

	if(datmodel->get_boxes_left() == 0 && !datmodel->get_status()) {
		cout << endl << endl << "GAME OVER" << endl << "with score = " << datmodel->get_score() << endl << endl;
		exit(EXIT_SUCCESS);
	}
	glutPostRedisplay();
	glutTimerFunc(1000.0/60.0, timer, 0);
}

//----------------------------------------------------------------------------

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitContextVersion( 4, 5 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

	// glutInitWindowSize(1366/2, 768/2);
	glutInitWindowSize(720,480);
	glutCreateWindow("GLUT");

	if(argc == 5) {
		// cout << argv[0] << endl;		//first arg is application name
		num_good_guys = atoi(argv[1]);	//second arg is number of good guys to use for the simulation
		num_bad_guys = atoi(argv[2]);	//third arg is the number of bad guys present
		num_trees = atoi(argv[3]);		//fourth arg is the number of trees to include
		num_boxes_initial = atoi(argv[4]);	//fifth arg is the number of boxes the player is initially able to place

	} else {
		cout << "Incorrect command line args" << endl;
		cout << "using defaults for the game parameters" << endl;

		//set defaults for:
		num_good_guys = 3;
		num_bad_guys = 3;
		num_trees = 3;
		num_boxes_initial = 3;
	}

	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc( mouse );
	glutTimerFunc(1000.0/60.0, timer, 0);
	glutMainLoop();
	return(EXIT_SUCCESS);
}
