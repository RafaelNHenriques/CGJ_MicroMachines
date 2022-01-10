#include "Table.h"
#define FLOAT_MAX_VALUE 99999999999999999999999999999999999.99
#define FLOAT_MIN_VALUE -99999999999999999999999999999999999.99

Table::Table(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height) : GameObject3D(material_in)
{
	
	std::copy(startPosition, startPosition + 3, position);
	std::copy(startPosition, startPosition + 3, startPos);
	
	meshId = latestMeshId;
	mesh = meshPtr;
	isEnabled = true;
	isCollisionEnabled = isCollisionEnabled_in;
	bb_type = GameObject3D::BB_TYPE::Cubic;
	type = GameObject3D::TYPE::Table;

	float sizeVar[] = { width, lenght, height };

	memcpy(size, sizeVar, 3 * sizeof(float));


	float scale_value[3] = {0.0f, 0.0f, 0.0f};
	scale_value[0] = size[0];
	scale_value[1] = size[1];
	scale_value[2] = size[2];
	scale_bb(scale_value);
	

	
}


void Table::Update()
{

	MoveTable();
	float offset[3] = { 0.0f, 0.0f, 0.0f };
	translate(MODEL, position[0], position[1], position[2]);
	subtract(old_position, position, offset);
	translate_bb(offset);
	std::copy(position, position + 3, old_position);


	scale(MODEL, size[0], size[1], size[2]);
	CalculateBoundingBox();
}

void Table::Paused()
{

	MoveTable();
	float offset[3] = { 0.0f, 0.0f, 0.0f };
	translate(MODEL, position[0], position[1], position[2]);
	subtract(old_position, position, offset);
	translate_bb(offset);
	std::copy(position, position + 3, old_position);


	scale(MODEL, size[0], size[1], size[2]);
	CalculateBoundingBox();
}

void Table::translate_bb(float offset[3]) {

	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < 3; i++) {
			bb[k][i] += offset[i];
		}
	}

}

void Table::scale_bb(float values[3])
{
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < 3; i++) {
			bb[k][i] *= values[i];
		}
	}
}

void Table::CalculateBoundingBox() {
	float min_x = FLOAT_MAX_VALUE, max_x = FLOAT_MIN_VALUE, min_y = FLOAT_MAX_VALUE, max_y = FLOAT_MIN_VALUE, min_z = FLOAT_MAX_VALUE
		, max_z = FLOAT_MIN_VALUE;

	for (int k = 0; k < 8; k++) {
		if (bb[k][0] < min_x) {
			min_x = bb[k][0];
		}

		if (bb[k][0] > max_x) {
			max_x = bb[k][0];
		}
		if (bb[k][1] < min_y) {
			min_y = bb[k][1];
		}
		if (bb[k][1] > max_y) {
			max_y = bb[k][1];
		}
		if (bb[k][2] < min_z) {
			min_z = bb[k][2];
		}
		if (bb[k][2] > max_z) {
			max_z = bb[k][2];
		}

	}

	bb_bounds[0] = min_x;
	bb_bounds[1] = max_x;
	bb_bounds[2] = min_y;
	bb_bounds[3] = max_y;
	bb_bounds[4] = min_z;
	bb_bounds[5] = max_z;
}

void Table::SetDirection(float* value)
{
	std::copy(value, value + 3, direction);
}

void Table::PrintBB()
{
	printf("{\n");
	for (int k = 0; k < 8; k++) {
		printf("{");
		for (int i = 0; i < 3; i++) {
			printf("[%f] ", bb[k][i]);
		}
		printf("}\n");
	}
	printf("}\n");
}

void Table::MoveTable()
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


bool Table::DetectCollision(GameObject3D* other)
{
	return false;
}

void Table::ResolveCollision(GameObject3D* other)
{
}

void Table::reset() {
	std::copy(startPos, startPos + 3, position);
}
