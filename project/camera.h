

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glut.h>
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	SHIFTDOWN,
	SHIFTUP
};

enum Mouse_Button {
	NOBUTTON,
	LEFTBUTTON,
	MIDDLEBUTTON,
	RIGHTBUTTON
};
enum Camera_Mode
{
	GAME,
	ROAM
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 45.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
	friend class Ghost;

public:
	//Camera Setting
	Camera_Mode Mode;  //游戏模式or漫游模式

    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
	float GroundY;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
		GroundY = position[1];
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
		
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;


        
		// FPS Game make sure the user stays at the ground level
		if(Mode == GAME)
			Position.y = GroundY; // <-- this one-liner keeps the user at the ground level (xz plane)
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, Mouse_Button button = NOBUTTON, GLboolean constrainPitch = true)
    {
		if (button == NOBUTTON)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			updateCameraVectors();
		}

		if (button == LEFTBUTTON && Mode == ROAM)
		{
			xoffset *= MouseSensitivity;
			Yaw += xoffset;
			float rad = -glm::radians(xoffset);
			OrbitRotate(Position+Front+Front, glm::vec3(0, 1, 0), rad);
		}


		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
		if (Mode == ROAM)
		{
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 45.0f)
				Zoom = 45.0f;
		}
    }

	// simple Orbit
	void OrbitRotate(glm::vec3 centre, glm::vec3 axis, float rad)
	{
		float dist = sqrt(glm::dot(centre - Position, centre - Position));
		glm::vec4 world_x = glm::vec4(-glm::cross(Up, Front), 1.0);
		glm::vec4 world_y = glm::vec4(Up, 1.0);
		glm::vec4 world_z = glm::vec4(-Front, 1.0);
		glm::mat4 basis = glm::mat4(world_x, world_y, world_z, glm::vec4(1.0));
		// 通过单位矩阵创建变换矩阵
		glm::mat4 trans = glm::rotate(glm::mat4(1.0), rad, axis);
		basis = basis * trans;
		Front = glm::normalize(glm::vec3(-basis[2]));
		Up = glm::normalize(glm::vec3(basis[1]));
		Right = glm::normalize(glm::cross(Front, WorldUp));
		//Position = glm::vec3(trans * glm::vec4(Position, 1.0));
		Position = centre - (Front*dist);
		// make sure the user stays at the ground level
		//Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)
	}


private:
	
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif

