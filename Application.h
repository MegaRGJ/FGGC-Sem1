#pragma once

#include <windows.h>
#include <Winuser.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "Camera.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pVertexBufferPy; 
	ID3D11Buffer*           _pIndexBufferPy;  
	ID3D11Buffer*           _pConstantBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame; 
	ConstantBuffer cb;
	ID3D11SamplerState* _pSamplerLinear;
	ID3D11ShaderResourceView* _pTextureRV;
	ID3D11ShaderResourceView* _pTextureRV2;
	ID3D11ShaderResourceView* _pTextureRV3;

	XMFLOAT4X4              _world;
	XMFLOAT4X4              _world1;
	XMFLOAT4X4				_world2;

	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularMtrl;
	XMFLOAT4 specularLight;
	float specularPower;

	Camera* camera1;
	Camera* camera2;
	Camera* camera3;
	Camera* camera4;
	Camera* currentCamera;
	XMFLOAT4 eye;
	XMFLOAT4 up;
	XMFLOAT4 at;
	XMFLOAT4 camera1Eye;
	XMFLOAT4 camera1At;
	XMFLOAT4 camera2Eye;
	XMFLOAT4 camera2At;
	FLOAT nearDepth;
	FLOAT farDepth;

	MeshData objMeshData;
	GameObject* Plane;
	GameObject* City;

	//plane movements
	float planePosX;
	float planePosY;
	float planePosZ;

	float planeRotX;
	float planeRotY;
	float planeRotZ;

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	void Update();
	void Draw();
};
