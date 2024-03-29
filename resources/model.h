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
#include <random>
#include <vector>
#include <iostream>
using std::cout;
using std::endl;

#define MIN_POINT_PLACEMENT_THRESHOLD 0.01f
#define GLOBAL_POINTSIZE 7.5f

#define POINT_SPRITE_PATH "resources/textures/height/sphere_small.png"

#define GROUND_NORMAL_PATH "resources/textures/normals/rock_norm.png"
#define GROUND_NORMAL2_PATH "resources/textures/normals/rock_norm_smooth1.png"
#define GROUND_NORMAL3_PATH "resources/textures/normals/rock_norm_smooth2.png"

#define GROUND_TEXTURE_PATH "resources/textures/height/rock_height.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/penny.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/fort_ross_lidar.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/MtRuapehuandNgauruhoe.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/pugetSound.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/volcano.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/Grts_RiverValley.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/hmpoland2048.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/nasa_rover_practice.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/sphere.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/bears.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/bears2.png"
// #define GROUND_TEXTURE_PATH "resources/textures/height/united-kingdom-2048.png"

#define WATER_HEIGHT_TEXTURE "resources/textures/height/water_height.png"
#define WATER_NORMAL_TEXTURE "resources/textures/normal/water_normal.png"
#define WATER_COLOR_TEXTURE "resources/textures/water_color.png"

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

class GroundModel {
public:
	GroundModel();

	void display(bool select=false);

	void set_time(int tin)        {time = tin;}
	void set_scroll(int sin)      {scroll = sin;}
	void toggle_normals()         {if(show_normals==0){show_normals=1;}else{show_normals=0;}}
	void scale_up()               {scale *= 1.618f;}
	void scale_down()             {scale /= 1.618f;}
	void set_proj(glm::mat4 pin)  {proj = pin;}

private:
	GLuint vao;
	GLuint buffer;

	GLuint height_tex;
	GLuint normal_tex_1;
	GLuint normal_tex_2;
	GLuint normal_tex_3;

	GLuint shader_program;
	GLuint selection_shader_program;

	int num_pts; //how many points?

	//VERTEX ATTRIB LOCATIONS
	GLuint vPosition;

	//UNIFORM LOCATIONS
	GLuint uTime;   //animation time
	GLuint uProj;   //projection matrix
	GLuint uScroll;
	GLuint uScale;
	GLuint uNorm;

	GLuint uHeightSampler;
	GLuint uNormal1Sampler;
	GLuint uNormal2Sampler;
	GLuint uNormal3Sampler;

	//VALUES OF THOSE UNIFORMS
	int time;
	int show_normals;
	int scroll;
	float scale;

	glm::mat4 proj;

	void generate_points();
	void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

	std::vector<glm::vec3> points;    //add the 1.0 w value in the shader
};

//****************************************************************************
//  Function: GroundModel Constructor
//
//  Purpose:
//    Calls generate_points() and then sets up everything related to the GPU
//****************************************************************************
GroundModel::GroundModel() {

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
	glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);

	//SHADERS (COMPILE, USE)
	cout << " compiling ground shaders" << endl;
	Shader s("resources/shaders/ground_vert.glsl", "resources/shaders/ground_frag.glsl");
	Shader s2("resources/shaders/ground_sel_vert.glsl", "resources/shaders/ground_sel_frag.glsl");

	shader_program = s.Program;
	selection_shader_program = s2.Program;

	glUseProgram(shader_program);
	glUseProgram(selection_shader_program);

	//VERTEX ATTRIB AND UNIFORM LOCATIONS

	// Initialize the vertex position attribute from the vertex shader
	vPosition = glGetAttribLocation(shader_program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*) (0)));

	//UNIFORMS
	uTime = glGetUniformLocation(shader_program, "t");
	glUniform1i(uTime, time);

	uScroll = glGetUniformLocation(shader_program, "scroll");
	glUniform1i(uScroll, scroll);

	uNorm = glGetUniformLocation(shader_program, "show_normals");
	glUniform1i(uNorm, show_normals);

	uProj = glGetUniformLocation(shader_program, "proj");
	proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));

	scale = 1.0;
	uScale = glGetUniformLocation(shader_program, "scale");
	glUniform1f(uScale, scale);

	//THE TEXTURE

	std::vector<unsigned char> image;
	std::vector<unsigned char> image2;
	std::vector<unsigned char> image3;
	std::vector<unsigned char> image4;

	unsigned width, height;
	unsigned width2, height2;
	unsigned width3, height3;
	unsigned width4, height4;

	unsigned error = lodepng::decode(image, width, height, GROUND_TEXTURE_PATH, LodePNGColorType::LCT_RGBA, 8);
	unsigned error2 = lodepng::decode(image2, width2, height2, GROUND_NORMAL_PATH, LodePNGColorType::LCT_RGBA, 8);
	unsigned error3 = lodepng::decode(image3, width3, height3, GROUND_NORMAL2_PATH, LodePNGColorType::LCT_RGBA, 8);
	unsigned error4 = lodepng::decode(image4, width4, height4, GROUND_NORMAL3_PATH, LodePNGColorType::LCT_RGBA, 8);

	// If there's an error, display it.
	if(error != 0) {
		std::cout << "error with lodepng texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	if(error2 != 0) {
		std::cout << "error2 with lodepng texture loading " << error2 << ": " << lodepng_error_text(error2) << std::endl;
	}

	if(error3 != 0) {
		std::cout << "error3 with lodepng texture loading " << error3 << ": " << lodepng_error_text(error3) << std::endl;
	}

	if(error4 != 0) {
		std::cout << "error4 with lodepng texture loading " << error4 << ": " << lodepng_error_text(error4) << std::endl;
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &height_tex);
	glBindTexture(GL_TEXTURE_2D, height_tex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	cout << " loaded height texture" << endl;

	glGenTextures(1, &normal_tex_1);
	glBindTexture(GL_TEXTURE_2D, normal_tex_1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded normal texture" << endl;

	glGenTextures(1, &normal_tex_2);
	glBindTexture(GL_TEXTURE_2D, normal_tex_2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image3[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded normal texture2" << endl;



	glGenTextures(1, &normal_tex_3);
	glBindTexture(GL_TEXTURE_2D, normal_tex_3);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image4[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded normal texture3" << endl;

	uHeightSampler = glGetUniformLocation(shader_program, "height_tex");
	uNormal1Sampler = glGetUniformLocation(shader_program, "normal_tex");
	uNormal2Sampler = glGetUniformLocation(shader_program, "normal_smooth1_tex");
	uNormal3Sampler = glGetUniformLocation(shader_program, "normal_smooth2_tex");

	glUniform1i(uHeightSampler, 0);  //height in texture unit 0
	glUniform1i(uNormal1Sampler, 1);  //normal1 goes in texture unit 1
	glUniform1i(uNormal2Sampler, 2);  //normal2 goes in texture unit 2
	glUniform1i(uNormal3Sampler, 3);  //normal3 goes in texture unit 3
}

//****************************************************************************
//  Function: GroundModel::generate_points()
//
//  Purpose:
//    This function produces all the data for representing this object.
//****************************************************************************
void GroundModel::generate_points() {
	//GENERATING GEOMETRY
	glm::vec3 a, b, c, d;
	float scale = 1.618f;

	a = glm::vec3(-1.0f*scale, -1.0f*scale, 0.0f);
	b = glm::vec3(-1.0f*scale, 1.0f*scale, 0.0f);
	c = glm::vec3(1.0f*scale, -1.0f*scale, 0.0f);
	d = glm::vec3(1.0f*scale, 1.0f*scale, 0.0f);

	subd_square(a, b, c, d);
	num_pts = points.size();
}

void GroundModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
	if(glm::distance(a, b) < MIN_POINT_PLACEMENT_THRESHOLD) {//add points
		// triangle 1 ABC
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		//triangle 2 BCD
		points.push_back(b);
		points.push_back(c);
		points.push_back(d);

		//middle
		// points.push_back((a+b+c+d)/4.0f);
	} else { //recurse
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

void GroundModel::display(bool select) {
	glBindVertexArray(vao);

	if(select) {
		glUseProgram(selection_shader_program);

		glUniform1i(uTime, time);
		glUniform1i(uScroll, scroll);
		glUniform1f(uScale, scale);
		// glUniform1i(uNorm, show_normals);

		glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
		glBindTexture(GL_TEXTURE_2D, height_tex);

		glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
		glBindTexture(GL_TEXTURE_2D, normal_tex_1);

		glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
		glBindTexture(GL_TEXTURE_2D, normal_tex_2);

		glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
		glBindTexture(GL_TEXTURE_2D, normal_tex_3);


		glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));

		glDrawArrays(GL_TRIANGLES, 0, num_pts);
	} else {
		glUseProgram(shader_program);

		glUniform1i(uTime, time);
		glUniform1i(uScroll, scroll);
		glUniform1f(uScale, scale);
		glUniform1i(uNorm, show_normals);

		glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
		glBindTexture(GL_TEXTURE_2D, height_tex);

		glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
		glBindTexture(GL_TEXTURE_2D, normal_tex_1);

		glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
		glBindTexture(GL_TEXTURE_2D, normal_tex_2);

		glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
		glBindTexture(GL_TEXTURE_2D, normal_tex_3);

		glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));

		glDrawArrays(GL_TRIANGLES, 0, num_pts);
	}
}

//******************************************************************************
//  Class: DudesAndTreesModel
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

class DudesAndTreesModel {
	typedef struct entity_t {
		glm::vec3 location; //really only using x,y
		int type; //0 good, 1 bad, 2 tree, 3 box
		bool dead;
	} entity;

public:
	DudesAndTreesModel(int num_good_guys, int num_bad_guys, int num_trees, int num_boxes_initial);

	void display();
	void update_sim();  //called from timer function
	void handle_click(glm::vec3 pixel_read);  //called from mouse callback
	void set_time(int tin)        {time = tin;}
	void set_scroll(int sin)      {scroll = sin;}
	void scale_up()               {scale *= 1.618f;}
	void scale_down()             {scale /= 1.618f;}
	void set_proj(glm::mat4 pin)  {proj = pin;}

	int get_score()               {return score;}
	int get_status()              {return status;}
	int get_boxes_left()          {return boxes_left;}

	void toggle_cursor_draw()     {cursor_draw = !cursor_draw;}

	void set_pos(glm::vec3 pin, glm::vec3 cin)   {point_sprite_position = pin; point_sprite_color = cin;}

	bool big_radius;

private:
	GLuint vao;
	GLuint buffer;
	GLuint ground_tex;
	GLuint ground_norm_tex;
	GLuint point_sprite;
	GLuint shader_program;

	std::vector<entity> entities;

	int num_box_pts, num_tree_pts, num_treetop_pts, box_start, boxes_left, score, status; //how many points?

	//VERTEX ATTRIB LOCATIONS
	GLuint vPosition;

	//UNIFORM LOCATIONS
	GLuint uTime;   //animation time
	GLuint uProj;   //projection matrix
	GLuint uScroll;
	GLuint uScale;
	GLuint uColor;
	GLuint uPosition;
	GLuint uBounce;

	GLuint uHeightSampler;  //textures
	GLuint uNormalSampler;
	GLuint uPointSpriteSampler;

	//VALUES OF THOSE UNIFORMS
	int time, bounce;
	int scroll;
	float scale;

	glm::vec3 point_sprite_color;
	glm::vec3 point_sprite_position;

	glm::mat4 proj;

	void generate_points();
	void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

	std::vector<glm::vec3> points;    //add the 1.0 w value in the shader

	bool cursor_draw;
};

  //****************************************************************************
  //  Function: DudesAndTreesModel Constructor
  //
  //  Purpose:
  //    Calls generate_points() and then sets up everything related to the GPU
  //****************************************************************************

DudesAndTreesModel::DudesAndTreesModel(int num_good_guys, int num_bad_guys, int num_trees, int num_boxes_initial) {
	cursor_draw = false;
	big_radius = false;

	//initialize all the vectors
	points.clear();

	//fill those vectors with geometry
	generate_points();

	cout << " Parameters of the game are as follows:" << endl;
	cout << "  " << num_good_guys << " good guys,";
	cout << "  " << num_bad_guys << " bad guys,";
	cout << "  " << num_trees << " tree(s),";
	cout << "  " << num_boxes_initial << " boxes" << endl << endl;

	boxes_left = num_boxes_initial;
	score = 0;

	// //generate a list of entitites
	//random number generation
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(-0.8f, 0.8f);

	//POPULATE THE LIST OF ENTITIES
	entity temp;

	//generate good guys starting locations
	for (int gg = 0; gg < num_good_guys; gg++) {
		//generate a good guy
		temp.location = glm::vec3(dist(mt),dist(mt),0); //change to random location
		temp.type = 0;
		temp.dead = false;
		entities.push_back(temp);
	}

	//generate bad guys starting locations
	for (int bg = 0; bg < num_bad_guys; bg++) {
		//generate a bad guy
		temp.location = glm::vec3(dist(mt),dist(mt),0); //change to random location
		temp.type = 1;
		temp.dead = false;
		entities.push_back(temp);
	}

	//generate tree locations
	for (int tc = 0; tc < num_trees; tc++) {
		//generate a tree
		temp.location = glm::vec3(dist(mt),dist(mt),0); //change to random location
		temp.type = 2;
		entities.push_back(temp);
	}

	for (int bx = 0; bx < num_boxes_initial; bx++) {
		//generate a tree
		temp.location = glm::vec3(0.9,0.8-bx*0.1,0); //change to random location
		temp.type = 3;
		// temp.dead = bx % 2;
		temp.dead = true;
		entities.push_back(temp);
	}

	//SETTING UP GPU STUFF
	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//BUFFER, SEND DATA
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	int num_bytes_points = sizeof(glm::vec3) * points.size();
	glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);

	//SHADERS (COMPILE, USE)
	cout << " compiling dudesandtrees shaders" << endl;
	Shader s("resources/shaders/dudesandtrees_vert.glsl", "resources/shaders/dudesandtrees_frag.glsl");

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

	uBounce = glGetUniformLocation(shader_program, "bounce");
	glUniform1i(uBounce, bounce);

	uScroll = glGetUniformLocation(shader_program, "scroll");
	glUniform1i(uScroll, scroll);

	uProj = glGetUniformLocation(shader_program, "proj");
	proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));

	scale = 1.0;
	uScale = glGetUniformLocation(shader_program, "scale");
	glUniform1f(uScale, scale);


	point_sprite_color = glm::vec3(0.0,0.0,0.0);
	uColor = glGetUniformLocation(shader_program, "ucolor");
	glUniform3fv(uColor, 1, glm::value_ptr(point_sprite_color));

	point_sprite_position = glm::vec3(0.0,0.0,0.0);
	uPosition = glGetUniformLocation(shader_program, "offset");
	glUniform3fv(uPosition, 1, glm::value_ptr(point_sprite_position));

	//THE TEXTURE

	std::vector<unsigned char> image;
	std::vector<unsigned char> image2;
	std::vector<unsigned char> image3;

	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, GROUND_TEXTURE_PATH, LodePNGColorType::LCT_RGBA, 8);

	unsigned width2, height2;
	unsigned error2 = lodepng::decode(image2, width2, height2, GROUND_NORMAL_PATH, LodePNGColorType::LCT_RGBA, 8);

	unsigned width3, height3;
	unsigned error3 = lodepng::decode(image3, width3, height3, POINT_SPRITE_PATH, LodePNGColorType::LCT_RGBA, 8);

	// If there's an error, display it.
	if(error != 0) {
		std::cout << "  error with lodepng ground height texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	if(error2 != 0) {
		std::cout << "  error with lodepng ground normal texture loading " << error2 << ": " << lodepng_error_text(error2) << std::endl;
	}

	if(error3 != 0) {
		std::cout << "  error with lodepng ground normal texture loading " << error3 << ": " << lodepng_error_text(error3) << std::endl;
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &ground_tex);
	glBindTexture(GL_TEXTURE_2D, ground_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded ground texture" << endl;


	glGenTextures(1, &ground_norm_tex);
	glBindTexture(GL_TEXTURE_2D, ground_norm_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded ground normal texture" << endl;


	glGenTextures(1, &point_sprite);
	glBindTexture(GL_TEXTURE_2D, point_sprite);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image3[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	cout << " loaded point sprite texture" << endl;

	uHeightSampler = glGetUniformLocation(shader_program, "rock_height_tex");
	uNormalSampler = glGetUniformLocation(shader_program, "rock_normal_tex");
	uPointSpriteSampler = glGetUniformLocation(shader_program, "point_sprite");

	glUniform1i(uHeightSampler,   0);   //height goes in texture unit 0
	glUniform1i(uNormalSampler,   1);   //normal goes in texture unit 1
	glUniform1i(uPointSpriteSampler,   2);   //normal goes in texture unit 2
}

//****************************************************************************
//  Function: DudesAndTreesModel::generate_points()
//
//  Purpose:
//    This function produces all the data for representing this object.
//****************************************************************************
void DudesAndTreesModel::generate_points() {

	points.push_back(glm::vec3(0.0,0.0,0.0));

	//generate tree points
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist0(-0.018f, 0.015f);

	points.push_back(glm::vec3(2.0*dist0(mt),2.0*dist0(mt),-0.01));
	points.push_back(glm::vec3(2.0*dist0(mt),2.0*dist0(mt),-0.02));
	points.push_back(glm::vec3(2.0*dist0(mt),2.0*dist0(mt),-0.03));
	points.push_back(glm::vec3(2.0*dist0(mt),2.0*dist0(mt),-0.04));
	points.push_back(glm::vec3(2.0*dist0(mt),2.0*dist0(mt),-0.05));
	points.push_back(glm::vec3(2.0*dist0(mt),2.0*dist0(mt),-0.06));

	for ( int i = 0; i < 3; i++ ) {
		for ( float v = 0.01; v <= 0.06; v += 0.01 ) {
			cout << "ding" << endl;
			points.push_back(glm::vec3(dist0(mt),dist0(mt),-v));
			points.push_back(glm::vec3(dist0(mt),dist0(mt),v));
		}
	}

	num_tree_pts = points.size();

	std::uniform_real_distribution<float> dist1(-0.14f, 0.12f);
	std::uniform_real_distribution<float> dist2(0.05f, 0.08f);

	for ( int i = 0; i < 45; i++ ) {
		points.push_back(glm::vec3(dist1(mt),dist1(mt),dist2(mt)));
		points.push_back(glm::vec3(dist1(mt),-dist1(mt),dist2(mt)));
		points.push_back(glm::vec3(-dist1(mt),dist1(mt),dist2(mt)));
		points.push_back(glm::vec3(-dist1(mt),-dist1(mt),dist2(mt)));
	}

	num_treetop_pts = points.size() - num_tree_pts;


// box points - conver this to a polygonal mesh, no point doing points
	box_start = points.size();

	glm::vec3 a,b,c,d,e,f,g,h;
	float nx,ny,nz,px,py,pz;
	nx = -0.05;
	ny = -0.05;
	nz = -0.0;
	px =  0.05;
	py =  0.05;
	pz =  0.03;

	a = glm::vec3(nx,py,pz);
	b = glm::vec3(nx,ny,pz);
	c = glm::vec3(px,py,pz);
	d = glm::vec3(px,ny,pz);
	e = glm::vec3(nx,py,nz);
	f = glm::vec3(nx,ny,nz);
	g = glm::vec3(px,py,nz);
	h = glm::vec3(px,ny,nz);

	// 	   e-------g    +y
	// 	  /|      /|		 |
	// 	 / |     / |     |___+x
	// 	a-------c  |    /
	// 	|  f----|--h   +z
	// 	| /     | /
	// 	|/      |/
	// 	b-------d

	subd_square(a,b,c,d);
	subd_square(a,b,e,f);
	subd_square(a,e,c,g);
	subd_square(e,g,f,h);
	subd_square(g,h,c,d);
	subd_square(b,f,d,h);
	num_box_pts = points.size() - box_start;
}

void DudesAndTreesModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
	if(glm::distance(a, b) < 0.02) {//add points
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		points.push_back(d);

		points.push_back((a+b+c+d)/4.0f);
	} else { //recurse
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
  //  Function: DudesAndTreesModel::display()
  //
  //  Purpose:
  //    This function does all the setup for the buffers and uniforms and then
  //    issues a draw call for the geometry representing this object
  //****************************************************************************

void DudesAndTreesModel::display() {
	glBindVertexArray(vao);
	glUseProgram(shader_program);

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, ground_tex);

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, ground_norm_tex);

	glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
	glBindTexture(GL_TEXTURE_2D, point_sprite);

	glUniform1i(uTime, time);
	glUniform1i(uScroll, scroll);
	glUniform1f(uScale, scale);

	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));

	for(auto x: entities) {
		glUniform3fv(uPosition, 1, glm::value_ptr(x.location));
		if(x.type == 0 or x.type == 1) { //good guy or bad guy
			glPointSize(14.0);
			bounce = 1;
			glUniform1i(uBounce, bounce);

			//set the color, 0 is good, they are blue, 1 is bad, they are red
			if(!x.dead) {
				if(!x.type)
					glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(0,0,1)));
				else
					glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(1,0,0)));
			} else { //black if dead
				glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(0,0,0)));
			}

			glDrawArrays(GL_POINTS, 0, 1);  //draw the point

		} else if(x.type == 2) {//drawing a tree

			bounce = 0;
			glUniform1i(uBounce, bounce);

			glPointSize(8.0); //small points for the more detailed models

			glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(0.5,0.2,0)));
			glDrawArrays(GL_POINTS, 0, num_tree_pts);  //draw the tree

			glPointSize(16.0);

			glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(0.3,0.4,0)));
			glDrawArrays(GL_POINTS, num_tree_pts, num_treetop_pts);  //draw the treetops

		} else if(x.type == 3) {//drawing a box

			bounce = 1;
			glUniform1i(uBounce, bounce);

			glPointSize(3.0);

			if(!x.dead)
				glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(0.9,0.6,0)));
			else
				glUniform3fv(uColor, 1, glm::value_ptr(glm::vec3(0.7,0.4,0)));

			glDrawArrays(GL_POINTS, box_start, num_box_pts);  //draw the tree

		}
	}

	if(cursor_draw) {
		glUniform3fv(uColor, 1, glm::value_ptr(point_sprite_color));
		glUniform3fv(uPosition, 1, glm::value_ptr(point_sprite_position));

		glPointSize(25.0);
		glDrawArrays(GL_POINTS, 0, 1);  //draw the point
	}
}

void DudesAndTreesModel::update_sim() {  //called from timer function
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(-0.003f, 0.003f);

	glm::vec3 closest_box_loc = glm::vec3(0,0,0);
	float closest_box_distance = 99999.0;
	float box_capture_threshold = 0.01;
	float tree_hit_threshold = 0.03;

	status = 0;

	bool captured_this_cycle = false;

	for (auto &x : entities) { //for all the entities
		if((x.type == 0 || x.type == 1) && !x.dead) { //don't move dead guys
			for(auto &x2 : entities) { //look for a box
				if(x2.type == 3 && !x2.dead) { //we hit a box, that's live, i.e. don't consider 'dead' boxes
					if (glm::distance(x2.location,x.location) < closest_box_distance) { //is this box closer than the last encountered?
						closest_box_loc = x2.location;
						closest_box_distance = glm::distance(x2.location,x.location);
					}//end distance update check
				}//end if it's a box
			}//end for entities, looking for box

			if(closest_box_distance < 99999.0) {
				status = 1;
				if(closest_box_distance < box_capture_threshold) {

					//remove that box from the running
					for(int i = 0; i < entities.size(); i++) {
						if(entities[i].location == closest_box_loc) {
							entities[i].dead = true;
						}
					}

					//logic to capture a box
					if(!x.type && !captured_this_cycle) { //good guys
						//score point
						cout << "good guys captured a box" << endl;
						captured_this_cycle = true;
						score++;
					} else if(x.type == 1 && !captured_this_cycle) { //bad guys
						//don't score point
						cout << "bad guys captured a box" << endl;
						captured_this_cycle = true;
					}

				} else {
					//move towards the closest box
					//0.02 times the normalized vector towards the nearest box
					x.location += 0.002f * glm::normalize(glm::vec3(closest_box_loc.x - x.location.x,closest_box_loc.y - x.location.y,0));
					//plus 0.01 times the normalized vector generated by (dist(mt),dist(mt))
					x.location +=  glm::vec3(dist(mt), dist(mt), 0);
				}
			} else {//no boxes found in the list
				//move some random amount
				float xmove = dist(mt);
				float ymove = dist(mt);
				x.location = glm::vec3(x.location.x + xmove, x.location.y + ymove, x.location.z);
			}

			glm::vec3 closest_tree_loc;
			float closest_tree_distance = 99999.0;

			for(auto &x3 : entities) {
				if(x3.type == 2) {  //a tree
					if(glm::distance(x3.location, x.location) < tree_hit_threshold) {
						// you're close enough, move away, along the vector from that tree to the dude
						x.location += 0.01f * glm::normalize(glm::vec3(x.location.x - x3.location.x,x.location.y - x3.location.y,0));
						x.location += glm::vec3(dist(mt), dist(mt), 0);
					}
				}
			}

			if(x.location.x > 0.8 || x.location.x < -0.8 || x.location.y > 0.8 || x.location.y < -0.8) {
				//set it back at 0,0
				x.location = glm::vec3(0,0,0);
			}
		}//end good guy/bad guy
		//trees and boxes don't need to be updated
	} //end for entities
}

void DudesAndTreesModel::handle_click(glm::vec3 pixel_read) {  //called from mouse callback
	bool inthewater = false;
	bool inatree = false;

	// glm::vec3 click_location;

	if(pixel_read == glm::vec3(0,0,0)) {//black pixel, off the board
		cout << "off the board" << endl;
		//no decrement
		return;
	}

	if(pixel_read.b > 0) {//clicked in the water
		cout << "you dropped it in the water" << endl;
		inthewater = true;
	}

	for(auto x : entities) {
		if(x.type == 2) {
			//take tree-to-click distance, tell user if it's too close to a tree
			if(glm::distance(x.location, glm::vec3(pixel_read.x, pixel_read.y,0)) < 0.05) {
				cout << "too close to a tree, box lost" << endl;
				inatree=true;
			}
		}
	}

	//if you haven't returned yet, you're dealing with a valid click
	for(auto &x : entities) {
		if((x.type == 0 || x.type == 1) && !x.dead) {
			//check distance to living good guy/bad guys, kill any that are too close, but do not return, because you still place a box there

			float thresh = 0.02;
			if(big_radius)
				thresh = 0.1;


			if(glm::distance(glm::vec3(pixel_read.x, pixel_read.y, 0), glm::vec3(x.location.x, x.location.y, 0) )< thresh) {
				cout << "aw jeez man, come on, you hit a ";
				x.dead = true;

				if(x.type == 1) { //you hit a bad guy
					score++;
					cout << "bad";
				}

				if(x.type == 0) { //you hit a good guy
					score--;
					cout << "good";
				}

				cout <<  " guy" << endl;
			}
		}
	}

	// start at the end of the list of entities, find the first dead box that's not on the board
	for(int i = entities.size()-1; i > 0; i--) {
		if(entities[i].type == 3 && entities[i].location.x > 0.8) {
			entities[i].location = glm::vec3(pixel_read.x, pixel_read.y, 0);
			if(inatree || inthewater){
				entities[i].dead = true;
			} else {
				entities[i].dead = false;
			}
			break;
		}
	}
	// move that box to the point indicated by the click
	// mark that box as live (!dead), unless inthewater or inatree, because then you want them inactive
	boxes_left--;
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
class WaterModel {
public:

	WaterModel();

	void display();

	void set_time(int tin)        {time = tin;}
	void set_proj(glm::mat4 pin)  {proj = pin;}

	void set_scroll(int sin)      {scroll = sin;}
	void scale_up()               {scale *= 1.618f;}
	void scale_down()             {scale /= 1.618f;}

	private:
	GLuint vao;
	GLuint buffer;

	//the three textures associated with the water's surface - we don't need the ground anymore, just using depth testing there now
	GLuint ground_tex, ground_tex_sampler;
	GLuint displacement_tex, displacement_tex_sampler;
	GLuint normal_tex, normal_tex_sampler;
	GLuint color_tex, color_tex_sampler;

	GLuint shader_program;

	int num_pts; //how many points?

	//VERTEX ATTRIB LOCATIONS
	GLuint vPosition;
	// GLuint vNormal;
	// GLuint vColor;

	//UNIFORM LOCATIONS
	GLuint uTime;   //animation time
	GLuint uProj;   //projection matrix

	GLuint uScroll;
	GLuint uScale;

	//VALUES OF THOSE UNIFORMS
	int time, scroll;
	float scale;
	glm::mat4 proj;

	void generate_points();
	void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

	std::vector<glm::vec3> points;    //add the 1.0 w value in the shader
};

//****************************************************************************
//  Function: WaterModel Constructor
//
//  Purpose:
//    Calls generate_points() and then sets up everything related to the GPU
//****************************************************************************
WaterModel::WaterModel() {
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
	glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);

	//SHADERS (COMPILE, USE)
	cout << " compiling water shaders" << endl;
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

	uScroll = glGetUniformLocation(shader_program, "scroll");
	glUniform1i(uScroll, scroll);

	scale = 1.0;
	uScale = glGetUniformLocation(shader_program, "scale");
	glUniform1f(uScale, scale);

	//THE TEXTURE
	std::vector<unsigned char> image;
	std::vector<unsigned char> image2;
	std::vector<unsigned char> image3;
	std::vector<unsigned char> image4;
	unsigned width, height;
	unsigned width2, height2;
	unsigned width3, height3;
	unsigned width4, height4;
	unsigned error = lodepng::decode(image, width, height, GROUND_TEXTURE_PATH, LodePNGColorType::LCT_RGBA, 8);
	unsigned error2 = lodepng::decode(image2, width2, height2, "resources/textures/height/wave_height.png", LodePNGColorType::LCT_RGBA, 8);
	unsigned error3 = lodepng::decode(image3, width3, height3, "resources/textures/normals/wave_norm.png", LodePNGColorType::LCT_RGBA, 8);
	unsigned error4 = lodepng::decode(image4, width4, height4, "resources/textures/water_color.png", LodePNGColorType::LCT_RGBA, 8);


	// If there's an error, display it.
	if(error != 0) {
		std::cout << "error with lodepng texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	if(error2 != 0) {
		std::cout << "error with lodepng texture loading " << error2 << ": " << lodepng_error_text(error2) << std::endl;
	}

	if(error3 != 0) {
		std::cout << "error with lodepng texture loading " << error3 << ": " << lodepng_error_text(error3) << std::endl;
	}

	if(error4 != 0) {
		std::cout << "error with lodepng texture loading " << error4 << ": " << lodepng_error_text(error4) << std::endl;
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &ground_tex);
	glBindTexture(GL_TEXTURE_2D, ground_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded ground texture" << endl;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &displacement_tex);
	glBindTexture(GL_TEXTURE_2D, displacement_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded wave displacement texture" << endl;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &normal_tex);
	glBindTexture(GL_TEXTURE_2D, normal_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image3[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded wave normal texture" << endl;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image4[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	cout << " loaded wave color texture" << endl;

	ground_tex_sampler = glGetUniformLocation(shader_program, "ground_tex");
	displacement_tex_sampler = glGetUniformLocation(shader_program, "height_tex");
	normal_tex_sampler = glGetUniformLocation(shader_program, "normal_tex");
	color_tex_sampler = glGetUniformLocation(shader_program, "color_tex");

	glUniform1i(ground_tex_sampler,   0);   //height of the ground goes in texture unit 0
	glUniform1i(displacement_tex_sampler,   1);   //height goes in texture unit 1
	glUniform1i(normal_tex_sampler,   2);   //normal goes in texture unit 2
	glUniform1i(color_tex_sampler,   3);   //color  goes in texture unit 3
}

//****************************************************************************
//  Function: WaterModel::generate_points()
//
//  Purpose:
//    This function produces all the data for representing this object.
//****************************************************************************
void WaterModel::generate_points() {
	//GENERATING GEOMETRY
	glm::vec3 a, b, c, d;
	float scale = 1.618f;

	a = glm::vec3(-1.0f*scale, -1.0f*scale, 0.0f);
	b = glm::vec3(-1.0f*scale, 1.0f*scale, 0.0f);
	c = glm::vec3(1.0f*scale, -1.0f*scale, 0.0f);
	d = glm::vec3(1.0f*scale, 1.0f*scale, 0.0f);

	subd_square(a, b, c, d);
	num_pts = points.size();
}

void WaterModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
	if(glm::distance(a, b) < MIN_POINT_PLACEMENT_THRESHOLD) {//add points
		// triangle 1 ABC
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		//triangle 2 BCD
		points.push_back(b);
		points.push_back(c);
		points.push_back(d);

		//middle
		// points.push_back((a+b+c+d)/4.0f);
	} else { //recurse
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

void WaterModel::display() {
	glBindVertexArray(vao);
	glUseProgram(shader_program);

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, ground_tex);

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, displacement_tex);

	glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, normal_tex);

	glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 2
	glBindTexture(GL_TEXTURE_2D, color_tex);

	glUniform1i(uTime, time);
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUniform1f(uScale, scale);
	glUniform1i(uScroll, scroll);

	glDrawArrays(GL_TRIANGLES, 0, num_pts);
}


//******************************************************************************
//  Class: SkirtModel
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
class SkirtModel {
public:
	SkirtModel();

	void display();

	void set_time(int tin)        {time = tin;}
	void set_scroll(int sin)      {scroll = sin;}
	void scale_up()               {scale *= 1.618f;}
	void scale_down()             {scale /= 1.618f;}
	void set_proj(glm::mat4 pin)  {proj = pin;}

	void increase_thresh()        {thresh += 0.01; cout << thresh << endl;}
	void decrease_thresh()        {thresh -= 0.01; cout << thresh << endl;}

private:
	GLuint vao;
	GLuint buffer;
	GLuint ground_tex, water_tex;

	GLuint ground_tex_sampler, water_tex_sampler;

	GLuint shader_program;

	int num_pts_front; //how many points?
	int num_pts_back; //how many points?

	//VERTEX ATTRIB LOCATIONS
	GLuint vPosition;
	// GLuint vNormal;
	// GLuint vColor;

	//UNIFORM LOCATIONS
	GLuint uTime;   //animation time
	GLuint uProj;   //projection matrix
	GLuint uThresh;   //cutoff for water
	GLuint uScale;
	GLuint uScroll;


	//VALUES OF THOSE UNIFORMS
	int time, scroll;
	float thresh, scale;
	glm::mat4 proj;

	void generate_points();
	void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

	std::vector<glm::vec3> points;    //add the 1.0 w value in the shader
};

//****************************************************************************
//  Function: SkirtModel Constructor
//
//  Purpose:
//    Calls generate_points() and then sets up everything related to the GPU
//****************************************************************************
SkirtModel::SkirtModel() {

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
	glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);

	//SHADERS (COMPILE, USE)
	cout << " compiling skirt shaders" << endl;
	Shader s("resources/shaders/skirt_vert.glsl", "resources/shaders/skirt_frag.glsl");
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

	uScroll = glGetUniformLocation(shader_program, "scroll");
	glUniform1i(uScroll, scroll);

	thresh = 0.56f;
	uThresh = glGetUniformLocation(shader_program, "thresh");
	glUniform1f(uThresh, thresh);

	scale = 1.0;
	uScale = glGetUniformLocation(shader_program, "scale");
	glUniform1f(uScale, scale);

	//THE TEXTURE

	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, GROUND_TEXTURE_PATH, LodePNGColorType::LCT_RGBA, 8);

	std::vector<unsigned char> image2;
	unsigned width2, height2;
	unsigned error2 = lodepng::decode(image2, width2, height2, "resources/textures/height/wave_height.png", LodePNGColorType::LCT_RGBA, 8);

	// If there's an error, display it.
	if(error != 0) {
		std::cout << "error with lodepng texture loading " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	if(error2 != 0) {
		std::cout << "error2 with lodepng texture loading " << error2 << ": " << lodepng_error_text(error2) << std::endl;
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &ground_tex);
	glBindTexture(GL_TEXTURE_2D, ground_tex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	cout << " loaded ground texture" << endl;

	glGenTextures(1, &water_tex);
	glBindTexture(GL_TEXTURE_2D, water_tex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	cout << " loaded water texture" << endl;

	ground_tex_sampler = glGetUniformLocation(shader_program, "ground_tex");
	water_tex_sampler = glGetUniformLocation(shader_program, "water_tex");

	glUniform1i(ground_tex_sampler,   0);   //height of the ground goes in texture unit 0
	glUniform1i(water_tex_sampler,   1);   //height of the water goes in texture unit 1
}

//****************************************************************************
//  Function: SkirtModel::generate_points()
//
//  Purpose:
//    This function produces all the data for representing this object.
//****************************************************************************
void SkirtModel::generate_points() {
	//GENERATING GEOMETRY
	glm::vec3 a, b, c, d;
	glm::vec3 alow, blow, clow, dlow;

	// float scale = 0.9f;
	float scale = 1.618f;

	a = glm::vec3(-1.0f*scale, -1.0f*scale, 0.0f);
	b = glm::vec3(-1.0f*scale, 1.0f*scale, 0.0f);
	c = glm::vec3(1.0f*scale, -1.0f*scale, 0.0f);
	d = glm::vec3(1.0f*scale, 1.0f*scale, 0.0f);

	alow = glm::vec3(-1.0f*scale, -1.0f*scale, -0.5f);
	blow = glm::vec3(-1.0f*scale, 1.0f*scale, -0.5f);
	clow = glm::vec3(1.0f*scale, -1.0f*scale, -0.5f);
	dlow = glm::vec3(1.0f*scale, 1.0f*scale, -0.5f);

	points.clear();

	subd_square(c,a,clow,alow); //back left
	subd_square(d,c,dlow,clow); //back right

	num_pts_back = points.size();

	a += glm::vec3(0.0f, 0.0f, 0.3f);
	b += glm::vec3(0.0f, 0.0f, 0.3f);
	c += glm::vec3(0.0f, 0.0f, 0.3f);
	d += glm::vec3(0.0f, 0.0f, 0.3f);

	subd_square(b,a,blow,alow); //front left
	subd_square(b,d,blow,dlow); //front right

	num_pts_front = points.size();
}

void SkirtModel::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
	if(glm::distance(a, c) < MIN_POINT_PLACEMENT_THRESHOLD) {//add points
		// triangle 1 ABC
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		//triangle 2 BCD
		points.push_back(b);
		points.push_back(c);
		points.push_back(d);

		//middle
		// points.push_back((a+b+c+d)/4.0f);
	} else { //recurse
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
//  Function: SkirtModel::display()
//
//  Purpose:
//    This function does all the setup for the buffers and uniforms and then
//    issues a draw call for the geometry representing this object
//****************************************************************************
void SkirtModel::display() {
	glBindVertexArray(vao);
	glUseProgram(shader_program);

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, ground_tex);

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, water_tex);

	glUniform1i(uTime, time);
	glUniform1f(uScale, scale);
	glUniform1i(uScroll, scroll);

	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUniform1f(uThresh, thresh);

	glDrawArrays(GL_TRIANGLES, 0, num_pts_back);
	glDrawArrays(GL_TRIANGLES, num_pts_back, num_pts_front);
}
