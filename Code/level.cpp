#include "Level.h"
#include <system/platform.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/scene.h>
#include "game_object.h"

#include <sony_sample_framework.h>
#include <sony_tracking.h>

Level::Level()
{

}

// Clean up the scenes
Level::~Level()
{

	delete first_scene_;
	first_scene_ = NULL;

	delete second_scene_;
	second_scene_ = NULL;

}

bool Level::InitLevel(int level_identifier, float tolerance_value, gef::Platform* platform_)
{

	level_id_ = level_identifier;
	tolerance_value_ = tolerance_value;

	// Set up for the first level (circle within a ring)
	if (level_id_ == 1)
	{

		num_transforms_ = 2;

		gef::Matrix44 transform;

		// Set the data of the origin transform (marker 02)
		transform.SetRow(0, gef::Vector4(0.067f, -0.003f, -0.002f, 0.0f));
		transform.SetRow(1, gef::Vector4(-0.003f, -0.004f, -0.067f, 0.0f));
		transform.SetRow(2, gef::Vector4(0.004f, 0.067f, -0.005f, 0.0f));
		transform.SetRow(3, gef::Vector4(0.012f, 0.045f, -0.504f, 1.0f));

		transforms_.push_back(transform);

		// Then set the data of the other transform (marker 01)
		transform.SetRow(0, gef::Vector4(0.05f, -0.003f, -0.001f, 0.0f));
		transform.SetRow(1, gef::Vector4(-0.002f, -0.005f, -0.05f, 0.0f));
		transform.SetRow(2, gef::Vector4(0.003f, 0.05f, -0.005f, 0.0f));
		transform.SetRow(3, gef::Vector4(0.013f, 0.04f, -0.5f, 1.0f));

		transforms_.push_back(transform);

		// Load the meshes from the scene files
		first_scene_ = new gef::Scene();
		first_scene_->ReadSceneFromFile(*platform_, "pipe1.scn");
		first_scene_->CreateMaterials(*platform_); 

		second_scene_ = new gef::Scene();
		second_scene_->ReadSceneFromFile(*platform_, "cylinder1.scn");
		second_scene_->CreateMaterials(*platform_);

		// Create game objects to hold the meshes
		GameObject mesh_marker_02_;
		GameObject mesh_marker_01_;

		// Set the first mesh to use the default cube mesh and marker ID 1 (aka 02)
		mesh_marker_02_.set_mesh(first_scene_->CreateMesh(*platform_, first_scene_->mesh_data.front()));
		mesh_marker_02_.set_marker(1);
		mesh_marker_02_.set_position(0.0f, 0.0f, 0.3f);
		mesh_marker_02_.set_rotation(-0.785f, 0.0f, 0.0f);

		// Set the second mesh to also use the default cube mesh and marker ID 0 (aka 01)
		mesh_marker_01_.set_mesh(second_scene_->CreateMesh(*platform_, second_scene_->mesh_data.front()));
		mesh_marker_01_.set_marker(0);
		mesh_marker_01_.set_local();
		mesh_marker_01_.set_position(0.2f, 0.0f, 0.32f);
		mesh_marker_01_.set_rotation(-0.785f, 0.0f, 0.0f);

		// Scale the meshes down to a reasonable size
		// 0.059f allows the cubes to be correctly fitted to the axis aligned bounding boxes
		float marker_scale_factor = 0.05f;
		gef::Matrix44 scale_matrix_geometry;
		gef::Matrix44 translation_matrix;
		gef::Matrix44 final_transform;
		mesh_marker_02_.set_scale(marker_scale_factor);
		mesh_marker_01_.set_scale(marker_scale_factor * 1.35f);

		game_objects_.push_back(mesh_marker_02_);
		game_objects_.push_back(mesh_marker_01_);

	}
	// Set up for the second level (two hemispheres)
	else if (level_id_ == 2)
	{

		num_transforms_ = 2;

		gef::Matrix44 transform;

		// Set the data of the origin transform (marker 02)
		transform.SetRow(0, gef::Vector4(0.0f, 0.007f, -0.013f, 0.0f));
		transform.SetRow(1, gef::Vector4(-0.015f, 0.0f, 0.0f, 0.0f));
		transform.SetRow(2, gef::Vector4(0.0f, 0.013f, 0.007f, 0.0f));
		transform.SetRow(3, gef::Vector4(-0.005f, 0.06f, -0.763f, 1.0f));

		transforms_.push_back(transform);

		// Then set the data of the other transform (marker 01)
		transform.SetRow(0, gef::Vector4(0.0f, -0.005f, 0.009f, 0.0f));
		transform.SetRow(1, gef::Vector4(0.01f, 0.0f, 0.0f, 0.0f));
		transform.SetRow(2, gef::Vector4(0.0f, 0.009f, 0.005f, 0.0f));
		transform.SetRow(3, gef::Vector4(-0.046f, 0.041f, -0.501f, 1.0f));

		transforms_.push_back(transform);

		// Load the meshes from the scene files
		first_scene_ = new gef::Scene();
		first_scene_->ReadSceneFromFile(*platform_, "hemi.scn");
		first_scene_->CreateMaterials(*platform_);

		second_scene_ = new gef::Scene();
		second_scene_->ReadSceneFromFile(*platform_, "hemi.scn");
		second_scene_->CreateMaterials(*platform_);

		// Create game objects to hold the meshes
		GameObject mesh_marker_02_;
		GameObject mesh_marker_01_;

		// Set the first mesh to use the default cube mesh and marker ID 1 (aka 02)
		mesh_marker_02_.set_mesh(first_scene_->CreateMesh(*platform_, first_scene_->mesh_data.front()));
		mesh_marker_02_.set_marker(1);
		mesh_marker_02_.set_position(0.0f, 0.0f, 0.1f);
		mesh_marker_02_.set_rotation(0.0f, 0.0f, 1.57f);

		// Set the second mesh to also use the default cube mesh and marker ID 0 (aka 01)
		mesh_marker_01_.set_mesh(second_scene_->CreateMesh(*platform_, second_scene_->mesh_data.front()));
		mesh_marker_01_.set_marker(0);
		mesh_marker_01_.set_local();
		mesh_marker_01_.set_position(0.05f, 0.0f, 0.2f);
		mesh_marker_01_.set_rotation(0.0f, 0.0f, 0.0f);

		// Scale the meshes down to a reasonable size
		// 0.059f allows the cubes to be correctly fitted to the axis aligned bounding boxes
		float marker_scale_factor = 0.01f;
		gef::Matrix44 scale_matrix_geometry;
		gef::Matrix44 translation_matrix;
		gef::Matrix44 final_transform;
		mesh_marker_02_.set_scale(marker_scale_factor * 1.5f);
		mesh_marker_01_.set_scale(marker_scale_factor);

		game_objects_.push_back(mesh_marker_02_);
		game_objects_.push_back(mesh_marker_01_);

	}

	return false;

}

bool Level::GetUpdate()
{

	// Check that the meshes are active before their positions are updated
	if (game_objects_[0].is_active())
	{

		game_objects_[0].update();

	}

	// This will only evaluate as true if the 0th game object is also active
	if (game_objects_[1].is_active())
	{

		game_objects_[1].update();

		// Hence we can check transforms here
		return CheckTransforms();

	}

	return false;

}

bool Level::CheckTransforms()
{

	// Counter keeps track of the number of values that are close enough to the reference value
	int counter = 0;

	float local_tolerance_value = tolerance_value_;

	gef::Vector4 comparisonVector = gef::Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	// Loop through each row of the first matrix
	for (int row = 0; row < 4; row++)
	{

		// Construct a vector using the reference transform and game object transform
		comparisonVector = gef::Vector4(
			abs(game_objects_[0].transform().GetRow(row).x() - transforms_[0].GetRow(row).x()),
			abs(game_objects_[0].transform().GetRow(row).y() - transforms_[0].GetRow(row).y()),
			abs(game_objects_[0].transform().GetRow(row).z() - transforms_[0].GetRow(row).z()),
			0.0f
		);

		if (row == 3)
		{

			local_tolerance_value *= 4;

		}

		// Check each value to the tolerance value (except for values in the last column)
		if (comparisonVector.x() < local_tolerance_value
			&& comparisonVector.y() < local_tolerance_value
			&& comparisonVector.z() < local_tolerance_value)
		{

			// Increment the counter whenever a row is within scope of the correct values
			counter++;

		}

	}

	local_tolerance_value = tolerance_value_;

	for (int row = 0; row < 4; row++)
	{

		// Construct a vector using the reference transform and game object transform
		comparisonVector = gef::Vector4(
			abs(game_objects_[1].transform().GetRow(row).x() - transforms_[1].GetRow(row).x()),
			abs(game_objects_[1].transform().GetRow(row).y() - transforms_[1].GetRow(row).y()),
			abs(game_objects_[1].transform().GetRow(row).z() - transforms_[1].GetRow(row).z()),
			0.0f
		);

		if (row == 3)
		{

			local_tolerance_value /= 4;

		}

		// Check each value to the tolerance value (except for values in the last column)
		if (comparisonVector.x() < local_tolerance_value
			&& comparisonVector.y() < local_tolerance_value
			&& comparisonVector.z() < local_tolerance_value)
		{

			// Increment the counter whenever a row is within scope of the correct values
			counter++;

		}

	}

	// If all rows are close enough to the correct value, return true
	if (counter == 8)
	{

		return true;

	}

	// Else return false
	return false;

}

void Level::SampleMarkers(bool& marker_02_found, bool& marker_01_found)
{

	// If marker 02 is found, we will be drawing the corresponding mesh
	// Marker 02 is being used as the origin in a shared coordinate system
	// so there's no point evaluating marker 01 if marker 02 has not been found
	if (sampleIsMarkerFound(game_objects_[0].get_marker()))
	{

		gef::Matrix44 marker02_transform_;

		// Get marker 02's position
		sampleGetTransform(game_objects_[0].get_marker(), &marker02_transform_);

		// Set transform for the corresponding mesh
		game_objects_[0].set_marker_transform(marker02_transform_);
		game_objects_[0].set_active();

		// Marker 02 has been found, so set to true
		marker_02_found = true;

		// Now we check for marker 01
		if (sampleIsMarkerFound(game_objects_[1].get_marker()))
		{

			gef::Matrix44 marker01_transform_;

			// Get marker 01 position
			sampleGetTransform(game_objects_[1].get_marker(), &marker01_transform_);

			// cube 3 is now active
			game_objects_[1].set_active();

			// Marker has been found
			marker_01_found = true;

			// Perform localisation calculations
			gef::Matrix44 inv_marker02_transform;
			inv_marker02_transform.SetIdentity();
			gef::Matrix44 marker01_local_transform;
			marker01_local_transform.SetIdentity();

			// Invert marker 02's transform then multiply by marker 01's transform to get the local transform we need
			inv_marker02_transform.Inverse(marker02_transform_);
			marker01_local_transform = marker01_transform_ * inv_marker02_transform;

			// Set marker 01's corresponding mesh's transforms
			game_objects_[1].set_marker_transform(marker02_transform_);

			game_objects_[1].set_local_transform(marker01_local_transform);

		}
		else
		{

			game_objects_[1].set_inactive();

		}

	}
	else
	{

		game_objects_[1].set_inactive();
		game_objects_[0].set_inactive();

	}

}

void Level::ReadyForUpdate()
{

	game_objects_[0].set_inactive();
	game_objects_[1].set_inactive();

}

void Level::Render(gef::Renderer3D* renderer_3d_)
{

	// Render the meshes according to their active status
	if (game_objects_[0].is_active())
	{

		renderer_3d_->DrawMesh(game_objects_[0]);

		if (game_objects_[1].is_active())
		{

			renderer_3d_->DrawMesh(game_objects_[1]);

		}

	}

}

bool Level::MarkersAreActive()
{

	return game_objects_[0].is_active() && game_objects_[1].is_active();

}

void Level::ResetLevel()
{

	transforms_.clear();
	game_objects_.clear();

	delete first_scene_;
	first_scene_ = NULL;

	delete second_scene_;
	second_scene_ = NULL;

}

gef::Matrix44* Level::GetTransform(int id)
{

	return &transforms_[id];

}

GameObject* Level::GetGameObject(int id)
{

	return &game_objects_[id];

}

int Level::GetID()
{

	return level_id_;

}