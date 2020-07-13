#include <catch.hpp>

#include "event.hpp"

using namespace centurion;

TEST_CASE("JoyBallEvent::set_which", "[JoyBallEvent]")
{
  joy_ball_event event;

  const auto which = 3;
  event.set_which(which);

  CHECK(event.which() == which);
}

TEST_CASE("JoyBallEvent::set_ball", "[JoyBallEvent]")
{
  joy_ball_event event;

  const auto ball = 7;
  event.set_ball(ball);

  CHECK(event.ball() == ball);
}

TEST_CASE("JoyBallEvent::set_dx", "[JoyBallEvent]")
{
  joy_ball_event event;

  const auto dx = 173;
  event.set_dx(dx);

  CHECK(event.dx() == dx);
}

TEST_CASE("JoyBallEvent::set_dy", "[JoyBallEvent]")
{
  joy_ball_event event;

  const auto dy = -57;
  event.set_dy(dy);

  CHECK(event.dy() == dy);
}

TEST_CASE("JoyBallEvent::which", "[JoyBallEvent]")
{
  SDL_JoyBallEvent sdlEvent;
  sdlEvent.which = 5;
  joy_ball_event event{sdlEvent};

  CHECK(event.which() == sdlEvent.which);
}

TEST_CASE("JoyBallEvent::ball", "[JoyBallEvent]")
{
  SDL_JoyBallEvent sdlEvent;
  sdlEvent.ball = 17;
  joy_ball_event event{sdlEvent};

  CHECK(event.ball() == sdlEvent.ball);
}

TEST_CASE("JoyBallEvent::dx", "[JoyBallEvent]")
{
  SDL_JoyBallEvent sdlEvent;
  sdlEvent.xrel = 723;
  joy_ball_event event{sdlEvent};

  CHECK(event.dx() == sdlEvent.xrel);
}

TEST_CASE("JoyBallEvent::dy", "[JoyBallEvent]")
{
  SDL_JoyBallEvent sdlEvent;
  sdlEvent.yrel = 5933;
  joy_ball_event event{sdlEvent};

  CHECK(event.dy() == sdlEvent.yrel);
}
