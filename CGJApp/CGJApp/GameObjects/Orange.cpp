#include "Orange.h"
# define M_PI           3.14159265358979323846


Orange::Orange(Material material_in, MyMesh* meshPtr, float startRadius, int latestMeshId, float table_pos_in[3], float table_lenght_in, bool isCollisionEnabled_in) : GameObject3D(material_in)
{
	float min_x = bb_bounds[0], max_x = bb_bounds[1];
	float min_y = bb_bounds[4], max_y = bb_bounds[5];

	startPos[0] = position[0] = min_x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_x - min_x)));
	startPos[1] = position[1] = 1.0f;
	startPos[2] = position[2] = min_y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_y - min_y)));

	startSpeed=speed = 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 3));
	meshId = latestMeshId;
	radius = startRadius;
	table_position = table_pos_in;
	mesh = meshPtr;
	table_length = table_lenght_in;
	onTable = true;
	isEnabled = true;
	isCollisionEnabled = isCollisionEnabled_in;
	int_time_updated = TimeUtil::deltaTime;
	int_time_respawn = 0;
	bb_type = GameObject3D::BB_TYPE::Spherical;
	type = GameObject3D::TYPE::Orange;

	float x = 75.0f - position[0];
	float z = 5.0f - position[2];

	float d[3] = { x, 0.0f, z};
	normalize(d);
	SetDirection(d);

	rotationAngle = 0.0f;
	rotationSpeed = speed / radius * 40.0f;
}

void Orange::SetDirection(float newDirection[3])
{
	subtract(direction, direction, direction);
	add(newDirection, direction, direction);
}


void Orange::Update()
{
	if (!isOnTable()) {
		int_time_respawn += TimeUtil::deltaTime;
		isEnabled = false;
	}
	else {
		isEnabled = true;
	}
	int_time_updated += TimeUtil::deltaTime;

	if (isEnabled) {

		MoveOrange();
		scale(MODEL, radius, radius, radius);

		if (int_time_updated >= 30) {
			if (speed + 0.1 <= max_speed) {
				speed += 5.0f;
				rotationSpeed = speed / radius * 40.0f;
			}
			int_time_updated = 0;

		}
	}
	else if (int_time_respawn >= 2) {
		RespawnOrange();
		int_time_respawn = 0;
	}

}

void Orange::Paused()
{
	translate(MODEL, position[0], position[1], position[2]);
	scale(MODEL, radius, radius, radius);
}

void Orange::MoveOrange()
{
	// translation
	float velocity[3];
	float rot_direction[3] = {0.0f, 0.0f, 0.0f};
	constProduct(TimeUtil::deltaTime * speed, direction, velocity);
	add(position, velocity, position);
	translate(MODEL, position[0], position[1], position[2]);


	// rotation
	float up[3] = { 0.0f, 1.0f, 0.0f };
	float right[3] = { 0.0f, 0.0f, 0.0f };
	
	crossProduct(direction, up, right);
	constProduct(-1.0f, right, rot_direction);
	rotationAngle += rotationSpeed * TimeUtil::deltaTime;
	rotate(MODEL, rotationAngle, rot_direction[0], rot_direction[1], rot_direction[2]); // FIXME hardcoded rotation direction
}  

void Orange::RespawnOrange()
{//Rep?e a laranja caso saia da mesa
	float min_x = bb_bounds[0], max_x = bb_bounds[1];
	float min_y = bb_bounds[4], max_y = bb_bounds[5];

	position[0] = min_x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_x - min_x)));
	position[2] = min_y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_y - min_y)));
	float x = 75.0f - position[0];
	float z = 5.0f - position[2];

	float d[3] = { x, 0.0f,  z};
	normalize(d);
	SetDirection(d);

	if (!isOnTable()) {
		RespawnOrange();
	}

}

bool Orange::DetectCollision(GameObject3D* other)
{

	if (other->GetBBType() == GameObject3D::BB_TYPE::Spherical) {
		float offset[3] = {0.0f, 0.0f, 0.0f}; 
		offset[0] = position[0] - other->GetPosition()[0];
		offset[1] = position[1] - other->GetPosition()[1];
		offset[2] = position[2] - other->GetPosition()[2];
		
		Orange* o = dynamic_cast<Orange*>(other);
		return length(offset) < (radius + o->radius);
	}
	else {
		//Other types of GameObjects except table
	}
	return false;
}

void Orange::ResolveCollision(GameObject3D* other)
{
}

bool Orange::isOnTable() {
	float x = std::max(bb_bounds[0], std::min(position[0], bb_bounds[1]));
	float z = std::max(bb_bounds[4], std::min(position[2], bb_bounds[5]));
	float aux[2];
	float distance[2] = { 0.0f, 0.0f };

	aux[0] = x;
	aux[1] = z;

	//subtract(aux, position, distance);
	distance[0] = position[0] - aux[0];
	distance[1] = position[2] - aux[1];

	float val = sqrt(distance[0] * distance[0] + distance[1] * distance[1]);
	return  val < radius;
}

void Orange::StoreBounds(float *table_bounds) {//Guarda os limites de onde as laranjas podem circular
	std::copy(table_bounds, table_bounds + 6, bb_bounds);
}

void Orange::reset() {
	std::copy(startPos, startPos + 3, position);
	speed = startSpeed;
	int_time_updated = TimeUtil::deltaTime;
	int_time_respawn = 0;

	float x = 75.0f - position[0];
	float z = 5.0f - position[2];

	float d[3] = { x, 0.0f, z };
	normalize(d);
	SetDirection(d);

	rotationAngle = 0.0f;
	rotationSpeed = speed / radius * 40.0f;

}