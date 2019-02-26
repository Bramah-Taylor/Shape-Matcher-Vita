#include "ui_manager.h"
#include <assets/png_loader.h>
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/font.h>
#include <graphics/sprite.h>
#include <graphics/image_data.h>

#include "level.h"
#include "game_object.h"
#include "ar_app.h"

UIManager::UIManager() :
	missing_marker_sprite_(NULL),
	missing_marker_texture_(NULL),
	controls_sprite_(NULL),
	controls_texture_(NULL),
	win_sprite_(NULL),
	win_texture_(NULL),
	top_sprite_(NULL),
	top_texture_(NULL),
	font_(NULL)
{
}

UIManager::~UIManager()
{



}

void UIManager::Init(gef::Platform* platform_, float camera_image_scale_factor)
{

	font_ = new gef::Font(*platform_);
	font_->Load("comic_sans");

	// Initialise the sprites
	missing_marker_sprite_ = new gef::Sprite();
	controls_sprite_ = new gef::Sprite();
	win_sprite_ = new gef::Sprite();
	top_sprite_ = new gef::Sprite();

	// Load a texture from file for the warning sprite from file
	std::string file_name = "warningTexture.png";
	gef::PNGLoader png_loader;
	gef::ImageData image_data;
	// Load from file
	png_loader.Load(file_name.c_str(), *platform_, image_data);
	// Create texture from the PNG data
	missing_marker_texture_ = gef::Texture::Create(*platform_, image_data);
	platform_->AddTexture(missing_marker_texture_);
	// Set the texture to the sprite
	missing_marker_sprite_->set_texture(missing_marker_texture_);

	// Do the same for the instructions sprite
	file_name = "controlsTexture.png";
	png_loader.Load(file_name.c_str(), *platform_, image_data);
	controls_texture_ = gef::Texture::Create(*platform_, image_data);
	platform_->AddTexture(controls_texture_);
	controls_sprite_->set_texture(controls_texture_);

	// And the top UI sprite
	file_name = "topTexture.png";
	png_loader.Load(file_name.c_str(), *platform_, image_data);
	top_texture_ = gef::Texture::Create(*platform_, image_data);
	platform_->AddTexture(top_texture_);
	top_sprite_->set_texture(top_texture_);

	// And the win screen sprite
	file_name = "winScreen.png";
	png_loader.Load(file_name.c_str(), *platform_, image_data);
	win_texture_ = gef::Texture::Create(*platform_, image_data);
	platform_->AddTexture(win_texture_);
	win_sprite_->set_texture(win_texture_);

	camera_image_scale_factor_ = camera_image_scale_factor;

	display_transforms_ = false;

}

void UIManager::CleanUp(gef::Platform* platform_)
{

	delete font_;
	font_ = NULL;
	
	delete missing_marker_sprite_;
	missing_marker_sprite_ = NULL;

	platform_->RemoveTexture(missing_marker_texture_);
	delete missing_marker_texture_;
	missing_marker_texture_ = NULL;

	delete controls_sprite_;
	controls_sprite_ = NULL;

	platform_->RemoveTexture(controls_texture_);
	delete controls_texture_;
	controls_texture_ = NULL;

	delete win_sprite_;
	win_sprite_ = NULL;

	platform_->RemoveTexture(win_texture_);
	delete win_texture_;
	win_texture_ = NULL;

	delete top_sprite_;
	top_sprite_ = NULL;

	platform_->RemoveTexture(top_texture_);
	delete top_texture_;
	top_texture_ = NULL;

}

void UIManager::Render(gef::Platform* platform_, gef::SpriteRenderer* sprite_renderer_, bool has_won_, bool show_controls_, bool marker_01_found_, bool marker_02_found_)
{

	// Update top sprite values
	top_sprite_->set_width(2.5f);
	top_sprite_->set_height(0.2f);
	top_sprite_->set_position(-0.0f, -0.95f, 1.0f);

	// Draw the sprite
	sprite_renderer_->DrawSprite(*top_sprite_);

	if (has_won_)
	{

		// Do the same for the win screen sprite if the player has won
		win_sprite_->set_width(0.5f);
		win_sprite_->set_height(0.5f);
		win_sprite_->set_position(0.0f, 0.0f, 1.0f);

		sprite_renderer_->DrawSprite(*win_sprite_);

	}
	// We don't want to draw both of these at once otherwise they'll overlap
	else if (show_controls_)
	{

		// Do the same for the instructions sprite if the player is viewing the instructions
		controls_sprite_->set_width(1.0f);
		controls_sprite_->set_height(1.0f);
		controls_sprite_->set_position(0.0f, 0.0f, 1.0f);

		sprite_renderer_->DrawSprite(*controls_sprite_);

	}

	if (!marker_01_found_ || !marker_02_found_)
	{

		// If there are markers missing, render the warning sprite
		missing_marker_sprite_->set_width(2.0f);
		missing_marker_sprite_->set_height(2.0f*camera_image_scale_factor_);
		missing_marker_sprite_->set_position(0.0f, 0.0f, 1.0f);

		// Draw the warning sprite
		sprite_renderer_->DrawSprite(*missing_marker_sprite_);

	}

}

void UIManager::DrawFont(gef::Platform* platform_, gef::SpriteRenderer* sprite_renderer_, Level* level_, float fps_, bool marker_01_found_, bool marker_02_found_, Difficulty difficulty)
{

	if (font_)
	{

		// Print the FPS
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		// Print warning text for when markers are missing
		if (!marker_02_found_)
		{

			font_->RenderText(sprite_renderer_, gef::Vector4(480.0f, 272.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE, "MARKER 02 MISSING; MARKER 01 WILL NOT BE EVALUATED");

		}
		else if (!marker_01_found_)
		{

			font_->RenderText(sprite_renderer_, gef::Vector4(480.0f, 272.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE, "MARKER 01 MISSING");

		}
		else
		{

			// Print the marker transforms if we need to
			if (display_transforms_)
			{

				gef::Vector4 mesh_marker_vector_ = level_->GetGameObject(0)->transform().GetTranslation();
				font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 450.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT,
					"M02 mesh pos: %.3f,  %.3f,  %.3f", mesh_marker_vector_.x(), mesh_marker_vector_.y(), mesh_marker_vector_.z());

				mesh_marker_vector_ = level_->GetGameObject(1)->transform().GetTranslation();
				font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 480.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT,
					"M01 mesh pos: %.3f,  %.3f,  %.3f", mesh_marker_vector_.x(), mesh_marker_vector_.y(), mesh_marker_vector_.z());

			}

			// Print the current level based on the ID
			font_->RenderText(sprite_renderer_, gef::Vector4(350.0f, 0.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Level: %i", level_->GetID());

			// Print the difficulty
			if (difficulty == DIFFICULTY_EASY)
			{

				font_->RenderText(sprite_renderer_, gef::Vector4(500.0f, 0.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Difficulty: Easy");

			}
			else if (difficulty == DIFFICULTY_NORMAL)
			{

				font_->RenderText(sprite_renderer_, gef::Vector4(500.0f, 0.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Difficulty: Normal");

			}

		}

	}

}

void UIManager::DisplayTransforms(bool value)
{

	display_transforms_ = value;

}

bool UIManager::IsDisplayingTransforms()
{

	return display_transforms_;

}