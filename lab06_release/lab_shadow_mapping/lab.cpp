// Include C++ headers
#include <iostream>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h> 

#include "Sphere.h"
#include "Box.h"
#include "Collision.h"

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024*2
#define W_HEIGHT 768*2
#define TITLE "Space Golf"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

// Creating a structure to store the material parameters of an object
struct Material {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ns;
};


// Global Variables
GLFWwindow* window;
Camera* camera;
Light* light;
GLuint shaderProgram, depthProgram, miniMapProgram;
Drawable* model1;
Drawable* model1moon;
Drawable* model2;
Drawable* model3;
Drawable* model4;
Drawable* model4moon;
Drawable* model5;
Drawable* model6;
Drawable* model6moon;
Drawable* plane;
Drawable* plane2;
Drawable* plane3;
Drawable* plane4;
Drawable* plane5;
Drawable* plane6;
GLuint modelDiffuseTexture, modelSpecularTexture,modelDiffuseTexture2, modelDiffuseTexture3, modelDiffuseTexture4, modelDiffuseTexture5, modelDiffuseTexture6, modelDiffuseTexture6moon, modelDiffuseTexturemoon, modelDiffuseTexture4moon;
GLuint planeDiffuseTexture, CommetDiffuseTexture;
GLuint depthFrameBuffer, depthTexture;

Sphere* sphere;
Sphere* sphere2;
Sphere* sphere3;
Sphere* sphere4;
Sphere* sphere5;
Sphere* sphere6;
Sphere* commet;
Box* box;

Drawable* quad;

float g =  9.80665f;

float c = 1.0;
float h = -30;
static double previous_seconds = glfwGetTime();

// locations for shaderProgram
GLuint viewMatrixLocation;
GLuint projectionMatrixLocation;
GLuint modelMatrixLocation;
GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation;
GLuint lightPowerLocation;
GLuint diffuseColorSampler; 
GLuint specularColorSampler;
GLuint useTextureLocation;
GLuint depthMapSampler;
GLuint lightVPLocation;
GLuint lightDirectionLocation;
GLuint lightFarPlaneLocation;
GLuint lightNearPlaneLocation;


// locations for depthProgram
GLuint shadowViewProjectionLocation; 
GLuint shadowModelLocation;

// locations for miniMapProgram
GLuint quadTextureSamplerLocation;

// Create two sample materials


const Material obsidian{
	vec4{0.05375, 0.05, 0.06625,0.82 },
	vec4{0.18275, 0.17, 0.22525, 0.82 },
	vec4{0.332741, 0.328634, 0.346435, 0.82},
	38.4
};


// NOTE: Since the Light and Material struct are used in the shader programs as well 
//		 it is recommended to create a function that will update all the parameters 
//       of an object.
// 
// Creating a function to upload (make uniform) the light parameters to the shader program
void uploadLight(const Light& light) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation, light.power);
}


// Creating a function to upload the material parameters of a model to the shader program
void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}





void createContext() {

	// Create and complile our GLSL program from the shader
	shaderProgram = loadShaders("ShadowMapping.vertexshader", "ShadowMapping.fragmentshader");
	
	// Task 3.1 
	// Create and load the shader program for the depth buffer construction
	// You need to load and use the Depth.vertexshader, Depth.fragmentshader
	// NOTE: These files are not visible in the visual studio, as they are not a part of
	//       the main project. To add a new file to our project, we need to add it to the
	//       CMAKELists.txt and build the project again. 
	// NOTE: We alse need to create a global variable to store new shader program
	 depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader");
	

	// Task 2.1
	// Use the SimpleTexture.vertexshader, "SimpleTexture.fragmentshader"
	 miniMapProgram = loadShaders("SimpleTexture.vertexshader", "SimpleTexture.fragmentshader");
	

	// NOTE: Don't forget to delete the shader programs on the free() function


	// Get pointers to uniforms
	// --- shaderProgram ---
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
	viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
	// for phong lighting
	KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
	lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
	diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
	specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");
	
	// Task 1.4
	useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture"); 

	// locations for shadow rendering
	depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
	lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");


	// --- depthProgram ---
	shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
	shadowModelLocation = glGetUniformLocation(depthProgram, "M");

	// --- miniMapProgram ---
	quadTextureSamplerLocation = glGetUniformLocation(miniMapProgram, "textureSampler");
	

	// 1. Using Drawable to load suzanne
	
	
	// Task 1.2 Load earth.obj using drawable 
	model1 = new Drawable("models/earth.obj");
	modelDiffuseTexture = loadSOIL("models/uranus_diffuse.jpg");

	model1moon = new Drawable("models/earth.obj");
	modelDiffuseTexturemoon = loadSOIL("models/moon2_diffuse.jpg");

	model2 = new Drawable("models/earth.obj");
	modelDiffuseTexture2 = loadSOIL("models/saturn_diffuse.jpg");
	
	model3 = new Drawable("models/earth.obj");
	modelDiffuseTexture3 = loadSOIL("models/mars_diffuse.jpg");

	model4 = new Drawable("models/earth.obj");
	modelDiffuseTexture4 = loadSOIL("models/venus_diffuse.jpg");

	model4moon = new Drawable("models/earth.obj");
	modelDiffuseTexture4moon = loadSOIL("models/moon3_diffuse.jpg");

	model5 = new Drawable("models/earth.obj");
	modelDiffuseTexture5 = loadSOIL("models/neptune_diffuse.jpg");

	model6 = new Drawable("models/earth.obj");
	modelDiffuseTexture6 = loadSOIL("models/earth_diffuse.jpg");
	modelSpecularTexture = loadSOIL("models/suzanne_specular.bmp");
	
	model6moon = new Drawable("models/earth.obj");
	modelDiffuseTexture6moon = loadSOIL("models/moon_diffuse.jpg");

	CommetDiffuseTexture = loadSOIL("models/commet_diffuse.jpg");

	sphere = new Sphere(vec3(-8, 10,-3 ), vec3(0, 0, 0), 0.9, 10);
	sphere2 = new Sphere(vec3(15, 6, 7), vec3(0, 0, 0), 1.3, 5);
	sphere3 = new Sphere(vec3(9, 15, -12), vec3(0, 0, 0), 1.5, 6);
	sphere4 = new Sphere(vec3(4, 8, 13.5), vec3(0, 0, 0), 1.2, 3);
	sphere5 = new Sphere(vec3(-12, 22, 15), vec3(0, 0, 0), 0.8, 7);
	sphere6 = new Sphere(vec3(-6, 5, 7), vec3(0, 0, 0), 1.8, 5.5);
	commet = new Sphere(vec3(4,4,4), vec3(0.3, 0.2, 0.4), 0.4, 10);
	//box = new Box(40);
	// Task 1.3
	// Creating a Drawable object using vertices, uvs, normals
	// In this task we will create a plane on which the shadows will be displayed
	
	// plane vertices
	float y = -1; // offset to move the place up/down across the y axis

	vector<vec3> floorVertices = {
		vec3(-20.0f*c, y, -20.0*c),
		vec3(-20.0f*c, y,  20.0f*c),
		vec3(20.0f*c,  y,  20.0f*c),
		vec3(20.0f*c,  y,  20.0f*c),
		vec3(20.0f*c,  y, -20.0f*c),
		vec3(-20.0f*c, y, -20.0f*c),

	};

	// plane normals
	vector<vec3> floorNormals = {
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	};

	// plane uvs
	vector<vec2> floorUVs = {
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
	};

	vector<vec3> floorVertices2 = {
		vec3(20.0f * c, y, -20.0 * c),
		vec3(20.0f * c, y*h,  -20.0f * c),
		vec3(20.0f * c,  y*h,  20.0f * c),
		vec3(20.0f * c,  y*h,  20.0f * c),
		vec3(20.0f * c,  y, 20.0f * c),
		vec3(20.0f * c, y, -20.0f * c),

	};

	vector<vec3> floorVertices3 = {
			vec3(-20.0f * c, y*h, -20.0 * c),
			vec3(-20.0f * c, y*h,  20.0f * c),
			vec3(20.0f * c,  y*h,  20.0f * c),
			vec3(20.0f * c,  y*h,  20.0f * c),
			vec3(20.0f * c,  y*h, -20.0f * c),
			vec3(-20.0f * c, y*h, -20.0f * c),

	};

	vector<vec3> floorVertices4 = {
		vec3(-20.0f * c, y, -20.0 * c),
		vec3(-20.0f * c, y * h,  -20.0f * c),
		vec3(-20.0f * c,  y * h,  20.0f * c),
		vec3(-20.0f * c,  y * h,  20.0f * c),
		vec3(-20.0f * c,  y, 20.0f * c),
		vec3(-20.0f * c, y, -20.0f * c),

	};

	vector<vec3> floorVertices5 = {
		vec3(-20.0f * c, y, 20.0 * c),
		vec3(-20.0f * c, y * h, 20.0f * c),
		vec3(20.0f * c,  y * h, 20.0f * c),
		vec3(20.0f * c,  y * h,  20.0f * c),
		vec3(20.0f * c,  y, 20.0f * c),
		vec3(-20.0f * c, y, 20.0f * c),

	};

	vector<vec3> floorVertices6 = {
		vec3(-20.0f * c, y, -20.0 * c),
		vec3(-20.0f * c, y * h, -20.0f * c),
		vec3(20.0f * c,  y * h,-20.0f * c),
		vec3(20.0f * c,  y * h,  -20.0f * c),
		vec3(20.0f * c,  y, -20.0f * c),
		vec3(-20.0f * c, y, -20.0f * c),

	};
	// Call the Drawable constructor
	// Notice, that this way we dont have to generate VAO and VBO for the matrices
	plane = new Drawable(floorVertices, floorUVs, floorNormals);

	plane2 = new Drawable(floorVertices2, floorUVs, floorNormals);
	plane3 = new Drawable(floorVertices3, floorUVs, floorNormals);
	plane4 = new Drawable(floorVertices4, floorUVs, floorNormals);
	plane5 = new Drawable(floorVertices5, floorUVs, floorNormals);
	plane6 = new Drawable(floorVertices6, floorUVs, floorNormals);
	
	planeDiffuseTexture = loadSOIL("models/star2_diffuse.jpg");
	// Task 2.2: Creating a 2D quad to visualize the depthmap
	// create geometry and vao for screen-space quad
	
	vector<vec3> quadVertices = {
	  vec3(0.5, 0.5, 0.0),
	  vec3(1.0, 0.5, 0.0),
	  vec3(1.0, 1.0, 0.0),
	  vec3(1.0, 1.0, 0.0),
	  vec3(0.5, 1.0, 0.0),
	  vec3(0.5, 0.5, 0.0)
	};

	vector<vec2> quadUVs = {
	  vec2(0.0, 0.0),
	  vec2(1.0, 0.0),
	  vec2(1.0, 1.0),
	  vec2(1.0, 1.0),
	  vec2(0.0, 1.0),
	  vec2(0.0, 0.0)
	};

	quad = new Drawable(quadVertices, quadUVs);

	


	// ---------------------------------------------------------------------------- //
	// -  Task 3.2 Create a depth framebuffer and a texture to store the depthmap - //
	// ---------------------------------------------------------------------------- //
	
	// Tell opengl to generate a framebuffer
	glGenFramebuffers(1, &depthFrameBuffer);
	// Binding the framebuffer, all changes bellow will affect the binded framebuffer
	// **Don't forget to bind the default framebuffer at the end of initialization
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);


	// We need a texture to store the depth image
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// Telling opengl the required information about the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// Task 4.5 Texture wrapping methods
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	

	// Task 4.5 Don't shadow area out of light's viewport
	
	// Step 1 : (Don't forget to comment out the respective lines above
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Set color to set out of border 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Next go to fragment shader and add an iff statement, so if the distance in the z-buffer is equal to 1, 
	// meaning that the fragment is out of the texture border (or further than the far clip plane) 
	// then the shadow value is 0.
	

	//Task 3.2 Continue
	// Attaching the texture to the framebuffer, so that it will monitor the depth component
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	

	// Since the depth buffer is only for the generation of the depth texture, 
	// there is no need to have a color output
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	// Finally, we have to always check that our frame buffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		throw runtime_error("Frame buffer not initialized correctly");
	}

	// Binding the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//*/

}


void free() {

	delete sphere;
	delete sphere2;
	delete sphere3;
	delete sphere4;
	delete sphere5;
	delete sphere6;
	delete commet;
	// Delete Shader Programs
	glDeleteProgram(shaderProgram);
	glDeleteProgram(depthProgram);
	glDeleteProgram(miniMapProgram);
	
	glfwTerminate();
}


void depth_pass(mat4 viewMatrix, mat4 projectionMatrix) {

	// Setting viewport to shadow map size
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	// Binding the depth framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

	// Cleaning the framebuffer depth information (stored from the last render)
	glClear(GL_DEPTH_BUFFER_BIT);

	// Selecting the new shader program that will output the depth component
	glUseProgram(depthProgram);

	// sending the view and projection matrix to the shader
	mat4 view_projection = projectionMatrix *viewMatrix ;
	glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);


	// ---- rendering the scene ---- //
	// creating suzanne model matrix and sending to GPU
	mat4 modelMatrix = translate(mat4(), vec3(-8.0, 10.0, -3.0)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model1->bind();
	model1->draw();

	modelMatrix = translate(mat4(), vec3(-8.0, 10.0, -3.0)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model1moon->bind();
	model1moon->draw();

	// same for sphere
	modelMatrix = translate(mat4(), vec3(15.0f, 6.0f, 7.0f)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model2->bind();
	model2->draw();
	
	modelMatrix = translate(mat4(), vec3(9.0f, 15.0f, -12.0f)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model3->bind();
	model3->draw();

	modelMatrix = translate(mat4(), vec3(4.0f, 8.0f, 13.5f)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model4->bind();
	model4->draw();


	modelMatrix = translate(mat4(), vec3(4.0f, 8.0f, 13.5f)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model4moon->bind();
	model4moon->draw();

	modelMatrix = translate(mat4(), vec3(-12.0f, 22.0f, 15.0f)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model5->bind();
	model5->draw();

	
	modelMatrix = translate(mat4(), vec3(-6.0f, 5.0f, 7.0f)) * scale(mat4(), vec3(0.5f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model6->bind();
	model6->draw();
	
	modelMatrix = translate(mat4(), vec3(-3.5f, 5.0f, 7.0f)) * scale(mat4(), vec3(0.2f));
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	model6moon->bind();
	model6moon->draw();

	// same for plane
	modelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	//plane->bind();
	//plane->draw();

	modelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	plane2->bind();
	plane2->draw();

	modelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	plane3->bind();
	plane3->draw();

	modelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	plane4->bind();
	plane4->draw();

	modelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	plane5->bind();
	plane5->draw();

	modelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	plane6->bind();
	plane6->draw();
	// binding the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
}

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix,float t) {
	
	

	// Step 1: Binding a frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// Step 2: Clearing color and depth info
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Step 3: Selecting shader program
	glUseProgram(shaderProgram);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	// uploading the light parameters to the shader program
	uploadLight(*light);

	// Task 4.1 Display shadows on the 
	
	// Sending the shadow texture to the shaderProgram
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 0);
	// Sending the light View-Projection matrix to the shader program
	mat4 lightVP = light->projectionMatrix * light->viewMatrix;
	glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);
	




	// ----------------------------------------------------------------- //
	// --------------------- Drawing scene objects --------------------- //	
	// ----------------------------------------------------------------- //

	float currentTime = glfwGetTime();
	float dt = currentTime - t;
	

	// SKYCUBE
	uploadMaterial(obsidian);
	glUniform1i(useTextureLocation, 0);
	mat4 planeModelMatrix = mat4(1.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix[0][0]);

	plane->bind();
	plane->draw();


	mat4 planeModelMatrix2 = mat4(1.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix2[0][0]);
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, planeDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position						//
	glUniform1i(useTextureLocation, 1);

	plane2->bind();
	plane2->draw();


	mat4 planeModelMatrix3 = mat4(1.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix3[0][0]);
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, planeDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position						//
	glUniform1i(useTextureLocation, 1);

	plane3->bind();
	plane3->draw();


	mat4 planeModelMatrix4 = mat4(1.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix4[0][0]);
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, planeDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position						//
	glUniform1i(useTextureLocation, 1);

	plane4->bind();
	plane4->draw();

	mat4 planeModelMatrix5 = mat4(1.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix5[0][0]);
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, planeDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position		// Same process for specular texture						//
	glUniform1i(useTextureLocation, 1);

	plane5->bind();
	plane5->draw();


	mat4 planeModelMatrix6 = mat4(1.0);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix6[0][0]);
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, planeDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position		// Same process for specular texture						//
	glUniform1i(useTextureLocation, 1);

	plane6->bind();
	plane6->draw();
	

	

	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
	glUniform1i(useTextureLocation, 1);
	sphere->update(t, dt);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere->modelMatrix[0][0]);
	sphere->draw();

	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture2);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
	glUniform1i(useTextureLocation, 1);
	sphere2->update(t, dt);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere2->modelMatrix[0][0]);
	sphere2->draw();

	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture3);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
	glUniform1i(useTextureLocation, 1);
	sphere3->update(t, dt);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere3->modelMatrix[0][0]);
	sphere3->draw();

	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture4);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
	glUniform1i(useTextureLocation, 1);
	sphere4->update(t, dt);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere4->modelMatrix[0][0]);
	sphere4->draw();

	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture5);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
	glUniform1i(useTextureLocation, 1);
	sphere5->update(t, dt);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere5->modelMatrix[0][0]);
	sphere5->draw();

	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture6);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
	glUniform1i(useTextureLocation, 1);
	sphere6->update(t, dt);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere6->modelMatrix[0][0]);
	sphere6->draw();

		glActiveTexture(GL_TEXTURE1);								// Activate texture position
		glBindTexture(GL_TEXTURE_2D, CommetDiffuseTexture);			// Assign texture to position 
		glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position				//
		glUniform1i(useTextureLocation, 1);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &commet->modelMatrix[0][0]);
		commet->update(t, dt);
		commet->draw();
	
	
		handleBoxSphereCollision(*commet);
		handleGravitationalPull(*sphere, *commet, 8);
		handleGravitationalPull(*sphere2, *commet, 5);
		handleGravitationalPull(*sphere3, *commet, 4.5);
		handleGravitationalPull(*sphere4, *commet, 7);
		handleGravitationalPull(*sphere5, *commet, 2);
		handleGravitationalPull(*sphere6, *commet, 9);

	//box->update(t, dt);
	//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->modelMatrix[0][0]);
	//box->update(t, dt);
	//box->draw();


}

// Task 2.3: visualize the depth_map on a sub-window at the top of the screen
void renderDepthMap() {
	// using the correct shaders to visualize the depth texture on the quad
	glUseProgram(miniMapProgram);

	//enabling the texture - follow the aforementioned pipeline
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(quadTextureSamplerLocation, 0);

	// Drawing the quad
	quad->bind();
	quad->draw();
}



void mainLoop() {


	light->update();
	mat4 light_proj = light->projectionMatrix;
	mat4 light_view = light->viewMatrix;

	float t = glfwGetTime();


	do {

	

		light->update();
		mat4 light_proj = light->projectionMatrix;
		mat4 light_view = light->viewMatrix;


		// Task 3.5
		// Create the depth buffer
		depth_pass(light_view, light_proj);

		// Getting camera information
		camera->update();
		mat4 projectionMatrix = camera->projectionMatrix;
		mat4 viewMatrix = camera->viewMatrix;
		


		//lighting_pass(viewMatrix, projectionMatrix);
		
		// Task 1.5
		// Rendering the scene from light's perspective when F1 is pressed
		
		
	// Render the scene from camera's perspective
	lighting_pass( viewMatrix , projectionMatrix,t );
			
		
		

		// Task 2.3:
		renderDepthMap();


		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

}


void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Gray background color
	glClearColor(0.5f, 0.5f, 0.50f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	// glEnable(GL_CULL_FACE);

	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);

	// Log
	logGLParameters();

	// Create camera
	camera = new Camera(window);
	
	// Task 1.1 Creating a light source
	// Creating a custom light 
	light = new Light(window,
		vec4{ 1.0, 1 , 1 , 1 },
		vec4{ 1, 1 , 1 , 1  },
		vec4{ 1 , 1 , 1 , 1  },
		vec3{0.0, 20.0, 0.0 },
		500.0f
	);


	
}

int main(void) {
	try {
		initialize();
		createContext();
		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}

	return 0;
}