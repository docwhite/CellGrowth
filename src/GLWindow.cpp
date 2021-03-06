////////////////////////////////////////////////////////////////////////////////
/// @file GLWindow.cpp
/// @author Ramon Blanquer
/// @author Fanny Marstrom
/// @author Carola Gille
/// @author Esme Prior
/// @author Lydia Kenton
/// @version 0.0.1
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
// Qt
#include <QKeyEvent>
#include <iostream>
#include <string>

// Project
#include "GLWindow.h"
#include "SkyBox.h"

// helpers
void subdivide(float*, float*, float*, long, std::vector<GLfloat>&);
float lerp(float a, float b, float f);

GLWindow::GLWindow(QWidget*_parent) : QOpenGLWidget(_parent)
{
  QSurfaceFormat fmt;
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setMajorVersion(4);
  fmt.setMinorVersion(1);
  fmt.setSamples(16);
  fmt.setSwapInterval(1);
  setFormat(fmt);

  setMouseTracking(true);
  setFocus();

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
  if(format().swapInterval() == -1)
  {
    // V_blank synchronization not available (tearing likely to happen)
    qDebug("Swap Buffers at v_blank not available: refresh at approx 60fps.");
    m_timer.setInterval(17);
  }
  else
  {
    // V_blank synchronization available
    m_timer.setInterval(0);
  }
  m_timer.start();
  m_draw_links = true;
}

GLWindow::~GLWindow()
{
  cleanup();
}

void GLWindow::cleanup()
{
  qDebug("Cleaning up...");

  // Destroy textures
  m_view_position_texture->destroy();
  m_world_position_texture->destroy();
  m_view_normal_texture->destroy();
  m_world_normal_texture->destroy();
  m_occlusion_texture->destroy();
  m_blurred_occlusion_texture->destroy();
  m_noise_texture->destroy();

  // Deallocate textures
  delete m_view_position_texture;
  delete m_world_position_texture;
  delete m_view_normal_texture;
  delete m_world_normal_texture;
  delete m_occlusion_texture;
  delete m_blurred_occlusion_texture;
  delete m_noise_texture;

  // Deallocate framebuffer objects
  delete m_gbuffer_fbo;
  delete m_ssao_fbo;
  delete m_blur_fbo;

}

void GLWindow::prepareSSAOPipeline()
{
  //////////////////////////////////////////////////////////////////////////////
  /// Framebuffer textures initialization
  //////////////////////////////////////////////////////////////////////////////
  qDebug("Setting texture sizes: %dx%d", width(), height());

  m_view_position_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_view_position_texture->setSize(width(), height());
  m_view_position_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_view_position_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_view_position_texture->setFormat(QOpenGLTexture::RGB32F);
  m_view_position_texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);

  m_world_position_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_world_position_texture->setSize(width(), height());
  m_world_position_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_world_position_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_world_position_texture->setFormat(QOpenGLTexture::RGB32F);
  m_world_position_texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);

  m_view_normal_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_view_normal_texture->setSize(width(), height());
  m_view_normal_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_view_normal_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_view_normal_texture->setFormat(QOpenGLTexture::RGB32F);
  m_view_normal_texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);

  m_world_normal_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_world_normal_texture->setSize(width(), height());
  m_world_normal_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_world_normal_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_world_normal_texture->setFormat(QOpenGLTexture::RGB32F);
  m_world_normal_texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);

  m_occlusion_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_occlusion_texture->setSize(width(), height());
  m_occlusion_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_occlusion_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_occlusion_texture->setFormat(QOpenGLTexture::RGB16_UNorm);
  m_occlusion_texture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float16);

  m_blurred_occlusion_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_blurred_occlusion_texture->setSize(width(), height());
  m_blurred_occlusion_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_blurred_occlusion_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_blurred_occlusion_texture->setFormat(QOpenGLTexture::RGB16_UNorm);
  m_blurred_occlusion_texture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float16);

  //////////////////////////////////////////////////////////////////////////////
  /// gBuffer FBO preparation
  /////////////////////////////////////////////////////////////////////////////
  m_gbuffer_fbo = new QOpenGLFramebufferObject(width(), height()); // GL_COLOR_ATTACHMENT0
  m_gbuffer_fbo->bind();
  m_gbuffer_fbo->addColorAttachment(width(), height(), GL_RGB);    // GL_COLOR_ATTACHMENT1

  glBindTexture(GL_TEXTURE_2D, m_world_position_texture->textureId());
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_world_position_texture->textureId(), 0);
  glBindTexture(GL_TEXTURE_2D, m_view_position_texture->textureId());
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_view_position_texture->textureId(), 0);
  glBindTexture(GL_TEXTURE_2D, m_world_normal_texture->textureId());
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_world_normal_texture->textureId(), 0);
  glBindTexture(GL_TEXTURE_2D, m_view_normal_texture->textureId());
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_view_normal_texture->textureId(), 0);
  const GLenum gbuffer_attachments[4] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3
  };
  glDrawBuffers(4, gbuffer_attachments);

  // Create and attach depth buffer (renderbuffer) =============================
  GLuint rbo_depth;

  // Generate renderbuffer object names
  glGenRenderbuffers(1, &rbo_depth);

  // Bind a named renderbuffer object
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);

  // Establish data storage, fromat and dimensions of a renderbuffer object's image
  glRenderbufferStorage(
    GL_RENDERBUFFER,     // Target
    GL_DEPTH_COMPONENT,  // Internal Format
    width(), height()    // Dimensions
  );

  // Attach a renderbuffer object to a framebuffer object
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER,       // Target
    GL_DEPTH_ATTACHMENT,  // Attachment
    GL_RENDERBUFFER,      // Renderbuffer Target
    rbo_depth             // Renderbuffer
  );

  // Finally check if framebuffer object is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    qCritical("gBuffer FBO not complete!");
  m_gbuffer_fbo->release();

  //////////////////////////////////////////////////////////////////////////////
  /// SSAO FBO preparation
  //////////////////////////////////////////////////////////////////////////////
  m_ssao_fbo = new QOpenGLFramebufferObject(width(), height());
  m_ssao_fbo->bind();

  qDebug("Occlusion texture ID: %d", m_occlusion_texture->textureId());
  glBindTexture(GL_TEXTURE_2D, m_occlusion_texture->textureId());
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_occlusion_texture->textureId(), 0);

  const GLenum ssao_attachments[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, ssao_attachments);

  // Finally check if framebuffer object is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    qCritical("SSAO FBO not complete");

  m_ssao_fbo->release();

  //////////////////////////////////////////////////////////////////////////////
  /// Blur FBO preparetion
  //////////////////////////////////////////////////////////////////////////////
  m_blur_fbo = new QOpenGLFramebufferObject(width(), height());
  m_blur_fbo->bind();

  qDebug("Blurred occlusion texture ID: %d", m_blurred_occlusion_texture->textureId());
  glBindTexture(GL_TEXTURE_2D, m_blurred_occlusion_texture->textureId());
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_blurred_occlusion_texture->textureId(), 0);

  const GLenum blur_attachments[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, blur_attachments);

  // Finally check if framebuffer object is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    qCritical("Blur FBO not complete");

  m_blur_fbo->release();

  //////////////////////////////////////////////////////////////////////////////
  /// SSAO kernel preparation
  //////////////////////////////////////////////////////////////////////////////
  std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
  std::default_random_engine generator;
  m_ssao_kernel.clear();
  for (size_t i = 0; i < 64; ++i)
  {
    QVector3D sample(
      randomFloats(generator) * 2.0 - 1.0,
      randomFloats(generator) * 2.0 - 1.0,
      randomFloats(generator)
    );

    sample.normalize();
    sample *= randomFloats(generator);
    float scale = (float)i/64.0f;
    scale = lerp(0.1f, 1.0f, scale*scale);
    sample *= scale;
    m_ssao_kernel.push_back(sample);
  }

  //////////////////////////////////////////////////////////////////////////////
  /// Noise texture generation
  //////////////////////////////////////////////////////////////////////////////
  std::vector<QVector3D> ssaoNoise;
  for (size_t i = 0; i < 16; i++)
  {
    QVector3D noise(
      randomFloats(generator) * 2.0 - 1.0,
      randomFloats(generator) * 2.0 - 1.0,
      0.0
    );

    ssaoNoise.push_back(noise);
  }

  m_noise_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  m_noise_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_noise_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  m_noise_texture->setWrapMode(QOpenGLTexture::Repeat);
  m_noise_texture->setSize(4,4);
  m_noise_texture->setFormat(QOpenGLTexture::RGB16F);
  m_noise_texture->allocateStorage();
  m_noise_texture->setData(0, QOpenGLTexture::RGB, QOpenGLTexture::Float16, &ssaoNoise[0]);

  //////////////////////////////////////////////////////////////////////////////
  /// Shader configuration
  //////////////////////////////////////////////////////////////////////////////
  // === SSAO ===
  m_ssao_program->bind();

    // Texture unit to use
    m_ssao_program->setUniformValue("tViewPosition" , 0);
    m_ssao_program->setUniformValue("tViewNormal"   , 1);
    m_ssao_program->setUniformValue("tTexNoise"     , 2);


  m_ssao_program->release();

  // === BLUR ===
  m_blur_program->bind();

    // Texture unit to use
    m_blur_program->setUniformValue("tInputSSAO", 0);

  m_blur_program->release();

  // === LIGHTING ===
  m_lighting_program->bind();

    // Texture unit to use
    m_lighting_program->setUniformValue("tWorldPosition" , 0);
    m_lighting_program->setUniformValue("tWorldNormal"   , 1);
    m_lighting_program->setUniformValue("tSSAO"          , 2);
    m_lighting_program->setUniformValue("tSkybox"        , 3);

    // Uniforms
    m_lighting_program->setUniformValue("drawLinks", true);
    m_lighting_program->setUniformValue("CameraPos", m_input_manager->getCameraPosition());


    // Subroutine ShadingPass Index.
    m_ADSIndex  = glGetSubroutineIndex(m_lighting_program->programId(), GL_FRAGMENT_SHADER, "ADSRender");
    m_XRayIndex = glGetSubroutineIndex(m_lighting_program->programId(), GL_FRAGMENT_SHADER, "XRayRender");
    m_AOIndex   = glGetSubroutineIndex(m_lighting_program->programId(), GL_FRAGMENT_SHADER, "AORender");
    m_NewOrderIndex   = glGetSubroutineIndex(m_lighting_program->programId(), GL_FRAGMENT_SHADER, "NewOrderRender");


    // Setting the active renderpass.
    m_activeRenderPassIndex = m_ADSIndex;
    m_rendering_mode = GLWindow::ADS;

  m_lighting_program->release();

  // === SSAO ===
  m_ssao_program->bind();
    m_ssao_program->setUniformValue("width", width());
    m_ssao_program->setUniformValue("height", height());
  m_ssao_program->release();
}

void GLWindow::initializeGL()
{

  initializeOpenGLFunctions();

  m_input_manager = new InputManager(this);
  m_skybox = new SkyBox(m_input_manager);

  generateSphereData(4); // Four subdivisions of an icosahedra
  m_sphere_vbo.create();
  m_sphere_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

  initializeMatrices();
  setupLights();
  m_skybox->prepare(width(), height(), context()->versionFunctions<QOpenGLFunctions_4_1_Core>());

  m_geom_program = new QOpenGLShaderProgram;
  m_geom_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shader/geom.vert");
  m_geom_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/geom.frag");
  m_geom_program->link();

  m_ssao_program = new QOpenGLShaderProgram;
  m_ssao_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/ssao.vert");
  m_ssao_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/ssao.frag");
  m_ssao_program->link();

  m_blur_program = new QOpenGLShaderProgram;
  m_blur_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/ssao.vert");
  m_blur_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/blur.frag");
  m_blur_program->link();

  m_lighting_program = new QOpenGLShaderProgram;
  m_lighting_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/ssao.vert");
  m_lighting_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/lighting.frag");
  m_lighting_program->link();

  prepareQuad();
  prepareParticles();
  prepareSSAOPipeline();

  glViewport(0, 0, width(), height());

  m_input_manager->setupCamera(45.0f, width(), height(), 0.1, 1000.f);

  //Initializing uniforms send to shader.
  m_ssaoRadius = 5.0;
  m_ssaoBias = 0.025;

  setRcolourMaterial(127);
  setGcolourMaterial(127);
  setBcolourMaterial(127);

  setRcolour(246);
  setGcolour(255);
  setBcolour(171);

  setAmbientLightR(100);
  setAmbientLightG(50);
  setAmbientLightB(0);

  setSpecularLightR(255);
  setSpecularLightG(10);
  setSpecularLightB(10);

  setAmbientLightR(100);
  setAmbientLightG(50);
  setAmbientLightB(0);

  setFillLight(10);

  m_ssao_program->bind();
    m_ssao_program->setUniformValue("Radius", m_ssaoRadius);
    m_ssao_program->setUniformValue("Bias", m_ssaoBias);
  m_ssao_program->release();
}


void GLWindow::paintGL()
{
  updateModelMatrix();

  m_input_manager->loadLightMatricesToShader();
  m_input_manager->doMovement(-m_ps.calculateParticleCentre());

  //////////////////////////////////////////////////////////////////////////////
  /// gBuffer: Geometry pass
  //////////////////////////////////////////////////////////////////////////////
  m_gbuffer_fbo->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (m_rendering_mode) {
    case GLWindow::XRAY:
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
      glBlendEquation(GL_FUNC_ADD);
      glEnable(GL_CULL_FACE);
      drawParticles();
      glDisable(GL_CULL_FACE);
      glDisable(GL_BLEND);

      break;

    default:
      drawParticles();
      break;
    }
  m_gbuffer_fbo->release();

  //////////////////////////////////////////////////////////////////////////////
  /// SSAO: Generate SSAO texture
  //////////////////////////////////////////////////////////////////////////////
  m_ssao_fbo->bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_ssao_program->bind();
    for (size_t i = 0; i < 64; ++i)
    {
      std::string s = "samples[" + std::to_string(i) + "]";
      m_ssao_program->setUniformValue(s.c_str(), m_ssao_kernel[i]);
    }
    m_ssao_program->setUniformValue("ProjectionMatrix", m_input_manager->getProjectionMatrix());
    m_view_position_texture->bind(0);
    m_view_normal_texture->bind(1);
    m_noise_texture->bind(2);
    m_quad_vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_quad_vao->release();
    m_ssao_program->release();
  m_ssao_fbo->release();

  //////////////////////////////////////////////////////////////////////////////
  /// Blur: Blur SSAO texture
  //////////////////////////////////////////////////////////////////////////////
  m_blur_fbo->bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_blur_program->bind();
    m_occlusion_texture->bind(0);
    m_quad_vao->bind();
      glDrawArrays(GL_TRIANGLES, 0, 6);
    m_quad_vao->release();
    m_blur_program->release();
  m_blur_fbo->release();

  //////////////////////////////////////////////////////////////////////////////
  /// Default FBO: lighting shader
  //////////////////////////////////////////////////////////////////////////////
  loadMaterialToShader();
  loadLightToShader();

  //Default colour. Dark grey.
  glClearColor(0.25, 0.25, 0.25, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // === Sky ===
  switch (m_rendering_mode) {
  case GLWindow::ADS:
    glClearColor(0, 0, 0, 0);
    glDepthMask(GL_FALSE);
    m_skybox->draw(context()->versionFunctions<QOpenGLFunctions_4_1_Core>());
    glDepthMask(GL_TRUE);
    break;

  //Ambient Occlusion render pass.
  case GLWindow::AO:
    break;

  //New Order artstyle render.
  case GLWindow::newOrder:
    break;

  default:
    break;
  }

  //////////////////////////////////////////////////////////////////////////////
  /// Quad
  //////////////////////////////////////////////////////////////////////////////
  m_lighting_program->bind();
  m_world_position_texture->bind(0);
  m_world_normal_texture->bind(1);
  m_blurred_occlusion_texture->bind(2);
  m_skybox->getCubeMapTexture()->bind(3);


  m_quad_vao->bind();
    m_lighting_program->setUniformValue("ModelMatrix", m_model_matrix);
    m_lighting_program->setUniformValue("ViewMatrix", m_input_manager->getViewMatrix());
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_activeRenderPassIndex);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);


  m_quad_vao->release();
  m_lighting_program->release();



  //////////////////////////////////////////////////////////////////////////////
  /// Manipulators and Lights
  //////////////////////////////////////////////////////////////////////////////
  // Flush the depth values
  glClear(GL_DEPTH_BUFFER_BIT);
  // Don't draw color, just depth
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  // Enable depth testing so manipulators / particles are tested
  glEnable(GL_DEPTH_TEST);
  // Draw the particles depth values
  drawParticles();
  // Enable back colour so we can paint manipulators
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  // Draw manipulators

  for(auto &s : m_object_list) { s->draw(); }

  if (m_draw_links) drawLinks();

  // Bring it back to previous state
  glDisable(GL_DEPTH_TEST);

  updateParticleSystem();
}

void GLWindow::resizeGL(int _w, int _h)
{
  qDebug("Window resized to %dx%d", _w, _h);
  m_input_manager->resized(_w, _h);
  m_input_manager->setupCamera(45.0f, width(), height(), 0.1f, 1000.0f);
  cleanup();
  prepareSSAOPipeline();
  m_ssao_program->bind();
  m_ssao_program->setUniformValue("width", _w);
  m_ssao_program->setUniformValue("height", _h);
  m_ssao_program->release();
  glViewport(0, 0, width(), height());

}

void GLWindow::initializeMatrices()
{
  m_model_matrix.setToIdentity();
  m_model_matrix.translate(0.0, 0.0, 0.0);
}

void GLWindow::loadLightToShader()
{
  m_lightPos = m_object_list[0]->getPosition();
  m_fillLightPos = m_object_list[1]->getPosition();
  m_ps.setLightPos(m_lightPos);
  m_ps.setLightPos(m_fillLightPos);

  //Setting light struct values.
  m_lighting_program->bind();
    m_lighting_program->setUniformValue("light.position", m_lightPos);
    m_lighting_program->setUniformValue("light.ambient", QVector3D(m_lightAmbientR, m_lightAmbientG, m_lightAmbientB));
    m_lighting_program->setUniformValue("light.diffuse", QVector3D(m_lightDiffuseR, m_lightDiffuseG, m_lightDiffuseB));
    m_lighting_program->setUniformValue("light.specular", QVector3D(m_lightSpecularR, m_lightSpecularG, m_lightSpecularB));
    m_lighting_program->setUniformValue("light.Linear", 0.09f);
    m_lighting_program->setUniformValue("light.Quadratic", 0.032f);

    m_lighting_program->setUniformValue("fillLight.position", m_fillLightPos);
    m_lighting_program->setUniformValue("fillLight.ambient", QVector3D(m_fillLight, m_fillLight, m_fillLight));
    m_lighting_program->setUniformValue("fillLight.diffuse", QVector3D(m_fillLight, m_fillLight, m_fillLight));
    m_lighting_program->setUniformValue("fillLight.specular", QVector3D(m_fillLight, m_fillLight, m_fillLight));
  m_lighting_program->release();
}

void GLWindow::loadMaterialToShader()
{
    //Setting material struct values.
  m_lighting_program->bind();
    m_lighting_program->setUniformValue("material.ambient", QVector3D(m_materialR*0.5, m_materialG*0.5, m_materialB*0.5));
    m_lighting_program->setUniformValue("material.diffuse", QVector3D(m_materialR, m_materialG, m_materialB));
    m_lighting_program->setUniformValue("material.specular", QVector3D(0.5f, 0.5f, 0.5f));
    m_lighting_program->setUniformValue("material.shininess", 32.0f);
    m_lighting_program->setUniformValue("material.attenuation", 0.5f);
  m_lighting_program->release();
}

void GLWindow::prepareQuad()
{
  static const float quad[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
  };

  m_quad_vao = new QOpenGLVertexArrayObject(this);

  m_quad_vao->create();
  m_quad_vbo.create();
  m_quad_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

  m_quad_vao->bind();
  m_quad_vbo.bind();

  m_quad_vbo.allocate(quad, 30 * sizeof(GLfloat));

  m_lighting_program->setAttributeBuffer("position", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
  m_lighting_program->enableAttributeArray("position");

  m_lighting_program->setAttributeBuffer("uv", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
  m_lighting_program->enableAttributeArray("uv");

  m_quad_vbo.release();
  m_quad_vao->release();
}

void GLWindow::prepareParticles()
{
  m_links_program = new QOpenGLShaderProgram(this);
  m_links_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/links.vert");
  m_links_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/links.frag");
  m_links_program->link();

  m_part_vao = new QOpenGLVertexArrayObject(this);
  m_part_vao->create();

  m_links_vao = new QOpenGLVertexArrayObject(this);
  m_links_vao->create();

  m_part_vbo.create();
  m_part_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);

  m_links_ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  m_links_ebo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
  m_links_ebo.create();

  // Initial batch
  sendParticleDataToOpenGL();
}

void GLWindow::drawParticles()
{
  m_geom_program->bind();
  m_geom_program->setUniformValue("ModelMatrix", m_model_matrix);
  m_geom_program->setUniformValue("ViewMatrix", m_input_manager->getViewMatrix());
  m_geom_program->setUniformValue("ProjectionMatrix", m_input_manager->getProjectionMatrix());
  m_part_vao->bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_sphere_data.size() / 3, m_ps.getSize());
  m_part_vao->release();
  m_geom_program->release();
}

void GLWindow::drawLinks()
{
  m_links_program->bind();
  m_links_program->setUniformValue("ProjectionMatrix", m_input_manager->getProjectionMatrix());
  m_links_program->setUniformValue("ModelMatrix", m_model_matrix);
  m_links_program->setUniformValue("ViewMatrix", m_input_manager->getViewMatrix());
  m_links_vao->bind();
    glDrawElements(GL_LINES, m_links_data.size(), GL_UNSIGNED_INT, 0);
  m_links_vao->release();
  m_links_program->release();
}

void GLWindow::setupLights()
{
  m_manipulator_program = new QOpenGLShaderProgram(this);
  m_manipulator_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/manip.vert");
  m_manipulator_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/manip.frag");
  m_manipulator_program->link();

  m_sun_program = new QOpenGLShaderProgram(this);
  m_sun_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sun.vert");
  m_sun_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sun.frag");
  m_sun_program->link();

  QVector3D masterUniqueColour=QVector3D(0.0f, 100.0f, 0.0f);

  SpotLight *spotlight = new SpotLight(QVector3D(-4,0,0), m_manipulator_program, m_sun_program);
  spotlight->createGeometry(masterUniqueColour);
  m_object_list.push_back(std::move(std::unique_ptr<SpotLight>(spotlight)));

  PointLight *pointlight = new PointLight(QVector3D(4,0,0), m_manipulator_program, m_sun_program);
  pointlight->createGeometry(masterUniqueColour);
  m_object_list.push_back(std::move(std::unique_ptr<PointLight>(pointlight)));

  m_input_manager->addShaderProgram(m_manipulator_program);
  m_input_manager->addShaderProgram(m_sun_program);
  m_input_manager->setObjectList(m_object_list);
}

void GLWindow::generateSphereData(uint _num_subdivisions)
{
  if (_num_subdivisions < 1) {
    qWarning("Subdivision number must be greater than 0. Using 1 as default.");
    _num_subdivisions = 1;
  }

  m_sphere_data.clear();

  GLfloat X = 0.525731112119133606;
  GLfloat Z = 0.850650808352039932;

  GLfloat vdata[12][3] = {
     {-X, 0.0f, Z}, {X, 0.0f, Z}, {-X, 0.0f, -Z}, {X, 0.0f, -Z},
     {0.0f, Z, X}, {0.0f, Z, -X}, {0.0f, -Z, X}, {0.0f, -Z, -X},
     {Z, X, 0.0}, {-Z, X, 0.0f}, {Z, -X, 0.0f}, {-Z, -X, 0.0f}
  };

  GLuint tindices[20][3] = {
     {0,  4,  1}, {0, 9,  4}, {9,  5, 4}, { 4, 5, 8}, {4, 8,  1},
     {8, 10,  1}, {8, 3, 10}, {5,  3, 8}, { 5, 2, 3}, {2, 7,  3},
     {7, 10,  3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1,  6},
     {6,  1, 10}, {9, 0, 11}, {9, 11, 2}, { 9, 2, 5}, {7, 2, 11}};

  for (size_t i = 0; i < 20; i++) {
    subdivide(
      vdata[tindices[i][0]],
      vdata[tindices[i][1]],
      vdata[tindices[i][2]],
      _num_subdivisions,
      m_sphere_data);
  }
}

void GLWindow::updateParticleSystem()
{
  m_ps.setLightPos(m_lightPos);
  m_ps.setLightPos(m_fillLightPos);
  if(m_lightON == true)
  {
    m_ps.splitRandomParticle();
    qInfo("%d", m_ps.getSize());
    qDebug("%d particles in the system", m_ps.getSize());
  }

  m_ps.advance();
  sendParticleDataToOpenGL();
}

void GLWindow::sendParticleDataToOpenGL()
{
  // Tell particle system to populate us a flattened float array for OpenGL
  m_ps.packageDataForDrawing(m_particle_data);

  // Uncomment to see what x, y, z, radius get sent to the shader
  // for_each(m_particle_data.begin(), m_particle_data.end(), [](float f){ qDebug("%f", f);});

  m_part_vao->bind();

    // Sphere Data =============================================================
    m_sphere_vbo.bind();
    m_sphere_vbo.allocate(&m_sphere_data[0], m_sphere_data.size() * sizeof(GLfloat));
    m_geom_program->enableAttributeArray("position");
    m_geom_program->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    // Instance Data ===========================================================
    m_part_vbo.bind();
    m_part_vbo.allocate(&m_particle_data[0], m_ps.getSize() * 4 * sizeof(GLfloat));

    m_geom_program->enableAttributeArray("instances");
    m_geom_program->setAttributeBuffer("instances", GL_FLOAT, 0, 4);

    m_part_vbo.release();
    glVertexAttribDivisor(m_geom_program->attributeLocation("instances"), 1);

  m_part_vao->release();

  // Link Data (on request) =========================================23===========
  if (m_draw_links)
  {
    // Work on the links
    m_ps.getLinksForDraw(m_links_data);

    // Uncomment to see what indices are being sent to ebo
    // for_each(m_links_data.begin(), m_links_data.end(), [](uint i){ qDebug("%d", i);});

    m_links_vao->bind();
      m_part_vbo.bind();
      m_links_ebo.bind();
      m_links_ebo.allocate(&m_links_data[0], m_links_data.size() * sizeof(uint));
      m_links_program->enableAttributeArray("position");
      m_links_program->setAttributeBuffer("position", GL_FLOAT, 0, 3, 4 * sizeof(GLfloat));
    m_links_vao->release();
  }
}

void GLWindow::updateModelMatrix()
{
  // Insert particle system position here
  m_model_matrix.setToIdentity();
  m_model_matrix.scale(1);
}

void GLWindow::keyPressEvent(QKeyEvent* ev)
{
  setFocus();
  switch(ev->key())
  {
    case Qt::Key_Space:
      m_ps.splitRandomParticle();
      qInfo("%d", m_ps.getSize());
      qDebug("%d particles in the system", m_ps.getSize());
      break;

    case Qt::Key_1:
      m_activeRenderPassIndex = m_ADSIndex;
      m_rendering_mode = GLWindow::ADS;
      emit changedShadingType(0);
      emit setConnectionState(true);
      qDebug("ADS Render.");
      break;

    case Qt::Key_2:
      m_activeRenderPassIndex = m_XRayIndex;
      m_rendering_mode = GLWindow::XRAY;

      emit changedShadingType(1);
      emit setConnectionState(true);
      qDebug("X-Ray visualisation.");
      break;

    case Qt::Key_3:
      m_activeRenderPassIndex = m_AOIndex;
      m_rendering_mode = GLWindow::AO;
      emit changedShadingType(2);
      emit setConnectionState(false);
      qDebug("Ambient Occlusion.");
      break;

  case Qt::Key_4:
    m_activeRenderPassIndex = m_NewOrderIndex;
    m_rendering_mode = GLWindow::newOrder;
    emit changedShadingType(3);
    qDebug("New Order Artstyle.");
    break;

    case Qt::Key_B:
      bulge();
      break;

    default:
      break;
  }

  m_input_manager->keyPressEvent(ev);
}

void GLWindow::keyReleaseEvent(QKeyEvent *key)
{
  setFocus();
  m_input_manager->keyReleaseEvent(key);
}

void GLWindow::mouseMoveEvent(QMouseEvent* event)
{
  makeCurrent();
  setFocus();
  m_input_manager->mouseMoveEvent(event);

}

void GLWindow::mousePressEvent(QMouseEvent *event)
{
  makeCurrent();
  setFocus();
  m_input_manager->mousePressEvent(event);
}

void GLWindow::mouseReleaseEvent(QMouseEvent *event)
{
  makeCurrent();
  setFocus();
  m_input_manager->mouseReleaseEvent(event);

  qDebug("Light Position length: %f", m_lightPos.length());
  qDebug("Fill Light Position length: %f", m_fillLightPos.length());
}

void GLWindow::wheelEvent(QWheelEvent *event)
{
  makeCurrent();
  setFocus();
  m_input_manager->wheelEvent(event);
}

void GLWindow::setLightIconScale(int _value)
{
  float t = (float)_value / 100.0f;
  float iconScale = 0.02f * (1.0f - t) + 0.08f * t;
  m_input_manager->setLightIconScales(iconScale);
}

void GLWindow::setBackgroundBlurIterations(int _value)
{
  qDebug("aiaiai  %d", _value);
  m_skybox->setBlurIterations(_value);

}

void GLWindow::setBackgroundSkymap(int _index)
{
  switch (_index)
  {
  case 0:
    m_skybox->setBackground("badomen");
  case 1:
    m_skybox->setBackground("criminal-impact");
    break;
  case 2:
    m_skybox->setBackground("cwd");
    break;
  case 3:
    m_skybox->setBackground("drakeq");
    break;
  case 4:
    m_skybox->setBackground("forest");
    break;
  case 5:
    m_skybox->setBackground("mandaris");
    break;
  case 6:
    m_skybox->setBackground("misty");
    break;
  case 7:
    m_skybox->setBackground("mnight");
    break;
  }
}

// Slots
void GLWindow::setParticleSize(double _size)
{
  m_ps.setParticleSize(_size);
  sendParticleDataToOpenGL();
}

void GLWindow::setParticleType(int _type)
{
  m_ps.splitRandomParticle();

  emit resetForces(true);
  emit resetParticleDeath(false);
  emit resetCohesion(80);
  emit resetLocalCohesion(5);
  emit resetChildrenThreshold(3);
  emit resetBranchLength(1);
  emit resetRColour(255);
  emit resetGColour(255);
  emit resetBColour(255);
  emit resetRMaterialColor(127);
  emit resetGMaterialColor(127);
  emit resetBMaterialColor(127);
  emit resetAmbientLightR(255);
  emit resetAmbientLightG(255);
  emit resetAmbientLightB(255);
  emit resetSpecularLightR(127);
  emit resetSpecularLightG(127);
  emit resetSpecularLightB(127);
  emit resetFillLight(50);
  emit resetAORadius(5.0);
  emit resetAOBias(0.025);

  char particleType = 'L';
  if (_type == 0)
  {
    particleType = 'L'; //Linked particle

    emit enableGrowthParticle(false);
    emit enableLinkedParticle(true);
    emit enableAutomataParticle(false);
    emit enableSplitType(true);
    emit enableConnections(true);
    emit setConnectionState(false);
    setShading("ADS");
    emit changedShadingType(0);
    emit resetNearestParticle(true);
  }
  else if (_type == 1)
  {
    particleType = 'G'; //Growth particle

    emit enableGrowthParticle(true);
    emit enableLinkedParticle(false);
    emit enableAutomataParticle(false);
    emit enableSplitType(false);
    emit enableConnections(true);
    emit setConnectionState(true);
    showConnections(true);
    setShading("X Ray");
    emit changedShadingType(1);
    emit resetNearestParticle(false);
  }
  else if (_type == 2)
  {
    particleType = 'A'; //Automata particle

    emit enableGrowthParticle(false);
    emit enableLinkedParticle(false);
    emit enableAutomataParticle(true);
    emit enableSplitType(false);
    emit enableConnections(false);
    setShading("Ambient Occlusion");
    emit changedShadingType(2);
    showConnections(false);
  }
  m_ps.reset(particleType);
  sendParticleDataToOpenGL();
}

void GLWindow::showConnections(bool _state)
{
  m_draw_links=_state;
  m_lighting_program->bind();
  m_lighting_program->setUniformValue("drawLinks", _state);
  m_lighting_program->release();
  sendParticleDataToOpenGL();
}

void GLWindow::setShading(QString _type)
{
  if (_type=="ADS")
  {
    emit setConnectionState(true);
    m_activeRenderPassIndex = m_ADSIndex;
    m_rendering_mode = GLWindow::ADS;

  }
  else if(_type=="Ambient Occlusion")
  {
    emit setConnectionState(false);
    m_activeRenderPassIndex = m_AOIndex;
    m_rendering_mode = GLWindow::AO;

  }
  else if(_type=="X Ray")
  {
    m_activeRenderPassIndex = m_XRayIndex;
    m_rendering_mode = GLWindow::XRAY;
  }

  else if(_type=="New Order")
  {
    m_activeRenderPassIndex = m_NewOrderIndex;
    m_rendering_mode = GLWindow::newOrder;
  }
  sendParticleDataToOpenGL();
}

void GLWindow::toggleForces(bool _state)
{
  // Only for LinkedParticles
  m_ps.toggleForces(_state);
  sendParticleDataToOpenGL();
}

void GLWindow::toggleParticleDeath(bool _state)
{
  // Only for LinkedParticles
  m_ps.toggleParticleDeath(_state);
  sendParticleDataToOpenGL();

  if(_state==true)
  {
    emit enableBulge(false);
  }
  else if (_state==false)
  {
    emit enableBulge(true);
  }
}

void GLWindow::setSplitType(int _type)
{
  sendParticleDataToOpenGL();
  std::cout<<"splitType:"<<_type<<std::endl;

  if (_type==0) //LIGHT IS ON
  {
    m_lightAmbientR = 1.0;
    m_lightAmbientG = 1.0;
    m_lightAmbientB = 1.0;

    m_lightSpecularR = 1.0;
    m_lightSpecularG = 1.0;
    m_lightSpecularB = 1.0;

    m_lightON = false;
    emit enableLightOn(false);
    emit enableLightOff(false);
  }

  else if (_type==1) //LIGHT IS OFF
  {
    m_lightAmbientR = 0.0;
    m_lightAmbientG = 0.0;
    m_lightAmbientB = 0.0;

    m_lightSpecularR = 0.0;
    m_lightSpecularG = 0.0;
    m_lightSpecularB = 0.0;


    emit enableLightOn(true);
    emit enableLightOff(true);
  }
  sendParticleDataToOpenGL();
}

void GLWindow::setCohesion(int _amount)
{
  //Only for LinkedParticles
  m_ps.setCohesion(_amount);
  sendParticleDataToOpenGL();
}

void GLWindow::setSSAORadius(double _radius)
{
    m_ssaoRadius = (float) _radius;
    qDebug("SSAO rad: %f", m_ssaoRadius);

    m_ssao_program->bind();
      m_ssao_program->setUniformValue("Radius", m_ssaoRadius);
    m_ssao_program->release();

}

void GLWindow::setSSAOBias(double _bias)
{
    m_ssaoBias = (float) _bias;
    m_ssao_program->bind();
      m_ssao_program->setUniformValue("Bias", m_ssaoBias);
    m_ssao_program->release();
}


/*-----------------------------------------------------------
 * Setting RGB values for light and material. Converted from
 * RGB space 0-255 and to 0.0-1.0 RGB space.
------------------------------------------------------------*/
void GLWindow::setRcolour(int _rColour)
{
  m_lightDiffuseR = (float)_rColour/255.0f;
}

void GLWindow::setGcolour(int _gColour)
{
  m_lightDiffuseG = (float)_gColour/255.0f;
}


void GLWindow::setBcolour(int _bColour)
{
  m_lightDiffuseB = (float)_bColour/255.0f;
}

void GLWindow:: setAmbientLightR(int _red)
{
    m_lightAmbientR = (float) _red/255;
}

void GLWindow:: setAmbientLightG(int _green)
{
    m_lightAmbientG = (float) _green/255;
}


void GLWindow:: setAmbientLightB(int _blue)
{
    m_lightAmbientB = (float) _blue/255;
}

void GLWindow::setSpecularLightR(int _red)
{
    m_lightSpecularR = (float) _red/255;
}

void GLWindow::setSpecularLightG(int _green)
{
    m_lightSpecularG = (float) _green/255;
}

void GLWindow::setSpecularLightB(int _blue)
{
    m_lightSpecularB = (float) _blue/255;
}

void GLWindow::setFillLight(int _amount)
{
   m_fillLight = (float) _amount/100;
}

void GLWindow::setRcolourMaterial(int _rColour)
{
    m_materialR = (float)_rColour/255.0f;
}

void GLWindow::setGcolourMaterial(int _gColour)
{
    m_materialG = (float)_gColour/255.0f;
}


void GLWindow::setBcolourMaterial(int _bColour)
{
    m_materialB = (float)_bColour/255.0f;
}

/*---------------------------------------------
 * Setting RGB values ends here.
----------------------------------------------*/

void GLWindow::bulge()
{
  //Only for LinkedParticles
  m_ps.bulge();
  sendParticleDataToOpenGL();
}

void GLWindow::lightOn()
{
  //Only for LinkedParticles
  m_lightAmbientR = 1.0;
  m_lightAmbientG = 1.0;
  m_lightAmbientB = 1.0;

  m_lightSpecularR = 1.0;
  m_lightSpecularG = 1.0;
  m_lightSpecularB = 1.0;

  m_lightON = true;
  sendParticleDataToOpenGL();
}

void GLWindow::lightOff()
{
  //Only for LinkedParticles
  m_lightAmbientR = 0.5;
  m_lightAmbientG = 0.5;
  m_lightAmbientB = 0.5;

  m_lightSpecularR = 0.0;
  m_lightSpecularG = 0.0;
  m_lightSpecularB = 0.0;
  m_lightON = false;
  sendParticleDataToOpenGL();
}

void GLWindow::setLocalCohesion(int _amount)
{
  m_ps.setLocalCohesion(_amount);
  sendParticleDataToOpenGL();
}

void GLWindow::setAutomataRadius(int _amount)
{
  //Only for AutomataParticles
  m_ps.setAutomataRadius(_amount);
  sendParticleDataToOpenGL();
}

void GLWindow::setAutomataTime(int _amount)
{
  //Only for AutomataParticles
  m_ps.setAutomataTime(_amount);
  sendParticleDataToOpenGL();
}

void GLWindow::setBranchLength(double _amount)
{
  // Only for GrowthParticles
  m_ps.setBranchLength(_amount);
  sendParticleDataToOpenGL();
}

void GLWindow::restart()
{
  emit resetParticleSize(2);
  emit resetParticleType(0);
  emit resetSplitType(0);
  emit resetParticleTab(0);
  emit resetForces(true);
  emit resetParticleDeath(false);
  emit resetCohesion(5);
  emit resetLocalCohesion(80);
  emit resetChildrenThreshold(3);
  emit resetBranchLength(1);
  emit changedShadingType(0);
  emit setConnectionState(false);
  emit resetNearestParticle(true);
  emit resetGrowToLight(true);

  m_ps.reset('L');
  // Add reset functions here
  emit resetRColour(255);
  emit resetGColour(255);
  emit resetBColour(255);
  emit resetRMaterialColor(127);
  emit resetGMaterialColor(127);
  emit resetBMaterialColor(127);
  emit resetAmbientLightR(255);
  emit resetAmbientLightG(255);
  emit resetAmbientLightB(255);
  emit resetSpecularLightR(127);
  emit resetSpecularLightG(127);
  emit resetSpecularLightB(127);
  emit resetFillLight(50);
  emit resetAORadius(5.0);
  emit resetAOBias(0.025);

  m_activeRenderPassIndex = m_ADSIndex;
  emit setConnectionState(false);
  }

void GLWindow::setChildThreshold(int _amount)
{
  m_ps.setChildThreshold(_amount);
}

void GLWindow::setNearestParticle(bool _state)
{
    m_ps.setNearestParticleState(_state);
}

void GLWindow::setGrowToLight(bool _state)
{
  m_ps.setGrowToLight(_state);
}

void GLWindow::cancel()
{

}
