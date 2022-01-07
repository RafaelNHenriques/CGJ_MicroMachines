#include "GameObjects/GameObject3D.h"
#include <Lights/SpotLight.h>

#define M_PI       3.14159265358979323846f


using namespace std;

#pragma once
class Car
{
public:
	float bb_identity[8][4] = { {1.0f, 0.0f, 0.0f, 1.0f}
				,{ 0.0f, 0.0f, 1.0f, 1.0f}
				,{ 1.0f, 0.0f, 1.0f, 1.0f}
				,{ 0.0f, 0.0f, 0.0f, 1.0f}
				,{ 1.0f, 1.0f, 1.0f, 1.0f}
				,{ 1.0f, 1.0f, 0.0f, 1.0f}
				,{ 0.0f, 1.0f, 1.0f, 1.0f}
				,{ 0.0f, 1.0f, 0.0f, 1.0f} };
	float bb[8][4] = { {1.0f, 0.0f, 0.0f, 1.0f}
					,{ 0.0f, 0.0f, 1.0f, 1.0f}
					,{ 1.0f, 0.0f, 1.0f, 1.0f}
					,{ 0.0f, 0.0f, 0.0f, 1.0f}
					,{ 1.0f, 1.0f, 1.0f, 1.0f}
					,{ 1.0f, 1.0f, 0.0f, 1.0f}
					,{ 0.0f, 1.0f, 1.0f, 1.0f}
					,{ 0.0f, 1.0f, 0.0f, 1.0f} };
private:
	float speed = 0.0f;
	float maxSpeed = 20.0f;
	float slLeft_offset[4] = { 2.0f, 2.0f, 0.0f, 1.0f };
	float slRight_offset[4] = { 2.0f, 2.0f, 1.0f, 1.0f };
	float position[3] = { 5.0f, 0.28f, 0.0f };
	float old_position[3] = { 0.0f, 0.0f, 0.0f };
	float direction[3] = { 1.0f, 0.0f, 0.0f };
	float acceleration = 15.0f;
	float rotationAngle = 0.0f;
	float wheel_rot_angle = 0.0f;
	float wheel_rot_speed = 0.0f;
	float up[3] = { 0.0f, 1.0f, 0.0f };
	float wheel_radius = 0.0f;
	float rot_dir[3] = { 0.0f, 0.0f, 0.0f };
	bool isCollisionEnabled;
	float rotationSpeed = 130.0f; // FIXME ROTATION SPEED
	//min_x, max_x, min_y, max_y, min_z, max_z

	float bb_bounds[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	bool IsStopping = false;
	bool accelerating = false;

	MyMesh* bodyMesh;
	Material bodyMaterial;
	Material topMaterial;
	MyMesh* wheelMesh;
	Material wheelMaterial;


	SpotLight slLeft;
	SpotLight slRight;
public:
	Car(MyMesh* cubeMesh, MyMesh* torusMesh, bool isCollisionEnabled, float radius);
	Car() = default;
	MyMesh* GetBodyMesh();
	MyMesh* GetWheelMesh();
	float* GetDirection() { return direction; }
	float* GetPosition() { return position; }


	bool GetIsStopping() { return IsStopping; }
	void SetIsStopping(bool stopping) { IsStopping = stopping; }

	void UpdateBody();
	void UpdateWheelTopLeft();
	void UpdateWheelTopRight();
	void UpdateWheelBotLeft();
	void UpdateWheelBotRight();


	void MoveLeft();
	void MoveRight();
	void MoveForward();
	void MoveBackward();
	void MoveCar();
	void StopMovement();


	void UpdateSpotLights();
	LightProperties* GetSpotLightLeft();
	LightProperties* GetSpotLightRight();
};
