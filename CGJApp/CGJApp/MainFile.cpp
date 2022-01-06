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
// Author: João Madeiras Pereira
//

#include <math.h>
#include <iostream>
#include <sstream>
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


#include "avtFreeType.h"
#include "Texture_Loader.h"

// Custom headers
#include "TimeUtil.h" 
#include "GameObjects/Table.h"
#include "GameObjects/Cheerios.h"
#include "GameObjects/Orange.h"
#include "GameObjects/Car.h"
#include "Camera/PerspectiveCamera.h"
#include "Camera/OrthographicCamera.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
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
const string font_name = "fonts/arial.ttf";

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
GLint tex_loc, tex_loc1, tex_loc2;
GLint texMode_uniformId, shadowMode_uniformId;

GLuint TextureArray[7];

//Flare effect
//FLARE_DEF AVTflare;
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

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
//float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

Table t;
Table t20, t21, t22, t23, t24, t25, t26, t27, t28;//Road
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

void renderScene(void);



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
	//printf("pos: %f %f %f \n", lProps->position[0], lProps->position[1], lProps->position[2]);
	//printf("dir: %f %f %f \n", lProps->coneDirection[0], lProps->coneDirection[1], lProps->coneDirection[2]);

	float lightPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float coneDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (lProps->isSpot == 0)
	{
		//if (lProps->isLocal == 0) 
			//printf("dir BEFORE VIEW: %f %f %f %f\n", lProps->position[0], lProps->position[1], lProps->position[2], lProps->position[3]);

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


void detectCollisions() {

	for (int obj_index_1 = 0; obj_index_1 < gameObjectsRef.size(); obj_index_1++)
	{
		GameObject3D* obj_a = gameObjectsRef[obj_index_1];
		if (!obj_a->GetIsCollisionEnabled())
			continue;

		for (int obj_index_2 = 0; obj_index_2 < gameObjectsRef.size(); obj_index_2++)
		{
			bool result = false;
			GameObject3D* obj_b = gameObjectsRef[obj_index_2];

			if (obj_b->GetIsCollisionEnabled()) {
				result = obj_a->DetectCollisionWithObject(obj_b);
			}

			if (result) {
				//printf("object [%s] colided with object %s\n", obj_a->GetType(), obj_b->GetType());
				obj_a->ResolveCollisionWithObject(obj_b);
			}
		}
	}

	//car.PrintBB();
	//printf("car position ( %f, %f, %f)\n", car.GetPosition()[0], car.GetPosition()[1], car.GetPosition()[2]);
	//printf("bb bounds ( %f, %f, %f, %f, %f, %f)\n", car.GetBBBounds()[0], car.GetBBBounds()[1], car.GetBBBounds()[2], car.GetBBBounds()[3], car.GetBBBounds()[4], car.GetBBBounds()[5]);

	for (int obj_index_1 = 0; obj_index_1 < gameObjectsRef.size(); obj_index_1++)
	{
		bool result = false;
		GameObject3D* obj_a = gameObjectsRef[obj_index_1];

		if (obj_a->GetIsCollisionEnabled() /* && car.GetIsCollisionEnabled()*/) {// Resolver colisões com car<<<<<<<<<<<<<<<<<<!!!!!!!!!!!!!!!!!!
			//printf("CAR    bb bounds ( %f, %f, %f, %f, %f, %f)\n", car.GetBBBounds()[0], car.GetBBBounds()[1], car.GetBBBounds()[2], car.GetBBBounds()[3], car.GetBBBounds()[4], car.GetBBBounds()[5]);
			//car.PrintBB();
			if (obj_a->GetType() == GameObject3D::TYPE::Table) {
				//Table* t = dynamic_cast<Table*>(obj_a);

				//printf("BUTTER bb bounds ( %f, %f, %f, %f, %f, %f)\n", t->GetBBBounds()[0], t->GetBBBounds()[1], t->GetBBBounds()[2], t->GetBBBounds()[3], t->GetBBBounds()[4], t->GetBBBounds()[5]);
				//printf("CAR    bb bounds ( %f, %f, %f, %f, %f, %f)\n", car.GetBBBounds()[0], car.GetBBBounds()[1], car.GetBBBounds()[2], car.GetBBBounds()[3], car.GetBBBounds()[4], car.GetBBBounds()[5]);
			}
			//Table* t = dynamic_cast<Table*>(obj_a);
			//t->PrintBB();
			//printf("bb bounds ( %f, %f, %f, %f, %f, %f)\n", t->GetBBBounds()[0], t->GetBBBounds()[1], t->GetBBBounds()[2], t->GetBBBounds()[3], t->GetBBBounds()[4], t->GetBBBounds()[5]);
			if (obj_a->GetType() == GameObject3D::TYPE::Table || obj_a->GetType() == GameObject3D::TYPE::Orange || obj_a->GetType() == GameObject3D::TYPE::Cheerio) {
				//result = car.DetectCollisionWithObject(obj_a);
			}
		}

		if (result) {
			printf("car colided with object \n");
			//car.ResolveCollisionWithObject(obj_a);
			if (obj_a->GetType() == GameObject3D::TYPE::Orange) {
				//
				// 
				//gameManager.DecreaseLives();
				//c1 = c2 = c3 = false;
			}
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
		//else if (objRef->GetType() == GameObject3D::TYPE::Billboard)
		//{
		//	//printf("Billboard in renderScene\n");
		//	Billboard* bb = dynamic_cast<Billboard*>(objRef);
		//	bb->orientateBillboard(pCam2.GetPosition(), 3);

		//	glUniform1i(texMode_uniformId, 4);
		//}
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
			if ((objId == gameObjectsRef.size() - 2 || objRef->GetType() == GameObject3D::TYPE::Billboard) && !isShadow) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);

			}
			if (objRef->GetType() != GameObject3D::TYPE::Billboard) {
				//glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
				computeDerivedMatrix(PROJ_VIEW_MODEL);
			}
			if (!(isShadow && objRef->GetType() == GameObject3D::TYPE::Billboard)) // don't draw billboard shadow
			{
				RenderMesh(mesh);
			}
			if ((objId == gameObjectsRef.size() - 2 || objRef->GetType() == GameObject3D::TYPE::Billboard) && !isShadow) {
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

		//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
		glUniform4fv(lPos_uniformId, 1, res);

	int objId=0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

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
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, TextureArray[3]);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, TextureArray[4]);
	//glActiveTexture(GL_TEXTURE5);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[5]);

	//Indicar aos quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	//glUniform1i(tex_loc3, 3);
	//glUniform1i(tex_normalMap_loc, 4);
	//glUniform1i(tex_cube_loc, 5);

	glUniform1i(texMode_uniformId, 0); // FIXME refactor

	drawTable();

	drawObjects(false);

	if (car.GetIsStopping()) {
		car.StopMovement();
	}

	car.MoveCar();
	glUniform1i(texMode_uniformId, 0);
	UpdateCarMeshes();


	//loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	//glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
	//loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	//glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
	//loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	//glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
	//loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	//glUniform1f(loc, myMeshes[objId].mat.shininess);
	//pushMatrix(MODEL);
	//translate(MODEL, 1.5 * 2.0f, 0.0f, 1 * 2.0f);

	// send matrices to OGL
	//computeDerivedMatrix(PROJ_VIEW_MODEL);
	//glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	//glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	//computeNormalMatrix3x3();
	//glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	//glBindVertexArray(myMeshes[objId].vao);

	//if (!shader.isProgramValid()) {
	//	printf("Program Not Valid!\n");
	//	exit(1);
	//}
	//glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	//popMatrix(MODEL);
	objId++;
	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	//the glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	//viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	//RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	//RenderText(shaderText, "CGJ Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

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
		case 'h':
			car.GetSpotLightLeft()->isEnabled = car.GetSpotLightLeft()->isEnabled ? 0 : 1;
			car.GetSpotLightRight()->isEnabled = car.GetSpotLightRight()->isEnabled ? 0 : 1;
			break;
		case 'n':
			if (pause == false) {
				dl.ToggleLight();
			}
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
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	
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



void initMeshPrimitives()
{
	cubeMesh = createCube();
	sphereMesh = createSphere(1.0f, 10);
	torusMesh = createTorus(0.5f, 1.0f, 10, 20);
	//billboardMesh = createQuad(6, 6);
	//quadMesh = createQuad(2, 2);
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
	//Texture2D_Loader(TextureArray, "tree.tga", 3);
	//Texture2D_Loader(TextureArray, "normal.tga", 4);

	//const char* filenames[] = { "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg" };

	//TextureCubeMap_Loader(TextureArray, filenames, 5);


	//Texture2D_Loader(TextureArray, "particle.tga", 6);
}


// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init()
{
	MyMesh amesh;

	initLights();
	initMeshPrimitives();

	//float carPos[3] = { 0.0f, 0.0f, 0.0f };
	car = Car(&cubeMesh, &torusMesh, true, 1.0f);
	bodyMesh = car.GetBodyMesh();
	wheelsMesh = car.GetWheelMesh();


	float alt = 0.35;

	float rad = 1.5f;

	Material m1;

	float amb[] = { 0.3f, 0.3f, 0.0f, 0.5f };
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

	t27.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t27);

	Material m4;

	float amb4[] = { 0.2f, 0.15f, 0.1f, 1.0f };
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



