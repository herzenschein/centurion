#include "key_state.h"
#include <string>
#include <algorithm>
#include <cassert>
#include <type_traits>
#include "centurion_exception.h"
#include "error.h"

namespace centurion {

static_assert(std::is_final_v<KeyState>);

static_assert(std::is_default_constructible_v<KeyState>);
static_assert(std::is_nothrow_destructible_v<KeyState>);

static_assert(std::is_nothrow_move_constructible_v<KeyState>);
static_assert(std::is_nothrow_move_assignable_v<KeyState>);

static_assert(std::is_nothrow_copy_constructible_v<KeyState>);
static_assert(std::is_nothrow_copy_assignable_v<KeyState>);

KeyState::KeyState() {
  states = SDL_GetKeyboardState(&nKeys);
  if (!states) {
    throw CenturionException("Failed to obtain key state! " + Error::msg());
  }
  assert(static_cast<unsigned long long>(nKeys) == previousStates.size());
  std::fill(previousStates.begin(), previousStates.end(), 0);
}

std::unique_ptr<KeyState> KeyState::unique() {
  return std::make_unique<KeyState>();
}

std::shared_ptr<KeyState> KeyState::shared() {
  return std::make_shared<KeyState>();
}

void KeyState::update() noexcept {
  std::copy(states, states + nKeys, previousStates.begin());
}

bool KeyState::is_pressed(SDL_Scancode code) const noexcept {
  assert(code < nKeys);
  return states[code];
}

bool KeyState::is_held(SDL_Scancode code) const noexcept {
  assert(code < nKeys);
  return states[code] && previousStates[code];
}

bool KeyState::was_just_pressed(SDL_Scancode code) const noexcept {
  assert(code < nKeys);
  return states[code] && !previousStates[code];
}

bool KeyState::was_just_released(SDL_Scancode code) const noexcept {
  assert(code < nKeys);
  return !states[code] && previousStates[code];
}

int KeyState::get_amount_of_keys() const noexcept {
  return nKeys;
}

}
