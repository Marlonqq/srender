#include "Camera.h"

Camera::Camera()
{
	fov = 45.0f;
	aspect = 1.0f;
	zNear = 1.0f;
	zFar = 100.0f;

	eye = glm::vec3(0.0f, 0.0f, 2.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	front = glm::vec3(0.0f, 0.0f, 1.0f);
	frontXup = glm::normalize(glm::cross(front, up));
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	pitch = 0.0f;
	yaw = -90.0f;

	SPEED = 0.05;
	SENSITIVITY = 0.1;

	updateVectors();
}


Camera::Camera(glm::vec3 _pos) : Camera()
{
	eye = _pos;
}

void Camera::forward()
{
	eye += SPEED * front;
}

void Camera::back()
{
	eye -= SPEED * front;
}

void Camera::left()
{
	eye -= SPEED * frontXup;
}

void Camera::right()
{
	eye += SPEED * frontXup;
}

void Camera::move_up()
{
	eye += SPEED * up;
}

void Camera::move_down()
{
	eye -= SPEED * up;
}

void Camera::rotate(float delta_pitch, float delta_yaw)
{
	pitch += glm::radians(SENSITIVITY * delta_pitch);
	yaw += glm::radians(SENSITIVITY * delta_yaw);

	updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	glm::vec3 f = glm::normalize(front);
	glm::vec3 r = glm::normalize(glm::cross(f, up));
	glm::vec3 u = glm::normalize(glm::cross(r, f));
	glm::mat4 view(1.0f);

	view[0][0] = r.x;
	view[1][0] = r.y;
	view[2][0] = r.z;
	view[0][1] = u.x;
	view[1][1] = u.y;
	view[2][1] = u.z;
	view[0][2] = f.x;
	view[1][2] = f.y;
	view[2][2] = f.z;
	view[3][0] = -glm::dot(eye, r);
	view[3][1] = -glm::dot(eye, u);
	view[3][2] = -glm::dot(eye, f);

	return view;
}

glm::mat4 Camera::getProjection()
{
	const float tanHalfFov = glm::tan(glm::radians(fov) / static_cast<float>(2));
	glm::mat4 persp(0.0f);
	persp[0][0] = static_cast<float>(1) / (aspect * tanHalfFov);
	persp[1][1] = static_cast<float>(1) / tanHalfFov;
	persp[2][3] = static_cast<float>(1);
	persp[2][2] = zFar / (zFar - zNear);
	persp[3][2] = -zFar * zNear / (zFar - zNear);

	//persp[2][2] = -zNear / (zFar - zNear);
	//persp[3][2] = zFar * zNear / (zFar - zNear);

	return persp;
}

void Camera::updateVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	frontXup = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(frontXup, front));
}

