#ifndef UI_MANAGER_H
#define UI_MANAGER_H

// GEF forward declarations
namespace gef
{

	class Sprite;
	class Texture;
	class Font;
	class Platform;
	class SpriteRenderer;
	class Font;

}

// Other forward declarations
class Level;
enum Difficulty;

// UI manager class
// Handles drawing the UI sprites and text to the screen
class UIManager
{

public:

	UIManager();
	~UIManager();

	// Initialise the user interface objects
	void Init(gef::Platform* platform_, float camera_image_scale_factor);
	// Clean up the user interface objects
	void CleanUp(gef::Platform* platform_);
	// Render the UI sprites
	void Render(gef::Platform* platform_, gef::SpriteRenderer* sprite_renderer_, bool has_won_, bool show_controls_, bool marker_01_found_, bool marker_02_found_);
	// Render the text
	void DrawFont(gef::Platform* platform_, gef::SpriteRenderer* sprite_renderer_, Level* level_, float fps_, bool marker_01_found_, bool marker_02_found_, Difficulty difficulty);

	// Get whether we're currently displaying the transforms
	void DisplayTransforms(bool value);
	// Set whether we want to display the transforms
	bool IsDisplayingTransforms();

private:

	gef::Font* font_;

	// Sprite holding a texture for the background of warnings when markers are missing
	gef::Sprite* missing_marker_sprite_;
	// Sprite holding the startup instructions
	gef::Sprite* controls_sprite_;
	// Sprite telling the player they've finished the level
	gef::Sprite* win_sprite_;
	// Sprite for the top of the UI
	gef::Sprite* top_sprite_;

	// Corresponding textures for the sprites
	class gef::Texture* missing_marker_texture_;
	class gef::Texture* controls_texture_;
	class gef::Texture* win_texture_;
	class gef::Texture* top_texture_;

	// Scale factor to make images fit the screen's aspect ratio
	float camera_image_scale_factor_;

	bool display_transforms_;

};

#endif // !UI_MANAGER_H