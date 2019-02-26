#include "game_object.h"

GameObject::GameObject()
{

	position_ = gef::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	rotation_x_ = 0.0f;
	rotation_y_ = 0.0f;
	rotation_z_ = 0.0f;
	scale_ = 1.0f;
	velocity_ = gef::Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	requires_transform_update_ = false;
	is_active_ = false;
	is_marker_object_ = false;
	is_local_ = false;

	marker_transform_.SetIdentity();
	marker_ = 0;
	local_transform_.SetIdentity();

}

GameObject::~GameObject()
{



}

void GameObject::update()
{

	// Update transform if we need to do so
	// Order is scale - rotate - translate
	if (requires_transform_update_ == true || is_moving())
	{

		// Get a new transform
		gef::Matrix44 transform_;
		transform_.SetIdentity();

		// Make a new scale matrix
		gef::Matrix44 scale_matrix_;

		// Set the scale matrix to the scale
		scale_matrix_.Scale(gef::Vector4(scale_, scale_, scale_));

		// Apply the scale transformation
		transform_ = transform_ * scale_matrix_;

		// Make some rotation matrices
		gef::Matrix44 rotation_x_matrix_;
		gef::Matrix44 rotation_y_matrix_;
		gef::Matrix44 rotation_z_matrix_;

		// Set the rotation matrices using class data
		rotation_x_matrix_.RotationX(rotation_x_);
		rotation_y_matrix_.RotationY(rotation_y_);
		rotation_z_matrix_.RotationZ(rotation_z_);

		// Apply the rotation transformations
		transform_ = transform_ * rotation_x_matrix_;
		transform_ = transform_ * rotation_y_matrix_;
		transform_ = transform_ * rotation_z_matrix_;

		// Update position based on velocity
		position_ = gef::Vector4(position_.x() + velocity_.x(), position_.y() + velocity_.y(), position_.z() + velocity_.z());

		// Make a new translation matrix
		gef::Matrix44 translation_;
		translation_.SetIdentity();

		// Set the translation matrix
		translation_.SetTranslation(position_);

		// Apply the translation transformation
		transform_ = transform_ * translation_;

		// Update the local transform
		if (is_local_)
		{

			transform_ = transform_ * local_transform_;

		}

		// If this is a marker object, transform by the marker transform
		if (is_marker_object_)
		{

			transform_ = transform_ * marker_transform_;

		}


		// Update the MeshInstance transform
		this->set_transform(transform_);

		// Set transform update back to default so that we don't recalculate unnecessarily
		requires_transform_update_ = false;

	}

}

void GameObject::set_position(float x, float y, float z)
{

	position_ = gef::Vector4(x, y, z);

	requires_transform_update_ = true;

}

void GameObject::set_velocity(float x, float y, float z)
{

	velocity_ = gef::Vector4(x, y, z);

}

void GameObject::set_rotation(float x, float y, float z)
{

	rotation_x_ = x;
	rotation_y_ = y;
	rotation_z_ = z;

	requires_transform_update_ = true;

}

void GameObject::set_scale(float scale)
{

	scale_ = scale;

	requires_transform_update_ = true;

}

gef::Matrix44 GameObject::get_local_transform()
{

	return local_transform_;

}

gef::Vector4 GameObject::get_position()
{

	return position_;

}

gef::Vector4 GameObject::get_velocity()
{

	return velocity_;

}

bool GameObject::is_moving()
{

	if (velocity_.x() > 0.01 || velocity_.x() < 0.01 || velocity_.y() > 0.01 || velocity_.y() < 0.01 || velocity_.z() > 0.01 || velocity_.z() < 0.01)
	{

		return true;

	}

	return false;

}

void GameObject::set_active()
{

	is_active_ = true;

}

void GameObject::set_inactive()
{

	is_active_ = false;

}

void GameObject::set_marker(int marker)
{

	is_marker_object_ = true;
	marker_ = marker;

}

void GameObject::set_marker_transform(gef::Matrix44 marker_transform)
{

	marker_transform_ = marker_transform;

}

void GameObject::set_local_transform(gef::Matrix44 local_transform)
{

	local_transform_ = local_transform;

}