//
// CGJ: Phong Shading and Text rendered with FreeType library
// The text rendering was based on https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: Jo„o Madeiras Pereira
//

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")

#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>




// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"
#include "flare.h"
#include "l3DBillboard.h"
#include "meshFromAssimp.h"
#include "avtFreeType.h"
#include "Texture_Loader.h"

#include "assimp/include/assimp/Importer.hpp"	//OO version Header!
#include "assimp/include/assimp/scene.h"

// Custom headers
#include "TimeUtil.h" 
#include "GameObjects/Billboard.h"
#include "GameObjects/Table.h"
#include "GameObjects/Cheerios.h"
#include "GameObjects/Orange.h"
#include "GameObjects/Car.h"
#include "Camera/PerspectiveCamera.h"
#include "Camera/OrthographicCamera.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "GameHudManager.h"
#define MAX_LIGHTS 9

using namespace std;

#define CAPTION "CGJ Demo: Phong Shading and Text rendered with FreeType"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

//File with the font
//const string font_name = "fonts/arial.ttf";
const string font_name = "fonts/ariblk.ttf";

//Vector with meshes
//vector<MyMesh> myMeshes;

vector<GameObject3D*> gameObjectsRef;

vector<Camera> cameras;
vector<Camera*> camerasRef;
Camera* activeCameraRef;
int activeCameraId;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint view_uniformId;
GLint tex_loc, tex_loc1, tex_loc2, tex_loc3, tex_normalMap_loc, tex_cube_loc;
GLint texMode_uniformId, shadowMode_uniformId;
GLint toggleFog_uniformId;
int toggleFog;

GLint useNormalMap_loc, specularMap_loc, diffMapCount_loc;

GLuint TextureArray[7];

GLuint FlareTextureArray[5];
//Flare effect
FLARE_DEF AVTflare;
float lightScreenPos[3];  //Position of the light in Window Coordinates
bool flareEffect;
float flareLightPos[4] = { 20.0f, 10.0f, 0.0f, 1.0f };

	
// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 0.0f, beta = 90.0f;
float deltaAlpha = 0.0f, deltaBeta = 0.0f;
float r = 10.0f;

float lightPos[4] = { 40.0f, 50.0f, 0.0f, 1.0f };

float position[4] = { 20.0f, 10.0f, 0.0f, 1.0f };
float position1[4] = { 20.0f, 10.0f, 75.0f, 1.0f };
float position2[4] = { 75.0f, 10.0f, 80.0f, 1.0f };
float position3[4] = { 150.0f, 10.0f, 75.0f, 1.0f };
float position4[4] = { 150.0f, 10.0f, 0.0f, 1.0f };
float position5[4] = { 75.0f, 10.0f, -75.0f, 1.0f };

float checkpoint1[6] = { -10.0f, 30.0f, -10.0f, 10.0f, 70.0f, 110.0f };
float checkpoint2[6] = { 130.0f, 170.0f, -10.0f, 10.0f, 70.0f, 110.0f };
float checkpoint3[6] = { 130.0f, 170.0f, -10.0f, 10.0f, -100.0f, -60.0f };
float checkpoint4[6] = { -10.0f, 30.0f, -10.0f, 10.0f, -100.0f, -60.0f };

bool c1, c2, c3, c4;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
//float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

Table t;
Table t20, t21, t22, t23, t24, t25, t26, t27, t28;//Road
Billboard b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12;
int billboardType = 0;
Cheerios t111, t112, t113, t114, t115, t120, t121;
Cheerios t211, t212, t213, t214, t215, t220, t221;
Cheerios t311, t312, t313, t314, t315, t320, t321;
Cheerios t411, t412, t413, t414, t415, t420, t421;
Cheerios t511, t512, t513, t514, t515, t516, t517, t518, t519, t520, t521, t522, t523;
Cheerios t611, t612, t613, t614, t615, t616, t617, t618, t619, t620, t621, t622, t623;
Cheerios t711, t712, t713, t714, t715, t716, t717, t718, t719, t720, t721, t722, t723;
Cheerios t811, t812, t813, t814, t815, t816, t817, t818, t819, t820, t821, t822, t823;
Cheerios t911, t912, t913, t914, t915, t916, t917, t918, t919, t920, t921, t922, t923;
Cheerios t1011, t1012, t1013, t1014, t1015, t1016, t1017, t1018, t1019, t1020, t1021, t1022, t1023;
Cheerios t1111, t1112, t1113, t1114, t1115, t1116, t1117, t1118, t1119, t1120, t1121, t1122, t1123;
Cheerios t1211, t1212, t1213, t1214, t1215, t1216, t1217, t1218, t1219, t1220, t1221, t1222, t1223;
Orange o1, o2, o3, o4, o5, o6, o7, o8, o9, o10, o11, o12, o13, o14, o15;
Table butter1, butter2, butter3, butter4, butter5, butter6, butter7, butter99;
Car car;

MyMesh cubeMesh;
MyMesh torusMesh;
MyMesh sphereMesh;
MyMesh quadMesh;
MyMesh billboardMesh;

// Created an instance of the Importer class in the meshFromAssimp.cpp file
extern Assimp::Importer importer;
// the global Assimp scene object
extern const aiScene* scene;
vector<MyMesh> backpackMesh;
vector<MyMesh> lampMesh;
char model_dir[50];
// scale factor for the Assimp model to fit in the window
//extern float scaleFactor;

MyMesh* bodyMesh;
MyMesh* wheelsMesh;



float o1Pos[3] = { 1.0f, -2.0f, 10.0f };

float tPos[3] = { -30.0f, 0.0f, -120.0f };

float table_length = 250.0f;

PointLight pl1, pl2, pl3, pl4, pl5, pl6;
DirectionalLight dl;
LightProperties* lights[MAX_LIGHTS];

PerspectiveCamera pCam1;
PerspectiveCamera pCam2;
OrthographicCamera oCam;
bool flag = true;


bool pause = false;
bool restart = false;

inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

GameHudManager gameManager;

void renderScene(void);

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  6000
typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // cor
	GLfloat x, y, z;    // posiçcao
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // aceleraçco
} Particle;
Particle particula[MAX_PARTICULAS];
int dead_num_particles = 0;
bool particlesActive;


void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	//PUT YOUR CODE HERE

	renderScene();
	glutTimerFunc(1000/60, refresh, 0);
}


void iniParticles(void)
{
	GLfloat v, theta, phi;
	int i;

	for (i = 0; i < 500; i++)
	{
		particula[i].x = 0.0f;
		particula[i].z = 0.0f;
	}
	for (i = 500; i < 1000; i++)
	{
		particula[i].x = 0.0f;
		particula[i].z = 80.0f;
	}
	for (i = 1000; i < 1500; i++)
	{
		particula[i].x = 0.0f;
		particula[i].z = -80.0f;
	}

	for (i = 1500; i < 2250; i++)
	{
		particula[i].x = 50.0f;
		particula[i].z = 80.0f;
	}
	for (i = 2250; i < 3000; i++)
	{
		particula[i].x = 50.0f;
		particula[i].z = -80.0f;
	}

	for (i = 3000; i < 3750; i++)
	{
		particula[i].x = 100.0f;
		particula[i].z = 80.0f;
	}
	for (i = 3750; i < 4500; i++)
	{
		particula[i].x = 100.0f;
		particula[i].z = -80.0f;
	}
	for (i = 4500; i < 5000; i++)
	{

		particula[i].x = 150.0f;
		particula[i].z = 0.0f;

	}
	for (i = 5000; i < 5500; i++)
	{

		particula[i].x = 150.0f;
		particula[i].z = 80.0f;

	}
	for (i = 5500; i < MAX_PARTICULAS; i++)
	{

		particula[i].x = 150.0f;
		particula[i].z = -80.0f;

	}

	for (i = 0; i < MAX_PARTICULAS; i++)
	{
		v = 0.8 * frand() + 0.2;
		phi = frand() * M_PI;
		theta = 2.0 * frand() * M_PI;

		particula[i].y = 10.0f;
		particula[i].vx = v * cos(theta) * sin(phi);
		particula[i].vy = v * cos(phi);
		particula[i].vz = v * sin(theta) * sin(phi);
		particula[i].ax = 0.1f; /* simular um pouco de vento */
		particula[i].ay = -0.15f; /* simular a aceleraÁ„o da gravidade */
		particula[i].az = 0.0f;

		/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
		particula[i].r = 0.117f;
		particula[i].g = 0.565f;
		particula[i].b = 1.0;

		particula[i].life = 1.0f;		/* vida inicial */
		particula[i].fade = 0.0025f;	    /* step de decrÈscimo da vida para cada iteraÁ„o */
	}
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void resizeCallbackWrapper(int w, int h) {
	activeCameraRef->ResizeWindow(w, h);
}

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	ratio = (1.0f * w) / h;
	loadIdentity(PROJECTION);
	perspective(53.13f, ratio, 0.1f, 1000.0f);
}


void SendMeshMaterial(MyMesh* mesh, int billboardType)
{
	GLint loc;
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, mesh->mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, mesh->mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, mesh->mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, mesh->mat.shininess);
}

void SendLights(LightProperties* lProps, int lightId)
{
	float lightPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float coneDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (lProps->isSpot == 0)
	{
		multMatrixPoint(VIEW, lProps->position, lightPos);

		if (lProps->isLocal == 0) {
			normalize(lightPos);
		}

	}
	else
	{
		multMatrixPoint(VIEW, lProps->position, lightPos);
		multMatrixPoint(VIEW, lProps->coneDirection, coneDir);

	}

	//When the type of internal state is boolean, zero integer or floating-point values 
	//are converted to FALSE and non-zero values are converted to TRUE.
	std::string lId = std::to_string(lightId);
	std::string isEnabled = "Lights[" + lId + "].isEnabled";
	std::string isLocal = "Lights[" + lId + "].isLocal";
	std::string isSpot = "Lights[" + lId + "].isSpot";
	std::string position = "Lights[" + lId + "].position";
	std::string halfVector = "Lights[" + lId + "].halfVector";
	std::string coneDirection = "Lights[" + lId + "].coneDirection";
	std::string spotCosCutoff = "Lights[" + lId + "].spotCosCutoff";
	std::string spotExponent = "Lights[" + lId + "].spotExponent";

	GLint loc;
	loc = glGetUniformLocation(shader.getProgramIndex(), isEnabled.c_str());
	glUniform1i(loc, lProps->isEnabled);
	loc = glGetUniformLocation(shader.getProgramIndex(), isLocal.c_str());
	glUniform1i(loc, lProps->isLocal);
	loc = glGetUniformLocation(shader.getProgramIndex(), isSpot.c_str());
	glUniform1i(loc, lProps->isSpot);
	loc = glGetUniformLocation(shader.getProgramIndex(), position.c_str());
	glUniform4fv(loc, 1, lightPos);
	loc = glGetUniformLocation(shader.getProgramIndex(), halfVector.c_str());
	glUniform3fv(loc, 1, lProps->halfVector);
	loc = glGetUniformLocation(shader.getProgramIndex(), coneDirection.c_str());
	glUniform4fv(loc, 1, coneDir);
	loc = glGetUniformLocation(shader.getProgramIndex(), spotCosCutoff.c_str());
	glUniform1f(loc, lProps->spotCosCutoff);
	loc = glGetUniformLocation(shader.getProgramIndex(), spotExponent.c_str());
	glUniform1f(loc, lProps->spotExponent);

	glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 
}


// ------------------------------------------------------------
//
// Render stufff
//

void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport) {  //lx, ly represent the projected position of light on viewport

	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0

	glUniform1i(texMode_uniformId, 4); // draw modulated textured particles 
	glUniform1i(tex_loc3, 0);  //use TU 3

	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FlareTextureArray[flare->element[i].textureId]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(quadMesh.vao);
			glDrawElements(quadMesh.type, quadMesh.numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

// Recursive render of the Assimp Scene Graph

void aiRecursive_render(const aiScene* sc, const aiNode* nd, vector<MyMesh> mesh)
{
	GLint loc;

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(MODEL, aux);


	// draw all meshes assigned to this node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {


		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.emissive");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.emissive);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, mesh[nd->mMeshes[n]].mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, mesh[nd->mMeshes[n]].mat.texCount);

		int  diffMapCount = 0;  //read 2 diffuse textures

		//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map


		glUniform1i(useNormalMap_loc, false); //GLSL normalMap variable initialized to 0
		glUniform1i(specularMap_loc, false);
		glUniform1ui(diffMapCount_loc, 0);
		glUniform1i(texMode_uniformId, 9);

		if (mesh[nd->mMeshes[n]].mat.texCount != 0)
			for (unsigned int i = 0; i < mesh[nd->mMeshes[n]].mat.texCount; ++i) {
				if (mesh[nd->mMeshes[n]].texTypes[i] == DIFFUSE) {
					if (diffMapCount == 0) {
						diffMapCount++;
						glUniform1i(diffMapCount_loc, diffMapCount);
						glUniform1i(tex_loc, mesh[nd->mMeshes[n]].texUnits[i] + 6);
					}
					else if (diffMapCount == 1) {
						diffMapCount++;
						glUniform1i(diffMapCount_loc, diffMapCount);
						glUniform1i(tex_loc1, mesh[nd->mMeshes[n]].texUnits[i] + 6);
					}
					else printf("Only supports a Material with a maximum of 2 diffuse textures\n");
				}
				else if (mesh[nd->mMeshes[n]].texTypes[i] == SPECULAR) {
					glUniform1i(tex_loc2, mesh[nd->mMeshes[n]].texUnits[i] + 6);
					glUniform1i(specularMap_loc, false);
				}
				else if (mesh[nd->mMeshes[n]].texTypes[i] == NORMALS) { //Normal map
					glUniform1i(useNormalMap_loc, false);
					glUniform1i(tex_normalMap_loc, mesh[nd->mMeshes[n]].texUnits[i] + 6);
				}
				else printf("Texture Map not supported\n");
			}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// bind VAO
		glBindVertexArray(mesh[nd->mMeshes[n]].vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		// draw
		glDrawElements(mesh[nd->mMeshes[n]].type, mesh[nd->mMeshes[n]].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		aiRecursive_render(sc, nd->mChildren[n], mesh);
	}
	popMatrix(MODEL);

	glUniform1i(useNormalMap_loc, false);
}

void restartObjects() {

	for (int objId = 8; objId < gameObjectsRef.size() - 1; objId++) {
		pushMatrix(MODEL);
		gameObjectsRef[objId]->reset();
		popMatrix(MODEL);
	}
}

void RenderMesh(MyMesh* mesh)
{
	// send matrices to OGL
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(mesh->vao);

	if (!shader.isProgramValid()) {
		printf("Program Not Valid!\n");
		exit(1);
	}
	glDrawElements(mesh->type, mesh->numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void detectCheckpoints() {
	if ((car.GetBBBounds()[0] <= checkpoint1[1] && car.GetBBBounds()[1] >= checkpoint1[0]) &&
		(car.GetBBBounds()[2] <= checkpoint1[3] && car.GetBBBounds()[3] >= checkpoint1[2]) &&
		(car.GetBBBounds()[4] <= checkpoint1[5] && car.GetBBBounds()[5] >= checkpoint1[4])) {
		c1 = true;
	}
	else if ((car.GetBBBounds()[0] <= checkpoint2[1] && car.GetBBBounds()[1] >= checkpoint2[0]) &&
		(car.GetBBBounds()[2] <= checkpoint2[3] && car.GetBBBounds()[3] >= checkpoint2[2]) &&
		(car.GetBBBounds()[4] <= checkpoint2[5] && car.GetBBBounds()[5] >= checkpoint2[4]) && c1 == true) {
		c2 = true;
	}
	else if ((car.GetBBBounds()[0] <= checkpoint3[1] && car.GetBBBounds()[1] >= checkpoint3[0]) &&
		(car.GetBBBounds()[2] <= checkpoint3[3] && car.GetBBBounds()[3] >= checkpoint3[2]) &&
		(car.GetBBBounds()[4] <= checkpoint3[5] && car.GetBBBounds()[5] >= checkpoint3[4]) && c1 == true && c2 == true) {
		c3 = true;
	}
	else if ((car.GetBBBounds()[0] <= checkpoint4[1] && car.GetBBBounds()[1] >= checkpoint4[0]) &&
		(car.GetBBBounds()[2] <= checkpoint4[3] && car.GetBBBounds()[3] >= checkpoint4[2]) &&
		(car.GetBBBounds()[4] <= checkpoint4[5] && car.GetBBBounds()[5] >= checkpoint4[4]) && c1 == true && c2 == true && c3 == true) {
		c1 = false;
		c2 = false;
		c3 = false;
		particlesActive = true;
		iniParticles();
		gameManager.IncrementPoints(100);
	}

}


void detectCollisions() {

	for (int obj_index_1 = 0; obj_index_1 < gameObjectsRef.size(); obj_index_1++)
	{
		GameObject3D* obj_a = gameObjectsRef[obj_index_1];
		if (!obj_a->GetIsCollisionEnabled())
			continue;
		for (int obj_index_2 = 0; obj_index_2 < gameObjectsRef.size() && gameObjectsRef[obj_index_1] != gameObjectsRef[obj_index_2]; obj_index_2++)
		{
			bool result = false;
			GameObject3D* obj_b = gameObjectsRef[obj_index_2];

			if (obj_b->GetIsCollisionEnabled()) {
			result = obj_a->DetectCollision(obj_b);
			}

			if (result) {
				printf("object [%s] colided with object %s\n", obj_a->GetType(), obj_b->GetType());
				obj_a->ResolveCollision(obj_b);
			}
		}
	}

	car.CalculateBoundingBox();
	for (int obj_index_1 = 0; obj_index_1 < gameObjectsRef.size(); obj_index_1++)
	{
		bool result = false;
		GameObject3D* obj_a = gameObjectsRef[obj_index_1];

		if (obj_a->GetIsCollisionEnabled() ) 
		{
			if (obj_a->GetType() == GameObject3D::TYPE::Orange || obj_a->GetType() == GameObject3D::TYPE::Cheerio || obj_a->GetType() == GameObject3D::TYPE::Table) {
				result = car.DetectCollision(obj_a);
			}
		}

		if (result) {
			printf("car colided with object \n");
			car.ResolveCollision(obj_a);
			if (obj_a->GetType() == GameObject3D::TYPE::Orange) {
				gameManager.DecreaseLives();
				c1 = c2 = c3 = false;
			}
		}
	}
}

void updateParticles()
{
	int i;
	float h;

	/* MÈtodo de Euler de integraÁ„o de eq. diferenciais ordin·rias
	h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	h = 0.033;
	if (particlesActive) {

		for (i = 0; i < MAX_PARTICULAS; i++)
		{
			particula[i].x += (h * particula[i].vx);
			particula[i].y += (h * particula[i].vy);
			particula[i].z += (h * particula[i].vz);
			particula[i].vx += (h * particula[i].ax);
			particula[i].vy += (h * particula[i].ay);
			particula[i].vz += (h * particula[i].az);
			particula[i].life -= particula[i].fade;
		}
	}
}


void drawTable() {
	for (int objId = 0; objId < 8; objId++)
	{
		GameObject3D* objRef = gameObjectsRef[objId];
		if (objRef == nullptr) {
			continue;
		}

		objRef->PrepareMeshMaterial();
		MyMesh* mesh = objRef->GetMesh();
		SendMeshMaterial(mesh, 0);

		pushMatrix(MODEL);

		objRef->Update();

		glUniform1i(texMode_uniformId, 0);
		if (objRef->GetIsEnabled()) {
			glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			RenderMesh(mesh);
		}

		popMatrix(MODEL);
	}

	GameObject3D* objRef = gameObjectsRef[gameObjectsRef.size() - 1];

	objRef->PrepareMeshMaterial();
	MyMesh* mesh = objRef->GetMesh();
	SendMeshMaterial(mesh, 0);

	pushMatrix(MODEL);

	objRef->Update();
	glUniform1i(texMode_uniformId, 3); // multitexturing
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_normalMap_loc, 4);


	if (objRef->GetIsEnabled()) {
		glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		RenderMesh(mesh);
	}

	popMatrix(MODEL);
}


void drawObjects(bool isShadow) {
	for (int objId = 8; objId < gameObjectsRef.size() - 1; objId++)
	{
		GameObject3D* objRef = gameObjectsRef[objId];
		if (objRef == nullptr) {
			continue;
		}

		objRef->PrepareMeshMaterial();
		MyMesh* mesh = objRef->GetMesh();
		if (objRef->GetType() == GameObject3D::TYPE::Billboard) {
			SendMeshMaterial(mesh, 1);
		}
		else {
			SendMeshMaterial(mesh, 0);
		}

		pushMatrix(MODEL);

		if (pause == false || objRef->GetType() == GameObject3D::TYPE::Billboard)
			objRef->Update();
		else
			objRef->Paused();

		// FIXME refactor
		if (objId == gameObjectsRef.size() - 1)
		{
			glUniform1i(texMode_uniformId, 3); // multitexturing
		}
		else if (objRef->GetType() == GameObject3D::TYPE::Billboard)
		{
		//	//printf("Billboard in renderScene\n");
			Billboard* bb = dynamic_cast<Billboard*>(objRef);
			bb->orientateBillboard(pCam2.GetPosition(), 3);

			glUniform1i(texMode_uniformId, 4);
		}
		//else if (objRef->GetType() == GameObject3D::TYPE::Orange) {
		//	if (bumpmap)
		//		glUniform1i(texMode_uniformId, 6);
		//	else
		//		glUniform1i(texMode_uniformId, 0); //bump mapping: normal comes from normalMap
		//}
		else
		{
			glUniform1i(texMode_uniformId, 0);
		}

		if (objRef->GetIsEnabled() && objId != 0) {
			if ((gameObjectsRef.size() - 7 <= objId <= gameObjectsRef.size() - 3 || objRef->GetType() == GameObject3D::TYPE::Billboard) && !isShadow) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);

			}
			if (objRef->GetType() != GameObject3D::TYPE::Billboard) {
				glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
				computeDerivedMatrix(PROJ_VIEW_MODEL);
			}
			if (!(isShadow && objRef->GetType() == GameObject3D::TYPE::Billboard)) // don't draw billboard shadow
			{
				RenderMesh(mesh);
			}
			if ((gameObjectsRef.size() - 7 <= objId <= gameObjectsRef.size() - 3 || objRef->GetType() == GameObject3D::TYPE::Billboard) && !isShadow) {
				glDisable(GL_BLEND);

			}
		}

		popMatrix(MODEL);

		if (objRef->GetType() == GameObject3D::TYPE::Orange) {
			Orange* orange = dynamic_cast<Orange*>(objRef);
			orange->StoreBounds(t.GetBBBounds());
		}
	}
}

void UpdateCarMeshes() {
	pushMatrix(MODEL);
	SendMeshMaterial(bodyMesh, 0);
	car.UpdateBody();

	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(bodyMesh);

	SendMeshMaterial(wheelsMesh, 0);
	car.UpdateWheelTopLeft();

	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelsMesh);

	car.UpdateWheelTopRight();

	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelsMesh);

	popMatrix(MODEL); // pop topleftwheel

	car.UpdateWheelBotRight();
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelsMesh);


	popMatrix(MODEL);
	car.UpdateWheelBotLeft();

	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelsMesh);

	popMatrix(MODEL);

}

void RenderParticles()
{
	GLint loc;

	float particle_color[4];

	updateParticles();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[6]); //particle.tga associated to TU0 
	glUniform1i(tex_loc, 0);  //use TU 0
	glUniform1i(texMode_uniformId, 8); // draw modulated textured particles 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_FALSE);  //Depth Buffer Read Only


	for (int i = 0; i < MAX_PARTICULAS; i++)
	{
		if (particula[i].life > 0.0f) /* sÛ desenha as que ainda est„o vivas */
		{

			/* A vida da partÌcula representa o canal alpha da cor. Como o blend est· activo a cor final È a soma da cor rgb do fragmento multiplicada pelo
			alpha com a cor do pixel destino */

			particle_color[0] = particula[i].r;
			particle_color[1] = particula[i].g;
			particle_color[2] = particula[i].b;
			particle_color[3] = particula[i].life;

			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, particle_color);

			pushMatrix(MODEL);
			translate(MODEL, particula[i].x, particula[i].y, particula[i].z);

			// send matrices to OGL
			float pos[3] = { (float)particula[i].x, (float)particula[i].y , (float)particula[i].z };
			l3dBillboardCylindricalBegin(pCam2.GetPosition(), pos);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(quadMesh.vao);
			glDrawElements(quadMesh.type, quadMesh.numIndexes, GL_UNSIGNED_INT, 0);
			popMatrix(MODEL);
		}
		else dead_num_particles++;
	}

	glDepthMask(GL_TRUE); //make depth buffer again writeable

	if (dead_num_particles == MAX_PARTICULAS) {
		particlesActive = false;
		c4 = false;
		dead_num_particles = 0;
		printf("All particles dead\n");
	}

}

void renderAssimpObjects()
{
	//pushMatrix(MODEL);
	//translate(MODEL, 0.0f, 2.0f, 3.0f);
	//scale(MODEL, 1.0f, 1.0f, 1.0f);
	//aiRecursive_render(scene, scene->mRootNode, backpackMesh);
	//popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position[0], 0.0f, position[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position1[0], 0.0f, position1[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position2[0], 0.0f, position2[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position3[0], 0.0f, position3[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position4[0], 0.0f, position4[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position5[0], 0.0f, position5[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

}

void renderScene(void) {


	TimeUtil::UpdateDeltaTime();

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);


	if (activeCameraId == 2)
	{

		float* dir = car.GetDirection();
		float targetOffset[3] = { -dir[0], 0, -dir[2] };
		constProduct(7.0f, targetOffset, targetOffset);
		targetOffset[1] = 5.0f;
		activeCameraRef->FollowTarget(car.GetPosition(), targetOffset, deltaAlpha, deltaBeta);
	}
	else
	{
		activeCameraRef->Update();
	}




	// use our shader
	glUseProgram(shader.getProgramIndex());

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		SendLights(lights[i], i);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);
	//glActiveTexture(GL_TEXTURE5);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[5]);

	//Indicar aos quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);
	glUniform1i(tex_normalMap_loc, 4);
	//glUniform1i(tex_cube_loc, 5);


	glUniform1i(texMode_uniformId, 0); // FIXME refactor

	glEnable(GL_STENCIL_TEST);        // Escrever 1 no stencil buffer onde se for desenhar a reflex„o e a sombra
	glStencilFunc(GL_NEVER, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	drawTable();


	glUniform1i(shadowMode_uniformId, 0);  //iluminaÁ„o phong

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);



	float res[4];
	float mat[16];
	GLfloat plano_chao[4] = { 0,1,0,0 };



	// Render the reflected geometry
	position[1] *= (-1.0f);  //mirror the position of light
	multMatrixPoint(VIEW, position, res);

	glUniform4fv(lPos_uniformId, 1, res);
	pushMatrix(MODEL);

	if (restart) {
		restartObjects();
		restart = false;
	}

	scale(MODEL, 1.0f, -1.0f, 1.0f);
	glCullFace(GL_FRONT);

	drawObjects(false);
	detectCollisions();
	detectCheckpoints();

	if (car.GetIsStopping()) {
		car.StopMovement();
	}

	car.MoveCar();
	glUniform1i(texMode_uniformId, 0);
	UpdateCarMeshes();

	renderAssimpObjects();

	glCullFace(GL_BACK);
	popMatrix(MODEL);

	position[1] *= (-1.0f);  //reset the light position
	multMatrixPoint(VIEW, position, res);
	glUniform4fv(lPos_uniformId, 1, res);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Blend specular Ground with reflected geometry
	drawTable();

	// Render the Shadows
	glUniform1i(shadowMode_uniformId, 1);  //Render with constant color
	shadow_matrix(mat, plano_chao, position);

	glDisable(GL_DEPTH_TEST); //To force the shadow geometry to be rendered even if behind the floor

	////Dark the color stored in color buffer
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

	pushMatrix(MODEL);
	multMatrix(MODEL, mat);
	drawObjects(true);

	renderAssimpObjects();

	// Render Car, FIXME Refactor gameObject3D
	if (car.GetIsStopping())
	{
		car.StopMovement();
	}

	if (pause == false) {
		car.MoveCar();
	}
	UpdateCarMeshes();
	popMatrix(MODEL);


	//	glEnable(GL_DEPTH_TEST);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	//render the geometry
	glUniform1i(shadowMode_uniformId, 0);
	drawObjects(false);

	renderAssimpObjects();

	// Render Car, FIXME Refactor gameObject3D
	if (car.GetIsStopping())
	{
		car.StopMovement();
	}

	if (pause == false) {
		car.MoveCar();
	}
	glUniform1i(texMode_uniformId, 0);
	UpdateCarMeshes();
	car.CalculateBoundingBox();

	if (particlesActive)
	{
		RenderParticles();
	}


	// LENS FLARE

	if (flareEffect) {

		int flarePos[2];
		int m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		pushMatrix(MODEL);
		loadIdentity(MODEL);
		translate(MODEL, flareLightPos[0], flareLightPos[1], flareLightPos[2]);
		computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function

		if (!project(flareLightPos, lightScreenPos, m_viewport))
			printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
		flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
		flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
		popMatrix(MODEL);

		//viewer looking down at  negative z direction
		pushMatrix(PROJECTION);
		loadIdentity(PROJECTION);
		pushMatrix(VIEW);
		loadIdentity(VIEW);
		ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
		render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport);
		popMatrix(PROJECTION);
		popMatrix(VIEW);
	}
	if (!gameManager.UpdateHUD())
		pause = true;

	glutSwapBuffers();
}


void LoadCamera(int camId)
{
	activeCameraId = camId;
	activeCameraRef = camerasRef[camId];
	activeCameraRef->SetProjection();
}


// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27:
			glutLeaveMainLoop();
			break;
		case '1':
			LoadCamera(0);
			break;
		case '2':
			LoadCamera(1);
			break;
		case '3':
			LoadCamera(2);
			break; 
		case 'c':
			pl1.ToggleLight();
			pl2.ToggleLight();
			pl3.ToggleLight();
			pl4.ToggleLight();
			pl5.ToggleLight();
			pl6.ToggleLight();
			break;
		case 'p':
			if (pause == false) {
				car.MoveRight();
			}
			break;
		case 'o':
			if (pause == false) {
				car.MoveLeft();
			}
			break;
		case 'q':
			if (pause == false) {
				car.SetIsStopping(false);
				car.MoveForward();
			}
			break;
		case 'a':
			if (pause == false) {
				car.SetIsStopping(false);
				car.MoveBackward();
			}
			break;
		case 'f':
			if (pause == false) {
				toggleFog = toggleFog == 0 ? 1 : 0;
				glUniform1i(toggleFog_uniformId, toggleFog);
			}
			break;
		case 'h':
			car.GetSpotLightLeft()->isEnabled = car.GetSpotLightLeft()->isEnabled ? 0 : 1;
			car.GetSpotLightRight()->isEnabled = car.GetSpotLightRight()->isEnabled ? 0 : 1;
			break;
		case 'l':
			flareEffect = !flareEffect;
			break;
		case 'n':
			if (pause == false) {
				dl.ToggleLight();
			}
			break;
		case 'k': billboardType++; if (billboardType == 5) billboardType = 0;
			switch (billboardType) {
			case 0: printf("Cheating Spherical (matrix reset)\n"); break;
			case 1: printf("Cheating Cylindrical (matrix reset)\n"); break;
			case 2: printf("True Spherical\n"); break;
			case 3: printf("True Cylindrical\n"); break;
			case 4: printf("No billboarding\n"); break;
			}
		case 's':
			if (gameManager.GetLives() != 0) {
				printf("pause\n");
				if (pause == false) {
					pause = true;
					gameManager.TogglePause();
				}
				else
				{
					gameManager.TogglePause();
					pause = false;
				}
			}
		case 'r':
			if (gameManager.GetLives() == 0) {
				gameManager.RestartGame();
				restart = true;
				//gameManager.TogglePause();
				pause = false;
			}
			break;
		case 'e':
			particlesActive = true;
			iniParticles();
			break;
	}
}

void processKeyUp(unsigned char key, int xx, int yy)
{
	if (key == 'a' || key == 'q')
	{
		car.SetIsStopping(true);
	}
}

// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaAlpha = deltaX;
	deltaY =    yy - startY;
	deltaBeta = deltaY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);


	float pos[3] = { camX, camY, camZ };
	activeCameraRef->SetPosition(pos);

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");
	glBindAttribLocation(shader.getProgramIndex(), TANGENT_ATTRIB, "tangent");

	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode"); // different modes of texturing
	shadowMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "shadowMode");
	
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	toggleFog_uniformId = glGetUniformLocation(shader.getProgramIndex(), "toggleFog");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader.getProgramIndex(), "texmap3");
	
	tex_normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	tex_cube_loc = glGetUniformLocation(shader.getProgramIndex(), "cubeMap");


	useNormalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "useNormalMap");
	specularMap_loc = glGetUniformLocation(shader.getProgramIndex(), "specularMap");
	diffMapCount_loc = glGetUniformLocation(shader.getProgramIndex(), "diffMapCount");


	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());
	
	return(shader.isProgramLinked() && shaderText.isProgramLinked());
}

void initCameras()
{
	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);
	//float pos[3] = { camX, camY, camZ };
	float pos[3] = { 75.0f, 150.0f, 0.0f };
	float pos1[3] = { 75.0f, 200.0f, 0.0f };
	float lookAt[3] = { 75.0f, 0.0f, 0.0f };
	float up[3] = { 0.0f, 0.0f, -1.0f };
	float up2[3] = { 0.0f, 1.0f, 0.0f };
	pCam1 = PerspectiveCamera(WinX, WinY, lookAt, pos1, up, 53.13f, 0.1f, 1000.0f);
	camerasRef.push_back(&pCam1);
	oCam = OrthographicCamera(WinX, WinY, lookAt, pos, up, 0.0f, 500.0f);
	camerasRef.push_back(&oCam);
	pCam2 = PerspectiveCamera(/*WinX*/1920, /*WinY*/1080, lookAt, pos, up2, 53.13f, 0.1f, 1000.0f);
	camerasRef.push_back(&pCam2);

	LoadCamera(0);
}

void initFog()
{
	toggleFog = 0;
	glUniform1i(toggleFog_uniformId, toggleFog);
}


void initMeshPrimitives()
{
	cubeMesh = createCube();
	sphereMesh = createSphere(1.0f, 10);
	torusMesh = createTorus(0.5f, 1.0f, 10, 20);
	billboardMesh = createQuad(6, 6);
	quadMesh = createQuad(2, 2);
}

void initLights()
{
	float position[4] = { 20.0f, 10.0f, 0.0f, 1.0f };
	float position1[4] = { 20.0f, 10.0f, 75.0f, 1.0f };
	float position2[4] = { 75.0f, 10.0f, 80.0f, 1.0f };
	float position3[4] = { 150.0f, 10.0f, 75.0f, 1.0f };
	float position4[4] = { 150.0f, 10.0f, 0.0f, 1.0f };
	float position5[4] = { 75.0f, 10.0f, -75.0f, 1.0f };
	pl1 = PointLight(position);
	pl2 = PointLight(position1);
	pl3 = PointLight(position2);
	pl4 = PointLight(position3);
	pl5 = PointLight(position4);
	pl6 = PointLight(position5);
	lights[0] = pl1.GetLightPtr();
	lights[1] = pl2.GetLightPtr();
	lights[2] = pl3.GetLightPtr();
	lights[3] = pl4.GetLightPtr();
	lights[4] = pl5.GetLightPtr();
	lights[5] = pl6.GetLightPtr();

	dl = DirectionalLight();
	lights[6] = dl.GetLightPtr();
	lights[7] = car.GetSpotLightLeft();
	lights[8] = car.GetSpotLightRight();
}

void initTextures()
{
	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	glGenTextures(7, TextureArray);
	//Texture2D_Loader(TextureArray, "orange_normal.tga", 0);
	Texture2D_Loader(TextureArray, "Orange.jpg", 0);
	//Texture2D_Loader(TextureArray, "checkers_pattern.png", 1);
	Texture2D_Loader(TextureArray, "Choco.jpg", 1);
	Texture2D_Loader(TextureArray, "lightwood.tga", 2);
	Texture2D_Loader(TextureArray, "tree.tga", 3);
	Texture2D_Loader(TextureArray, "ChocoNormal2.png", 4);

	//const char* filenames[] = { "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg" };

	//TextureCubeMap_Loader(TextureArray, filenames, 5);


	Texture2D_Loader(TextureArray, "particle.tga", 6);
}


void initFlare()
{
	//// create geometry and VAO of the quad for flare elements
	//amesh = createQuad(1, 1);
	//myMeshes.push_back(amesh);

	//Flare elements textures
	glGenTextures(5, FlareTextureArray);
	Texture2D_Loader(FlareTextureArray, "crcl.tga", 0);
	Texture2D_Loader(FlareTextureArray, "flar.tga", 1);
	Texture2D_Loader(FlareTextureArray, "hxgn.tga", 2);
	Texture2D_Loader(FlareTextureArray, "ring.tga", 3);
	Texture2D_Loader(FlareTextureArray, "sun.tga", 4);


	////Load flare from file
	loadFlareFile(&AVTflare, "flare.txt");
	flareEffect = false;
}

int initObjectModels(vector<MyMesh>* objMesh, char* name, bool loadTextures) {
	std::string filepath;

	strcat(model_dir, name);
	std::ostringstream oss;
	oss << model_dir << "/" << model_dir << ".obj";
	filepath = oss.str();   //path of OBJ file in the VS project

	strcat(model_dir, "/");  //directory path in the VS project

	//check if file exists
	ifstream fin(filepath.c_str());
	if (!fin.fail()) {
		fin.close();
		//break;
	}
	else
		printf("Couldn't open file: %s\n", filepath.c_str());

	if (!Import3DFromFile(filepath))
		return(0);
	*objMesh = createMeshFromAssimp(scene, loadTextures);
	return(1);
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init()
{
	MyMesh amesh;

	initTextures();
	initLights();
	initMeshPrimitives();
	initFog();
	initFlare();
	initObjectModels(&lampMesh, "lamp", false);
	//initObjectModels(&backpackMesh, "backpack", true);


	gameManager = GameHudManager(&shaderText);

	car = Car(&cubeMesh, &torusMesh, true, 1.0f);
	bodyMesh = car.GetBodyMesh();
	wheelsMesh = car.GetWheelMesh();


	float alt = 0.35;

	float rad = 1.5f;

	Material m1;

	float amb[] = { 0.5f, 0.3f, 0.0f, 0.5f };
	float diff[] = { 0.8f, 0.7f, 0.1f, 0.5f };
	float spec[] = { 0.3f, 0.3f, 0.3f, 1.0f };


	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	memcpy(m1.ambient, amb, 4 * sizeof(float));
	memcpy(m1.diffuse, diff, 4 * sizeof(float));
	memcpy(m1.specular, spec, 4 * sizeof(float));
	memcpy(m1.emissive, emissive, 4 * sizeof(float));
	m1.shininess = shininess;
	m1.texCount = texcount;


	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);


	Material m2;

	float amb2[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diff2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec2[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	float emissive2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess2 = 800.0f;
	int texcount2 = 0;


	memcpy(m2.ambient, amb2, 4 * sizeof(float));
	memcpy(m2.diffuse, diff2, 4 * sizeof(float));
	memcpy(m2.specular, spec2, 4 * sizeof(float));
	memcpy(m2.emissive, emissive2, 4 * sizeof(float));
	m2.shininess = shininess2;
	m2.texCount = texcount2;

	float tPos20[3] = { 3.0f, -1.45f, -65.0f };


	t20 = Table(m2, &cubeMesh, tPos20, gameObjectsRef.size(), false, 15.0f, 1.5f, 140.0f);

	t20.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t20);

	printf("%d\n", t20.GetMeshId());

	float tPos21[3] = { 0.0f, -1.45f, 75.0f };

	t21 = Table(m2, &cubeMesh, tPos21, gameObjectsRef.size(), false, 30.0f, 1.5f, 30.0f);


	t21.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t21);

	float tPos22[3] = { 30.0f, -1.45f, 80.0f };

	t22 = Table(m2, &cubeMesh, tPos22, gameObjectsRef.size(), false, 105.0f, 1.5f, 20.0f);

	t22.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t22);

	float tPos23[3] = { 135.0f, -1.45f, 75.0f };

	t23 = Table(m2, &cubeMesh, tPos23, gameObjectsRef.size(), false, 30.0f, 1.5f, 30.0f);

	t23.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t23);

	float tPos24[3] = { 145.0f, -1.45f, -65.0f };

	t24 = Table(m2, &cubeMesh, tPos24, gameObjectsRef.size(), false, 15.0f, 1.5f, 140.0f);

	t24.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t24);

	float tPos25[3] = { 0.0f, -1.45f, -95.0f };

	t25 = Table(m2, &cubeMesh, tPos25, gameObjectsRef.size(), false, 30.0f, 1.5f, 30.0f);


	t25.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t25);

	float tPos26[3] = { 30.0f, -1.45f, -90.0f };

	t26 = Table(m2, &cubeMesh, tPos26, gameObjectsRef.size(), false, 105.0f, 1.5f, 20.0f);

	t26.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t26);

	float tPos27[3] = { 135.0f, -1.45f, -95.0f };

	t27 = Table(m2, &cubeMesh, tPos27, gameObjectsRef.size(), false, 30.0f, 1.5f, 30.0f);

	printf("%d\n", t27.GetMeshId());

	t27.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t27);

	//Billboards

	Material mtree;

	float tree_spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float tree_shininess = 10.0f;
	float emissive_tree[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount_tree = 0;

	//memcpy(mtree.ambient, amb3, 4 * sizeof(float));
	//memcpy(mtree.diffuse, diff3, 4 * sizeof(float));
	memcpy(mtree.specular, tree_spec, 4 * sizeof(float));
	memcpy(mtree.emissive, emissive_tree, 4 * sizeof(float));
	mtree.shininess = tree_shininess;
	mtree.texCount = texcount_tree;

	float tPosBillboard1[3] = { 0.0f, 3.0f, -10.0f };
	b1 = Billboard(mtree, &billboardMesh, tPosBillboard1, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b1);

	float tPosBillboard2[3] = { 0.0f, 3.0f, 25.0f };
	b2 = Billboard(mtree, &billboardMesh, tPosBillboard2, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b2);

	float tPosBillboard3[3] = { 20.0f, 3.0f, 25.0f };
	b3 = Billboard(mtree, &billboardMesh, tPosBillboard3, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b3);

	float tPosBillboard4[3] = { 0.0f, 3.0f, 45.0f };
	b4 = Billboard(mtree, &billboardMesh, tPosBillboard4, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b4);

	float tPosBillboard5[3] = { 50.0f, 3.0f, 103.0f };
	b5 = Billboard(mtree, &billboardMesh, tPosBillboard5, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b5);

	float tPosBillboard6[3] = { 120.0f, 3.0f, 103.0f };
	b6 = Billboard(mtree, &billboardMesh, tPosBillboard6, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b6);

	float tPosBillboard7[3] = { 50.0f, 3.0f, -97.0f };
	b7 = Billboard(mtree, &billboardMesh, tPosBillboard7, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b7);

	float tPosBillboard8[3] = { 120.0f, 3.0f, -97.0f };
	b8 = Billboard(mtree, &billboardMesh, tPosBillboard8, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b8.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b8);

	float tPosBillboard9[3] = { 163.0f, 3.0f, 0.0f };
	b9 = Billboard(mtree, &billboardMesh, tPosBillboard9, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b9.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b9);


	float tPosBillboard10[3] = { 163.0f, 3.0f, -60.0f };
	b10 = Billboard(mtree, &billboardMesh, tPosBillboard10, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b10.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b10);

	float tPosBillboard11[3] = { 163.0f, 3.0f, 60.0f };
	b11 = Billboard(mtree, &billboardMesh, tPosBillboard11, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b11.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b11);

	float tPosBillboard12[3] = { 20.0f, 3.0f, -50.0f };
	b12 = Billboard(mtree, &billboardMesh, tPosBillboard12, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b12.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b12);


	Material m4;

	float amb4[] = { 0.36f, 0.2f, 0.2f, 1.0f };
	float diff4[] = { 0.545f, 0.27f, 0.255f, 1.0f };
	float spec4[] = { 0.545f, 0.27f, 0.255f, 1.0f };

	float emissive4[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess4 = 800.0f;
	int texcount4 = 0;

	memcpy(m4.ambient, amb4, 4 * sizeof(float));
	memcpy(m4.diffuse, diff4, 4 * sizeof(float));
	memcpy(m4.specular, spec4, 4 * sizeof(float));
	memcpy(m4.emissive, emissive4, 4 * sizeof(float));
	m4.shininess = shininess4;
	m4.texCount = texcount4;


	float tPos111[3] = { 2.0f, alt, 72.0f };
	t111 = Cheerios(m4, &torusMesh, tPos111, gameObjectsRef.size(), rad);

	t111.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t111);

	float tPos112[3] = { 6.0f, alt, 82.0f };
	t112 = Cheerios(m4, &torusMesh, tPos112, gameObjectsRef.size(), rad);

	t112.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t112);

	float tPos113[3] = { 12.0f, alt, 90.0f };
	t113 = Cheerios(m4, &torusMesh, tPos113, gameObjectsRef.size(), rad);

	t113.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t113);

	float tPos114[3] = { 19.0f, alt, 97.0f };
	t114 = Cheerios(m4, &torusMesh, tPos114, gameObjectsRef.size(), rad);

	t114.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t114);


	float tPos115[3] = { 27.0f, alt, 101.0f };
	t115 = Cheerios(m4, &torusMesh, tPos115, gameObjectsRef.size(), rad);

	t115.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t115);

	float tPos120[3] = { 19.0f, alt, 72.0f };
	t120 = Cheerios(m4, &torusMesh, tPos120, gameObjectsRef.size(), rad);

	t120.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t120);


	float tPos121[3] = { 26.0f, alt, 79.0f };
	t121 = Cheerios(m4, &torusMesh, tPos121, gameObjectsRef.size(), rad);

	t121.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t121);



	float rPos1 = 2.0f;
	float rPos2 = 19.0f;

	float tPos11[3] = { rPos1, alt, -51.0f };
	float tPos12[3] = { rPos2, alt, -51.0f };


	t511 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t511.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t511);

	t611 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t611.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t611);

	tPos11[2] = tPos12[2] += 11.0f;

	t512 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t512.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t512);

	t612 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t612.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t612);

	tPos11[2] = tPos12[2] += 11.0f;

	t513 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t513.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t513);

	t613 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t613.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t613);

	tPos11[2] = tPos12[2] += 11.0f;

	t514 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t514.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t514);

	t614 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t614.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t614);

	tPos11[2] = tPos12[2] += 11.0f;

	t515 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t515.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t515);

	t615 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t615.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t615);

	tPos11[2] = tPos12[2] += 11.0f;

	t516 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t516.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t516);

	t616 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t616.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t616);

	tPos11[2] = tPos12[2] += 11.0f;

	t517 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t517.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t517);

	t617 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t617.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t617);

	tPos11[2] = tPos12[2] += 11.0f;

	t518 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t518.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t518);

	t618 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t618.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t618);

	tPos11[2] = tPos12[2] += 11.0f;

	t519 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t519.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t519);

	t619 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t619.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t619);

	tPos11[2] = tPos12[2] += 11.0f;

	t520 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t520.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t520);

	t620 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t620.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t620);

	tPos11[2] = tPos12[2] += 11.0f;

	t521 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t521.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t521);

	t621 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t621.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t621);


	float tPos211[3] = { 132.0f, alt, 101.0f };
	t211 = Cheerios(m4, &torusMesh, tPos211, gameObjectsRef.size(), rad);

	t211.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t211);

	float tPos212[3] = { 140.0f, alt, 98.0f };
	t212 = Cheerios(m4, &torusMesh, tPos212, gameObjectsRef.size(), rad);

	t212.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t212);

	float tPos213[3] = { 148.0f, alt, 91.0f };
	t213 = Cheerios(m4, &torusMesh, tPos213, gameObjectsRef.size(), rad);

	t213.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t213);

	float tPos214[3] = { 155.0f, alt, 83.0f };
	t214 = Cheerios(m4, &torusMesh, tPos214, gameObjectsRef.size(), rad);

	t214.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t214);

	float tPos215[3] = { 160.0f, alt, 73.0f };
	t215 = Cheerios(m4, &torusMesh, tPos215, gameObjectsRef.size(), rad);

	t215.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t215);

	float tPos220[3] = { 133.0f, alt, 79.0f };
	t220 = Cheerios(m4, &torusMesh, tPos220, gameObjectsRef.size(), rad);

	t220.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t220);

	float tPos221[3] = { 142.0f, alt, 72.0f };
	t221 = Cheerios(m4, &torusMesh, tPos221, gameObjectsRef.size(), rad);

	t221.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t221);


	float rPos3 = 80.0f;
	float rPos4 = 102.0f;

	float tPos13[3] = { 35.0f, alt, rPos3 };
	float tPos14[3] = { 35.0f, alt, rPos4 };

	t711 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t711.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t711);

	t811 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t811.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t811);

	tPos13[0] = tPos14[0] += 11.0f;

	t712 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t712.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t712);

	t812 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t812.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t812);

	tPos13[0] = tPos14[0] += 11.0f;

	t713 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t713.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t713);

	t813 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t813.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t813);

	tPos13[0] = tPos14[0] += 11.0f;

	t714 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t714.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t714);

	t814 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t814.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t814);

	tPos13[0] = tPos14[0] += 11.0f;

	t715 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t715.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t715);

	t815 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t815.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t815);

	tPos13[0] = tPos14[0] += 11.0f;

	t716 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t716.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t716);

	t816 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t816.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t816);

	tPos13[0] = tPos14[0] += 11.0f;

	t717 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t717.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t717);

	t817 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t817.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t817);

	tPos13[0] = tPos14[0] += 11.0f;

	t718 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t718.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t718);

	t818 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t818.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t818);

	tPos13[0] = tPos14[0] += 11.0f;

	t719 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t719.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t719);

	t819 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t819.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t819);



	float tPos311[3] = { 160.0f, alt, -62.0f };
	t311 = Cheerios(m4, &torusMesh, tPos311, gameObjectsRef.size(), rad);

	t311.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t311);

	float tPos312[3] = { 155.0f, alt, -72.0f };
	t312 = Cheerios(m4, &torusMesh, tPos312, gameObjectsRef.size(), rad);

	t312.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t312);

	float tPos313[3] = { 148.0f, alt, -80.0f };
	t313 = Cheerios(m4, &torusMesh, tPos313, gameObjectsRef.size(), rad);

	t313.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t313);

	float tPos314[3] = { 140.0f, alt, -88.0f };
	t314 = Cheerios(m4, &torusMesh, tPos314, gameObjectsRef.size(), rad);

	t314.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t314);

	float tPos315[3] = { 132.0f, alt, -92.0f };
	t315 = Cheerios(m4, &torusMesh, tPos315, gameObjectsRef.size(), rad);

	t315.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t315);

	float tPos320[3] = { 142.0f, alt, -62.0f };
	t320 = Cheerios(m4, &torusMesh, tPos320, gameObjectsRef.size(), rad);

	t320.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t320);

	float tPos321[3] = { 133.0f, alt, -69.0f };
	t321 = Cheerios(m4, &torusMesh, tPos321, gameObjectsRef.size(), rad);

	t321.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t321);



	rPos1 = 142.0f;
	rPos2 = 162.0f;

	tPos11[0] = rPos1;
	tPos12[0] = rPos2,
		tPos11[2] = tPos12[2] = -57.0f;


	t911 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t911.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t911);

	t1011 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1011.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1011);

	tPos11[2] = tPos12[2] += 11.0f;

	t912 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t912.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t912);

	t1012 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1012.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1012);

	tPos11[2] = tPos12[2] += 11.0f;

	t913 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t913.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t913);

	t1013 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1013.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1013);

	tPos11[2] = tPos12[2] += 11.0f;

	t914 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t914.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t914);

	t1014 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1014.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1014);

	tPos11[2] = tPos12[2] += 11.0f;

	t915 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t915.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t915);

	t1015 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1015.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1015);

	tPos11[2] = tPos12[2] += 11.0f;

	t916 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t916.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t916);

	t1016 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1016.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1016);

	tPos11[2] = tPos12[2] += 11.0f;

	t917 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t917.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t917);

	t1017 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1017.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1017);

	tPos11[2] = tPos12[2] += 11.0f;

	t918 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t918.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t918);

	t1018 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1018.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1018);

	tPos11[2] = tPos12[2] += 11.0f;

	t919 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t919.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t919);

	t1019 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1019.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1019);

	tPos11[2] = tPos12[2] += 11.0f;

	t920 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t920.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t920);

	t1020 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1020.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1020);

	tPos11[2] = tPos12[2] += 11.0f;

	t921 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t921.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t921);

	t1021 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1021.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1021);

	tPos11[2] = tPos12[2] += 11.0f;

	t922 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t922.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t922);

	t1022 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1022.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1022);





	float tPos411[3] = { 2.0f, alt, -62.0f };
	t411 = Cheerios(m4, &torusMesh, tPos411, gameObjectsRef.size(), rad);

	t411.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t411);

	float tPos412[3] = { 6.0f, alt, -72.0f };
	t412 = Cheerios(m4, &torusMesh, tPos412, gameObjectsRef.size(), rad);

	t412.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t412);

	float tPos413[3] = { 12.0f, alt, -80.0f };
	t413 = Cheerios(m4, &torusMesh, tPos413, gameObjectsRef.size(), rad);

	t413.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t413);

	float tPos414[3] = { 19.0f, alt, -88.0f };
	t414 = Cheerios(m4, &torusMesh, tPos414, gameObjectsRef.size(), rad);

	t414.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t414);

	float tPos415[3] = { 27.0f, alt, -92.0f };
	t415 = Cheerios(m4, &torusMesh, tPos415, gameObjectsRef.size(), rad);

	t415.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t415);

	float tPos420[3] = { 19.0f, alt, -62.0f };
	t420 = Cheerios(m4, &torusMesh, tPos420, gameObjectsRef.size(), rad);

	t420.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t420);


	float tPos421[3] = { 26.0f, alt, -69.0f };
	t421 = Cheerios(m4, &torusMesh, tPos421, gameObjectsRef.size(), rad);

	t421.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t421);



	rPos3 = -93.0f;
	rPos4 = -71.0f;

	tPos13[0] = 35.0f;
	tPos13[2] = rPos3;
	tPos14[0] = 35.0f;
	tPos14[2] = rPos4;

	t1111 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1111.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1111);

	t1211 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1211.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1211);

	tPos13[0] = tPos14[0] += 11.0f;

	t1112 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1112.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1112);

	t1212 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1212.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1212);

	tPos13[0] = tPos14[0] += 11.0f;

	t1113 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1113.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1113);

	t1213 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1213.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1213);

	tPos13[0] = tPos14[0] += 11.0f;

	t1114 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1114.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1114);

	t1214 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1214.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1214);

	tPos13[0] = tPos14[0] += 11.0f;

	t1115 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1115.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1115);

	t1215 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1215.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1215);

	tPos13[0] = tPos14[0] += 11.0f;

	t1116 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1116.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1116);

	t1216 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1216.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1216);

	tPos13[0] = tPos14[0] += 11.0f;

	t1117 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1117.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1117);

	t1217 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1217.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1217);

	tPos13[0] = tPos14[0] += 11.0f;

	t1118 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1118.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1118);

	t1218 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1218.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1218);

	tPos13[0] = tPos14[0] += 11.0f;

	t1119 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1119.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1119);

	t1219 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1219.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1219);


	Material m3;
	float amb3[] = { 1.0f, 0.5f, 0.3f, 1.0f };
	float diff3[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec3[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float emissive3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess3 = 0.0f;
	int texcount3 = 0;

	memcpy(m3.ambient, amb3, 4 * sizeof(float));
	memcpy(m3.diffuse, diff3, 4 * sizeof(float));
	memcpy(m3.specular, spec3, 4 * sizeof(float));
	memcpy(m3.emissive, emissive3, 4 * sizeof(float));
	m3.shininess = shininess3;
	m3.texCount = texcount3;

	o1 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o1);

	o2 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o2);

	o3 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o3);

	o4 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o4);

	o5 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o5);

	o6 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o6);

	o7 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o7);

	o8 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o8.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o8);

	o9 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o9.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o9);

	o10 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o10.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o10);

	o11 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o11.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o11);

	o12 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o12.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o12);

	o13 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o13.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o13);

	o14 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o14.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o14);

	o15 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o15.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o15);

	//Butters
	float tPosButter1[3] = { 5.0f, 0.0f, 5.0f };
	butter1 = Table(m1, &cubeMesh, tPosButter1, gameObjectsRef.size(), true, 6.0f, 1.5f, 4.0f);
	butter1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter1);

	float tPosButter2[3] = { 6.0f, 0.0f, 30.0f };
	butter2 = Table(m1, &cubeMesh, tPosButter2, gameObjectsRef.size(), true, 6.0f, 1.5f, 4.0f);
	butter2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter2);

	float tPosButter3[3] = { 50.0f, 0.0f, 87.0f };
	butter3 = Table(m1, &cubeMesh, tPosButter3, gameObjectsRef.size(), true, 6.0f, 1.5f, 4.0f);
	butter3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter3);

	float tPosButter4[3] = { 95.0f, 0.0f, 83.0f };
	butter4 = Table(m1, &cubeMesh, tPosButter4, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter4);

	float tPosButter5[3] = { 155.0f, 0.0f, 10.0f };
	butter5 = Table(m1, &cubeMesh, tPosButter5, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter5);

	float tPosButter6[3] = { 40.0f, 0.0f, -85.0f };
	butter6 = Table(m1, &cubeMesh, tPosButter6, gameObjectsRef.size(), true, 6.0f, 1.5f, 4.0f);
	butter6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter6);

	float tPosButter7[3] = { 90.0f, 0.0f, -80.0f };
	butter7 = Table(m1, &cubeMesh, tPosButter7, gameObjectsRef.size(), true, 6.0f, 1.5f, 4.0f);
	butter7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter7);



	t = Table(m1, &cubeMesh, tPos, gameObjectsRef.size(), false, table_length, 0.001f, table_length);
	t.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t);

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (4, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);

	initCameras();

//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	//glutIdleFunc(renderScene);  // Use it for maximum performance
	glutTimerFunc(1000 / 60, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	glutKeyboardUpFunc(processKeyUp);

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}


unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}

void loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char            name[8] = { '\0', };
			double          dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}




