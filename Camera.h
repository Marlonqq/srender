#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	float fov;
	float zNear;
	float zFar;
	float aspect;

	Camera();
	Camera(glm::vec3);

	void forward();
	void back();
	void left();
	void right();
	void move_up();
	void move_down();
	void rotate(float, float);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjection();

private:
	void updateVectors();

public:
	glm::vec3 eye;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 frontXup;
	glm::vec3 worldUp;
	float pitch;  //pitch
	float yaw;   // yaw
	float SPEED;
	float SENSITIVITY;
};


#endif // !_CAMERA_H_


