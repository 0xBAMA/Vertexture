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
CloudModel*        clouds;
SkirtModel*        skirts;


//should you draw the models?
bool drawground = true;
bool drawwater = true;

bool drawclouds = false;


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
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}






void init()
{
  ground = new GroundModel();
  datmodel = new DudesAndTreesModel();
  water = new WaterModel();
  clouds = new CloudModel();
  skirts = new SkirtModel();


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
  datmodel->set_proj(proj);
  clouds->set_proj(proj);
  skirts->set_proj(proj);

  ground->set_scroll(scroll);
  datmodel->set_scroll(scroll);
  skirts->set_scroll(scroll);

  glEnable(GL_DEPTH_TEST);

  // glEnable(GL_DITHER); //something interesting to mess with in the future - might be depreciated, use a shader

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);





  //DEBUG

  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

  cout << endl << endl << " GL_DEBUG_OUTPUT ENABLED " << endl;




  // double phi = (1 + std::sqrt(5.0))/2.0;  //golden ratio, used to compute icosahedron
  // glClearColor(1/phi, 1/phi, 1/phi, 1.0); // grey background

  // glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

//----------------------------------------------------------------------------

void display()
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  if(rotate)
  {
    animation_time++;
    ground->set_time(animation_time);
    datmodel->set_time(animation_time);
    water->set_time(animation_time);
    clouds->set_time(animation_time);
    skirts->set_time(animation_time);
  }




//DRAW THE GROUND
  if(drawground)
    ground->display();



//DRAW THE WATER
  if(drawwater)
    water->display();

//DRAW THE DUDES AND TREES
  datmodel->display();


//DRAW THE CLOUDS
  if(drawclouds)
    clouds->display();

//DRAW THE SKIRTS
  skirts->display();



  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();

}

//----------------------------------------------------------------------------

//reshape

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
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




  case 'z':
    //toggle drawing of ground
    drawground = !drawground;
    break;

  case 'x': //cycle speeds
    switch(scroll)
    {
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

  case 'c':
    //toggle drawing of clouds
    drawclouds = !drawclouds;
    break;


  case 'v':
    //stop or start the rotation;
    temp_time = animation_time;
    rotate = !rotate;
    break;


  case 'b':
    ground->toggle_normals();
    break;
  }
  glutPostRedisplay();
}

//----------------------------------------------------------------------------



void mouse( int button, int state, int x, int y )
{
  if ( state == GLUT_DOWN )
	{
		switch( button )
		{
		    case GLUT_LEFT_BUTTON:    cout << "left" << endl;   break;
		    case GLUT_MIDDLE_BUTTON:  cout << "middle" << endl; break;
		    case GLUT_RIGHT_BUTTON:   cout << "right" << endl;  break;
		}

    if(button == GLUT_LEFT_BUTTON)
    {

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

      cout << "R: " << (int)pixel[0] << " which is x: " << calcx << endl;
      cout << "G: " << (int)pixel[1] << " which is y: " << calcy  << endl;
      cout << "B: " << (int)pixel[2];

      if((int) pixel[2] > 0)
        cout << " which is ... UNDERWATER" << endl;

      //do whatever you need to do

      //the r value ranges from 25 to 230 -> map to the range (-phi to phi)
      //the g value ranges from 25 to 230 -> map to the range (-phi to phi)
      //the b is usaully either 0 or 255  -> >0 tells you it's in the water

      datmodel->set_pos(glm::vec3(calcx/2, calcy/2, 0.0), glm::vec3(1,0,(float)pixel[2]));

      cout << endl;




      //clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glutPostRedisplay();

























    }
  }
}

//----------------------------------------------------------------------------

void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000.0/60.0, timer, 0);
}

//----------------------------------------------------------------------------



void idle( void )
{
	// glutPostRedisplay();
}


//----------------------------------------------------------------------------

// static int menu_id;
// static int submenu_id;
//
// void menu(int choice)
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

  // glutInitWindowSize(1366/2, 768/2);
  glutInitWindowSize(720,480);
  glutCreateWindow("GLUT");
  // glutFullScreen();



  glewInit();

  init();

  // create_menu();



  // glutGameModeString("640x480");   //not working, crashes shit - would be nice to get it working though
  // glutEnterGameMode();   //the main benefit I'm seeing is that it sets up a framebuffer of that dimension
                    //but there are other ways to achieve this effect.


  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc( mouse );
  glutIdleFunc( idle );
  glutTimerFunc(1000.0/60.0, timer, 0);
  // glutReshapeFunc(reshape);

  GLint textureCount;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureCount);

  cout << endl << " GL_MAX_TEXTURE_IMAGE_UNITS returned:" << textureCount << endl;


  GLint data[3];

  glGetIntegeri_v( 	GL_MAX_COMPUTE_WORK_GROUP_COUNT,0, &data[0]);
  glGetIntegeri_v( 	GL_MAX_COMPUTE_WORK_GROUP_COUNT,1, &data[1]);
  glGetIntegeri_v( 	GL_MAX_COMPUTE_WORK_GROUP_COUNT,2, &data[2]);

  cout << endl << " GL_MAX_COMPUTE_WORK_GROUP_COUNT returned x:" << data[0] << " y:" << data[1] << " z:" << data[2] << endl;


  GLint max;
  glGetIntegerv(  GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max);

  cout << endl << " GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS returned:" << max << endl << endl;



  // if (glewIsSupported("GL_EXT_shader_image_load_store"))
  // {  //supposedly you need this to write to a shader - (NOT ACCURATE 10/5 - but does require using image load/store)
  //   cout << " GL_EXT_shader_image_load_store is supported" << endl;
  // }





  //checking extensions

  cout << "list of extensions: " << endl;

  GLint n=0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &n);

  for (GLint i=0; i<n; i++)
  {
    const char* extension =
      (const char*)glGetStringi(GL_EXTENSIONS, i);
    printf("      Ext %d: %s\n", i, extension);
  }




  glutMainLoop();
  return(EXIT_SUCCESS);
}
