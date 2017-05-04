////////////////////////////////////////////////////////////////////////////////
/// @file GLWindow.h
/// @author Ramon Blanquer
/// @author Fanny Marstrom
/// @author Carola Gille
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////

#ifndef GLWINDOW_H
#define GLWINDOW_H

// Standard
#include <array>
#include <random>

// Qt
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QWheelEvent>
#include <QMainWindow>

// Project
#include "ParticleSystem.h"
#include "InputManager.h"
#include "SkyBox.h"

////////////////////////////////////////////////////////////////////////////////
/// @class Window
/// @brief Handles all the windowing related functionality.
///
/// Subclasses QOpenGLWindow which wraps all the functionality that allows
/// QWindow to render OpenGL graphics.
////////////////////////////////////////////////////////////////////////////////
class GLWindow : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core

{
  Q_OBJECT

public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Constructor.
  /// @param[in] parent Sets the upper class parent.
  //////////////////////////////////////////////////////////////////////////////
  GLWindow(QWidget *_parent);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Destructor.
  //////////////////////////////////////////////////////////////////////////////
  ~GLWindow();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Constant enumerator for the Rendering Mode.
  /////////////////////////////////////////////////////////////////////////////////
  enum RenderingMode
  {
    ADS  = 0,
    XRAY = 1,
    AO   = 2
  };

private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets up the shader pipeline with all the framebuffer object and
  /// textures needed for rendering all the types.
  //////////////////////////////////////////////////////////////////////////////
  void prepareSSAOPipeline();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets up a VAO that hold a vertex buffer for the quad and state
  /// needed to draw a full screen quad.
  //////////////////////////////////////////////////////////////////////////////
  void prepareQuad();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Internally calls the Scene::initialize() of the bound Scene.
  //////////////////////////////////////////////////////////////////////////////
  void initializeGL();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Internally calls the Scene::paint() of the bound Scene.
  //////////////////////////////////////////////////////////////////////////////
  void paintGL();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Updates the Scene's Viewport accordingly to the new Window size.
  //////////////////////////////////////////////////////////////////////////////
  void resizeGL(int _w, int _h);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Initialize all matricies.
  //////////////////////////////////////////////////////////////////////////////
  void initializeMatrices();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Method for all light calculations and sending light info to shader.
  //////////////////////////////////////////////////////////////////////////////
  void loadLightToShader();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Method for loading material defined in Material class to shader.
  //////////////////////////////////////////////////////////////////////////////
  void loadMaterialToShader();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Initializes particle system geometry.
  //////////////////////////////////////////////////////////////////////////////
  void prepareParticles();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Draws particles.
  //////////////////////////////////////////////////////////////////////////////
  void drawParticles();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Draws the links on top of everything
  //////////////////////////////////////////////////////////////////////////////
  void drawLinks();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Creates and initialises lights and adds them to the list of
  /// objects.
  //////////////////////////////////////////////////////////////////////////////
  void setupLights();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Generates OpenGL vertex data for a sphere of _num_subdivisions
  /// subdivisions. It will get stored in m_sphere_data.
  /// @param[in] _num_subdivisions Recursion level for icosahedra subdivision.
  //////////////////////////////////////////////////////////////////////////////
  void generateSphereData(uint _num_subdivisions);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Now it selects randomly a particle, it splits it and advances them.
  //////////////////////////////////////////////////////////////////////////////
  void updateParticleSystem();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sends updated particle information to OpenGL.
  //////////////////////////////////////////////////////////////////////////////
  void sendParticleDataToOpenGL();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Updates particle system model matrix.
  //////////////////////////////////////////////////////////////////////////////
  void updateModelMatrix();

private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Current rendering mode for the particle shading.
  //////////////////////////////////////////////////////////////////////////////
  RenderingMode m_rendering_mode;

  // ===========================================================================
  // Framebuffer Objects
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief FBO that will render to the world and view position and normals as
  /// well as the links by using the Geometry program and the Links program.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLFramebufferObject* m_gbuffer_fbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief FBO that will handle all the AO occlusion by using the view
  /// position texture, view normal texture and noise texture. It will render to
  /// the occlusion texture as a result.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLFramebufferObject* m_ssao_fbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief FBO that takes as input the occlusion texture generated by the SSAO
  /// FBO and blurs it. The output goes to the blurred occlusion texture.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLFramebufferObject* m_blur_fbo;

  // ===========================================================================
  // Textures
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the geometry position color pass in world space coordinates.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_world_position_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the geometry position color pass in world view coordinates.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_view_position_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the geometry normal color pass in world space coordinates.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_world_normal_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the geometry normal color pass in view space coordinates.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_view_normal_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds a single-channel occlusion factor result of the SSAO shader.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_occlusion_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the single-channel blurred SSAO factor.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_blurred_occlusion_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the links that interconnect the particles.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_links_texture;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief A manually created texture initialized from random values.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLTexture* m_noise_texture;

  // ===========================================================================
  // Shader programs
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handles the instanced geometry rendering to the gBuffer.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram* m_geom_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief In charge of generating the occlusion factor.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram* m_ssao_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Blurs the occlusion pass from the SSAO shader.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram* m_blur_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Responsible for the final composite.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram* m_lighting_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle links shader program.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram *m_links_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Manipulator shader program.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram *m_manipulator_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Light representation shader program.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLShaderProgram *m_sun_program;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the sampling directions for the SSAO shader.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<QVector3D> m_ssao_kernel;

  // ===========================================================================
  // Uniforms and shader routine indices
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Float for ambient colour variable.
  //////////////////////////////////////////////////////////////////////////////
  float m_ambient = 1.0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Float for specular colour variable.
  //////////////////////////////////////////////////////////////////////////////
  float m_specular = 1.0;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief vec3 to store light position.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_lightPos;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief View position to store camera position.
  //////////////////////////////////////////////////////////////////////////////
  QVector3D m_viewPos;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Model matrix for the particle system.
  //////////////////////////////////////////////////////////////////////////////
  QMatrix4x4 m_model_matrix;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief View matrix for the particle system.
  //////////////////////////////////////////////////////////////////////////////
  QMatrix4x4 m_view_matrix;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Projection matrix for the particle system.
  //////////////////////////////////////////////////////////////////////////////
  QMatrix4x4 m_projection_matrix;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Index for the RenderPass subroutine to be called.
  //////////////////////////////////////////////////////////////////////////////
  GLuint m_activeRenderPassIndex;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Index for RenderPass subroutine that draws world-space positions.
  //////////////////////////////////////////////////////////////////////////////
  GLuint m_AOIndex;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Index for RenderPass subroutine that draws depth.
  //////////////////////////////////////////////////////////////////////////////
  GLuint m_XRayIndex;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Index for RenderPass subroutine that draws diffuse.
  //////////////////////////////////////////////////////////////////////////////
  GLuint m_ADSIndex;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Renders just the normals.
  //////////////////////////////////////////////////////////////////////////////
  GLuint m_normalIndex;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Toggled with key L. If true will render the links between
  /// particles.
  //////////////////////////////////////////////////////////////////////////////
  bool m_draw_links;

  // ===========================================================================
  // ParticleSystem related parameters
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Bool toggled by pressing 'Light On/Off buttons'.
  //////////////////////////////////////////////////////////////////////////////
  bool m_lightON = false;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Particle system associated with the scene.
  //////////////////////////////////////////////////////////////////////////////
  ParticleSystem m_ps;

  // ===========================================================================
  // Vertex Array Objects and Buffers
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief VAO that will store the state for drawing the quad.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLVertexArrayObject *m_quad_vao;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief VAO that will store the state for drawing the particles.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLVertexArrayObject *m_part_vao;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief VAO that will store state for drawing particle links.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLVertexArrayObject *m_links_vao;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief VBO buffer that stores the point data for the quad.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer m_quad_vbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief VBO buffer that stores the point data for the particles.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer m_part_vbo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief EBO that stores the indices of the particles to draw links for.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer m_links_ebo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Buffer object holding the the vertex data of the sphere on GPU.
  //////////////////////////////////////////////////////////////////////////////
  QOpenGLBuffer m_sphere_vbo;

  // ===========================================================================
  // Vertex data to send to OpenGL
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief All the particle positions and radius ready for OpenGL to be drawn
  /// as instance position of the geometry defined in m_sphere_data.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<GLfloat> m_particle_data;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Hold the indices of the particles for which we will be drawing
  /// lines between them. In other words, this draws the links where one exists.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<uint> m_links_data;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Vertex data of the shape that will define an individual particle.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<GLfloat> m_sphere_data;

  // ===========================================================================
  // Miscellaneous
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Manager processing inputs and handling the camera.
  //////////////////////////////////////////////////////////////////////////////
  InputManager* m_input_manager;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief List of all scene objects. Not including particles.
  //////////////////////////////////////////////////////////////////////////////
  std::vector<std::shared_ptr<SelectableObject>> m_object_list;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Holds the functoins for cube map texture that will be used for IBL.
  //////////////////////////////////////////////////////////////////////////////
  SkyBox *m_skybox;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief QTimer clocks schedule updates for repainting the scene
  //////////////////////////////////////////////////////////////////////////////
  QTimer m_timer;

  // ===========================================================================
  // Event handlers
  // ===========================================================================
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handle the key pressing events.
  /// @param[in] ev Event.
  //////////////////////////////////////////////////////////////////////////////
  void keyPressEvent(QKeyEvent * ev);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handle the key pressing events.
  /// @param[in] key Event.
  //////////////////////////////////////////////////////////////////////////////
  void keyReleaseEvent(QKeyEvent *key);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handle the mouse moving events.
  /// @param[in] event Event.
  //////////////////////////////////////////////////////////////////////////////
  void mouseMoveEvent(QMouseEvent *event);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handle the mouse pressing events.
  /// @param[in] event Event.
  //////////////////////////////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handle the mouse release events.
  /// @param[in] event Event.
  //////////////////////////////////////////////////////////////////////////////
  void mouseReleaseEvent(QMouseEvent *event);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Handle the mouse wheel events
  /// @param[in] event Event.
  //////////////////////////////////////////////////////////////////////////////
  void wheelEvent(QWheelEvent *event);

public slots:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for changing particle size.
  /// @param[in] _size Size of particle.
  //////////////////////////////////////////////////////////////////////////////
  void setParticleSize(double _size);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for changing particle type.
  /// @param[in] _type Type of particle.
  //////////////////////////////////////////////////////////////////////////////
  void setParticleType(int _type);

  //not implemented yet
  void cancel();

  /////////////////////////////////////////////////////////////////////////////
  /// @brief showConnections  changes state of link visibility by change
  /// m_draw_links
  /// @param[in] _state state to set link rendering to
  /////////////////////////////////////////////////////////////////////////////
  void showConnections(bool _state);

  /////////////////////////////////////////////////////////////////////////////
  /// @brief setShading sets the active render pass index
  /// @param[in] _type type of shading
  /////////////////////////////////////////////////////////////////////////////
  void setShading(QString _type);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for changing state of forces.
  /// @param[in] _state State of forces
  //////////////////////////////////////////////////////////////////////////////
  void toggleForces(bool _state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for choosing for particles to die.
  /// @param[in] _state State of particle death
  //////////////////////////////////////////////////////////////////////////////
  void toggleParticleDeath(bool _state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for changing cohesion amount only valid for Linked Particles.
  /// @param[in] _amount Amount of cohesion.
  //////////////////////////////////////////////////////////////////////////////
  void setCohesion(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot that triggers bulge.
  //////////////////////////////////////////////////////////////////////////////
  void bulge();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot that triggers light on.
  //////////////////////////////////////////////////////////////////////////////
  void lightOn();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot that triggers light off.
  //////////////////////////////////////////////////////////////////////////////
  void lightOff();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for changing local cohesion amount only vallied for Linked Particles.
  /// @param[in] _amount Amount of local cohesion.
  //////////////////////////////////////////////////////////////////////////////
  void setLocalCohesion(int _amount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for the length of the branches in Growth Particles.
  /// @param[in] _amount length of branch.
  //////////////////////////////////////////////////////////////////////////////
  void setBranchLength(double _amount);

  //not implemented yet
  void setGrowthRadius(int _amount);

  //not implemented yet
  void restart();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot specifying how you want the particles to split.
  /// @param[in] _type Type of split.
  //////////////////////////////////////////////////////////////////////////////
  void setSplitType(int _type);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Slot for changing amount of children per GrowthParticle.
  /// @param[in] _amount amount of children.
  //////////////////////////////////////////////////////////////////////////////
  void setChildThreshold(int _amount);

signals:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emit when particle type needs to be reset
  /// @param[in] indexNumber index number that selects the right Particle.
  //////////////////////////////////////////////////////////////////////////////
  void resetParticleType(int indexNumber);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emit when split type needs to be reset
  /// @param[in] indexNumber index number that selects the right split type.
  //////////////////////////////////////////////////////////////////////////////
  void resetSplitType(int indexNumber);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when particle size needs to be reset
  /// @param[in] value size of particle.
  //////////////////////////////////////////////////////////////////////////////
  void resetParticleSize(double value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when particle tap needs to be reset
  /// @param[in] index index of tap.
  //////////////////////////////////////////////////////////////////////////////
  void resetParticleTap(int index);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when forces needs to be reset
  /// @param[in] state state that forces is set to.
  //////////////////////////////////////////////////////////////////////////////
  void resetForces(bool state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Signal emits when particle death needs to be reset
  /// @param[in] state the particle death is set to
  //////////////////////////////////////////////////////////////////////////////
  void resetParticleDeath (bool state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when cohesion value needs to be reset
  /// @param[in] value cohesion amount.
  //////////////////////////////////////////////////////////////////////////////
  void resetCohesion(int value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when localCohesion value needs to be reset
  /// @param[in] value localCohesion amount.
  //////////////////////////////////////////////////////////////////////////////
  void resetLocalCohesion(int value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Signal emits when child threshold for growth particle needs to be
  /// reset.
  /// @param[in] value Child threshold value.
  //////////////////////////////////////////////////////////////////////////////
  void resetChildrenThreshold(int value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when branch Length needs to be reset.
  /// @param[in] value length of branches.
  //////////////////////////////////////////////////////////////////////////////
  void resetBranchLength(double value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when particles tap needs to be enabled/disabled.
  /// @param[in] value state that tap is set to.
  //////////////////////////////////////////////////////////////////////////////
  void enableGrowthParticle(bool value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when particles tap needs to be enabled/disabled.
  /// @param[in] value state that tap is set to.
  //////////////////////////////////////////////////////////////////////////////
  void enableLinkedParticle(bool value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief setShadingType Signal emits when shading type is changed
  /// @param[in] index
  //////////////////////////////////////////////////////////////////////////////
  void changedShadingType(int index);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief setConnectionState
  /// @param[in] state state of the connections
  //////////////////////////////////////////////////////////////////////////////
  void setConnectionState(bool state);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Signal emits when bulge needs to be enabled/disabled.
  /// @param[in] value state that tap is set to
  //////////////////////////////////////////////////////////////////////////////
  void enableBulge(bool value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Signal emits when light on needs to be disabled/enabled
  /// @param[in] value state that tap is set to.
  //////////////////////////////////////////////////////////////////////////////
  void enableLightOn(bool value);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Singal emits when light off needs to be enabled/disabled.
  /// @param[in] value state that tap is set to.
  //////////////////////////////////////////////////////////////////////////////
  void enableLightOff(bool value);
  void enableSplitType(bool value);
};

#endif // GLWINDOW_H

