#pragma once
#include <Windows.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib")

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


extern int Running;
extern IDirect3D9 *D3DInterface;
extern IDirect3DDevice9 *D3DDevice;
extern IDirect3DVertexBuffer9 *VertexBuffer;
extern IDirect3DIndexBuffer9 *IndexBuffer;

// materials
extern D3DMATERIAL9 WHITE_MTRL;
extern D3DMATERIAL9 RED_MTRL;
extern D3DMATERIAL9 GREEN_MTRL;
extern D3DMATERIAL9 BLUE_MTRL;
extern D3DMATERIAL9 YELLOW_MTRL;

// colors 
extern D3DXCOLOR      WHITE;
extern D3DXCOLOR      BLACK;
extern D3DXCOLOR        RED;
extern D3DXCOLOR      GREEN;
extern D3DXCOLOR       BLUE;
extern D3DXCOLOR     YELLOW;
extern D3DXCOLOR       CYAN;
extern D3DXCOLOR    MAGENTA;

// the vertex format
typedef struct
{
    float X, Y, Z;
    float NX, NY, NZ;
} vertex;
#define MYFVF (D3DFVF_XYZ | D3DFVF_NORMAL)


// Lights stuff
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3 *direction, D3DXCOLOR *color);
D3DLIGHT9 InitPointLight(D3DXVECTOR3 *position, D3DXCOLOR *color);
D3DLIGHT9 InitializeSpotLight(D3DXVECTOR3 *position, D3DXVECTOR3 *direction, D3DXCOLOR *color);

// Materials stuff
D3DMATERIAL9 InitializeMaterial(D3DXCOLOR AmbientColor, D3DXCOLOR DiffuseColor, D3DXCOLOR SpecularColor,
                                D3DXCOLOR EmissiveColor, float EmisivePower);
