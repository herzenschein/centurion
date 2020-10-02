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
 * @file cursor.hpp
 *
 * @brief Provides the cursor API.
 *
 * @author Albin Johansson
 *
 * @date 2019-2020
 *
 * @copyright MIT License
 */

#ifndef CENTURION_CURSOR_HEADER
#define CENTURION_CURSOR_HEADER

#include <SDL.h>

#include <memory>       // unique_ptr
#include <type_traits>  // enable_if_t, conditional_t, true_type, false_type

#include "centurion_api.hpp"
#include "detail/utils.hpp"
#include "point.hpp"
#include "surface.hpp"

#ifdef CENTURION_USE_PRAGMA_ONCE
#pragma once
#endif  // CENTURION_USE_PRAGMA_ONCE

namespace cen {

/**
 * @enum system_cursor
 *
 * @ingroup graphics
 *
 * @brief Represents the various available system cursors.
 *
 * @details Mirrors the values of the `SDL_SystemCursor` enum.
 *
 * @since 4.0.0
 *
 * @see `SDL_SystemCursor`
 *
 * @headerfile cursor.hpp
 */
enum class system_cursor {
  arrow = SDL_SYSTEM_CURSOR_ARROW,
  ibeam = SDL_SYSTEM_CURSOR_IBEAM,
  wait = SDL_SYSTEM_CURSOR_WAIT,
  crosshair = SDL_SYSTEM_CURSOR_CROSSHAIR,
  wait_arrow = SDL_SYSTEM_CURSOR_WAITARROW,
  arrow_nw_se = SDL_SYSTEM_CURSOR_SIZENWSE,
  arrow_ne_sw = SDL_SYSTEM_CURSOR_SIZENESW,
  arrow_w_e = SDL_SYSTEM_CURSOR_SIZEWE,
  arrow_n_s = SDL_SYSTEM_CURSOR_SIZENS,
  arrow_all = SDL_SYSTEM_CURSOR_SIZEALL,
  no = SDL_SYSTEM_CURSOR_NO,
  hand = SDL_SYSTEM_CURSOR_HAND
};

/**
 * @brief Indicates whether or not two system cursor values are the same.
 *
 * @ingroup graphics
 *
 * @param lhs the left-hand side system cursor value.
 * @param rhs the right-hand side system cursor value.
 *
 * @return `true` if the system cursor values are the same; `false` otherwise.
 *
 * @since 4.0.0
 */
[[nodiscard]] inline constexpr auto operator==(system_cursor lhs,
                                               SDL_SystemCursor rhs) noexcept
    -> bool
{
  return static_cast<SDL_SystemCursor>(lhs) == rhs;
}

/**
 * @copydoc operator==(system_cursor, SDL_SystemCursor)
 *
 * @ingroup graphics
 */
[[nodiscard]] inline constexpr auto operator==(SDL_SystemCursor lhs,
                                               system_cursor rhs) noexcept
    -> bool
{
  return rhs == lhs;
}

/**
 * @brief Indicates whether or not two system cursor values aren't the same.
 *
 * @ingroup graphics
 *
 * @param lhs the left-hand side system cursor value.
 * @param rhs the right-hand side system cursor value.
 *
 * @return `true` if the system cursor values aren't the same; `false`
 * otherwise.
 *
 * @since 4.0.0
 */
[[nodiscard]] inline constexpr auto operator!=(system_cursor lhs,
                                               SDL_SystemCursor rhs) noexcept
    -> bool
{
  return !(lhs == rhs);
}

/**
 * @copydoc operator!=(system_cursor, SDL_SystemCursor)
 *
 * @ingroup graphics
 */
[[nodiscard]] inline constexpr auto operator!=(SDL_SystemCursor lhs,
                                               system_cursor rhs) noexcept
    -> bool
{
  return !(lhs == rhs);
}

template <typename T>
using is_cursor_owning = std::enable_if_t<std::is_same_v<T, std::true_type>>;

template <typename T>
using is_cursor_handle = std::enable_if_t<std::is_same_v<T, std::false_type>>;

/**
 * @class basic_cursor
 *
 * @brief Represents a mouse cursor.
 *
 * @details Depending on the template type parameter, this class can
 * represent either an owning or non-owning cursor.
 *
 * @tparam T `std::true_type` for owning cursors, `std::false_type` for
 * non-owning cursors.
 *
 * @since 5.0.0
 *
 * @see cursor
 * @see cursor_handle
 *
 * @headerfile cursor.hpp
 */
template <typename T>
class basic_cursor final
{
 public:
  /**
   * @brief Creates a cursor based on a cursor type.
   *
   * @tparam U dummy template parameter used for SFINAE.
   *
   * @param cursor the type of the cursor that will be created.
   *
   * @throws sdl_error if the cursor cannot be created.
   *
   * @since 4.0.0
   */
  template <typename U = T, typename = is_cursor_owning<U>>
  explicit basic_cursor(system_cursor cursor)
      : m_cursor{SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor))}
  {
    if (!m_cursor) {
      throw sdl_error{"Failed to create system cursor"};
    }
  }

  /**
   * @brief Creates a cursor based on a surface and an associated hotspot.
   *
   * @tparam U dummy template parameter used for SFINAE.
   *
   * @param surface the icon associated with the cursor.
   * @param hotspot the hotspot that will be used to determine the location
   * of mouse clicks.
   *
   * @throws sdl_error if the cursor cannot be created.
   *
   * @since 4.0.0
   */
  template <typename U = T, typename = is_cursor_owning<U>>
  basic_cursor(const surface& surface, const ipoint& hotspot)
      : m_cursor{SDL_CreateColorCursor(surface.get(), hotspot.x(), hotspot.y())}
  {
    if (!m_cursor) {
      throw sdl_error{"Failed to create color cursor"};
    }
  }

  /**
   * @brief Creates a handle to a cursor based on a raw pointer.
   *
   * @note This constructor is only available for handles since it would be
   * very easy to introduce subtle bugs by creating owning cursors from
   * `SDL_GetCursor` or `SDL_GetDefaultCursor`, which should not be freed.
   *
   * @tparam U dummy template parameter used for SFINAE.
   *
   * @param cursor a pointer to the associated cursor.
   *
   * @since 5.0.0
   */
  template <typename U = T, typename = is_cursor_handle<U>>
  explicit basic_cursor(SDL_Cursor* cursor) noexcept : m_cursor{cursor}
  {}

  /**
   * @brief Creates a handle to an owning cursor.
   *
   * @tparam U dummy template parameter used for SFINAE.
   *
   * @param cursor the associated owning cursor.
   *
   * @since 5.0.0
   */
  template <typename U = T, typename = is_cursor_handle<U>>
  explicit basic_cursor(const basic_cursor<std::true_type>& cursor) noexcept
      : m_cursor{cursor.get()}
  {}

  /**
   * @brief Returns a handle to the default cursor for the system.
   *
   * @return a handle to the default cursor for the system.; might not be
   * present.
   *
   * @since 5.0.0
   */
  [[nodiscard]] static auto get_default() noexcept
  {
    return handle_t{SDL_GetDefaultCursor()};
  }

  /**
   * @brief Returns a handle to the currently active cursor.
   *
   * @return a handle to the currently active cursor; might not be present.
   *
   * @since 5.0.0
   */
  [[nodiscard]] static auto get_current() noexcept
  {
    return handle_t{SDL_GetCursor()};
  }

  /**
   * @brief Enables the cursor by making it the currently active cursor.
   *
   * @since 4.0.0
   */
  void enable() noexcept
  {
    SDL_SetCursor(get());
  }

  /**
   * @brief Indicates whether or not this cursor is currently active.
   *
   * @note This function checks whether or not the associated cursor is
   * active by comparing the pointer obtained from `SDL_GetCursor` with the
   * internal pointer.
   *
   * @return `true` if the cursor is currently enabled; `false` otherwise.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto is_enabled() const noexcept -> bool
  {
    return SDL_GetCursor() == get();
  }

  /**
   * @brief Resets the active cursor to the system default.
   *
   * @since 4.0.0
   */
  static void reset() noexcept
  {
    SDL_SetCursor(SDL_GetDefaultCursor());
  }

  /**
   * @brief Forces a cursor redraw.
   *
   * @since 4.0.0
   */
  static void force_redraw() noexcept
  {
    SDL_SetCursor(nullptr);
  }

  /**
   * @brief Sets whether or not any mouse cursor is visible.
   *
   * @param visible `true` if cursors should be visible; `false` otherwise.
   *
   * @since 4.0.0
   */
  static void set_visible(bool visible) noexcept
  {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
  }

  /**
   * @brief Indicates whether or not cursors are visible.
   *
   * @return `true` if cursors are visible; `false` otherwise.
   *
   * @since 4.0.0
   */
  [[nodiscard]] static auto visible() noexcept -> bool
  {
    return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE;
  }

  /**
   * @brief Returns the number of system cursors.
   *
   * @return the amount of system cursors.
   *
   * @since 5.0.0
   */
  [[nodiscard]] constexpr static auto num_system_cursors() noexcept -> int
  {
    return static_cast<int>(SDL_NUM_SYSTEM_CURSORS);
  }

  /**
   * @brief Indicates whether or not the cursor handle holds a non-null pointer.
   *
   * @tparam U dummy template parameter used for SFINAE.
   *
   * @return `true` if the internal pointer is not null; `false` otherwise.
   *
   * @since 5.0.0
   */
  template <typename U = T, typename = is_cursor_handle<U>>
  explicit operator bool() const noexcept
  {
    return m_cursor != nullptr;
  }

  /**
   * @brief Returns a pointer to the associated cursor.
   *
   * @warning Don't claim ownership of the returned pointer unless you enjoy
   * playing with fire...
   *
   * @return a pointer to the associated cursor.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto get() const noexcept -> SDL_Cursor*
  {
    if constexpr (is_owner()) {
      return m_cursor.get();
    } else {
      return m_cursor;
    }
  }

 private:
  class deleter final
  {
   public:
    void operator()(SDL_Cursor* cursor) noexcept
    {
      SDL_FreeCursor(cursor);
    }
  };

  using rep_t = std::conditional_t<T::value,
                                   std::unique_ptr<SDL_Cursor, deleter>,
                                   SDL_Cursor*>;
  using owner_t = basic_cursor<std::true_type>;
  using handle_t = basic_cursor<std::false_type>;

  rep_t m_cursor;

  [[nodiscard]] constexpr static auto is_owner() noexcept -> bool
  {
    return std::is_same_v<T, std::true_type>;
  }
};

/**
 * @typedef cursor
 *
 * @brief Represents an owning cursor.
 *
 * @since 5.0.0
 */
using cursor = basic_cursor<std::true_type>;

/**
 * @typedef cursor_handle
 *
 * @brief Represents a non-owning cursor.
 *
 * @since 5.0.0
 */
using cursor_handle = basic_cursor<std::false_type>;

}  // namespace cen

#endif  // CENTURION_CURSOR_HEADER