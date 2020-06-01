#include <Windows.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib")

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

IDirect3D9 *D3DInterface;
IDirect3DDevice9 *D3DDevice;
IDirect3DVertexBuffer9 *VertexBuffer;
IDirect3DIndexBuffer9 *IndexBuffer;
int Running;


struct vertex
{
    float x, y, z;
};
#define MYFVF D3DFVF_XYZ


void SetupPipeline()
{
    // fill a vertex buffer with a cube
    D3DDevice->CreateVertexBuffer(8 * sizeof(struct vertex),
                                  D3DUSAGE_WRITEONLY,
                                  MYFVF,
                                  D3DPOOL_MANAGED,
                                  &VertexBuffer, 0);
    vertex *Vertices;
    VertexBuffer->Lock(0, 0, (void **) &Vertices, 0);
    Vertices[0] = {-1.0f, -1.0f, -1.0f};
    Vertices[1] = {-1.0f, 1.0f, -1.0f};
    Vertices[2] = {1.0f, 1.0f, -1.0f};
    Vertices[3] = {1.0f, -1.0f, -1.0f};
    Vertices[4] = {-1.0f, -1.0f, 1.0f};
    Vertices[5] = {-1.0f, 1.0f, 1.0f};
    Vertices[6] = {1.0f, 1.0f, 1.0f};
    Vertices[7] = {1.0f, -1.0f, 1.0f};
    VertexBuffer->Unlock();

    // create an index buffer for that triangle and fill it
    D3DDevice->CreateIndexBuffer(36 * sizeof(WORD),
                                 D3DUSAGE_WRITEONLY,
                                 D3DFMT_INDEX16,
                                 D3DPOOL_MANAGED,
                                 &IndexBuffer,
                                 0);
    WORD *indices = 0;
    IndexBuffer->Lock(0, 0, (void **) &indices, 0);
    // front side
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;
    // back side
    indices[6] = 4; indices[7] = 6; indices[8] = 5;
    indices[9] = 4; indices[10] = 7; indices[11] = 6;
    // left side
    indices[12] = 4; indices[13] = 5; indices[14] = 1;
    indices[15] = 4; indices[16] = 1; indices[17] = 0;
    // right side
    indices[18] = 3; indices[19] = 2; indices[20] = 6;
    indices[21] = 3; indices[22] = 6; indices[23] = 7;
    // top
    indices[24] = 1; indices[25] = 5; indices[26] = 6;
    indices[27] = 1; indices[28] = 6; indices[29] = 2;
    // bottom
    indices[30] = 4; indices[31] = 0; indices[32] = 3;
    indices[33] = 4; indices[34] = 3; indices[35] = 7;
    IndexBuffer->Unlock();

    // view (camera) transformation
    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f); // eye
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // at
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // up
    D3DXMATRIX ViewTrans;
    D3DXMatrixLookAtLH(&ViewTrans, &position, &target, &up);
    D3DDevice->SetTransform(D3DTS_VIEW, &ViewTrans);

    // setup the projection matrix
    D3DXMATRIX ProjMat;
    D3DXMatrixPerspectiveFovLH(&ProjMat, D3DX_PI * 0.5f, 
                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
                               1.0f, 1000.0f);
    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);

    // set the render mode as wireframe
    D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}


int
UpdateAndRender(float TimeDelta)
{
    if(D3DDevice)
    {
        // setup the world transform (rotating with time)
        D3DXMATRIX XRotation, YRotation;

        // rotate on x-axis with time
        static float XAngle = 0.0f;
        D3DXMatrixRotationX(&XRotation, 3.14f / 4.0f * XAngle);
        XAngle += TimeDelta;
        if(XAngle >= 6.28f)
            XAngle = 0.0f;

        // incremement Y-rotation angle with time
        static float YAngle = 0.0f;
        D3DXMatrixRotationY(&YRotation, YAngle);
        YAngle += TimeDelta;

        // reset angle to zero when angle reaches 2*PI
        if(YAngle >= 6.28f)
            YAngle = 0.0f;

        // calculate the final world transform
        D3DXMATRIX WorldTrans = XRotation * YRotation;
        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTrans);

        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
        D3DDevice->BeginScene();

        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(struct vertex));
        D3DDevice->SetIndices(IndexBuffer);
        D3DDevice->SetFVF(MYFVF);

        // draw a cube
        D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

        D3DDevice->EndScene();
        D3DDevice->Present(0, 0, 0, 0);
    }
    return 1;
}


void
CleanUp()
{
    VertexBuffer->Release();
    D3DDevice->Release();
    D3DInterface->Release();
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
    PresentParams.Windowed = 0;
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
        UpdateAndRender(TimeDelta);
        LastTime = Time;
    }

    CleanUp();
    return(0);
}