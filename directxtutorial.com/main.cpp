#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

#pragma comment (lib, "d3d9.lib")

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct custom_vertex
{
    FLOAT x, y, z, rhw;
    DWORD color;
};

LPDIRECT3DVERTEXBUFFER9 VertexBuffer = NULL;
LPDIRECT3D9 D3DInterface;
LPDIRECT3DDEVICE9 D3DDevice;
bool Running;


void InitD3D(HWND hWnd);
void RenderFrame(void);
void ReleaseD3D(void);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam);
void InitGraphics(void);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND Window;
    WNDCLASSEX WindowClass;

    ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));

    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = hInstance;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    WindowClass.lpszClassName = L"WindowClass";

    RegisterClassEx(&WindowClass);

    Window = CreateWindowEx(NULL,
                            L"WindowClass",
                            L"Direct3D App",
                            WS_OVERLAPPEDWINDOW,
                            0, 0,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            NULL, NULL, hInstance, NULL);

    ShowWindow(Window, nCmdShow);
    InitD3D(Window);

    MSG msg;
    Running = true;
    while(Running)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RenderFrame();
    }

    ReleaseD3D();
    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_QUIT:
            Running = false;
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


void InitD3D(HWND Window)
{
    D3DInterface = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

    D3DPRESENT_PARAMETERS D3DPresentParams;

    ZeroMemory(&D3DPresentParams, sizeof(D3DPresentParams)); // clear out the struct for use
    D3DPresentParams.Windowed = TRUE; // program fullscreen, not windowed
    D3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    D3DPresentParams.hDeviceWindow = Window; // set the window to be used by Direct3D
    D3DPresentParams.BackBufferFormat = D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
    D3DPresentParams.BackBufferWidth = SCREEN_WIDTH;
    D3DPresentParams.BackBufferHeight = SCREEN_HEIGHT;

    // create a device class using this information and the info from the D3DPresentParams stuct
    D3DInterface->CreateDevice(D3DADAPTER_DEFAULT,
                               D3DDEVTYPE_HAL,
                               Window,
                               D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                               &D3DPresentParams,
                               &D3DDevice);
    InitGraphics();
}


// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the window to a deep blue
    D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

    D3DDevice->BeginScene();

    // select which vertex format we are using
    D3DDevice->SetFVF(CUSTOMFVF);
    // select the vertex buffer to display
    D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(custom_vertex));
    // copy the vertex buffer to the back buffer
    D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    D3DDevice->EndScene();

    D3DDevice->Present(NULL, NULL, NULL, NULL);
}


void ReleaseD3D(void)
{
    VertexBuffer->Release();
    D3DDevice->Release();
    D3DInterface->Release();
}


void InitGraphics()
{
    // create the TriangleVertices using the CUSTOMVERTEX struct
    custom_vertex TriangleVertices[] =
    {
        {400.0f, 62.5f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255)},
        {650.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0)},
        {150.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0)},
    };

    // create a vertex buffer interface called VertexBuffer
    D3DDevice->CreateVertexBuffer(3 * sizeof(custom_vertex),
                                  0, CUSTOMFVF,
                                  D3DPOOL_MANAGED,
                                  &VertexBuffer, NULL);

    VOID *BufferBeginning;
    // lock VertexBuffer and load the TriangleVertices into it
    VertexBuffer->Lock(0, 0, (void **) &BufferBeginning, 0);
    memcpy(BufferBeginning, TriangleVertices, sizeof(TriangleVertices));
    VertexBuffer->Unlock();
}