#include "Cheerios.h"

Cheerios::Cheerios(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, float extRadious) : GameObject3D(material_in)
{
	std::copy(startPosition, startPosition + 3, position);
	std::copy(startPosition, startPosition + 3, startPos);

	meshId = latestMeshId;
	mesh = meshPtr;
	radious = extRadious;
	isCollisionEnabled = true;
	bb_type = GameObject3D::BB_TYPE::Spherical;
	type = GameObject3D::TYPE::Cheerio;



	float sizeVar[] = { 2.0f, 1.5f, 2.0f };

	memcpy(size, sizeVar, 3 * sizeof(float));
}

void Cheerios::Update()
{
	translate(MODEL, position[0], position[1], position[2]);
	std::copy(position, position + 3, old_position);
	scale(MODEL, size[0], size[1], size[2]);
	MoveCheerio();
}

void Cheerios::Paused()
{
	translate(MODEL, position[0], position[1], position[2]);
	std::copy(position, position + 3, old_position);
	scale(MODEL, size[0], size[1], size[2]);
	MoveCheerio();
}

void Cheerios::SetDirection(float* value)
{
	std::copy(value, value + 3, direction);
}


void Cheerios::MoveCheerio()
{
	float velocity[3];
	if (speed + acceleration >= 0) {
		speed += acceleration;
	}
	else {
		speed = 0;
	}

	constProduct(TimeUtil::deltaTime * speed, direction, velocity);
	if (!length(velocity) == 0) {
		std::copy(position, position + 3, old_position);
	}
	add(position, velocity, position);
}

void Cheerios::reset() {
	std::copy(startPos, startPos + 3, position);
}
