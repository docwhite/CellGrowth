#ifndef CAMERA_H
#define CAMERA_H

#pragma once

// Std. Includes
#include <vector>


// QT
#include <QVector3D>
#include <QMatrix4x4>
#include <QtMath>




// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const GLfloat YAW        =  -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  0.2f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    QVector3D Position;
    QVector3D Front;
    QVector3D Up;
    QVector3D Right;
    QVector3D WorldUp;
    // Eular Angles
    GLfloat Yaw; // Left and right
    GLfloat Pitch; // Up and down
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    // Constructor with vectors
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW,
           GLfloat pitch = PITCH) :
        Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = QVector3D(posX, posY, posZ);
        this->WorldUp = QVector3D(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }


    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    QMatrix4x4 GetViewMatrix()
    {
        // Create and instantiate matrix
        QMatrix4x4 view;
        view.setToIdentity();

        QVector3D center(0,0,0); //(this->Position + this->Front)
        center = (this->Position + this->Front);
        view.lookAt(this->Position, center , this->Up);
        return view;
    }




    QVector3D getPosition() { return Position; }


    GLfloat getPitch() { return this->Pitch; }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * velocity;
        if (direction == RIGHT)
            this->Position += this->Right * velocity;

    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw   += xoffset;
        this->Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        this->updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
            this->Zoom -= yoffset;
        if (this->Zoom <= 1.0f)
            this->Zoom = 1.0f;
        if (this->Zoom >= 45.0f)
            this->Zoom = 45.0f;
    }



private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {


        // Calculate the new Front vector
        QVector3D front (cos(qDegreesToRadians(this->Yaw)) * cos(qDegreesToRadians(this->Pitch)),
                         sin(qDegreesToRadians(this->Pitch)),
                         sin(qDegreesToRadians(this->Yaw)) * cos(qDegreesToRadians(this->Pitch)));
//        front.x() = cos(qDegreesToRadians(this->Yaw)) * cos(qDegreesToRadians(this->Pitch));
//        front.y() = sin(qDegreesToRadians(this->Pitch));
//        front.z() = sin(qDegreesToRadians(this->Yaw)) * cos(qDegreesToRadians(this->Pitch));
        this->Front = front.normalized();
        // Also re-calculate the Right and Up vector
        QVector3D right   = QVector3D::crossProduct(this->Front,this->WorldUp);
        QVector3D up      = QVector3D::crossProduct(this->Right,this->Front);
//        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
        this->Right       = right.normalized();
        this->Up          = up.normalized();


    }
};

#endif
