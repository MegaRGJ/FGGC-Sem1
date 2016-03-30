#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	_pVertexBufferPy = nullptr;
	_pIndexBufferPy = nullptr;

}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_world1, XMMatrixIdentity());
	XMStoreFloat4x4(&_world2, XMMatrixIdentity());

	//First person
	eye = XMFLOAT4(0.0f, 1.0f, 5.0f, 0.0f); // location
	at = XMFLOAT4(0.0f, 1.0f, 20.0f, 0.0f);// look at
	up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); // look to camera
	nearDepth = FLOAT(0.01f);
	farDepth = FLOAT(1000.0f);
	camera1 = new Camera(eye, at, up, _WindowWidth, _WindowHeight, nearDepth, farDepth);
	//Third person
	eye = XMFLOAT4(0.0f, 5.0f, -10.0f, 0.0f);
	at = XMFLOAT4(0.0f, 3.0f, 10.0f, 0.0f);
	up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	nearDepth = FLOAT(0.01f);
	farDepth = FLOAT(1000.0f);
	camera2 = new Camera(eye, at, up, _WindowWidth, _WindowHeight, nearDepth, farDepth);

	eye = XMFLOAT4(20.0f, 0.0f, -4.0f, 0.0f);
	at = XMFLOAT4(20.0f, 0.0f, 0.0f, 0.0f);
	up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); 
	nearDepth = FLOAT(0.01f);
	farDepth = FLOAT(1000.0f);
	camera3 = new Camera(eye, at, up, _WindowWidth, _WindowHeight, nearDepth, farDepth);

	eye = XMFLOAT4(30.0f, 50.0f, -30.0f, 0.0f);
	at = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); 
	nearDepth = FLOAT(0.01f);
	farDepth = FLOAT(1000.0f);
	camera4 = new Camera(eye, at, up, _WindowWidth, _WindowHeight, nearDepth, farDepth);

	currentCamera = camera1;
	
	ifstream inputTextFile;
	inputTextFile.open("PlaneConfig.txt");
	
	//Plane 
	inputTextFile >> planePosX;
	inputTextFile >> planePosY;
	inputTextFile >> planePosZ;

	inputTextFile >> planeRotX;
	inputTextFile >> planeRotY;
	inputTextFile >> planeRotZ;
	inputTextFile.close();



	// Light direction from surface (XYZ)
	lightDirection = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// Diffuse material properties (RGBA)
	diffuseMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	// Diffuse light colour (RGBA)
	diffuseLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // red,green,blue,alpha
	
	//Ambient
	ambientMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	//Specular
	specularMtrl = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	specularPower = float(10.0f);

	//objMeshData = OBJLoader::Load("", _pd3dDevice, false); // Blender

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	objMeshData = OBJLoader::Load("F-35_Lightning_II.obj", _pd3dDevice); // 3dsmax
	Plane = new GameObject();
	Plane->Initialise(objMeshData);
	Plane->SetTranslation(planePosX, planePosY, planePosZ);
	Plane->SetRotation(planeRotX, planeRotY, planeRotZ);
	Plane->UpdateWorld();

	//objMeshData = OBJLoader::Load("lowpolymountains.obj", _pd3dDevice, false);
	objMeshData = OBJLoader::Load("terrain.obj", _pd3dDevice);
	City = new GameObject();
	City->Initialise(objMeshData);
	City->SetRotation(0.0f, 3.142f, 0.0f);
	City->SetTranslation(0.0f, -150.0f, 0.0f);
	City->SetScale(1000.0f,50.0f, 1000.0f);
	City->UpdateWorld();
	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_pTextureRV3);
	CreateDDSTextureFromFile(_pd3dDevice, L"F35_Tex.dds", nullptr, &_pTextureRV2);
	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_pTextureRV);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] = // Cube 24
    {
		//front
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },//2
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		//back
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },//4
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		//top
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },//8
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		//right
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },//12
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		//bottom
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },//16
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		//left
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },//20
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },//21
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },//22
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },//23
	};

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);
	if (FAILED(hr))
		return hr;                              

	D3D11_RASTERIZER_DESC wfdesc; //Rasterizer
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;

	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame); 


    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		//front
        0, 1, 2,
        0, 2, 3,
		//back
		4, 6, 5,
		4, 7, 6,
		//top
		8, 9, 10,
		8, 10, 11,
		//right
		12, 13, 14,
		12, 14, 15,
		//bottom
		16, 17, 18,
		16, 18, 19,
		//left
		20, 21, 22,
		20, 22, 23,
    };

	WORD indicesP[] =
	{
		0, 1, 4,
		2, 3, 4,
		1, 2, 4,
		0, 4, 3,
		0, 3, 2,
		0, 2, 1

	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;
	D3D11_BUFFER_DESC bdp;
	ZeroMemory(&bdp, sizeof(bdp));

	bdp.Usage = D3D11_USAGE_DEFAULT;
	bdp.ByteWidth = sizeof(WORD) * 18;
	bdp.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdp.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataP;
	ZeroMemory(&InitDataP, sizeof(InitDataP));
	InitDataP.pSysMem = indicesP;
	hr = _pd3dDevice->CreateBuffer(&bdp, &InitDataP, &_pIndexBufferPy);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
	RECT rc = { 0, 0, 1200, 1200 };
    AdjustWindowRect(&rc, WS_BORDER, FALSE);
	_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Plane Game", WS_BORDER,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;


#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;
	

	D3D11_TEXTURE2D_DESC depthStencilDesc; // Depth/Stencil buffer

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	InitIndexBuffer();

    // Set index buffer
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);


    if (FAILED(hr))
        return hr;


    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();

}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;
	cb.gTime = t;
    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }
	// keyboard keys
	if (GetAsyncKeyState('X'))
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}
	if (GetAsyncKeyState('C'))
	{
		_pImmediateContext->RSSetState(nullptr);
	}
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		DestroyWindow(_hWnd);
	}
	if (GetAsyncKeyState('1'))
	{
		currentCamera = camera1;
	}
	if (GetAsyncKeyState('2'))
	{
		currentCamera = camera2;
	}
	if (GetAsyncKeyState('3'))
	{
		currentCamera = camera3;
	}
	if (GetAsyncKeyState('4'))
	{
		currentCamera = camera4;
	}
	//Forward and Back movement
	if (GetAsyncKeyState('W'))
	{ 
		float f = 0.05f;
		planePosZ = planePosZ + f;
		camera1->ForwardBack(f);
		camera2->ForwardBack(f);
	}	
	if (GetAsyncKeyState('S'))
	{
		float f = -0.05f;
		planePosZ = planePosZ + f;
		camera1->ForwardBack(f);
		camera2->ForwardBack(f);
	}
	//right and left movement
	if (GetAsyncKeyState('D'))
	{
		float f = 0.05f;
		planePosX = planePosX + f;
		camera1->LeftRight(f);
		camera2->LeftRight(f);
	}
	if (GetAsyncKeyState('A'))
	{
		float f = -0.05f;
		planePosX = planePosX + f;
		camera1->LeftRight(f);
		camera2->LeftRight(f);
	}
	// up and down movement
	if (GetAsyncKeyState('R'))
	{
		float f = 0.005f;
		planePosY = planePosY + f;
		camera1->UpDown(f);
		camera2->UpDown(f);
	}
	if (GetAsyncKeyState('F'))
	{
		float f = -0.005f;
		planePosY = planePosY + f;
		camera1->UpDown(f);
		camera2->UpDown(f);
	}
	//Right and left rotation 
	if (GetAsyncKeyState('Q'))
	{
		float f = 0.0005f;
		planeRotZ = planeRotZ + f;
		camera1->Rotation(f);
		//camera2->Rotation(f);

	}
	if (GetAsyncKeyState('E'))
	{
		float f = -0.0005f;
		planeRotZ = planeRotZ + f;
		camera1->Rotation(f);
		//camera2->Rotation(f);
	}
	Plane->SetRotation(planeRotX, planeRotY, planeRotZ);
	Plane->SetTranslation(planePosX, planePosY, planePosZ);
	Plane->UpdateWorld();
	//City->UpdateWorld();
	currentCamera->CalculateViewProjection();
    //
    // Animate the cube
    //
	XMStoreFloat4x4(&_world, XMMatrixRotationZ(t) * XMMatrixRotationX(t) * XMMatrixTranslation(20.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&_world1, XMMatrixRotationZ(t) * XMMatrixRotationX(t) *XMMatrixTranslation(10.0f, 0.0f, 0.0f)*  XMMatrixRotationY(t));

}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha BackGround colour
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	
	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX world1 = XMLoadFloat4x4(&_world1);
	//XMMATRIX world2 = XMLoadFloat4x4(&_world2);
	XMMATRIX TheMatrix = XMLoadFloat4x4(&Plane->GetWorld());
	XMMATRIX CityMatrix = XMLoadFloat4x4(&City->GetWorld());
	XMMATRIX view = XMLoadFloat4x4(&currentCamera->GetViewProjection());
	XMMATRIX projection = XMLoadFloat4x4(&currentCamera->GetProjection());
    //
    // Update variables
    //
    // ConstantBuffer Stuff
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	cb.LightVecW = lightDirection;
	cb.DiffuseMtrl = diffuseMaterial;
	cb.DiffuseLight = diffuseLight;
	cb.AmbientLight = ambientMaterial;
	cb.AmbientMaterial = ambientLight;
	cb.SpecularMtrl = specularMtrl;
	cb.SpecularLight = specularLight;
	cb.SpecularPower = specularPower;
	cb.EyePosW = currentCamera->EyePos;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


	//cube
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->DrawIndexed(36, 0, 0);        

	cb.mWorld = XMMatrixTranspose(world1*XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//obj Plane
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV2);
	cb.mWorld = XMMatrixTranspose(TheMatrix);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	Plane->Draw(_pd3dDevice, _pImmediateContext);

	//obj City
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV3);
	cb.mWorld = XMMatrixTranspose(CityMatrix);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	City->Draw(_pd3dDevice, _pImmediateContext);


    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}