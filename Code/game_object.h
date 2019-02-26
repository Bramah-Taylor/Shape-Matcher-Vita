#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <maths/vector2.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <graphics/mesh.h>
#include <graphics/mesh_instance.h>

class GameObject : public gef::MeshInstance
{
public:

	GameObject();
	~GameObject();

	// Update the object's position according to transform data
	void update();

	// Check if the object is moving
	bool is_moving();

	// Setters
	void set_position(float x, float y, float z);
	void set_velocity(float x, float y, float z);
	void set_rotation(float x, float y, float z);
	void set_scale(float scale);
	void set_active();
	void set_inactive();
	void set_marker(int marker);
	void set_marker_transform(gef::Matrix44 marker_transform);
	void set_local_transform(gef::Matrix44 local_transform);
	inline void set_local() { is_local_ = true; };
	
	// Getters
	gef::Matrix44 get_local_transform();
	gef::Vector4 get_position();
	gef::Vector4 get_velocity();
	inline float get_rotation_x() { return rotation_x_; };
	inline float get_rotation_y() { return rotation_y_; };
	inline float get_rotation_z() { return rotation_z_; };
	inline float get_scale() { return scale_; };
	inline bool is_active() { return is_active_; };
	inline bool is_marker_object() { return is_marker_object_; };
	inline int get_marker() { return marker_; };

private:

	gef::Matrix44 local_transform_;
	gef::Vector4 position_;
	gef::Vector4 velocity_;
	float rotation_x_;
	float rotation_y_;
	float rotation_z_;
	float scale_;
	gef::Matrix44 marker_transform_;
	int marker_;

	bool requires_transform_update_;
	bool is_active_;
	bool is_marker_object_;
	bool is_local_;

};

#endif // _GAME_OBJECT_H