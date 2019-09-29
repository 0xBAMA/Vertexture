//******************************************************************************
//  Program: vertexture
//
//  Author: Jon Baker
//  Email: jb239812@ohio.edu
//
//  Description: This is an experiment to see if you can displace verticies with
//    the values contained in a texture. This will be done by generating a large
//    number of triangles on a flat shape, in this case a subdivided square,
//    then loading a texture then finding a texture coordinate for each vertex
//    which is based on the location of that vertex. In theory, this will allow
//    animation, by simply adding a variable offset to the texture coordinate.
//
//  Date: 28 September 2019
//******************************************************************************


#include <vector>
#include <iostream>
using std::cout;
using std::endl;

#include <random>

#define MIN_POINT_PLACEMENT_THRESHOLD 0.05
#define GLOBAL_POINTSIZE 7.5f

//**********************************************

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>


// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>


// Shader Compilation
#include "shaders/Shader.h"

//**********************************************

#include "../resources/LodePNG/lodepng.h"
// Good, simple png library

//**********************************************

#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp" //general vector types
#include "glm/gtc/matrix_transform.hpp" // for glm::ortho
#include "glm/gtc/type_ptr.hpp" //to send matricies gpu-side


//******************************************************************************
//  Function: planetest
//
//  NOTE: This comes up with some frequency. I'll be carrying this along for a while.
//
//  Purpose:
//      Return true if the test point is below the plane. Return false if the
//      test point is below the plane. Above and below are determined with
//      respect to the normal specified by plane_norm. This is used to confirm
//      that computed normals are actually pointing outwards, by testing a
//      point that is known to be inside the shape against the computed normal.
//      If the point is below the plane specified by plane_norm and plane_point,
//      we know that that normal will be valid. Otherwise, it needs to be
//      inverted.
//
//  Parameters:
//      plane_point - the point from which the normal will originate
//      plane_norm - what direction is 'up'?
//      test_point - you want to know if this is above or below the plane
//
//  Preconditions:
//      plane_norm must be given as a nonzero vector
//
//  Postconditions:
//      true or false is returned to tell the user the restult of their query
//
//******************************************************************************

bool planetest(glm::vec3 plane_point, glm::vec3 plane_norm, glm::vec3 test_point)
{
  double result, a, b, c, x, x1, y, y1, z, z1;

  a  =  plane_norm.x;   b  =  plane_norm.y;  c  =  plane_norm.z;
  x  =  test_point.x;   y  =  test_point.y;  z  =  test_point.z;
  x1 = plane_point.x;   y1 = plane_point.y;  z1 = plane_point.z;

  //equation of a plane is:
    // a (x-x1) + b (y-y1) + c (z-z1) = 0;

  result = a * (x-x1) + b * (y-y1) + c * (z-z1);

  return (result < 0) ? true:false;
}







//******************************************************************************
//  Class: GroundModel
//
//  Purpose:  To represent the ground on the GPU, and everything that goes along
//        with displaying this ball to the user.
//
//  Functions:
//
//    Constructor:
//        Takes no arguments, calls generate_points() to create geometry. Then
//        buffers all this data to the GPU memory.
//
//    Setters:
//        Used to update the values of the uniform variables.
//
//    Generate Points:
//        Creates a square, subdivides the faces several times, and creates
//        triangles to span the shape. This data is used to populate the
//        vectors containing point data.
//
//    Display:
//        Makes sure the correct shader is being used, that the correct buffers
//        are bound, that the vertex attributes are set up, and that all the
//        latest values of the uniform variables are sent to the GPU.
//******************************************************************************


class GroundModel
{
public:

  GroundModel();

  void display();

  void set_time(int tin)        {time = tin;}
  void set_proj(glm::mat4 pin)  {proj = pin;}




private:
  GLuint vao;
  GLuint buffer;
  GLuint tex;

  GLuint shader_program;

  int num_pts; //how many points?

//VERTEX ATTRIB LOCATIONS
  GLuint vPosition;
  // GLuint vNormal;
  // GLuint vColor;

//UNIFORM LOCATIONS
  GLuint uTime;   //animation time
  GLuint uProj;   //projection matrix


//VALUES OF THOSE UNIFORMS
  int time;
  glm::mat4 proj;

  void generate_points();
  void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

  std::vector<glm::vec3> points;    //add the 1.0 w value in the shader
  // std::vector<glm::vec3> normals;
  // std::vector<glm::vec3> colors;
};

  //****************************************************************************
  //  Function: GroundModel Constructor
  //
  //  Purpose:
  //    Calls generate_points() and then sets up everything related to the GPU
  //****************************************************************************


GroundModel::GroundModel()
{

  //initialize all the vectors
  points.clear();

  //fill those vectors with geometry
  generate_points();


//SETTING UP GPU STUFF
  //VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  //BUFFER, SEND DATA
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  int num_bytes_points = sizeof(glm::vec3) * points.size();
  // int num_bytes_normals = sizeof(glm::vec3) * normals.size();
  // int num_bytes_colors = sizeof(glm::vec3) * colors.size();

  // glBufferData(GL_ARRAY_BUFFER, num_bytes_points + num_bytes_normals + num_bytes_colors, NULL, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);
  // glBufferSubData(GL_ARRAY_BUFFER, num_bytes_points, num_bytes_normals, &normals[0]);
  // glBufferSubData(GL_ARRAY_BUFFER, num_bytes_points + num_bytes_normals, num_bytes_colors, &colors[0]);

  //SHADERS (COMPILE, USE)


  // shader_program = InitShader("shaders/vSphere.glsl", "shaders/fSphere.glsl");

  Shader s("resources/shaders/ground_vert.glsl", "resources/shaders/ground_frag.glsl");

  shader_program = s.Program;

  glUseProgram(shader_program);

  //VERTEX ATTRIB AND UNIFORM LOCATIONS

  // Initialize the vertex position attribute from the vertex shader
  vPosition = glGetAttribLocation(shader_program, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*) (0)));

  //UNIFORMS
  uTime = glGetUniformLocation(shader_program, "t");
  glUniform1i(uTime, time);

  uProj = glGetUniformLocation(shader_program, "proj");
  proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));





  //THE TEXTURE

  std::vector<unsigned char> image;

  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, "resources/rock_cave.png", LodePNGColorType::LCT_RGBA, 8);

  // for(auto el:image)
  // {
  //   cout << el;  //I'm getting an image...
  // }

  // If there's an error, display it.
  if(error != 0) {
    std::cout << "error with lodepng texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
  }

  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

  glGenerateMipmap(GL_TEXTURE_2D);

  glPointSize(GLOBAL_POINTSIZE);

}

  //****************************************************************************
  //  Function: GroundModel::generate_points()
  //
  //  Purpose:
  //    This function produces all the data for representing this object.
  //****************************************************************************

void GroundModel::generate_points()
{
  //GENERATING GEOMETRY

  glm::vec3 a, b, c, d;

  float scale = 1.618f;
  // float scale = 0.9f;

  a = glm::vec3(-1.0f*scale, -1.0f*scale, 0.0f);
  b = glm::vec3(-1.0f*scale, 1.0f*scale, 0.0f);
  c = glm::vec3(1.0f*scale, -1.0f*scale, 0.0f);
  d = glm::vec3(1.0f*scale, 1.0f*scale, 0.0f);

  subd_square(a, b, c, d);

  num_pts = points.size();

  // cout << "num_pts is " << num_pts << endl;

}

void GroundModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
  if(glm::distance(a, b) < MIN_POINT_PLACEMENT_THRESHOLD)
  {//add points
    //triangle 1 ABC
    // points.push_back(a);
    // points.push_back(b);
    // points.push_back(c);
    // //triangle 2 BCD
    // points.push_back(b);
    // points.push_back(c);
    // points.push_back(d);

    //middle
    points.push_back((a+b+c+d)/4.0f);
  }
  else
  { //recurse
    glm::vec3 center = (a + b + c + d) / 4.0f;    //center of the square

    glm::vec3 bdmidp = (b + d) / 2.0f;            //midpoint between b and d
    glm::vec3 abmidp = (a + b) / 2.0f;            //midpoint between a and b
    glm::vec3 cdmidp = (c + d) / 2.0f;            //midpoint between c and d
    glm::vec3 acmidp = (a + c) / 2.0f;            //midpoint between a and c

    subd_square(abmidp, b, center, bdmidp);
    subd_square(a, abmidp, acmidp, center);
    subd_square(center, bdmidp, cdmidp, d);
    subd_square(acmidp, center, c, cdmidp);
  }
}




  //****************************************************************************
  //  Function: GroundModel::display()
  //
  //  Purpose:
  //    This function does all the setup for the buffers and uniforms and then
  //    issues a draw call for the geometry representing this object
  //****************************************************************************

void GroundModel::display()
{
  glBindVertexArray(vao);
  glUseProgram(shader_program);

  glBindTexture(GL_TEXTURE_2D, tex);

  glUniform1i(uTime, time);
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));
  // glUniform1i(uScan, scan);
  // glUniform1i(uDcol, dcolor);

  glDrawArrays(GL_POINTS, 0, num_pts);

  // glDrawArrays(GL_TRIANGLES, 0, num_pts);
}



























//******************************************************************************
//  Class: WaterModel
//
//  Purpose:  To represent the ground on the GPU, and everything that goes along
//        with displaying this ball to the user.
//
//  Functions:
//
//    Constructor:
//        Takes no arguments, calls generate_points() to create geometry. Then
//        buffers all this data to the GPU memory.
//
//    Setters:
//        Used to update the values of the uniform variables.
//
//    Generate Points:
//        Creates a square, subdivides the faces several times, and creates
//        triangles to span the shape. This data is used to populate the
//        vectors containing point data.
//
//    Display:
//        Makes sure the correct shader is being used, that the correct buffers
//        are bound, that the vertex attributes are set up, and that all the
//        latest values of the uniform variables are sent to the GPU.
//******************************************************************************


class WaterModel
{
public:

  WaterModel();

  void display();

  void set_time(int tin)        {time = tin;}
  void set_proj(glm::mat4 pin)  {proj = pin;}

  void increase_thresh()        {thresh += 0.01; cout << thresh << endl;}
  void decrease_thresh()        {thresh -= 0.01; cout << thresh << endl;}


private:
  GLuint vao;
  GLuint buffer;
  GLuint tex;

  GLuint shader_program;

  int num_pts; //how many points?

//VERTEX ATTRIB LOCATIONS
  GLuint vPosition;
  // GLuint vNormal;
  // GLuint vColor;

//UNIFORM LOCATIONS
  GLuint uTime;   //animation time
  GLuint uProj;   //projection matrix
  GLuint uThresh;   //cutoff for water


//VALUES OF THOSE UNIFORMS
  int time;
  float thresh;
  glm::mat4 proj;

  void generate_points();
  void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

  std::vector<glm::vec3> points;    //add the 1.0 w value in the shader
  // std::vector<glm::vec3> normals;
  // std::vector<glm::vec3> colors;
};

  //****************************************************************************
  //  Function: WaterModel Constructor
  //
  //  Purpose:
  //    Calls generate_points() and then sets up everything related to the GPU
  //****************************************************************************


WaterModel::WaterModel()
{

  //initialize all the vectors
  points.clear();

  //fill those vectors with geometry
  generate_points();


//SETTING UP GPU STUFF
  //VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  //BUFFER, SEND DATA
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  int num_bytes_points = sizeof(glm::vec3) * points.size();
  // int num_bytes_normals = sizeof(glm::vec3) * normals.size();
  // int num_bytes_colors = sizeof(glm::vec3) * colors.size();

  // glBufferData(GL_ARRAY_BUFFER, num_bytes_points + num_bytes_normals + num_bytes_colors, NULL, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);
  // glBufferSubData(GL_ARRAY_BUFFER, num_bytes_points, num_bytes_normals, &normals[0]);
  // glBufferSubData(GL_ARRAY_BUFFER, num_bytes_points + num_bytes_normals, num_bytes_colors, &colors[0]);

  //SHADERS (COMPILE, USE)

  Shader s("resources/shaders/water_vert.glsl", "resources/shaders/water_frag.glsl");

  shader_program = s.Program;

  glUseProgram(shader_program);

  //VERTEX ATTRIB AND UNIFORM LOCATIONS

  // Initialize the vertex position attribute from the vertex shader
  vPosition = glGetAttribLocation(shader_program, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*) (0)));

  //UNIFORMS
  uTime = glGetUniformLocation(shader_program, "t");
  glUniform1i(uTime, time);

  uProj = glGetUniformLocation(shader_program, "proj");
  proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));


  thresh = 0.56f;
  uThresh = glGetUniformLocation(shader_program, "thresh");
  glUniform1f(uThresh, thresh);





  //THE TEXTURE

  std::vector<unsigned char> image;

  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, "resources/rock_cave.png", LodePNGColorType::LCT_RGBA, 8);

  // for(auto el:image)
  // {
  //   cout << el;  //I'm getting an image...
  // }

  // If there's an error, display it.
  if(error != 0) {
    std::cout << "error with lodepng texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
  }

  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

  glGenerateMipmap(GL_TEXTURE_2D);

  glPointSize(GLOBAL_POINTSIZE);

}

  //****************************************************************************
  //  Function: WaterModel::generate_points()
  //
  //  Purpose:
  //    This function produces all the data for representing this object.
  //****************************************************************************

void WaterModel::generate_points()
{
  //GENERATING GEOMETRY

  glm::vec3 a, b, c, d;

  // float scale = 0.9f;
  float scale = 1.618f;

  a = glm::vec3(-1.0f*scale, -1.0f*scale, 0.0f);
  b = glm::vec3(-1.0f*scale, 1.0f*scale, 0.0f);
  c = glm::vec3(1.0f*scale, -1.0f*scale, 0.0f);
  d = glm::vec3(1.0f*scale, 1.0f*scale, 0.0f);

  subd_square(a, b, c, d);

  num_pts = points.size();

  // cout << "num_pts is " << num_pts << endl;

}

void WaterModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
  // if(glm::distance(a, b) < MIN_POINT_PLACEMENT_THRESHOLD)
  if(glm::distance(a, b) < MIN_POINT_PLACEMENT_THRESHOLD * 5)
  {//add points
    //triangle 1 ABC
    // points.push_back(a);
    // points.push_back(b);
    // points.push_back(c);
    // //triangle 2 BCD
    // points.push_back(b);
    // points.push_back(c);
    // points.push_back(d);

    //middle
    points.push_back((a+b+c+d)/4.0f);
  }
  else
  { //recurse
    glm::vec3 center = (a + b + c + d) / 4.0f;    //center of the square

    glm::vec3 bdmidp = (b + d) / 2.0f;            //midpoint between b and d
    glm::vec3 abmidp = (a + b) / 2.0f;            //midpoint between a and b
    glm::vec3 cdmidp = (c + d) / 2.0f;            //midpoint between c and d
    glm::vec3 acmidp = (a + c) / 2.0f;            //midpoint between a and c

    subd_square(abmidp, b, center, bdmidp);
    subd_square(a, abmidp, acmidp, center);
    subd_square(center, bdmidp, cdmidp, d);
    subd_square(acmidp, center, c, cdmidp);
  }
}




  //****************************************************************************
  //  Function: WaterModel::display()
  //
  //  Purpose:
  //    This function does all the setup for the buffers and uniforms and then
  //    issues a draw call for the geometry representing this object
  //****************************************************************************

void WaterModel::display()
{
  glBindVertexArray(vao);
  glUseProgram(shader_program);

  glBindTexture(GL_TEXTURE_2D, tex);

  glUniform1i(uTime, time);
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));
  glUniform1f(uThresh, thresh);

  glDrawArrays(GL_POINTS, 0, num_pts);

  // glDrawArrays(GL_TRIANGLES, 0, num_pts);
}




















//******************************************************************************
//  Class: CloudModel
//
//  Purpose:  To represent the ground on the GPU, and everything that goes along
//        with displaying this ball to the user.
//
//  Functions:
//
//    Constructor:
//        Takes no arguments, calls generate_points() to create geometry. Then
//        buffers all this data to the GPU memory.
//
//    Setters:
//        Used to update the values of the uniform variables.
//
//    Generate Points:
//        Creates a square, subdivides the faces several times, and creates
//        triangles to span the shape. This data is used to populate the
//        vectors containing point data.
//
//    Display:
//        Makes sure the correct shader is being used, that the correct buffers
//        are bound, that the vertex attributes are set up, and that all the
//        latest values of the uniform variables are sent to the GPU.
//******************************************************************************


class CloudModel
{
public:

  CloudModel();

  void display();

  void set_time(int tin)        {time = tin;}
  void set_proj(glm::mat4 pin)  {proj = pin;}

  void increase_thresh()        {thresh += 0.01; cout << thresh << endl;}
  void decrease_thresh()        {thresh -= 0.01; cout << thresh << endl;}


private:
  GLuint vao;
  GLuint buffer;
  GLuint tex;

  GLuint shader_program;

  int num_pts; //how many points?

//VERTEX ATTRIB LOCATIONS
  GLuint vPosition;
  // GLuint vNormal;
  // GLuint vColor;

//UNIFORM LOCATIONS
  GLuint uTime;   //animation time
  GLuint uProj;   //projection matrix
  GLuint uThresh;   //cutoff for water


//VALUES OF THOSE UNIFORMS
  int time;
  float thresh;
  glm::mat4 proj;

  void generate_points();
  void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

  std::vector<glm::vec3> points;    //add the 1.0 w value in the shader
  // std::vector<glm::vec3> normals;
  // std::vector<glm::vec3> colors;
};

  //****************************************************************************
  //  Function: CloudModel Constructor
  //
  //  Purpose:
  //    Calls generate_points() and then sets up everything related to the GPU
  //****************************************************************************


CloudModel::CloudModel()
{

  //initialize all the vectors
  points.clear();

  //fill those vectors with geometry
  generate_points();


//SETTING UP GPU STUFF
  //VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  //BUFFER, SEND DATA
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  int num_bytes_points = sizeof(glm::vec3) * points.size();
  // int num_bytes_normals = sizeof(glm::vec3) * normals.size();
  // int num_bytes_colors = sizeof(glm::vec3) * colors.size();

  // glBufferData(GL_ARRAY_BUFFER, num_bytes_points + num_bytes_normals + num_bytes_colors, NULL, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);
  // glBufferSubData(GL_ARRAY_BUFFER, num_bytes_points, num_bytes_normals, &normals[0]);
  // glBufferSubData(GL_ARRAY_BUFFER, num_bytes_points + num_bytes_normals, num_bytes_colors, &colors[0]);

  //SHADERS (COMPILE, USE)

  Shader s("resources/shaders/cloud_vert.glsl", "resources/shaders/cloud_frag.glsl");

  shader_program = s.Program;

  glUseProgram(shader_program);

  //VERTEX ATTRIB AND UNIFORM LOCATIONS

  // Initialize the vertex position attribute from the vertex shader
  vPosition = glGetAttribLocation(shader_program, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*) (0)));

  //UNIFORMS
  uTime = glGetUniformLocation(shader_program, "t");
  glUniform1i(uTime, time);

  uProj = glGetUniformLocation(shader_program, "proj");
  proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));


  thresh = 0.56f;
  uThresh = glGetUniformLocation(shader_program, "thresh");
  glUniform1f(uThresh, thresh);





  //THE TEXTURE

  std::vector<unsigned char> image;

  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, "resources/rock_cave.png", LodePNGColorType::LCT_RGBA, 8);

  // for(auto el:image)
  // {
  //   cout << el;  //I'm getting an image...
  // }

  // If there's an error, display it.
  if(error != 0) {
    std::cout << "error with lodepng texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
  }

  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

  glGenerateMipmap(GL_TEXTURE_2D);

  glPointSize(GLOBAL_POINTSIZE);

}

  //****************************************************************************
  //  Function: CloudModel::generate_points()
  //
  //  Purpose:
  //    This function produces all the data for representing this object.
  //****************************************************************************

void CloudModel::generate_points()
{
  //GENERATING GEOMETRY

  glm::vec3 a, b, c, d;

  // float scale = 0.9f;
  float scale = 1.618f;

  a = glm::vec3(-1.0f*scale, -1.0f*scale, 0.0f);
  b = glm::vec3(-1.0f*scale, 1.0f*scale, 0.0f);
  c = glm::vec3(1.0f*scale, -1.0f*scale, 0.0f);
  d = glm::vec3(1.0f*scale, 1.0f*scale, 0.0f);

  subd_square(a, b, c, d);

  num_pts = points.size();

  // cout << "num_pts is " << num_pts << endl;

}

void CloudModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
  if(glm::distance(a, b) < MIN_POINT_PLACEMENT_THRESHOLD)
  {//add points
    //triangle 1 ABC
    // points.push_back(a);
    // points.push_back(b);
    // points.push_back(c);
    // //triangle 2 BCD
    // points.push_back(b);
    // points.push_back(c);
    // points.push_back(d);

    //middle
    points.push_back((a+b+c+d)/4.0f);
  }
  else
  { //recurse
    glm::vec3 center = (a + b + c + d) / 4.0f;    //center of the square

    glm::vec3 bdmidp = (b + d) / 2.0f;            //midpoint between b and d
    glm::vec3 abmidp = (a + b) / 2.0f;            //midpoint between a and b
    glm::vec3 cdmidp = (c + d) / 2.0f;            //midpoint between c and d
    glm::vec3 acmidp = (a + c) / 2.0f;            //midpoint between a and c

    subd_square(abmidp, b, center, bdmidp);
    subd_square(a, abmidp, acmidp, center);
    subd_square(center, bdmidp, cdmidp, d);
    subd_square(acmidp, center, c, cdmidp);
  }
}




  //****************************************************************************
  //  Function: CloudModel::display()
  //
  //  Purpose:
  //    This function does all the setup for the buffers and uniforms and then
  //    issues a draw call for the geometry representing this object
  //****************************************************************************

void CloudModel::display()
{
  glBindVertexArray(vao);
  glUseProgram(shader_program);

  glBindTexture(GL_TEXTURE_2D, tex);

  glUniform1i(uTime, time);
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));
  glUniform1f(uThresh, thresh);

  glDrawArrays(GL_POINTS, 0, num_pts);

  // glDrawArrays(GL_TRIANGLES, 0, num_pts);
}
