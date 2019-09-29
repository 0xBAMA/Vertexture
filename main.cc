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

int animation_time = 0;

//the model
GroundModel*       ground;
WaterModel*        water;


//should you draw the models?
bool drawground = true;


bool rotate = true;
int temp_time = animation_time;





//DEBUG STUFF

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}






void init()
{
  ground = new GroundModel();
  water = new WaterModel();


  // GLfloat left = -1.920f;
  // GLfloat right = 1.920f;
  // GLfloat top =  -1.080f;
  // GLfloat bottom = 1.080f;
  // GLfloat zNear = -1.0f;
  // GLfloat zFar = 1.0f;

  GLfloat left = -1.366f;
  GLfloat right = 1.366f;
  GLfloat top = -0.768f;
  GLfloat bottom = 0.768f;
  GLfloat zNear = 1.0f;
  GLfloat zFar = -1.0f;

  glm::mat4 proj = glm::ortho(left, right, top, bottom, zNear, zFar);

  ground->set_proj(proj);
  water->set_proj(proj);

  glEnable(GL_DEPTH_TEST);

  // glEnable(GL_DITHER); //something interesting to mess with in the future - might be depreciated, use a shader

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);





  //DEBUG

  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );




  // double phi = (1 + std::sqrt(5.0))/2.0;  //golden ratio, used to compute icosahedron
  // glClearColor(1/phi, 1/phi, 1/phi, 1.0); // grey background

  // glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

//----------------------------------------------------------------------------

extern "C" void display()
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  if(rotate)
  {
    animation_time++;
    ground->set_time(animation_time);
    water->set_time(animation_time);
  }

  ground->display();
  water->display();


  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();

}

//----------------------------------------------------------------------------

//reshape

//----------------------------------------------------------------------------

extern "C" void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 033:
    exit(EXIT_SUCCESS);
    break;



  case 'f':
    glutFullScreen();
    break;




  case 'z':
    //toggle drawing of ground
    drawground = !drawground;
    break;
  case 'v':
    //stop or start the rotation;
    temp_time = animation_time;
    rotate = !rotate;
    break;
  }
  glutPostRedisplay();
}

//----------------------------------------------------------------------------



// static int menu_id;
// static int submenu_id;
//
// extern "C" void menu(int choice)
// {
//   cout << choice << endl;
// }
//
// void create_menu()
// {
//   //this is some example code from https://www.programming-techniques.com/2012/05/glut-tutorial-creating-menus-and-submenus-in-glut.html
//   submenu_id = glutCreateMenu(menu);
//
//   glutAddMenuEntry("Sphere", 2);
//   glutAddMenuEntry("Cone", 3);
//   glutAddMenuEntry("Torus", 4);
//   glutAddMenuEntry("Teapot", 5);
//
//   menu_id = glutCreateMenu(menu);
//
//   glutAddMenuEntry("Clear", 1);
//   glutAddSubMenu("Draw", submenu_id);
//   glutAddMenuEntry("Quit", 0);
//
//   glutAttachMenu(GLUT_RIGHT_BUTTON);
// }





//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // no MSAA


  glutInitContextVersion( 4, 5 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

  glutInitWindowSize(1366/2, 768/2);
  glutCreateWindow("GLUT");
  // glutFullScreen();


  // glutGameModeString("640x480:16@60");   //not working
  // glutEnterGameMode();

  glewInit();

  init();

  // create_menu();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  // glutReshapeFunc(reshape);

  glutMainLoop();
  return(EXIT_SUCCESS);
}
