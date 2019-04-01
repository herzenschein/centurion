#pragma once
#include <SDL_video.h>
#include <memory>
#include <string>
#include "color.h"
#include "dimensioned.h"
#include "drawable.h"
#include "font.h"
#include "point.h"
#include "renderer.h"

// TODO delegatation functions to Renderer

namespace centurion {
namespace visuals {

/**
\brief The Window class represents a top-level frame.
*/
class Window : public centurion::geo::Dimensioned {
 private:
  SDL_Window* window;
  std::unique_ptr<centurion::visuals::Renderer> renderer;
  std::shared_ptr<centurion::visuals::Drawable> drawable;
  const int width;
  const int height;

  void CheckWindowDimensions(int width, int height);

 public:
  /**
  \param title - the title of the window.
  \param width - the desired width of the window.
  \param height - the desired height of the window.
  \param flags - flags providing information about the window to be created, the
  flag values are specified by SDL_WindowFlags. For example,
  SDL_WindowFlags::SDL_WINDOW_FULLSCREEN.
  */
  Window(const std::string& title, int width, int height,
         Uint32 flags);  // FIXME use uint32_t rather than Uint32

  ~Window();

  /*
  \brief Makes this window visible.
  */
  void Show();

  /*
  \brief	Makes this window invisible.
  */
  void Hide();

  /**
  \brief Renders this window.
  */
  void Render();

  void Clear();

  /**
  \brief Assigns the Drawable that will be invoked whenever the Render() method
  is called. \param drawable - the Drawable that will be used.
  */
  void SetDrawable(
      const std::shared_ptr<centurion::visuals::Drawable> drawable);

  /**
  \brief Sets whether this window is resizable or not.
  \param resizable true if the window is resizable, false otherwise.
  */
  void SetResizable(bool resizable);

  /**
  \brief Returns the width of this window.
  */
  int GetWidth() const override { return width; };

  /**
  \brief Returns the height of this window.
  */
  int GetHeight() const override { return height; };

  std::unique_ptr<centurion::visuals::Renderer>& GetRenderer() {
    return renderer;
  }

  /**
  \brief Renders a texture to the rendering target.
  \param texture - the texture that will be rendered.
  \param x - the desired x-coordinate.
  \param y - the desired y-coordinate.
  \param w - the desired width of the image.
  \param h - the desired height of the image.
  */
  void Render(centurion::visuals::Texture& texture, int x, int y, int w, int h);

  /**
  \brief Renders a texture to the rendering target.
  \param texture - the texture that will be rendered.
  \param rect - the Rectangle that provides the dimensions for the rendered
  image.
  */
  void Render(centurion::visuals::Texture& texture,
              const centurion::geo::Rectangle& rect);

  /**
  \brief Renders a texture to the rendering target.
  \param texture - the texture that will be rendered.
  \param x - the desired x-coordinate.
  \param y - the desired y-coordinate.
  */
  void Render(centurion::visuals::Texture& texture, int x, int y);

  void Render(centurion::visuals::Texture& texture,
              const centurion::geo::Positionable& posit,
              const centurion::geo::Dimensioned& dimensioned);

  /**
  \brief Renders a filled rectangle with the currently selected color.
  \param x - the desired x-coordinate.
  \param y - the desired y-coordinate.
  \param w - the desired width of the rectangle.
  \param h - the desired height of the rectangle.
  */
  void RenderFilledRect(int x, int y, int w, int h);

  /**
  \brief Renders an outlined rectangle with the currently selected color.
  \param x - the desired x-coordinate.
  \param y - the desired y-coordinate.
  \param w - the desired width of the rectangle.
  \param h - the desired height of the rectangle.
  */
  void RenderOutlinedRect(int x, int y, int w, int h);

  /**
  \brief Renders a line between two points with the currently selected color.
  \param x1 - the x-coordinate of the first point.
  \param y1 - the y-coordinate of the first point.
  \param x2 - the x-coordinate of the second point.
  \param y2 - the y-coordinate of the second point.
  */
  void RenderLine(int x1, int y1, int x2, int y2);

  /**
  \brief Renders a line between two points with the currently selected color.
  \param p1 - the first point.
  \param p2 - the second point.
  */
  void RenderLine(centurion::geo::Point p1, centurion::geo::Point p2);

  void RenderText(const std::string& text, int x, int y);

  /**
  \brief Assigns the currently active font.
  \param font - the font that will be used.
  */
  void SetFont(const std::shared_ptr<centurion::visuals::Font> font);

  /**
  \brief Assigns the active color.
  \param color - the color that will be used.
  */
  void SetColor(centurion::visuals::Color color);

  // TODO add listener methods
};

}  // namespace visuals
}  // namespace centurion