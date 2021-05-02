#ifndef CENTURION_JOY_HAT_EVENT_HEADER
#define CENTURION_JOY_HAT_EVENT_HEADER

#include <SDL.h>

#include "../core/integers.hpp"
#include "../core/to_underlying.hpp"
#include "common_event.hpp"

namespace cen {

/// \addtogroup event
/// \{

/**
 * \enum joy_hat_position
 *
 * \brief Serves as a wrapper for the `SDL_HAT_x` macro values.
 *
 * \since 4.0.0
 */
enum class joy_hat_position : u8
{
  left_up = SDL_HAT_LEFTUP,
  left = SDL_HAT_LEFT,
  left_down = SDL_HAT_LEFTDOWN,
  up = SDL_HAT_UP,
  centered = SDL_HAT_CENTERED,
  down = SDL_HAT_DOWN,
  right_up = SDL_HAT_RIGHTUP,
  right = SDL_HAT_RIGHT,
  right_down = SDL_HAT_RIGHTDOWN
};

/**
 * \class joy_hat_event
 *
 * \brief Represents an event that is triggered whenever a user moves a hat
 * on a joystick.
 *
 * \see `SDL_JoyHatEvent`
 *
 * \since 4.0.0
 *
 * \headerfile joy_hat_event.hpp
 */
class joy_hat_event final : public common_event<SDL_JoyHatEvent>
{
 public:
  /**
   * \brief Creates a joy hat event.
   *
   * \since 4.0.0
   */
  joy_hat_event() noexcept : common_event{event_type::joystick_hat_motion}
  {}

  /**
   * \brief Creates a joy hat event based on the supplied SDL event.
   *
   * \param event the SDL event that will be copied.
   *
   * \since 4.0.0
   */
  explicit joy_hat_event(const SDL_JoyHatEvent& event) noexcept : common_event{event}
  {}

  /**
   * \brief Sets the hat index associated with the event.
   *
   * \param hat the hat index.
   *
   * \since 4.0.0
   */
  void set_hat(const u8 hat) noexcept
  {
    m_event.hat = hat;
  }

  /**
   * \brief Sets the joystick hat position associated with the event.
   *
   * \param value the joystick hat position associated with the event.
   *
   * \since 4.0.0
   */
  void set_position(const joy_hat_position value) noexcept
  {
    m_event.value = to_underlying(value);
  }

  /**
   * \brief Returns the index of the hat that changed.
   *
   * \return the index of the hat that changed.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto hat() const noexcept -> u8
  {
    return m_event.hat;
  }

  /**
   * \brief Returns the position of the associated joystick hat.
   *
   * \return the position of the associated joystick hat.
   *
   * \since 4.0.0
   */
  [[nodiscard]] auto position() const noexcept -> joy_hat_position
  {
    return static_cast<joy_hat_position>(m_event.value);
  }
};

template <>
inline auto as_sdl_event(const common_event<SDL_JoyHatEvent>& event) -> SDL_Event
{
  SDL_Event e;
  e.jhat = event.get();
  return e;
}

/// \} End of group event

}  // namespace cen

#endif  // CENTURION_JOY_HAT_EVENT_HEADER
