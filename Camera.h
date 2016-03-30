#pragma once
#include <d3d11_1.h>
#include <directxmath.h>


using namespace DirectX;

class Camera
{
private:
	XMFLOAT4X4  _view;
	XMFLOAT4X4 _projection;

	XMFLOAT4 _eye;
	XMFLOAT4 _at;
	XMFLOAT4 _up;

	//XMFLOAT3 EyePos;
	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;
	
public:
	XMFLOAT3 EyePos;
	Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	void ForwardBack(float);
	void Rotation(float);
	void LeftRight(float);
	void UpDown(float);
	void CalculateViewProjection();
	void WindowResize(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	
	void SetEye(XMFLOAT4);
	void SetAt(XMFLOAT4);
	void SetUp(XMFLOAT4);

	XMFLOAT4 GetEye() const { return _eye; }
	XMFLOAT4 GetAt() const { return _at; }
	XMFLOAT4 GetUp() const { return _up; }

	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProjection() const { return _projection; }

	XMFLOAT4X4 GetViewProjection() const;




};

