#ifndef CENTURION_QUIT_EVENT_HEADER
#define CENTURION_QUIT_EVENT_HEADER

#include <SDL.h>

#include "../centurion_cfg.hpp"
#include "common_event.hpp"

#ifdef CENTURION_USE_PRAGMA_ONCE
#pragma once
#endif  // CENTURION_USE_PRAGMA_ONCE

namespace cen {

/// \addtogroup event
/// \{

/**
 * \class quit_event
 *
 * \brief Represents the event of the user wanting to close the application,
 * usually by pressing the "X"-button on the window frame.
 *
 * \see `SDL_QuitEvent`
 *
 * \since 4.0.0
 *
 * \headerfile event.hpp
 */
class quit_event final : public common_event<SDL_QuitEvent>
{
 public:
  /**
   * \brief Creates a quit event.
   *
   * \since 4.0.0
   */
  quit_event() noexcept : common_event{event_type::quit}
  {}

  /**
   * \brief Creates a quit event based on the supplied SDL event.
   *
   * \param event the SDL event that will be copied.
   *
   * \since 4.0.0
   */
  explicit quit_event(const SDL_QuitEvent& event) noexcept : common_event{event}
  {}
};

template <>
inline auto as_sdl_event(const common_event<SDL_QuitEvent>& event) -> SDL_Event
{
  SDL_Event e;
  e.quit = event.get();
  return e;
}

/// \} End of group event

}  // namespace cen

#endif  // CENTURION_QUIT_EVENT_HEADER
