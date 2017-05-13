////////////////////////////////////////////////////////////////////////////////
/// @file ArcBallCamera.h
/// @author Glenn Nygard
/// @version 0.1.0
////////////////////////////////////////////////////////////////////////////////

#ifndef ARCBALLCAMERA_H
#define ARCBALLCAMERA_H

// Standard
#include <vector>

// Qt
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>
#include <QtMath>

////////////////////////////////////////////////////////////////////////////////
/// @enum ARCCamera_Movement
/// @brief Defines several possible options for camera movement.
/// @var ARCFORWARD
/// @var ARCBACKWARD
/// @var ARCLEFT
/// @var ARCRIGHT
////////////////////////////////////////////////////////////////////////////////
enum ARCCamera_Movement {
    ARCFORWARD,
    ARCBACKWARD,
    ARCLEFT,
    ARCRIGHT
};



////////////////////////////////////////////////////////////////////////////////
/// @class ArcBallCamera
/// @brief Camera rotating around origin.
/// Movement:
/// - alt + left mouse button to rotate around the rotation point
/// - scroll wheel or w/s to zoom
/// - middle mouse button to strafe (not added yet)
////////////////////////////////////////////////////////////////////////////////
class ArcBallCamera
{

public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera constructor setting all initial parameters.
  /// All parameters have default values.
  /// @param _position   Position of the camera.
  /// @param _dist       Camera distance from rotation point.
  /// @param _up         Up vector of camera.
  /// @param _yaw        Yaw vector of camera.
  /// @param _pitch      Pitch vector of camera.
  //////////////////////////////////////////////////////////////////////////////
  ArcBallCamera(QVector3D _position = QVector3D(0.0f, 0.0f, 0.0f));

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the view matrix calculated using Eular Angles and the
  /// LookAt Matrix.
  //////////////////////////////////////////////////////////////////////////////
  QMatrix4x4 getViewMatrix();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the current position of the camera.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D getPosition();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the position from the matrix provided.
  /// @param[in] _mat The matrix to extract the position from.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D getPosition(QMatrix4x4 _mat);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the current pitch of the camera.
  //////////////////////////////////////////////////////////////////////////////
  GLfloat getPitch();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the point the camera rotates around.
  //////////////////////////////////////////////////////////////////////////////
  void setRotationPoint(QVector3D _rp);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Processes keyboard inputs. Moves camera in a given direction.
  //////////////////////////////////////////////////////////////////////////////
  void processKeyboard(ARCCamera_Movement _direction, GLfloat _deltaTime);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Processes input received from a mouse input system.
  /// Expects the offset value in both the x and y direction.
  //////////////////////////////////////////////////////////////////////////////
  void processMouseMovement(GLfloat _xoffset, GLfloat _yoffset);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Refocuses the camera to the focus point. This is to reset the
  /// camera if rotations/position gets out of hand. Because the position of
  /// the rotation center (cell cluster) is in constant movement, focus will
  /// not remain correct and might have to be refocused at regular intervals.
  //////////////////////////////////////////////////////////////////////////////
  void refocus();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Processes input received from a mouse scroll-wheel event.
  /// Only requires input on the vertical wheel-axis.
  /// @param[in] steps The amount of steps the scroll wheel has moved.
  //////////////////////////////////////////////////////////////////////////////
  void processMouseScroll(int _steps);

private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates a new distance to the rotation point based on a velocity
  /// input.
  //////////////////////////////////////////////////////////////////////////////
  void move(QVector3D _velocity);


  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates a new rotation quaternion.
  /// @param[in] _vx The rotation axis x value.
  /// @param[in] _vy The rotation axis y value.
  /// @param[in] _vz The rotation axis z value.
  /// @param[in] _offset  Rotation offset. Amount to be rotated.
  //////////////////////////////////////////////////////////////////////////////
  QQuaternion createFromAngle(const double _vx,
                                const double _vy,
                                const double _vz,
                                const double _offset);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera front vector. The facing direction of the camera.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_front;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera up vector. Tells the camera what direction is camera-up.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_up;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera right vector. Tells camera what direction is camera-right.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_right;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Up vector. Tells the camera what direction is scene-up.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_worldUp;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera rotation point. Tells the camera where in the scene the
  /// rotation focus is.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_rotationPoint;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief View matrix.
  //////////////////////////////////////////////////////////////////////////////
  QMatrix4x4 m_view;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera scroll speed.
  //////////////////////////////////////////////////////////////////////////////
  static constexpr GLfloat m_scrollSpeed = 0.02f;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera movement speed. Noticed when using wasd to move around.
  //////////////////////////////////////////////////////////////////////////////
  static constexpr GLfloat m_movementSpeed = 0.4f;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera mouse speed/sensitivity. Noticed when rotating camera.
  //////////////////////////////////////////////////////////////////////////////
  static constexpr GLfloat m_mouseSensitivity = 0.25f;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera yaw vector. Left and right rotation. Euler angle.
  //////////////////////////////////////////////////////////////////////////////
  static constexpr GLfloat m_yaw = -90.0f;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Camera pitch vector. Up and down rotation. Euler angle.
  //////////////////////////////////////////////////////////////////////////////
  static constexpr GLfloat m_pitch = 0.0f;





};

#endif // ARCBALLCAMERA_H
