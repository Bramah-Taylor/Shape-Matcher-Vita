#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

// GEF Forward declarations
namespace gef
{

	class Matrix44;
	class Renderer3D;
	class Scene;
	class Platform;

}

// App specific forward declarations
class GameObject;
class PrimitiveBuilder;

// Level class
// Holds all data relevant to each level, i.e. transforms to check, game objects to draw on markers, where to draw game objects
class Level
{
public:

	Level();
	~Level();

	// Initialise the level based on the level's identifier
	bool InitLevel(int level_identifier, float tolerance_value, gef::Platform* platform_);
	// Reset the level when the level is changed
	void ResetLevel();

	// Update the objects in the level and check their transforms with the reference transforms
	bool GetUpdate();
	// Sample the markers' positions using the Sony sample framework
	void SampleMarkers(bool& marker_02_found, bool& marker_01_found);
	// Default objects to inactive before updating
	void ReadyForUpdate();
	// Render the objects in the level
	void Render(gef::Renderer3D* renderer_3d_);

	// Check if the markers are all in the current camera view
	bool MarkersAreActive();

	// Getters
	gef::Matrix44* GetTransform(int id);
	GameObject* GetGameObject(int id);
	int GetID();

private:

	// Compare the game object transforms to the reference transforms
	bool CheckTransforms();

	// Vector holding the reference transforms
	std::vector<gef::Matrix44> transforms_;
	// Vector holding the game objects
	std::vector<GameObject> game_objects_;
	// Scenes holding the model data loaded from file
	gef::Scene* first_scene_;
	gef::Scene* second_scene_;

	bool check_rotation_;
	int num_transforms_;
	int level_id_;
	float tolerance_value_;

};

#endif //!LEVEL_H