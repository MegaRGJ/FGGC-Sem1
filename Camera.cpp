#include "Camera.h"
//EyePos = XMFLOAT3(30.0f, 0.0f, -30.0f);

Camera::Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _eye(eye), _at(at), _up(up), _windowWidth(windowWidth), _windowHeight(windowHeight), _nearDepth(nearDepth), _farDepth(farDepth)
{
	CalculateViewProjection();
	EyePos = XMFLOAT3(20.0f, 0.0f, -4.0f); //NEED TO DO SOMETHING ABOUT THIS
}
Camera::~Camera()
{
}
void Camera::ForwardBack(float f)
{
	_eye.z = _eye.z + f;
	_at.z = _at.z + f;
}
void Camera::LeftRight(float f)
{
	_eye.x = _eye.x + f;
	_at.x = _at.x + f;
}
void Camera::UpDown(float f)
{
	_eye.y = _eye.y + f;
	_at.y = _at.y + f;
}
void Camera::Rotation(float f)
{
	_up.x = _up.x - f;

}
void Camera::CalculateViewProjection()
{
	XMVECTOR Eye = XMLoadFloat4(&_eye);
	XMVECTOR At = XMLoadFloat4(&_at);
	XMVECTOR Up = XMLoadFloat4(&_up);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

XMFLOAT4X4 Camera::GetViewProjection() const
{
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4(&viewProj, view * projection);

	return viewProj;
}
void Camera::WindowResize(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

void Camera::SetEye(XMFLOAT4 eye)
{
	_eye = eye;
}

void Camera::SetAt(XMFLOAT4 at)
{
	_at = at;
}

void Camera::SetUp(XMFLOAT4 up)
{
	_up = up;
}


