#include <iostream>
#include <fstream>
#include <memory>

// QT
#include <QDebug>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtMath>
#include <QWidget>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTime>


// OpenGL
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

// Custom

#include "viewport.h"
#include "input.h"
#include "manipulator.h"
#include "selectobject.h"
#include "pointlight.h"
#include "camera.h"


class OpenGLWindow : public Viewport
{
    Q_OBJECT
public:
    explicit OpenGLWindow();
    ~OpenGLWindow();

    void render() Q_DECL_OVERRIDE;

    void initialize() Q_DECL_OVERRIDE;

    void Update();

    void Do_Movement();

    void keyPressEvent(QKeyEvent *key);
    void keyReleaseEvent(QKeyEvent *key);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);


    void setInputManager(InputManager *_input);

    void doSelection(const int _x, const int _y);

    void createGeometryPoints(std::vector<QVector3D>& vertices,
                           float radius,
                           unsigned int rings,
                           unsigned int sectors);

    std::string readFile(const char *filePath);
    QVector3D unProject(float x, float y, float z);
    void setupCamera();



private:

    void getUniqueColour(const int _x, const int _y);


    // VIEWPORT/CAMERA
    int screenWidth, screenHeight;
    InputManager inputManager;





    // Array of all scene objects
    std::vector<std::shared_ptr<SelectObject>> objectList;



    // QT IMPLEMENTATION
    QOpenGLShaderProgram *m_manipulatorProgram;
    QOpenGLShaderProgram *m_sunProgram;

    QOpenGLContext *m_context;
};
