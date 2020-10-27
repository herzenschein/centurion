/*
 * MIT License
 *
 * Copyright (c) 2019-2020 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file texture.hpp
 *
 * @brief Provides the `texture` class.
 *
 * @author Albin Johansson
 *
 * @date 2019-2020
 *
 * @copyright MIT License
 */

#ifndef CENTURION_TEXTURE_HEADER
#define CENTURION_TEXTURE_HEADER

#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include <memory>       // unique_ptr
#include <ostream>      // ostream
#include <string>       // string
#include <type_traits>  // true_type, false_type, enable_if_t

#include "area.hpp"
#include "blend_mode.hpp"
#include "centurion_api.hpp"
#include "centurion_fwd.hpp"
#include "color.hpp"
#include "detail/to_string.hpp"
#include "exception.hpp"
#include "pixel_format.hpp"
#include "point.hpp"
#include "scale_mode.hpp"
#include "surface.hpp"
#include "texture_access.hpp"
#include "types.hpp"

#ifdef CENTURION_USE_PRAGMA_ONCE
#pragma once
#endif  // CENTURION_USE_PRAGMA_ONCE

namespace cen {

template <typename T>
using is_texture_owning =
    std::enable_if_t<std::is_same_v<T, std::true_type>, bool>;

template <typename T>
using is_texture_handle =
    std::enable_if_t<std::is_same_v<T, std::false_type>, bool>;

/**
 * @class basic_texture
 *
 * @ingroup graphics
 *
 * @brief Represents an hardware-accelerated image.
 *
 * @since 3.0.0
 *
 * @see `SDL_Texture`
 * @see `texture`
 * @see `texture_handle`
 *
 * @headerfile texture.hpp
 */
template <typename T>
class basic_texture final
{
  using owner_t = basic_texture<std::true_type>;

  [[nodiscard]] constexpr static auto is_owning() noexcept -> bool
  {
    return std::is_same_v<T, std::true_type>;
  }

 public:
  /**
   * @brief Creates an texture from a pre-existing SDL texture.
   *
   * @param src a pointer to the associated SDL texture.
   *
   * @throws exception if the supplied pointer is null *and* the texture is
   * owning.
   *
   * @since 3.0.0
   */
  explicit basic_texture(SDL_Texture* src) : m_texture{src}
  {
    if constexpr (is_owning()) {
      if (!m_texture) {
        throw exception{"Cannot create texture from null pointer!"};
      }
    }
  }

  /**
   * @brief Creates a handle to texture instance.
   *
   * @param owner the associated owning texture.
   *
   * @since 5.0.0
   */
  template <typename T_ = T, is_texture_handle<T_> = true>
  explicit basic_texture(owner_t& owner) noexcept : m_texture{owner.get()}
  {}

  /**
   * @brief Creates a texture based the image at the specified path.
   *
   * @tparam Renderer the type of the renderer, e.g. `renderer` or
   * `renderer_handle`.
   *
   * @param renderer the renderer that will be used to create the texture.
   * @param path the file path of the texture, can't be null.
   *
   * @throws img_error if the texture cannot be loaded.
   *
   * @since 4.0.0
   */
  template <typename Renderer, typename U = T, is_texture_owning<U> = true>
  basic_texture(const Renderer& renderer, nn_czstring path)
      : m_texture{IMG_LoadTexture(renderer.get(), path)}
  {
    if (!m_texture) {
      throw img_error{"Failed to load texture from file"};
    }
  }

  /**
   * @brief Creates an texture that is a copy of the supplied surface.
   *
   * @tparam Renderer the type of the renderer, e.g. `renderer` or
   * `renderer_handle`.
   *
   * @param renderer the renderer that will be used to create the texture.
   * @param surface the surface that the texture will be based on.
   *
   * @throws sdl_error if the texture cannot be loaded.
   *
   * @since 4.0.0
   */
  template <typename Renderer, typename U = T, is_texture_owning<U> = true>
  basic_texture(const Renderer& renderer, const surface& surface)
      : m_texture{SDL_CreateTextureFromSurface(renderer.get(), surface.get())}
  {
    if (!m_texture) {
      throw sdl_error{"Failed to create texture from surface"};
    }
  }

  /**
   * @brief Creates an texture with the specified characteristics.
   *
   * @tparam Renderer the type of the renderer, e.g. `renderer` or
   * `renderer_handle`.
   *
   * @param renderer the associated renderer instance.
   * @param format the pixel format of the created texture.
   * @param access the access of the created texture.
   * @param size the size of the texture.
   *
   * @throws sdl_error if the texture cannot be created.
   *
   * @since 4.0.0
   */
  template <typename Renderer, typename U = T, is_texture_owning<U> = true>
  basic_texture(const Renderer& renderer,
                pixel_format format,
                texture_access access,
                const iarea& size)
      : m_texture{SDL_CreateTexture(renderer.get(),
                                    static_cast<u32>(format),
                                    static_cast<int>(access),
                                    size.width,
                                    size.height)}
  {
    if (!m_texture) {
      throw sdl_error{"Failed to create texture"};
    }
  }

  /**
   * @brief Creates and returns a texture with streaming access.
   *
   * @details The created texture is based on the image at the specified path
   * with the `streaming` texture access.
   *
   * @tparam Renderer the type of the renderer, e.g. `renderer` or
   * `renderer_handle`.
   *
   * @param renderer the renderer that will be used to create the texture.
   * @param path the path of the image file to base the texture on, can't be
   * null.
   * @param format the pixel format that will be used by the texture.
   *
   * @throws exception if something goes wrong.
   *
   * @return a texture with `streaming` texture access.
   *
   * @since 4.0.0
   */
  template <typename Renderer, typename U = T, is_texture_owning<U> = true>
  [[nodiscard]] static auto streaming(const Renderer& renderer,
                                      nn_czstring path,
                                      pixel_format format) -> basic_texture
  {
    const auto blendMode = blend_mode::blend;
    const auto createSurface = [=](czstring path, pixel_format format) {
      surface source{path};
      source.set_blend_mode(blendMode);
      return source.convert(format);
    };
    const auto surface = createSurface(path, format);
    auto tex = texture{renderer,
                       format,
                       texture_access::streaming,
                       {surface.width(), surface.height()}};
    tex.set_blend_mode(blendMode);

    u32* pixels = nullptr;
    const auto success = tex.lock(&pixels);
    if (!success) {
      throw exception{"Failed to lock texture!"};
    }

    const auto maxCount = static_cast<size_t>(surface.pitch()) *
                          static_cast<size_t>(surface.height());
    std::memcpy(pixels, surface.pixels(), maxCount);

    tex.unlock();

    return tex;
  }

  /**
   * @brief Sets the color of the pixel at the specified coordinate.
   *
   * @details This method has no effect if the texture access isn't
   * `Streaming` or if the coordinate is out-of-bounds.
   *
   * @param pixel the pixel that will be changed.
   * @param color the new color of the pixel.
   *
   * @since 4.0.0
   */
  void set_pixel(const ipoint& pixel, const color& color) noexcept
  {
    if (access() != texture_access::streaming || pixel.x() < 0 ||
        pixel.y() < 0 || pixel.x() >= width() || pixel.y() >= height()) {
      return;
    }

    u32* pixels = nullptr;
    int pitch;
    const auto success = lock(&pixels, &pitch);
    if (!success) {
      return;
    }

    const int nPixels = (pitch / 4) * height();
    const int index = (pixel.y() * width()) + pixel.x();

    if ((index >= 0) && (index < nPixels)) {
      auto* pixelFormat = SDL_AllocFormat(static_cast<u32>(format()));
      const auto value = SDL_MapRGBA(pixelFormat,
                                     color.red(),
                                     color.green(),
                                     color.blue(),
                                     color.alpha());

      SDL_FreeFormat(pixelFormat);

      pixels[index] = value;
    }

    unlock();
  }

  /**
   * @brief Sets the alpha value of the texture.
   *
   * @param alpha the alpha value, in the range [0, 255].
   *
   * @since 3.0.0
   */
  void set_alpha(u8 alpha) noexcept
  {
    SDL_SetTextureAlphaMod(get(), alpha);
  }

  /**
   * @brief Sets the blend mode that will be used by the texture.
   *
   * @param mode the blend mode that will be used.
   *
   * @since 3.0.0
   */
  void set_blend_mode(blend_mode mode) noexcept
  {
    SDL_SetTextureBlendMode(get(), static_cast<SDL_BlendMode>(mode));
  }

  /**
   * @brief Sets the color modulation of the texture.
   *
   * @note The alpha component in the color struct is ignored by this method.
   *
   * @param color the color that will be used to modulate the color of the
   * texture.
   *
   * @since 3.0.0
   */
  void set_color_mod(const color& color) noexcept
  {
    SDL_SetTextureColorMod(get(), color.red(), color.green(), color.blue());
  }

  /**
   * @brief Sets the scale mode that will be used by the texture.
   *
   * @param mode the scale mode that will be used.
   *
   * @since 4.0.0
   */
  void set_scale_mode(scale_mode mode) noexcept
  {
    SDL_SetTextureScaleMode(get(), static_cast<SDL_ScaleMode>(mode));
  }

  /**
   * @brief Releases ownership of the associated SDL texture and returns a
   * pointer to it.
   *
   * @warning Usage of this function should be considered dangerous, since
   * you might run into memory leak issues. You **must** call
   * `SDL_DestroyTexture` on the returned pointer to free the associated
   * memory.
   *
   * @return a pointer to the associated SDL texture.
   *
   * @since 5.0.0
   */
  [[nodiscard]] auto release() noexcept -> owner<SDL_Texture*>
  {
    return m_texture.release();
  }

  /**
   * @brief Returns the pixel format that is used by the texture.
   *
   * @return the pixel format that is used by the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto format() const noexcept -> pixel_format
  {
    u32 format{};
    SDL_QueryTexture(get(), &format, nullptr, nullptr, nullptr);
    return static_cast<pixel_format>(format);
  }

  /**
   * @brief Returns the texture access of the texture.
   *
   * @return the texture access of the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto access() const noexcept -> texture_access
  {
    int access{};
    SDL_QueryTexture(get(), nullptr, &access, nullptr, nullptr);
    return static_cast<texture_access>(access);
  }

  /**
   * @brief Returns the width of the texture.
   *
   * @return the width of the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto width() const noexcept -> int
  {
    const auto [width, height] = size();
    return width;
  }

  /**
   * @brief Returns the height of the texture.
   *
   * @return the height of the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto height() const noexcept -> int
  {
    const auto [width, height] = size();
    return height;
  }

  /**
   * @brief Returns the size of the texture.
   *
   * @return the size of the texture.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto size() const noexcept -> iarea
  {
    int width{};
    int height{};
    SDL_QueryTexture(get(), nullptr, nullptr, &width, &height);
    return {width, height};
  }

  /**
   * @brief Indicates whether or not the texture is a possible render target.
   *
   * @return `true` if the texture is a possible render target; `false`
   * otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto is_target() const noexcept -> bool
  {
    return access() == texture_access::target;
  }

  /**
   * @brief Indicates whether or not the texture has static texture access.
   *
   * @return `true` if the texture has static texture access.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto is_static() const noexcept -> bool
  {
    return access() == texture_access::no_lock;
  }

  /**
   * @brief Indicates whether or not the texture has streaming texture access.
   *
   * @return `true` if the texture has streaming texture access; `false`
   * otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto is_streaming() const noexcept -> bool
  {
    return access() == texture_access::streaming;
  }

  /**
   * @brief Returns the alpha value of the texture.
   *
   * @return the alpha value of the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto alpha() const noexcept -> u8
  {
    u8 alpha{};
    SDL_GetTextureAlphaMod(get(), &alpha);
    return alpha;
  }

  /**
   * @brief Returns the blend mode of the texture.
   *
   * @return the blend mode of the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto get_blend_mode() const noexcept -> blend_mode
  {
    SDL_BlendMode mode{};
    SDL_GetTextureBlendMode(get(), &mode);
    return static_cast<blend_mode>(mode);
  }

  /**
   * @brief Returns the color modulation of the texture.
   *
   * @return the modulation of the texture.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto color_mod() const noexcept -> color
  {
    u8 red{};
    u8 green{};
    u8 blue{};
    SDL_GetTextureColorMod(get(), &red, &green, &blue);
    return {red, green, blue, 0xFF};
  }

  /**
   * @brief Returns the scale mode that is used by the texture.
   *
   * @return the scale mode that is used by the texture.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto get_scale_mode() const noexcept -> scale_mode
  {
    SDL_ScaleMode mode{};
    SDL_GetTextureScaleMode(get(), &mode);
    return static_cast<scale_mode>(mode);
  }

  /**
   * @brief Indicates whether or not a texture handle holds a non-null pointer.
   *
   * @tparam T_ dummy parameter for SFINAE.
   *
   * @return `true` if the handle holds a non-null pointer; `false` otherwise.
   *
   * @since 5.0.0
   */
  template <typename T_ = T, is_texture_handle<T_> = true>
  explicit operator bool() const noexcept
  {
    return m_texture != nullptr;
  }

  /**
   * @brief Returns a pointer to the associated `SDL_Texture`.
   *
   * @return a pointer to the associated `SDL_Texture`.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto get() const noexcept -> SDL_Texture*
  {
    if constexpr (is_owning()) {
      return m_texture.get();
    } else {
      return m_texture;
    }
  }

  /**
   * @brief Converts to `SDL_Texture*`.
   *
   * @return a pointer to the associated `SDL_Texture`.
   *
   * @since 3.0.0
   */
  [[nodiscard]] explicit operator SDL_Texture*() noexcept
  {
    return get();
  }

  /**
   * @brief Converts to `const SDL_Texture*`.
   *
   * @return a pointer to the associated `SDL_Texture`.
   *
   * @since 3.0.0
   */
  [[nodiscard]] explicit operator const SDL_Texture*() const noexcept
  {
    return get();
  }

 private:
  struct deleter final
  {
    void operator()(SDL_Texture* texture) noexcept
    {
      SDL_DestroyTexture(texture);
    }
  };
  using rep_t = std::conditional_t<T::value,
                                   std::unique_ptr<SDL_Texture, deleter>,
                                   SDL_Texture*>;
  rep_t m_texture;

  /**
   * @brief Locks the texture for write-only pixel access.
   *
   * @remarks This method is only applicable if the texture access of the
   * texture is `Streaming`.
   *
   * @param pixels this will be filled with a pointer to the locked pixels.
   * @param pitch This is filled in with the pitch of the locked pixels, can
   * safely be null if it isn't needed.
   *
   * @return `true` if all went well; `false` otherwise.
   *
   * @since 4.0.0
   */
  auto lock(u32** pixels, int* pitch = nullptr) noexcept -> bool
  {
    if (pitch) {
      const auto result = SDL_LockTexture(get(),
                                          nullptr,
                                          reinterpret_cast<void**>(pixels),
                                          pitch);
      return result == 0;
    } else {
      int dummyPitch;
      const auto result = SDL_LockTexture(get(),
                                          nullptr,
                                          reinterpret_cast<void**>(pixels),
                                          &dummyPitch);
      return result == 0;
    }
  }

  /**
   * @brief Unlocks the texture.
   *
   * @since 4.0.0
   */
  void unlock() noexcept
  {
    SDL_UnlockTexture(get());
  }
};

using texture = basic_texture<std::true_type>;
using texture_handle = basic_texture<std::false_type>;

/**
 * @brief Returns a textual representation of a texture.
 *
 * @ingroup graphics
 *
 * @param texture the texture that will be converted.
 *
 * @return a string that represents the texture.
 *
 * @since 5.0.0
 */
template <typename T>
[[nodiscard]] auto to_string(const basic_texture<T>& texture) -> std::string
{
  using detail::to_string;
  return "[texture | ptr: " + detail::address_of(texture.get()) +
         ", width: " + to_string(texture.width()).value() +
         ", height: " + to_string(texture.height()).value() + "]";
}

/**
 * @brief Prints a textual representation of a texture.
 *
 * @ingroup graphics
 *
 * @param stream the stream that will be used.
 * @param texture
 *
 * @return the used stream.
 *
 * @since 5.0.0
 */
template <typename T>
auto operator<<(std::ostream& stream, const basic_texture<T>& texture)
    -> std::ostream&
{
  stream << to_string(texture);
  return stream;
}

}  // namespace cen

#endif  // CENTURION_TEXTURE_HEADER