#pragma once

namespace centurion {
namespace geo {

/**
\brief The Positionable class provides an interface for objects that are
positionable (in two dimensions).
*/
class Positionable {
 protected:
  Positionable() = default;

 public:
  virtual ~Positionable() = default;

  /**
  \brief Sets the x-coordinate of this object.
  \param x - the new x-coordinate.
  */
  virtual void SetX(int x) = 0;

  /**
  \brief Sets the y-coordinate of this object.
  \param y - the new y-coordinate.
  */
  virtual void SetY(int y) = 0;

  /**
  \brief Sets the location of this object.
  \param x - the new x-coordinate.
  \param y - the new y-coordinate.
  */
  virtual void SetLocation(int x, int y) = 0;

  /**
  \brief Returns the x-coordinate of this object.
  */
  virtual int GetX() const = 0;

  /**
  \brief Returns the y-coordinate of this object.
  */
  virtual int GetY() const = 0;
};

}  // namespace geo
}  // namespace centurion