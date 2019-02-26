#ifndef _AR_APPLICATION_H
#define _AR_APPLICATION_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>
#include "game_object.h"
#include "level.h"
#include "ui_manager.h"

// Vita AR includes removed for copyright purposes

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;
}

// Enumerated type for difficulty
enum Difficulty
{

	DIFFICULTY_NORMAL,				// The player has to press the square button to detect if the configuration is correct
	DIFFICULTY_EASY					// Configuration detection is done automatically

};

class ARApp : public gef::Application
{

public:

	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();

private:

	void SetupLights();

	// Function for switching between the two levels
	void SwitchLevels();

	// Function for handling controller input
	void HandleInput();

	// Function for resetting the game
	void Reset();

	gef::InputManager* input_manager_;
	gef::SpriteRenderer* sprite_renderer_;
	class gef::Renderer3D* renderer_3d_;

	float fps_;

	// Handles the user interface & text
	UIManager* ui_manager_;
	// Handles the game objects, transforms, and configuration calculation
	Level* level_;

	// Sprite holding the camera image data
	gef::Sprite* camera_sprite_;
	// Texture used by the camera sprite
	gef::TextureVita camera_texture_;

	// Matrix transform data used within the scene for projecting to clip space
	gef::Matrix44 orthographic_frustum_camera;
	gef::Matrix44 perspective_projection_;

	// Identity matrix
	gef::Matrix44 identity_matrix_;

	Difficulty difficulty;

	// Float value used for scaling the camera image from camera resolution to screen resolution
	float camera_image_scale_factor_;
	bool marker_02_found_;
	bool marker_01_found_;

	// Value that transforms are checked against to detect the correct transforms
	float tolerance_value_;

	bool correct_transforms_;
	bool show_controls_;
	bool has_won_;
	
	int level_id_;
	
};

#endif // !_AR_APPLICATION_H