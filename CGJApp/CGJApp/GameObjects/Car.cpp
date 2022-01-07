#include "Car.h"



Car::Car(MyMesh* cubeMesh, MyMesh* torusMesh, bool isCollisionEnabled_in, float radius) {
	bodyMesh = cubeMesh;
	wheelMesh = torusMesh;

	// body material
	float amb[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	float diff[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(bodyMaterial.ambient, amb, 4 * sizeof(float));
	memcpy(bodyMaterial.diffuse, diff, 4 * sizeof(float));
	memcpy(bodyMaterial.specular, spec, 4 * sizeof(float));
	memcpy(bodyMaterial.emissive, emissive, 4 * sizeof(float));
	bodyMaterial.shininess = 0.0f;
	bodyMaterial.texCount = 0;
	bodyMesh->mat = bodyMaterial;
	// top material
	//float amb2[] = { 0.5f, 0.2f, 1.0f, 1.0f };
	//float diff2[] = { 0.5f, 0.2f, 1.0f, 1.0f };
	//float spec2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//float emissive2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//memcpy(topMaterial.ambient, amb2, 4 * sizeof(float));
	//memcpy(topMaterial.diffuse, diff2, 4 * sizeof(float));
	//memcpy(topMaterial.specular, spec2, 4 * sizeof(float));
	//memcpy(topMaterial.emissive, emissive2, 4 * sizeof(float));
	//topMaterial.shininess = 0.0f;
	//topMaterial.texCount = 0;
	// wheel material
	float amb3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diff3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float spec3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(wheelMaterial.ambient, amb3, 4 * sizeof(float));
	memcpy(wheelMaterial.diffuse, diff3, 4 * sizeof(float));
	memcpy(wheelMaterial.specular, spec3, 4 * sizeof(float));
	memcpy(wheelMaterial.emissive, emissive3, 4 * sizeof(float));
	wheelMaterial.shininess = 0.0f;
	wheelMaterial.texCount = 0;

	wheelMesh->mat = wheelMaterial;

	isCollisionEnabled = isCollisionEnabled_in;

	wheel_radius = radius;
	wheel_rot_speed = speed / wheel_radius * 40.0f;
	float slLeftPos[4] = { 6.0f, 0.28f, 0.0f, 1.0f };
	float slRightPos[4] = { 6.0f, 0.28f, 0.0f, 1.0f };
	float slDir[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	slLeft = SpotLight(slLeftPos, slDir);
	slRight = SpotLight(slRightPos, slDir);
	float dir[4];
	dir[0] = position[0]+ direction[0];
	dir[1] = position[1]+ direction[1];
	dir[2] = position[2]+ direction[2];
	dir[3] = 1.0f;
	slLeft.SetConeDirection(dir);
	slRight.SetConeDirection(dir);
}

MyMesh* Car::GetWheelMesh() { return wheelMesh; }
MyMesh* Car::GetBodyMesh() { return bodyMesh; }

LightProperties* Car::GetSpotLightLeft() {
	return slLeft.GetLightPtr();
}
LightProperties* Car::GetSpotLightRight() {
	return slRight.GetLightPtr();
}


void Car::UpdateSpotLights()
{
	pushMatrix(MODEL);
	slLeft.UpdateTransform(MODEL, slLeft_offset);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	slRight.UpdateTransform(MODEL, slRight_offset);
	popMatrix(MODEL);

	float dir[4];
	dir[0] = position[0]+direction[0];
	dir[1] = position[1]+ direction[1];
	dir[2] = position[2]+ direction[2];
	dir[3] = 1.0f;
	slLeft.SetConeDirection(dir);
	slRight.SetConeDirection(dir);

}

void Car::UpdateBody() {
	float offset[3] = { 0.0f, 0.0f, 0.0f };
	float up[3] = { 0.0f, 1.0f, 0.0f };
	float aux[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	translate(MODEL, position[0], position[1], position[2]);

	//rotate around center of bottom axis
	translate(MODEL, 0.2f, 0.5f, 0.5f);
	rotate(MODEL, -rotationAngle, 0.0f, 1.0f, 0.0f);
	translate(MODEL, -0.2f, -0.5f, -0.5f);

	UpdateSpotLights();

	scale(MODEL, 2.0f, 0.5f, 1.0f); // tamanho do carro

	for (int k = 0; k < 8; k++) {
		std::copy(bb_identity[k], bb_identity[k] + 4, bb[k]);
	}

	for (int k = 0; k < 8; k++) {
		multMatrixPoint(MODEL, bb[k], aux);
		std::copy(aux, aux + 4, bb[k]);
	}

}

void Car::UpdateWheelTopLeft() {

	crossProduct(up, direction, rot_dir);

	// body model being main model
	translate(MODEL, 0.7f, 0.2f, 0.0f);

	scale(MODEL, 0.5f, 2.0f, 1.0f); // scale back to primitive mesh size
	rotate(MODEL, -90, 1.0f, 0.0f, 0.0f);
	scale(MODEL, 0.3f, 0.3f, 0.3f);

	pushMatrix(MODEL);

	wheel_rot_angle += wheel_rot_speed * TimeUtil::deltaTime;
	rotate(MODEL, wheel_rot_angle, 0.0f, 1.0f, 0.0f);
}


void Car::UpdateWheelTopRight()
{
	//translate using topleft wheel as reference


	translate(MODEL, 0.0f, -1.1f * 3, 0.0f); // y is z in world cause rotation, multiply by 3 cause scale	

}

void Car::UpdateWheelBotRight()
{
	translate(MODEL, -1.2f * 3, 0.0f, 0.0f);

	pushMatrix(MODEL);

	wheel_rot_angle += wheel_rot_speed * TimeUtil::deltaTime;
	rotate(MODEL, wheel_rot_angle, 0.0f, 1.0f, 0.0f);

}

void Car::UpdateWheelBotLeft()
{
	translate(MODEL, 0.0f, -1.1f * 3, 0.0f);
	wheel_rot_angle += wheel_rot_speed * TimeUtil::deltaTime;
	rotate(MODEL, wheel_rot_angle, 0.0f, 1.0f, 0.0f);
}


void Car::MoveLeft() {
	float up[3] = { 0.0f, 1.0f, 0.0f };
	rotationAngle -= rotationSpeed * 2 * TimeUtil::deltaTime;
	float rotationRad = (float)(rotationAngle * (M_PI / 180.0f));
	direction[0] = cos(rotationRad);
	direction[2] = sin(rotationRad);
	add(direction, direction, direction);
	normalize(direction);
	
}
void Car::MoveRight() {
	float up[3] = { 0.0f, 1.0f, 0.0f };
	rotationAngle += rotationSpeed * 2 * TimeUtil::deltaTime;
	float rotationRad = (float)(rotationAngle * (M_PI / 180.0f));
	direction[0] = cos(rotationRad);
	direction[2] = sin(rotationRad);
	add(direction, direction, direction);
	normalize(direction);
	
}
void Car::MoveForward() {
	if (speed < maxSpeed)
	{
		speed += acceleration * TimeUtil::deltaTime;
		accelerating = true;
		wheel_rot_speed = speed / wheel_radius * 40.0f;
	}
}
void Car::MoveBackward() {
	if (speed > -maxSpeed)
	{
		speed -= acceleration * TimeUtil::deltaTime;
		accelerating = true;
		wheel_rot_speed = speed / wheel_radius * 40.0f;
	}
}


void Car::MoveCar()
{
	float velocity[3];
	constProduct(TimeUtil::deltaTime * speed, direction, velocity);
	if (!length(velocity) == 0) {
		std::copy(position, position + 3, old_position);
		
	}
	add(position, velocity, position);

	// rotate wheels TODO
}

void Car::StopMovement()
{
	if (speed > 0.1f)
	{
		speed -= acceleration * TimeUtil::deltaTime;
		wheel_rot_speed = speed / wheel_radius * 40.0f;
	}
	else if (speed < -0.1f)
	{
		speed += acceleration * TimeUtil::deltaTime;
		wheel_rot_speed = speed / wheel_radius * 40.0f;
	}
	else
	{
		speed = 0;
		wheel_rot_speed = 0;
	}
}

