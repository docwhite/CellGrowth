////////////////////////////////////////////////////////////////////////////////
/// @file PointLight.h
/// @author Glenn Nygard
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

// Qt
#include <QtMath>

// Project
#include "Manipulator.h"
#include "SelectableObject.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief Default position
////////////////////////////////////////////////////////////////////////////////
const QVector3D SPOT_POSITION     = QVector3D(0.0f, 0.0f, 0.0f);

////////////////////////////////////////////////////////////////////////////////
/// @brief Default light colour
////////////////////////////////////////////////////////////////////////////////
const QVector3D SPOT_LIGHT_COLOUR = QVector3D(0.8f, 0.8f, 0.8f);

////////////////////////////////////////////////////////////////////////////////
/// @class PointLight
/// @brief Deals with inputs, and diverts them to the appropriate classes. Point
/// light class. Stores functions and parameters related to the scene lights.
////////////////////////////////////////////////////////////////////////////////
class SpotLight : public SelectableObject
{
public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Custom constructor.
  /// @param[in] _position Light position.
  /// @param[in] _manipshaderp Shader to be used for manipulator.
  /// @param[in] _sunshaderp Shader to be used for light.
  //////////////////////////////////////////////////////////////////////////////
  SpotLight(
      QVector3D _position,
      QOpenGLShaderProgram *_manipshaderp,
      QOpenGLShaderProgram *_sunshaderp);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Compare a unique colour with custom object unique colour.
  /// @param[in] _colour !!!MISSING
  //////////////////////////////////////////////////////////////////////////////
  int compareUniqueColour(QVector3D _colour) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create light geometry.
  /// @param[in] _context Scene context.
  /// @param[out] _masterUniqueColour Object unique colour. Used to draw
  /// geometry to back buffer.
  //////////////////////////////////////////////////////////////////////////////
  void createGeometry(QVector3D &_masterUniqueColour) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Draw light object to main buffer.
  //////////////////////////////////////////////////////////////////////////////
  void draw() override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Draw light object to back buffer using the object unique colour.
  //////////////////////////////////////////////////////////////////////////////
  void drawBackBuffer() override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Process mouse movements.
  /// @param[in] _offsetx Movement in x direction since last update.
  /// @param[in] _offsety Movement in y direction since last update.
  /// @param[in] _offsetz Movement in z direction since last update.
  //////////////////////////////////////////////////////////////////////////////
  void processMouseMovement(float _offsetx,
                            float _offsety,
                            float _offsetz,
                            QMatrix4x4 _view,
                            QMatrix4x4 _proj) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Set object as clicked.
  /// @param[in] _uColour Unique colour.
  /// @param[in] _state State to be set (clicked/unclicked)
  //////////////////////////////////////////////////////////////////////////////
  void setClicked(QVector3D _uColour, bool _state) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Mark the object as being under the mouse cursor.
  /// @param[in] _id Axis id.
  //////////////////////////////////////////////////////////////////////////////
  void setHover(int _id) override;


  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns light position.
  /// @returns Get light position.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D getPosition() override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns light object manipulator.
  /// @returns Get light manipulator.
  //////////////////////////////////////////////////////////////////////////////
  Manipulator getManipulator();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns main light shader program.
  /// @param[in] retshader Get light main shader program.
  //////////////////////////////////////////////////////////////////////////////
  void getMainProgram(QOpenGLShaderProgram **_retshader);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Updates the light model matrix for use with the camera.
  /// @param[in] _offsetx Movement in x direction since last update.
  /// @param[in] _offsety Movement in y direction since last update.
  /// @param[in] _offsetz Movement in z direction since last update.
  //////////////////////////////////////////////////////////////////////////////
  void updateModelMatrix();

private:

  QQuaternion create_from_angle(
      const double &xx, const double &yy,
      const double &zz, const double &a);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Rotates the object based on mouse input.
  //////////////////////////////////////////////////////////////////////////////
  void rotate(float _offsetx, float _offsety);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief How fast (sensitive) the light movement should be for the user.
  /// Its static as the value remains constant between instances of the class.
  //////////////////////////////////////////////////////////////////////////////
  static constexpr GLfloat m_sensitivity=0.04f;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief The light angle of the light in degrees. Must be between 1 and 179.
  //////////////////////////////////////////////////////////////////////////////
  const GLfloat m_coneangle;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light manipulator.
  //////////////////////////////////////////////////////////////////////////////
  Manipulator m_manip;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light position.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_position;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light VBO.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer* m_vbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light VAO.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLVertexArrayObject *m_vao;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Stores the light representation position (the little icon
  /// indicating the light).
  //////////////////////////////////////////////////////////////////////////////
  GLfloat m_points[3];

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Number of points to be drawn using OpenGL draw function.
  //////////////////////////////////////////////////////////////////////////////
  GLint m_numberOfPoints;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light representation shader program.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram *m_sunshaderp;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Manipulator shader program.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram *m_manipshaderp;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light model matrix.
  //////////////////////////////////////////////////////////////////////////////
  QMatrix4x4 m_model;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light front direction (along x axis).
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_x;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light up direction (along y axis).
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_y;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light right direction (along z axis).
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_z;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Vector stating the scene up direction.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_worldUp;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Current yaw rotation.
  //////////////////////////////////////////////////////////////////////////////
  float m_yaw;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Current pitch rotation.
  //////////////////////////////////////////////////////////////////////////////
  float m_pitch;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Quaternion for object rotation.
  /// Set to identity quaternion (1,0,0,0).
  //////////////////////////////////////////////////////////////////////////////
  QQuaternion m_quat= QQuaternion();



};

#endif
