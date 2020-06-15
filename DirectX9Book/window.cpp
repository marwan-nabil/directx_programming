#include "window.h"


int Running;
IDirect3D9 *D3DInterface;
IDirect3DDevice9 *D3DDevice;
IDirect3DVertexBuffer9 *VertexBuffer;
IDirect3DIndexBuffer9 *IndexBuffer;

// -------------------------------------
//  rotating pyramid with directional light source
// -------------------------------------


void SetupPipeline()
{
    // enable lighting
    D3DDevice->SetRenderState(D3DRS_LIGHTING, 1);

    // put a pyramid in the vertex buffer, each side has 3 vertices,
    // we'll not share vertices because each face has a different normal
    D3DDevice->CreateVertexBuffer(12 * sizeof(vertex), D3DUSAGE_WRITEONLY,
                                  MYFVF, D3DPOOL_MANAGED, &VertexBuffer, 0);
    vertex *VPtr;
    VertexBuffer->Lock(0, 0, (void **) &VPtr, 0);
    // front face (no vertices sharing)
    VPtr[0] = {-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f};
    VPtr[1] = {0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f};
    VPtr[2] = {1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f};
    // left face
    VPtr[3] = {-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f};
    VPtr[4] = {0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f};
    VPtr[5] = {-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f};
    // right face
    VPtr[6] = {1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0};
    VPtr[7] = {0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f};
    VPtr[8] = {1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f};
    // back face
    VPtr[9] = {1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f};
    VPtr[10] = {0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f};
    VPtr[11] = {-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f};
    VertexBuffer->Unlock();

    // create the pyramid material
    D3DMATERIAL9 PyramidMaterial;
    PyramidMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    PyramidMaterial.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    PyramidMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    PyramidMaterial.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
    PyramidMaterial.Power = 5.0f;
    D3DDevice->SetMaterial(&PyramidMaterial);

    // create a directional light source
    D3DLIGHT9 DirectionalLight;
    ZeroMemory(&DirectionalLight, sizeof(D3DLIGHT9));
    DirectionalLight.Type = D3DLIGHT_DIRECTIONAL;
    DirectionalLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    DirectionalLight.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.3f;
    DirectionalLight.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.6f;
    DirectionalLight.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

    D3DDevice->SetLight(0, &DirectionalLight);
    D3DDevice->LightEnable(0, 1);

    // re-normalize vertex normals after transformations
    D3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);
    // enable specular lighting
    D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, 1);

    // view (camera) matrix
    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f); // eye
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // at
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // up
    D3DXMATRIX ViewMat;
    D3DXMatrixLookAtLH(&ViewMat, &position, &target, &up);
    D3DDevice->SetTransform(D3DTS_VIEW, &ViewMat);

    // projection matrix
    D3DXMATRIX ProjectionMat;
    D3DXMatrixPerspectiveFovLH(&ProjectionMat, 0.5f * D3DX_PI,
                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
                               1.0f, 1000.0f);
    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjectionMat);
}


int
UpdateAndRender(float TimeDelta)
{
    if(D3DDevice)
    {
        // 
        // Rotate the pyramid.
        //
        D3DXMATRIX YRotMat;
        static float YAngle = 0.0f;
        D3DXMatrixRotationY(&YRotMat, YAngle);
        YAngle += TimeDelta;

        if(YAngle >= 6.28f)
            YAngle = 0.0f;

        D3DDevice->SetTransform(D3DTS_WORLD, &YRotMat);

        // clear the screen and draw the pyramid
        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
        D3DDevice->BeginScene();

        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(vertex));
        D3DDevice->SetFVF(MYFVF);

        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

        D3DDevice->EndScene();
        D3DDevice->Present(0, 0, 0, 0);
    }
    return 1;
}

void
InitializeDirect3D(HWND Window)
{
    D3DInterface = Direct3DCreate9(D3D_SDK_VERSION);
    D3DCAPS9 DeviceCaps;
    D3DInterface->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps);
    int Flags = 0;
    if(DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        Flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        Flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    D3DPRESENT_PARAMETERS PresentParams;
    PresentParams.BackBufferWidth = WINDOW_WIDTH;
    PresentParams.BackBufferHeight = WINDOW_HEIGHT;
    PresentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
    PresentParams.BackBufferCount = 1;
    PresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    PresentParams.MultiSampleQuality = 0;
    PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    PresentParams.hDeviceWindow = Window;
    PresentParams.Windowed = 1;
    PresentParams.EnableAutoDepthStencil = 1;
    PresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    PresentParams.Flags = 0;
    PresentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    HRESULT Result = D3DInterface->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, Flags, &PresentParams, &D3DDevice
        );
    if(FAILED(Result))
    {
        MessageBoxA(0, "D3DInterface->CreateDevice() Failed.", 0, 0);
    }
}


void
CleanUp()
{
    D3DDevice->Release();
    D3DInterface->Release();
    VertexBuffer->Release();
}


LRESULT CALLBACK
MainWindowProcedure(HWND   Window,
                    UINT   Message,
                    WPARAM wParam,
                    LPARAM lParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
            {
                DestroyWindow(Window);
            }
            break;
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, wParam, lParam);
        } break;
    }
    return Result;
}


int CALLBACK
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow)
{
    // ---------------
    // create a window
    // ---------------
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = MainWindowProcedure;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "MainWindowClass";

    HRESULT LastResult = RegisterClassA(&WindowClass);
    if(FAILED(LastResult))
    {
        OutputDebugStringA("RegisterClassA() FAILED, Exiting.\n");
        return 0;
    }

    HWND Window = CreateWindowExA(0, "MainWindowClass", "Backbuffer Experiments",
                                  WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0, 0, hInstance, 0);
    if(!Window)
    {
        OutputDebugStringA("CreateWindowExA() FAILED, Exiting.\n");
        return 0;
    }

    // ----------
    // init stuff
    // ----------
    InitializeDirect3D(Window);
    SetupPipeline();

    // ---------
    // main loop
    // ---------
    Running = 1;
    float LastTime = (float) timeGetTime();
    while(Running)
    {
        // -------------------
        // handle all messages
        // -------------------
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            if(Message.message == WM_QUIT)
            {
                return (int) Message.wParam;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        // -----------------
        // update and render
        // -----------------
        float Time = (float) timeGetTime();
        float TimeDelta = (Time - LastTime) * 0.001f;
        LastTime = Time;
        UpdateAndRender(TimeDelta);
    }

    CleanUp();
    return(0);
}