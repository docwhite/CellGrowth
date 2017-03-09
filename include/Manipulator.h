////////////////////////////////////////////////////////////////////////////////
/// @file Manipulator.h
/// @author Glenn Nygard
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

// Qt
#include <QVector3D>

// OpenGL
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

////////////////////////////////////////////////////////////////////////////////
/// @brief Struct storing the individual data for each manipulator arrow.
////////////////////////////////////////////////////////////////////////////////
struct Arrow {
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Is the arrow currently clicked.
  //////////////////////////////////////////////////////////////////////////////
  bool clicked;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief What axis does the arrow originally point down.
  //////////////////////////////////////////////////////////////////////////////
  int axis;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Number of vertices that make up the arrow object.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int numberOfPoints;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Arrow unique colour.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D uniqueColour;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Arrow render colour. The actual colour of the arrow.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D renderColour;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Arrow VBO.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer *vbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Arrow VAO.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLVertexArrayObject *vao;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Struct storing the individual data for each manipulator rotation
/// circle.
////////////////////////////////////////////////////////////////////////////////
struct RotCircle {
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Is the circle currently clicked?
  //////////////////////////////////////////////////////////////////////////////
  bool clicked;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Along what axis does the circle rotate the manipulator?
  //////////////////////////////////////////////////////////////////////////////
  int direction;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Number of vertices that make up the circle object.
  //////////////////////////////////////////////////////////////////////////////
  unsigned int numberOfPoints;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Circle unique colour.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D uniqueColour;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Circle render colour. The actual colour of the Circle.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D renderColour;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Circle VBO.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer *vbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Circle VAO.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLVertexArrayObject *vao;
};

////////////////////////////////////////////////////////////////////////////////
/// @class Manipulator
/// @brief The handles used for moving objects around in the scene. Can be
/// used by any object that needs to be moved around.
////////////////////////////////////////////////////////////////////////////////
class Manipulator
{

public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Constructor taking position and the shaderprogram to be used for
  /// the manipulator.
  /// @param[in] _position Position of the manipulator.
  /// @param[in] _lightProgram !!!MISSING
  //////////////////////////////////////////////////////////////////////////////
  explicit Manipulator (QVector3D _position, QOpenGLShaderProgram *_lightProgram);

  ~Manipulator();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Compares the given colour with the unique colour of each arrow in
  /// manipulator.
  /// @param[in] _colour Colour to be compared
  //////////////////////////////////////////////////////////////////////////////
  int compareUniqueColour(QVector3D _colour);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates the manipulator geometry.
  /// @param[in] context QOpenGL scene context.
  /// @param[in] uColourVec Vector of unique colours to be assigned to the
  /// manipulator arrows.
  //////////////////////////////////////////////////////////////////////////////
  void createGeometry(QOpenGLContext *_context, std::vector<QVector3D> _uColourVec);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Draws the manipulator to the main buffer.
  //////////////////////////////////////////////////////////////////////////////
  void draw();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Draws the manipulator to a temporary buffer, using manipulators
  /// unique colours.
  //////////////////////////////////////////////////////////////////////////////
  void drawBackBuffer();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Processes mouse movement and calculates a new light position. Which
  /// axis was pressed must be checked before a new position can be calculated.
  /// This new position is then returned to the higher class.
  /// @param[in] offsetx The x offset.
  /// @param[in] offsety The y offset.
  /// @param[in] offsetz The z offset.
  /// @param[in] currentPos Current light position.
  /// \return QVector3D New position returned to the class owning the manipulator.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D processMouseMovement(
    float _offsetx,
    float _offsety,
    float _offsetz,
    QVector3D _x,
    QVector3D _y,
    QVector3D _z );

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the manipulator arrows to clicked/not clicked.
  /// @param[in] uColourIdentity Unique colour that has been clicked.
  /// @param[in] state Boolean stating wether it is being run from mouse click
  /// or release. If the latter clicked should always be set to false.
  //////////////////////////////////////////////////////////////////////////////
  void setClicked(QVector3D _uColourIdentity, bool _state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Changes the colour of the manipulator arrows if the mouse is
  /// hovering over them.
  /// @param[in] axis Specifies which axis is being affected.
  //////////////////////////////////////////////////////////////////////////////
  void setHover(int _axis);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns all arrows in the arrow vector.
  /// @param[out] _arrows Vector where to write the arrows.
  //////////////////////////////////////////////////////////////////////////////
  void getArrows(std::vector<Arrow> &_arrows);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the currently clicked arrow.
  /// @returns The arrow/axis currently being manipulated.
  //////////////////////////////////////////////////////////////////////////////
  int getClickedAxis();

private:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates and sets up the vbo for the provided arrow object
  /// @param[in] vertices Vertices being set to the VBO
  /// @param[out] arrow The arrow struct after the vbo has been added to it.
  //////////////////////////////////////////////////////////////////////////////
  void setupRotCircleVBO(
      std::vector<QVector3D> _vertices,
      RotCircle &_circle);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates and sets up the vao for the provided arrow object
  /// @param[out] arrow The circle struct after the vbo has been added to it.
  /// @param[in] vao The initialised vao to be set up.
  //////////////////////////////////////////////////////////////////////////////
  void setupRotCircleVAO(Arrow &_circle, QOpenGLVertexArrayObject *_vao);



  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates and sets up the vbo for the provided arrow object
  /// @param[in] vertices Vertices being set to the VBO
  /// @param[in] normals Normals being set to the VBO
  /// @param[out] arrow The arrow struct after the vbo has been added to it.
  //////////////////////////////////////////////////////////////////////////////
  void setupVBO(
      std::vector<QVector3D> _vertices,
      std::vector<QVector3D> _normals,
      Arrow &_arrow);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates and sets up the vao for the provided arrow object
  /// @param[out] arrow The arrow struct after the vbo has been added to it.
  /// @param[in] vao The initialised vao to be set up.
  //////////////////////////////////////////////////////////////////////////////
  void setupVAO(Arrow &_arrow, QOpenGLVertexArrayObject *_vao);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates an arrow. Changes the colour of the manipulator arrows if
  /// the mouse is hovering over them
  /// @param[in] vao VAO being used to create arrow.
  /// @param[in] offsetPos Position of the created arrow.
  /// @param[in] uniqueColour the unique colour assigned to the arrow.
  /// @param[in] axis Axis assigned to the arrow.
  //////////////////////////////////////////////////////////////////////////////
  void createArrow(
      QOpenGLVertexArrayObject *_vao,
      QVector3D _offsetPos,
      QVector3D _uniqueColour,
      int _axis);

  void createRotCircle(QOpenGLVertexArrayObject *_vao);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Manipulator shader program
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram *m_manipshaderp;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Manipulator position
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_position;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Vector storing the arrows
  //////////////////////////////////////////////////////////////////////////////
  std::vector<Arrow> m_arrows;

};


#endif
