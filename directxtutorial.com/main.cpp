#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct custom_vertex
{
    FLOAT x, y, z;
    DWORD color;
};

LPDIRECT3DVERTEXBUFFER9 VertexBuffer = NULL;
LPDIRECT3D9 D3DInterface;
LPDIRECT3DDEVICE9 D3DDevice;
bool Running;
float XPosition = 0.0f, YPosition = 0.0f;
float YRotation = 0.0f;

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
    DWORD VKCode = wParam;
    switch(message)
    {
        case WM_KEYDOWN:
            switch(VKCode)
            {
                case 'W':
                    YPosition += 5;
                    break;
                case 'S':
                    YPosition -= 5;
                    break;
                case 'D':
                    XPosition += 5;
                    break;
                case 'A':
                    XPosition -= 5;
                    break;
                case 'E':
                    YRotation += 0.5f;
                    break;
                case 'Q':
                    YRotation -= 0.5f;
                    break;
                default:
                    break;
            }
            break;
        case WM_QUIT:
            Running = false;
            break;

        case WM_DESTROY:
            Running = false;
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
    D3DPresentParams.Windowed = TRUE;
    D3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    D3DPresentParams.hDeviceWindow = Window; // set the window to be used by Direct3D
    D3DPresentParams.BackBufferFormat = D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
    D3DPresentParams.BackBufferWidth = SCREEN_WIDTH;
    D3DPresentParams.BackBufferHeight = SCREEN_HEIGHT;
    D3DPresentParams.EnableAutoDepthStencil = TRUE;
    D3DPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
    // create a device class using this information and the info from the D3DPresentParams stuct
    D3DInterface->CreateDevice(D3DADAPTER_DEFAULT,
                               D3DDEVTYPE_HAL,
                               Window,
                               D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                               &D3DPresentParams,
                               &D3DDevice);
    InitGraphics();
    D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);   // turnoff 3d lighting
    D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // shade both sides of the triangles
    D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE); // turn on the z-buffer
}


// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the window to a deep blue, and the z-buffer
    D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);
    D3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    D3DDevice->BeginScene();
    D3DDevice->SetFVF(CUSTOMFVF); // select which vertex format we are using

    // world transforms
    //D3DXMATRIX MatWorldRotY;     // a matrix to store the rotation information
    //D3DXMatrixRotationY(&MatWorldRotY, YRotation);
    //D3DDevice->SetTransform(D3DTS_WORLD, &MatWorldRotY);

    // view transform
    D3DXMATRIX MatView;     // the view transform matrix
    D3DXMatrixLookAtLH(&MatView,
                       &D3DXVECTOR3(XPosition, YPosition, 10.0f),     // the camera position
                       &D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
                       &D3DXVECTOR3(0.0f, 1.0f, 0.0f));     // the up direction
    D3DDevice->SetTransform(D3DTS_VIEW, &MatView);  // set the view transform to MatView

    // projection transform
    D3DXMATRIX MatProjection;     // the projection transform matrix
    D3DXMatrixPerspectiveFovLH(&MatProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               (FLOAT) SCREEN_WIDTH / (FLOAT) SCREEN_HEIGHT, // aspect ratio
                               1.0f,    // the near view-plane
                               100.0f); // the far view-plane
    D3DDevice->SetTransform(D3DTS_PROJECTION, &MatProjection);    // set the projection

    // select the vertex buffer to display
    D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(custom_vertex));
    D3DXMATRIX MatTranslateA; // a matrix to store the translation for triangle A
    D3DXMATRIX MatTranslateB; // a matrix to store the translation for triangle B
    D3DXMATRIX MatRotateY; // a matrix to store the rotation for each triangle
    
    // build MULTIPLE matrices to translate the model and one to rotate
    D3DXMatrixTranslation(&MatTranslateA, 0.0f, 0.0f, 2.0f);
    D3DXMatrixTranslation(&MatTranslateB, 0.0f, 0.0f, -2.0f);
    D3DXMatrixRotationY(&MatRotateY, YRotation); // the front side

    // tell Direct3D about each world transform, and then draw another triangle
    D3DDevice->SetTransform(D3DTS_WORLD, &(MatTranslateA * MatRotateY));
    D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    D3DDevice->SetTransform(D3DTS_WORLD, &(MatTranslateB * MatRotateY));
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
        {2.5f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
        {0.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
        {-2.5f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
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