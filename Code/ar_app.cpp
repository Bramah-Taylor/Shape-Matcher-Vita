#include "ar_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>

#include <input/sony_controller_input_manager.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>

ARApp::ARApp(gef::Platform& platform) :
	Application(platform),
	input_manager_(NULL),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	camera_sprite_(NULL),
	ui_manager_(NULL),
	level_(NULL)
{
}

void ARApp::Init()
{

	// Init objects
	input_manager_ = gef::InputManager::Create(platform_);
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	ui_manager_ = new UIManager();
	level_ = new Level();

	SetupLights();

	// Initialise sony framework
	sampleInitialize();
	smartInitialize();

	// Reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);

	// Initialise the camera sprite
	camera_sprite_ = new gef::Sprite();

	// Set camera image scale factor
	float camera_aspect_ratio_ = (float)SCE_SMART_IMAGE_WIDTH / (float)SCE_SMART_IMAGE_HEIGHT;
	float screen_aspect_ratio = (float)platform_.width() / (float)platform_.height();
	camera_image_scale_factor_ = screen_aspect_ratio / camera_aspect_ratio_;

	// Initialise the UI
	ui_manager_->Init(&platform_, camera_image_scale_factor_);

	// Setup the orthographic frustum for the camera
	orthographic_frustum_camera.OrthographicFrustumGL(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	// Setup the projection matrix for rendering geometry to the camera's perspective
	gef::Matrix44 fov_projection_matrix;
	gef::Matrix44 scale_matrix;
	fov_projection_matrix.PerspectiveFovGL(SCE_SMART_IMAGE_FOV, camera_aspect_ratio_, 0.01f, 10.0f);
	scale_matrix.Scale(gef::Vector4(1.0f, camera_image_scale_factor_, 1.0f));
	perspective_projection_ = fov_projection_matrix * scale_matrix;

	// Self explanatory
	identity_matrix_.SetIdentity();

	// Initially the markers have not been found
	marker_01_found_ = false;
	marker_02_found_ = false;

	// The player has not won yet, so set win values to false and set instructions to true on startup
	correct_transforms_ = false;
	show_controls_ = true;
	has_won_ = false;
	ui_manager_->DisplayTransforms(false);

	tolerance_value_ = 0.05f;
	difficulty = DIFFICULTY_EASY;
	level_id_ = 1;

	// Initialise the first level
	level_->InitLevel(level_id_, tolerance_value_, &platform_);

}

// Clean up objects
void ARApp::CleanUp()
{

	smartRelease();
	sampleRelease();

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;

	delete camera_sprite_;
	camera_sprite_ = NULL;

	ui_manager_->CleanUp(&platform_);
	delete ui_manager_;
	ui_manager_ = NULL;

	delete level_;
	level_ = NULL;

}

bool ARApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	HandleInput();

	// Set the game objects to be inactive by default
	level_->ReadyForUpdate();

	// Begin camera image sampling
	AppData* dat = sampleUpdateBegin();

	// Use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	// Reset markers to not be found each frame
	marker_01_found_ = false;
	marker_02_found_ = false;

	// Sample the camera image for markers
	level_->SampleMarkers(marker_02_found_, marker_01_found_);

	// Stop sampling camera image data
	sampleUpdateEnd(dat);

	// Detect if the transforms are close enough to the correct values
	correct_transforms_ = level_->GetUpdate();

	// If the current difficulty is easy, automatically detect if the player has won
	if (difficulty == DIFFICULTY_EASY)
	{

		if (correct_transforms_)
		{

			has_won_ = true;

		}

	}

	return true;
}

void ARApp::Render()
{
	AppData* dat = sampleRenderBegin();

	// Set the sprite to cover the end of the frustum in normalised device space
	camera_sprite_->set_width(2.0f);
	camera_sprite_->set_height(2.0f*camera_image_scale_factor_);
	camera_sprite_->set_position(0.0f, 0.0f, 1.0f);

	// Set the camera sprite to use the camera feed's texture
	camera_sprite_->set_texture(&camera_texture_);

	// Set the camera's projection matrix to the orthographic frustum we just set up
	sprite_renderer_->set_projection_matrix(orthographic_frustum_camera);

	// Begin sprite rendering

	sprite_renderer_->Begin(true);

	// Check there is data present for the camera image before trying to draw it
	if (dat->currentImage)
	{
		camera_texture_.set_texture(dat->currentImage->tex_yuv);
		sprite_renderer_->DrawSprite(*camera_sprite_);
	}

	// End sprite rendering
	sprite_renderer_->End();

	// Begin 3D rendering

	// Set the projection and view matrix
	renderer_3d_->set_projection_matrix(perspective_projection_);
	renderer_3d_->set_view_matrix(identity_matrix_);

	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d_->Begin(false);

	level_->Render(renderer_3d_);

	// End 3D rendering
	renderer_3d_->End();

	// Set the projection matrix again
	sprite_renderer_->set_projection_matrix(orthographic_frustum_camera);

	// Begin sprite rendering again for the UI
	sprite_renderer_->Begin(false);

	// Render UI
	ui_manager_->Render(&platform_, sprite_renderer_, has_won_, show_controls_, marker_01_found_, marker_02_found_);

	sprite_renderer_->End();

	// Set a new projection matrix for the text to be drawn correctly
	gef::Matrix44 proj_matrix2d;
	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);

	// Begin sprite rendering one final time for the text
	sprite_renderer_->Begin(false);

	// Draw the text
	ui_manager_->DrawFont(&platform_, sprite_renderer_, level_, fps_, marker_01_found_, marker_02_found_, difficulty);

	sprite_renderer_->End();

	// End rendering

	sampleRenderEnd();

}

void ARApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void ARApp::SwitchLevels()
{

	// Flip the level ID
	if (level_id_ == 1)
	{

		level_id_ = 2;

	}
	else
	{

		level_id_ = 1;

	}

	// Reset the level
	level_->ResetLevel();
	level_->InitLevel(level_id_, tolerance_value_, &platform_);

	// Reset win values
	has_won_ = false;
	correct_transforms_ = false;

}

void ARApp::Reset()
{

	// Reset the level
	level_->ResetLevel();
	level_->InitLevel(level_id_, tolerance_value_, &platform_);

	// Reset the UI
	ui_manager_->CleanUp(&platform_);
	ui_manager_->Init(&platform_, camera_image_scale_factor_);

}

void ARApp::HandleInput()
{

	input_manager_->Update();

	// Get input from the controller
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (controller)
	{

		// If square is pressed and difficulty is normal, detect if the player has won
		if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE)
		{

			if (difficulty == DIFFICULTY_NORMAL)
			{

				if (correct_transforms_)
				{

					has_won_ = true;

				}

			}

		}
		if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
		{

			// If the player has won, switch levels
			if (has_won_)
			{

				SwitchLevels();

			}
			else
			{

				// Else hide/show instructions
				show_controls_ = !show_controls_;

			}

		}
		// If the circle is pressed, switch the difficulty
		if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
		{

			if (difficulty == DIFFICULTY_NORMAL)
			{

				difficulty = DIFFICULTY_EASY;

			}
			else
			{

				difficulty = DIFFICULTY_NORMAL;

			}

		}
		// If the triangle is pressed, reset the game
		if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
		{

			Reset();

		}
		// If the down button is pressed, switch levels
		if (controller->buttons_pressed() & gef_SONY_CTRL_DOWN)
		{

			SwitchLevels();

		}
		// If the up button is pressed, display the transforms of the marker game objects
		if (controller->buttons_pressed() & gef_SONY_CTRL_UP)
		{

			ui_manager_->DisplayTransforms(!ui_manager_->IsDisplayingTransforms());

		}

	}

}