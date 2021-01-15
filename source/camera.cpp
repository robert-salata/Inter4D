#include "camera.h"

Camera::Camera()
{
	position = D3DXVECTOR3(0, 0, 0);
	rotation = D3DXVECTOR3(0, 0, 0);
}

void Camera::set_position(float x, float y, float z)
{
	position = D3DXVECTOR3(x, y, z);
}

void Camera::set_rotation(float x, float y, float z)
{
	rotation = D3DXVECTOR3(x, y, z);
}

void Camera::render()
{
	float pitch = rotation.x;
	float yaw   = rotation.y;
	float roll  = rotation.z;

	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	D3DXVECTOR3 lookAt = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);

	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	lookAt = position + lookAt;
	D3DXMatrixLookAtLH(&view_matrix, &position, &lookAt, &up);
}

void Camera::get_view_matrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = view_matrix;
}