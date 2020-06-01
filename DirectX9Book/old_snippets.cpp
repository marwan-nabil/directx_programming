// -------------------------------------
//  paint a surface
// -------------------------------------
//void
//PaintSurfaceRed()
//{
//    D3DSURFACE_DESC SurfaceDesc;
//    Surface->GetDesc(&SurfaceDesc);
//    int SurfaceHeight = SurfaceDesc.Height;
//    int SurfaceWidth = SurfaceDesc.Width;
//
//    // lock the surface and get a pointer to it
//    D3DLOCKED_RECT LockedRect;
//    Surface->LockRect(&LockedRect, 0, 0);
//
//    DWORD *Pixel = (DWORD *) LockedRect.pBits;
//    int Pitch = LockedRect.Pitch;
//    for(int i = 0;
//        i < SurfaceHeight;
//        i++)
//    {
//        for(int j = 0;
//            j < SurfaceWidth;
//            j++)
//        {
//            int dword_index = i * Pitch / 4 + j;
//            Pixel[dword_index] = 0xffff0000; // red
//        }
//    }
//
//    Surface->UnlockRect();
//}


// -------------------------------------
//  a simple triangle
// -------------------------------------
//void SetupPipeline()
//{
//    // fill a vertex buffer with a triangle
//    D3DDevice->CreateVertexBuffer(3 * sizeof(struct vertex),
//                                  D3DUSAGE_WRITEONLY,
//                                  MYFVF,
//                                  D3DPOOL_MANAGED,
//                                  &VertexBuffer, 0);
//    vertex *Vertices;
//    VertexBuffer->Lock(0, 0, (void **) &Vertices, 0);
//    Vertices[0] = {-1.0f, 0.0f, 2.0f};
//    Vertices[1] = {0.0f, 1.0f, 2.0f};
//    Vertices[2] = {1.0f, 0.0f, 2.0f};
//    VertexBuffer->Unlock();
//
//    // setup the projection matrix, no world and view transformations
//    D3DXMATRIX ProjMat;
//    D3DXMatrixPerspectiveFovLH(&ProjMat, D3DX_PI * 0.5f,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
//
//    // set the render mode as wireframe
//    D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(struct vertex));
//        D3DDevice->SetFVF(MYFVF);
//
//        // Draw one triangle.
//        D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}




// -------------------------------------
//  a rotating cube
// -------------------------------------
//
//void SetupPipeline()
//{
//    // fill a vertex buffer with a cube
//    D3DDevice->CreateVertexBuffer(8 * sizeof(struct vertex),
//                                  D3DUSAGE_WRITEONLY,
//                                  MYFVF,
//                                  D3DPOOL_MANAGED,
//                                  &VertexBuffer, 0);
//    vertex *Vertices;
//    VertexBuffer->Lock(0, 0, (void **) &Vertices, 0);
//    Vertices[0] = {-1.0f, -1.0f, -1.0f};
//    Vertices[1] = {-1.0f, 1.0f, -1.0f};
//    Vertices[2] = {1.0f, 1.0f, -1.0f};
//    Vertices[3] = {1.0f, -1.0f, -1.0f};
//    Vertices[4] = {-1.0f, -1.0f, 1.0f};
//    Vertices[5] = {-1.0f, 1.0f, 1.0f};
//    Vertices[6] = {1.0f, 1.0f, 1.0f};
//    Vertices[7] = {1.0f, -1.0f, 1.0f};
//    VertexBuffer->Unlock();
//
//    // create an index buffer for that triangle and fill it
//    D3DDevice->CreateIndexBuffer(36 * sizeof(WORD),
//                                 D3DUSAGE_WRITEONLY,
//                                 D3DFMT_INDEX16,
//                                 D3DPOOL_MANAGED,
//                                 &IndexBuffer,
//                                 0);
//    WORD *indices = 0;
//    IndexBuffer->Lock(0, 0, (void **) &indices, 0);
//    // front side
//    indices[0] = 0; indices[1] = 1; indices[2] = 2;
//    indices[3] = 0; indices[4] = 2; indices[5] = 3;
//    // back side
//    indices[6] = 4; indices[7] = 6; indices[8] = 5;
//    indices[9] = 4; indices[10] = 7; indices[11] = 6;
//    // left side
//    indices[12] = 4; indices[13] = 5; indices[14] = 1;
//    indices[15] = 4; indices[16] = 1; indices[17] = 0;
//    // right side
//    indices[18] = 3; indices[19] = 2; indices[20] = 6;
//    indices[21] = 3; indices[22] = 6; indices[23] = 7;
//    // top
//    indices[24] = 1; indices[25] = 5; indices[26] = 6;
//    indices[27] = 1; indices[28] = 6; indices[29] = 2;
//    // bottom
//    indices[30] = 4; indices[31] = 0; indices[32] = 3;
//    indices[33] = 4; indices[34] = 3; indices[35] = 7;
//    IndexBuffer->Unlock();
//
//    // view (camera) transformation
//    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f); // eye
//    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // at
//    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // up
//    D3DXMATRIX ViewTrans;
//    D3DXMatrixLookAtLH(&ViewTrans, &position, &target, &up);
//    D3DDevice->SetTransform(D3DTS_VIEW, &ViewTrans);
//
//    // setup the projection matrix
//    D3DXMATRIX ProjMat;
//    D3DXMatrixPerspectiveFovLH(&ProjMat, D3DX_PI * 0.5f,
//                               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
//                               1.0f, 1000.0f);
//    D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
//
//    // set the render mode as wireframe
//    D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//}
//
//
//int
//UpdateAndRender(float TimeDelta)
//{
//    if(D3DDevice)
//    {
//        // setup the world transform (rotating with time)
//        D3DXMATRIX XRotation, YRotation;
//
//        // rotate on x-axis with time
//        static float XAngle = 0.0f;
//        D3DXMatrixRotationX(&XRotation, 3.14f / 4.0f * XAngle);
//        XAngle += TimeDelta;
//        if(XAngle >= 6.28f)
//            XAngle = 0.0f;
//
//        // incremement Y-rotation angle with time
//        static float YAngle = 0.0f;
//        D3DXMatrixRotationY(&YRotation, YAngle);
//        YAngle += TimeDelta;
//
//        // reset angle to zero when angle reaches 2*PI
//        if(YAngle >= 6.28f)
//            YAngle = 0.0f;
//
//        // calculate the final world transform
//        D3DXMATRIX WorldTrans = XRotation * YRotation;
//        D3DDevice->SetTransform(D3DTS_WORLD, &WorldTrans);
//
//        D3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
//        D3DDevice->BeginScene();
//
//        D3DDevice->SetStreamSource(0, VertexBuffer, 0, sizeof(struct vertex));
//        D3DDevice->SetIndices(IndexBuffer);
//        D3DDevice->SetFVF(MYFVF);
//
//        // draw a cube
//        D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
//
//        D3DDevice->EndScene();
//        D3DDevice->Present(0, 0, 0, 0);
//    }
//    return 1;
//}