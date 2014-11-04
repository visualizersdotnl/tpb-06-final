#if 0
//
// FX Version: fx_4_0
// Child effect (requires effect pool): false
//
// 1 local buffer(s)
//
cbuffer paramsOnlyOnce
{
    float2  renderScale;                // Offset:    0, size:    8
    float4x4 viewProjMatrix;            // Offset:   16, size:   64
    float4x4 projMat;                   // Offset:   80, size:   64
    float4x4 mWorld;                    // Offset:  144, size:   64
    float4x4 mWorldInv;                 // Offset:  208, size:   64
    float   shininess;                  // Offset:  272, size:    4
    float   overbright;                 // Offset:  276, size:    4
}

//
// 3 local object(s)
//
Texture2D textureMap;
Texture2D projMap;
SamplerState samplerTexture
{
    AddressU = uint(WRAP /* 1 */);
    AddressV = uint(WRAP /* 1 */);
    Filter   = uint(MIN_MAG_MIP_LINEAR /* 21 */);
};

//
// 1 technique(s)
//
technique10 Blobs
{
    pass Default
    {
        VertexShader = asm {
            //
            // Generated by Microsoft (R) HLSL Shader Compiler 9.30.960.8229
            //
            //
            // Buffer Definitions: 
            //
            // cbuffer paramsOnlyOnce
            // {
            //
            //   float2 renderScale;                // Offset:    0 Size:     8 [unused]
            //   float4x4 viewProjMatrix;           // Offset:   16 Size:    64
            //   float4x4 projMat;                  // Offset:   80 Size:    64 [unused]
            //   float4x4 mWorld;                   // Offset:  144 Size:    64
            //   float4x4 mWorldInv;                // Offset:  208 Size:    64
            //   float shininess;                   // Offset:  272 Size:     4
            //   float overbright;                  // Offset:  276 Size:     4
            //
            // }
            //
            //
            // Resource Bindings:
            //
            // Name                                 Type  Format         Dim Slot Elements
            // ------------------------------ ---------- ------- ----------- ---- --------
            // paramsOnlyOnce                    cbuffer      NA          NA    0        1
            //
            //
            //
            // Input signature:
            //
            // Name                 Index   Mask Register SysValue  Format   Used
            // -------------------- ----- ------ -------- -------- ------- ------
            // POSITION                 0   xyzw        0     NONE   float   xyzw
            // NORMAL                   0   xyz         1     NONE   float   xyz 
            //
            //
            // Output signature:
            //
            // Name                 Index   Mask Register SysValue  Format   Used
            // -------------------- ----- ------ -------- -------- ------- ------
            // SV_Position              0   xyzw        0      POS   float   xyzw
            // COLOR                    0   xyzw        1     NONE   float   xyzw
            // TEXCOORD                 0   xy          2     NONE   float   xy  
            // TEXCOORD                 1     zw        2     NONE   float     zw
            // TEXCOORD                 2   xyz         3     NONE   float   xyz 
            // TEXCOORD                 3   xyz         4     NONE   float   xyz 
            //
            vs_4_0
            dcl_constantbuffer cb0[18], immediateIndexed
            dcl_input v0.xyzw
            dcl_input v1.xyz
            dcl_output_siv o0.xyzw, position
            dcl_output o1.xyzw
            dcl_output o2.xy
            dcl_output o2.zw
            dcl_output o3.xyz
            dcl_output o4.xyz
            dcl_temps 3
            dp4 r0.w, v0.xyzw, cb0[12].xyzw
            dp4 r0.z, v0.xyzw, cb0[11].xyzw
            dp4 r0.x, v0.xyzw, cb0[9].xyzw
            dp4 r0.y, v0.xyzw, cb0[10].xyzw
            dp4 o0.x, r0.xyzw, cb0[1].xyzw
            dp4 o0.y, r0.xyzw, cb0[2].xyzw
            dp4 o0.z, r0.xyzw, cb0[3].xyzw
            dp4 o0.w, r0.xyzw, cb0[4].xyzw
            add r1.x, -v0.x, cb0[13].z
            add r1.y, -v0.y, cb0[14].z
            add r1.z, -v0.z, cb0[15].z
            dp3 r0.w, r1.xyzx, r1.xyzx
            rsq r0.w, r0.w
            mul r1.xyz, r0.wwww, r1.xyzx
            add r2.xyz, r1.xyzx, r1.xyzx
            dp3 r0.w, v1.xyzx, r1.xyzx
            max r0.w, r0.w, l(0.000000)
            dp3 r1.x, r2.xyzx, r2.xyzx
            rsq r1.x, r1.x
            mul r1.xyz, r1.xxxx, r2.xyzx
            dp3 r1.x, v1.xyzx, r1.xyzx
            max r1.x, r1.x, l(0.000000)
            log r1.x, r1.x
            mul r1.x, r1.x, cb0[17].x
            exp r1.x, r1.x
            mad r0.w, cb0[17].y, r1.x, r0.w
            mul o1.xyz, r0.wwww, cb0[17].yyyy
            mov o1.w, l(1.000000)
            mov o2.zw, r0.xxxy
            dp3 r1.x, v1.xyzx, cb0[9].xyzx
            dp3 r1.y, v1.xyzx, cb0[10].xyzx
            mad o2.xy, r1.xyxx, l(0.500000, 0.500000, 0.000000, 0.000000), l(0.500000, 0.500000, 0.000000, 0.000000)
            mov o3.xy, r1.xyxx
            dp3 o3.z, v1.xyzx, cb0[11].xyzx
            mov r1.x, cb0[1].w
            mov r1.y, cb0[2].w
            mov r1.z, cb0[3].w
            add r0.xyz, -r0.xyzx, r1.xyzx
            dp3 r0.w, r0.xyzx, r0.xyzx
            rsq r0.w, r0.w
            mul o4.xyz, r0.wwww, r0.xyzx
            ret 
            // Approximately 42 instruction slots used
                    
        };
        PixelShader = asm {
            //
            // Generated by Microsoft (R) HLSL Shader Compiler 9.30.960.8229
            //
            //
            // Resource Bindings:
            //
            // Name                                 Type  Format         Dim Slot Elements
            // ------------------------------ ---------- ------- ----------- ---- --------
            // samplerTexture                    sampler      NA          NA    0        1
            // textureMap                        texture  float4          2d    0        1
            // projMap                           texture  float4          2d    1        1
            //
            //
            //
            // Input signature:
            //
            // Name                 Index   Mask Register SysValue  Format   Used
            // -------------------- ----- ------ -------- -------- ------- ------
            // SV_Position              0   xyzw        0      POS   float       
            // COLOR                    0   xyzw        1     NONE   float   xyzw
            // TEXCOORD                 0   xy          2     NONE   float   xy  
            // TEXCOORD                 1     zw        2     NONE   float     zw
            // TEXCOORD                 2   xyz         3     NONE   float   xyz 
            // TEXCOORD                 3   xyz         4     NONE   float   xyz 
            //
            //
            // Output signature:
            //
            // Name                 Index   Mask Register SysValue  Format   Used
            // -------------------- ----- ------ -------- -------- ------- ------
            // SV_Target                0   xyzw        0   TARGET   float   xyzw
            //
            ps_4_0
            dcl_sampler s0, mode_default
            dcl_resource_texture2d (float,float,float,float) t0
            dcl_resource_texture2d (float,float,float,float) t1
            dcl_input_ps linear v1.xyzw
            dcl_input_ps linear v2.xy
            dcl_input_ps linear v2.zw
            dcl_input_ps linear v3.xyz
            dcl_input_ps linear v4.xyz
            dcl_output o0.xyzw
            dcl_temps 3
            dp3 r0.x, v4.xyzx, v4.xyzx
            rsq r0.x, r0.x
            mul r0.xyz, r0.xxxx, v4.xyzx
            dp3 r0.w, v3.xyzx, v3.xyzx
            rsq r0.w, r0.w
            mul r1.xyz, r0.wwww, v3.xyzx
            dp3 r0.x, r0.xyzx, r1.xyzx
            add r0.x, r0.x, l(-0.200000)
            mul r0.x, r0.x, l(8.000000)
            mov_sat r0.xyz, r0.xxxx
            sample r1.xyzw, v2.xyxx, t0.xyzw, s0
            sample r2.xyzw, v2.zwzz, t1.xyzw, s0
            mul r1.xyzw, r1.xyzw, r2.xyzw
            mul r1.xyzw, r1.xyzw, v1.xyzw
            mov r0.w, l(1.000000)
            mul o0.xyzw, r0.xyzw, r1.xyzw
            ret 
            // Approximately 17 instruction slots used
                    
        };
    }

}

#endif

const BYTE gCompiledShader_Blobs[] =
{
     68,  88,  66,  67,  62,  13, 
     35, 200, 108,  93, 218, 225, 
    123, 131, 249,  79, 118, 114, 
     20, 135,   1,   0,   0,   0, 
    237,  16,   0,   0,   1,   0, 
      0,   0,  36,   0,   0,   0, 
     70,  88,  49,  48, 193,  16, 
      0,   0,   1,  16, 255, 254, 
      1,   0,   0,   0,   7,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 241,  14, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0, 112,  97, 
    114,  97, 109, 115,  79, 110, 
    108, 121,  79, 110,  99, 101, 
      0, 102, 108, 111,  97, 116, 
     50,   0,  19,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     16,   0,   0,   0,   8,   0, 
      0,   0,  10,  17,   0,   0, 
    114, 101, 110, 100, 101, 114, 
     83,  99,  97, 108, 101,   0, 
    102, 108, 111,  97, 116,  52, 
    120,  52,   0,  66,   0,   0, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   0,  64,   0,   0, 
      0,  64,   0,   0,   0,  64, 
      0,   0,   0,  11, 100,   0, 
      0, 118, 105, 101, 119,  80, 
    114, 111, 106,  77,  97, 116, 
    114, 105, 120,   0, 112, 114, 
    111, 106,  77,  97, 116,   0, 
    109,  87, 111, 114, 108, 100, 
      0, 109,  87, 111, 114, 108, 
    100,  73, 110, 118,   0, 102, 
    108, 111,  97, 116,   0, 143, 
      0,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,   4, 
      0,   0,   0,  16,   0,   0, 
      0,   4,   0,   0,   0,   9, 
      9,   0,   0, 115, 104, 105, 
    110, 105, 110, 101, 115, 115, 
      0, 111, 118, 101, 114,  98, 
    114, 105, 103, 104, 116,   0, 
     84, 101, 120, 116, 117, 114, 
    101,  50,  68,   0, 198,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  12,   0, 
      0,   0, 116, 101, 120, 116, 
    117, 114, 101,  77,  97, 112, 
      0, 112, 114, 111, 106,  77, 
     97, 112,   0,  83,  97, 109, 
    112, 108, 101, 114,  83, 116, 
     97, 116, 101,   0, 255,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  21,   0, 
      0,   0, 115,  97, 109, 112, 
    108, 101, 114,  84, 101, 120, 
    116, 117, 114, 101,   0,   1, 
      0,   0,   0,   2,   0,   0, 
      0,   1,   0,   0,   0,   1, 
      0,   0,   0,   2,   0,   0, 
      0,   1,   0,   0,   0,   1, 
      0,   0,   0,   2,   0,   0, 
      0,  21,   0,   0,   0,  66, 
    108, 111,  98, 115,   0,  68, 
    101, 102,  97, 117, 108, 116, 
      0, 184,   8,   0,   0,  68, 
     88,  66,  67, 103, 182, 109, 
      3, 131, 247,  36, 173, 207, 
    247, 149,  93, 251, 100, 240, 
    229,   1,   0,   0,   0, 184, 
      8,   0,   0,   5,   0,   0, 
      0,  52,   0,   0,   0, 244, 
      1,   0,   0,  68,   2,   0, 
      0,   0,   3,   0,   0,  60, 
      8,   0,   0,  82,  68,  69, 
     70, 184,   1,   0,   0,   1, 
      0,   0,   0,  76,   0,   0, 
      0,   1,   0,   0,   0,  28, 
      0,   0,   0,   0,   4, 254, 
    255,   0,   1,   0,   0, 135, 
      1,   0,   0,  60,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0, 112, 
     97, 114,  97, 109, 115,  79, 
    110, 108, 121,  79, 110,  99, 
    101,   0, 171,  60,   0,   0, 
      0,   7,   0,   0,   0, 100, 
      0,   0,   0,  32,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  12,   1,   0, 
      0,   0,   0,   0,   0,   8, 
      0,   0,   0,   0,   0,   0, 
      0,  24,   1,   0,   0,   0, 
      0,   0,   0,  40,   1,   0, 
      0,  16,   0,   0,   0,  64, 
      0,   0,   0,   2,   0,   0, 
      0,  56,   1,   0,   0,   0, 
      0,   0,   0,  72,   1,   0, 
      0,  80,   0,   0,   0,  64, 
      0,   0,   0,   0,   0,   0, 
      0,  56,   1,   0,   0,   0, 
      0,   0,   0,  80,   1,   0, 
      0, 144,   0,   0,   0,  64, 
      0,   0,   0,   2,   0,   0, 
      0,  56,   1,   0,   0,   0, 
      0,   0,   0,  87,   1,   0, 
      0, 208,   0,   0,   0,  64, 
      0,   0,   0,   2,   0,   0, 
      0,  56,   1,   0,   0,   0, 
      0,   0,   0,  97,   1,   0, 
      0,  16,   1,   0,   0,   4, 
      0,   0,   0,   2,   0,   0, 
      0, 108,   1,   0,   0,   0, 
      0,   0,   0, 124,   1,   0, 
      0,  20,   1,   0,   0,   4, 
      0,   0,   0,   2,   0,   0, 
      0, 108,   1,   0,   0,   0, 
      0,   0,   0, 114, 101, 110, 
    100, 101, 114,  83,  99,  97, 
    108, 101,   0,   1,   0,   3, 
      0,   1,   0,   2,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 118, 105, 101, 119,  80, 
    114, 111, 106,  77,  97, 116, 
    114, 105, 120,   0, 171,   3, 
      0,   3,   0,   4,   0,   4, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 112, 114, 111, 
    106,  77,  97, 116,   0, 109, 
     87, 111, 114, 108, 100,   0, 
    109,  87, 111, 114, 108, 100, 
     73, 110, 118,   0, 115, 104, 
    105, 110, 105, 110, 101, 115, 
    115,   0, 171,   0,   0,   3, 
      0,   1,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 111, 118, 101, 114,  98, 
    114, 105, 103, 104, 116,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  57, 
     46,  51,  48,  46,  57,  54, 
     48,  46,  56,  50,  50,  57, 
      0,  73,  83,  71,  78,  72, 
      0,   0,   0,   2,   0,   0, 
      0,   8,   0,   0,   0,  56, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   3, 
      0,   0,   0,   0,   0,   0, 
      0,  15,  15,   0,   0,  65, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   3, 
      0,   0,   0,   1,   0,   0, 
      0,   7,   7,   0,   0,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  78,  79,  82,  77, 
     65,  76,   0,  79,  83,  71, 
     78, 180,   0,   0,   0,   6, 
      0,   0,   0,   8,   0,   0, 
      0, 152,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,   3,   0,   0,   0,   0, 
      0,   0,   0,  15,   0,   0, 
      0, 164,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   1, 
      0,   0,   0,  15,   0,   0, 
      0, 170,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   2, 
      0,   0,   0,   3,  12,   0, 
      0, 170,   0,   0,   0,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   2, 
      0,   0,   0,  12,   3,   0, 
      0, 170,   0,   0,   0,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   3, 
      0,   0,   0,   7,   8,   0, 
      0, 170,   0,   0,   0,   3, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   4, 
      0,   0,   0,   7,   8,   0, 
      0,  83,  86,  95,  80, 111, 
    115, 105, 116, 105, 111, 110, 
      0,  67,  79,  76,  79,  82, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0, 171,  83, 
     72,  68,  82,  52,   5,   0, 
      0,  64,   0,   1,   0,  77, 
      1,   0,   0,  89,   0,   0, 
      4,  70, 142,  32,   0,   0, 
      0,   0,   0,  18,   0,   0, 
      0,  95,   0,   0,   3, 242, 
     16,  16,   0,   0,   0,   0, 
      0,  95,   0,   0,   3, 114, 
     16,  16,   0,   1,   0,   0, 
      0, 103,   0,   0,   4, 242, 
     32,  16,   0,   0,   0,   0, 
      0,   1,   0,   0,   0, 101, 
      0,   0,   3, 242,  32,  16, 
      0,   1,   0,   0,   0, 101, 
      0,   0,   3,  50,  32,  16, 
      0,   2,   0,   0,   0, 101, 
      0,   0,   3, 194,  32,  16, 
      0,   2,   0,   0,   0, 101, 
      0,   0,   3, 114,  32,  16, 
      0,   3,   0,   0,   0, 101, 
      0,   0,   3, 114,  32,  16, 
      0,   4,   0,   0,   0, 104, 
      0,   0,   2,   3,   0,   0, 
      0,  17,   0,   0,   8, 130, 
      0,  16,   0,   0,   0,   0, 
      0,  70,  30,  16,   0,   0, 
      0,   0,   0,  70, 142,  32, 
      0,   0,   0,   0,   0,  12, 
      0,   0,   0,  17,   0,   0, 
      8,  66,   0,  16,   0,   0, 
      0,   0,   0,  70,  30,  16, 
      0,   0,   0,   0,   0,  70, 
    142,  32,   0,   0,   0,   0, 
      0,  11,   0,   0,   0,  17, 
      0,   0,   8,  18,   0,  16, 
      0,   0,   0,   0,   0,  70, 
     30,  16,   0,   0,   0,   0, 
      0,  70, 142,  32,   0,   0, 
      0,   0,   0,   9,   0,   0, 
      0,  17,   0,   0,   8,  34, 
      0,  16,   0,   0,   0,   0, 
      0,  70,  30,  16,   0,   0, 
      0,   0,   0,  70, 142,  32, 
      0,   0,   0,   0,   0,  10, 
      0,   0,   0,  17,   0,   0, 
      8,  18,  32,  16,   0,   0, 
      0,   0,   0,  70,  14,  16, 
      0,   0,   0,   0,   0,  70, 
    142,  32,   0,   0,   0,   0, 
      0,   1,   0,   0,   0,  17, 
      0,   0,   8,  34,  32,  16, 
      0,   0,   0,   0,   0,  70, 
     14,  16,   0,   0,   0,   0, 
      0,  70, 142,  32,   0,   0, 
      0,   0,   0,   2,   0,   0, 
      0,  17,   0,   0,   8,  66, 
     32,  16,   0,   0,   0,   0, 
      0,  70,  14,  16,   0,   0, 
      0,   0,   0,  70, 142,  32, 
      0,   0,   0,   0,   0,   3, 
      0,   0,   0,  17,   0,   0, 
      8, 130,  32,  16,   0,   0, 
      0,   0,   0,  70,  14,  16, 
      0,   0,   0,   0,   0,  70, 
    142,  32,   0,   0,   0,   0, 
      0,   4,   0,   0,   0,   0, 
      0,   0,   9,  18,   0,  16, 
      0,   1,   0,   0,   0,  10, 
     16,  16, 128,  65,   0,   0, 
      0,   0,   0,   0,   0,  42, 
    128,  32,   0,   0,   0,   0, 
      0,  13,   0,   0,   0,   0, 
      0,   0,   9,  34,   0,  16, 
      0,   1,   0,   0,   0,  26, 
     16,  16, 128,  65,   0,   0, 
      0,   0,   0,   0,   0,  42, 
    128,  32,   0,   0,   0,   0, 
      0,  14,   0,   0,   0,   0, 
      0,   0,   9,  66,   0,  16, 
      0,   1,   0,   0,   0,  42, 
     16,  16, 128,  65,   0,   0, 
      0,   0,   0,   0,   0,  42, 
    128,  32,   0,   0,   0,   0, 
      0,  15,   0,   0,   0,  16, 
      0,   0,   7, 130,   0,  16, 
      0,   0,   0,   0,   0,  70, 
      2,  16,   0,   1,   0,   0, 
      0,  70,   2,  16,   0,   1, 
      0,   0,   0,  68,   0,   0, 
      5, 130,   0,  16,   0,   0, 
      0,   0,   0,  58,   0,  16, 
      0,   0,   0,   0,   0,  56, 
      0,   0,   7, 114,   0,  16, 
      0,   1,   0,   0,   0, 246, 
     15,  16,   0,   0,   0,   0, 
      0,  70,   2,  16,   0,   1, 
      0,   0,   0,   0,   0,   0, 
      7, 114,   0,  16,   0,   2, 
      0,   0,   0,  70,   2,  16, 
      0,   1,   0,   0,   0,  70, 
      2,  16,   0,   1,   0,   0, 
      0,  16,   0,   0,   7, 130, 
      0,  16,   0,   0,   0,   0, 
      0,  70,  18,  16,   0,   1, 
      0,   0,   0,  70,   2,  16, 
      0,   1,   0,   0,   0,  52, 
      0,   0,   7, 130,   0,  16, 
      0,   0,   0,   0,   0,  58, 
      0,  16,   0,   0,   0,   0, 
      0,   1,  64,   0,   0,   0, 
      0,   0,   0,  16,   0,   0, 
      7,  18,   0,  16,   0,   1, 
      0,   0,   0,  70,   2,  16, 
      0,   2,   0,   0,   0,  70, 
      2,  16,   0,   2,   0,   0, 
      0,  68,   0,   0,   5,  18, 
      0,  16,   0,   1,   0,   0, 
      0,  10,   0,  16,   0,   1, 
      0,   0,   0,  56,   0,   0, 
      7, 114,   0,  16,   0,   1, 
      0,   0,   0,   6,   0,  16, 
      0,   1,   0,   0,   0,  70, 
      2,  16,   0,   2,   0,   0, 
      0,  16,   0,   0,   7,  18, 
      0,  16,   0,   1,   0,   0, 
      0,  70,  18,  16,   0,   1, 
      0,   0,   0,  70,   2,  16, 
      0,   1,   0,   0,   0,  52, 
      0,   0,   7,  18,   0,  16, 
      0,   1,   0,   0,   0,  10, 
      0,  16,   0,   1,   0,   0, 
      0,   1,  64,   0,   0,   0, 
      0,   0,   0,  47,   0,   0, 
      5,  18,   0,  16,   0,   1, 
      0,   0,   0,  10,   0,  16, 
      0,   1,   0,   0,   0,  56, 
      0,   0,   8,  18,   0,  16, 
      0,   1,   0,   0,   0,  10, 
      0,  16,   0,   1,   0,   0, 
      0,  10, 128,  32,   0,   0, 
      0,   0,   0,  17,   0,   0, 
      0,  25,   0,   0,   5,  18, 
      0,  16,   0,   1,   0,   0, 
      0,  10,   0,  16,   0,   1, 
      0,   0,   0,  50,   0,   0, 
     10, 130,   0,  16,   0,   0, 
      0,   0,   0,  26, 128,  32, 
      0,   0,   0,   0,   0,  17, 
      0,   0,   0,  10,   0,  16, 
      0,   1,   0,   0,   0,  58, 
      0,  16,   0,   0,   0,   0, 
      0,  56,   0,   0,   8, 114, 
     32,  16,   0,   1,   0,   0, 
      0, 246,  15,  16,   0,   0, 
      0,   0,   0,  86, 133,  32, 
      0,   0,   0,   0,   0,  17, 
      0,   0,   0,  54,   0,   0, 
      5, 130,  32,  16,   0,   1, 
      0,   0,   0,   1,  64,   0, 
      0,   0,   0, 128,  63,  54, 
      0,   0,   5, 194,  32,  16, 
      0,   2,   0,   0,   0,   6, 
      4,  16,   0,   0,   0,   0, 
      0,  16,   0,   0,   8,  18, 
      0,  16,   0,   1,   0,   0, 
      0,  70,  18,  16,   0,   1, 
      0,   0,   0,  70, 130,  32, 
      0,   0,   0,   0,   0,   9, 
      0,   0,   0,  16,   0,   0, 
      8,  34,   0,  16,   0,   1, 
      0,   0,   0,  70,  18,  16, 
      0,   1,   0,   0,   0,  70, 
    130,  32,   0,   0,   0,   0, 
      0,  10,   0,   0,   0,  50, 
      0,   0,  15,  50,  32,  16, 
      0,   2,   0,   0,   0,  70, 
      0,  16,   0,   1,   0,   0, 
      0,   2,  64,   0,   0,   0, 
      0,   0,  63,   0,   0,   0, 
     63,   0,   0,   0,   0,   0, 
      0,   0,   0,   2,  64,   0, 
      0,   0,   0,   0,  63,   0, 
      0,   0,  63,   0,   0,   0, 
      0,   0,   0,   0,   0,  54, 
      0,   0,   5,  50,  32,  16, 
      0,   3,   0,   0,   0,  70, 
      0,  16,   0,   1,   0,   0, 
      0,  16,   0,   0,   8,  66, 
     32,  16,   0,   3,   0,   0, 
      0,  70,  18,  16,   0,   1, 
      0,   0,   0,  70, 130,  32, 
      0,   0,   0,   0,   0,  11, 
      0,   0,   0,  54,   0,   0, 
      6,  18,   0,  16,   0,   1, 
      0,   0,   0,  58, 128,  32, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,  54,   0,   0, 
      6,  34,   0,  16,   0,   1, 
      0,   0,   0,  58, 128,  32, 
      0,   0,   0,   0,   0,   2, 
      0,   0,   0,  54,   0,   0, 
      6,  66,   0,  16,   0,   1, 
      0,   0,   0,  58, 128,  32, 
      0,   0,   0,   0,   0,   3, 
      0,   0,   0,   0,   0,   0, 
      8, 114,   0,  16,   0,   0, 
      0,   0,   0,  70,   2,  16, 
    128,  65,   0,   0,   0,   0, 
      0,   0,   0,  70,   2,  16, 
      0,   1,   0,   0,   0,  16, 
      0,   0,   7, 130,   0,  16, 
      0,   0,   0,   0,   0,  70, 
      2,  16,   0,   0,   0,   0, 
      0,  70,   2,  16,   0,   0, 
      0,   0,   0,  68,   0,   0, 
      5, 130,   0,  16,   0,   0, 
      0,   0,   0,  58,   0,  16, 
      0,   0,   0,   0,   0,  56, 
      0,   0,   7, 114,  32,  16, 
      0,   4,   0,   0,   0, 246, 
     15,  16,   0,   0,   0,   0, 
      0,  70,   2,  16,   0,   0, 
      0,   0,   0,  62,   0,   0, 
      1,  83,  84,  65,  84, 116, 
      0,   0,   0,  42,   0,   0, 
      0,   3,   0,   0,   0,   0, 
      0,   0,   0,   8,   0,   0, 
      0,  33,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   7,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 105, 
      1,   0,   0,   0,   0,   0, 
      0, 184,   4,   0,   0,  68, 
     88,  66,  67, 157, 232,  32, 
     97, 210, 158,  42, 137,  19, 
    234,  19, 148, 234, 240, 232, 
    243,   1,   0,   0,   0, 184, 
      4,   0,   0,   5,   0,   0, 
      0,  52,   0,   0,   0,  12, 
      1,   0,   0, 200,   1,   0, 
      0, 252,   1,   0,   0,  60, 
      4,   0,   0,  82,  68,  69, 
     70, 208,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,  28, 
      0,   0,   0,   0,   4, 255, 
    255,   0,   1,   0,   0, 158, 
      0,   0,   0, 124,   0,   0, 
      0,   3,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0, 139, 
      0,   0,   0,   2,   0,   0, 
      0,   5,   0,   0,   0,   4, 
      0,   0,   0, 255, 255, 255, 
    255,   0,   0,   0,   0,   1, 
      0,   0,   0,  12,   0,   0, 
      0, 150,   0,   0,   0,   2, 
      0,   0,   0,   5,   0,   0, 
      0,   4,   0,   0,   0, 255, 
    255, 255, 255,   1,   0,   0, 
      0,   1,   0,   0,   0,  12, 
      0,   0,   0, 115,  97, 109, 
    112, 108, 101, 114,  84, 101, 
    120, 116, 117, 114, 101,   0, 
    116, 101, 120, 116, 117, 114, 
    101,  77,  97, 112,   0, 112, 
    114, 111, 106,  77,  97, 112, 
      0,  77, 105,  99, 114, 111, 
    115, 111, 102, 116,  32,  40, 
     82,  41,  32,  72,  76,  83, 
     76,  32,  83, 104,  97, 100, 
    101, 114,  32,  67, 111, 109, 
    112, 105, 108, 101, 114,  32, 
     57,  46,  51,  48,  46,  57, 
     54,  48,  46,  56,  50,  50, 
     57,   0, 171,  73,  83,  71, 
     78, 180,   0,   0,   0,   6, 
      0,   0,   0,   8,   0,   0, 
      0, 152,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,   3,   0,   0,   0,   0, 
      0,   0,   0,  15,   0,   0, 
      0, 164,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   1, 
      0,   0,   0,  15,  15,   0, 
      0, 170,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   2, 
      0,   0,   0,   3,   3,   0, 
      0, 170,   0,   0,   0,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   2, 
      0,   0,   0,  12,  12,   0, 
      0, 170,   0,   0,   0,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   3, 
      0,   0,   0,   7,   7,   0, 
      0, 170,   0,   0,   0,   3, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   4, 
      0,   0,   0,   7,   7,   0, 
      0,  83,  86,  95,  80, 111, 
    115, 105, 116, 105, 111, 110, 
      0,  67,  79,  76,  79,  82, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0, 171,  79, 
     83,  71,  78,  44,   0,   0, 
      0,   1,   0,   0,   0,   8, 
      0,   0,   0,  32,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   3,   0,   0, 
      0,   0,   0,   0,   0,  15, 
      0,   0,   0,  83,  86,  95, 
     84,  97, 114, 103, 101, 116, 
      0, 171, 171,  83,  72,  68, 
     82,  56,   2,   0,   0,  64, 
      0,   0,   0, 142,   0,   0, 
      0,  90,   0,   0,   3,   0, 
     96,  16,   0,   0,   0,   0, 
      0,  88,  24,   0,   4,   0, 
    112,  16,   0,   0,   0,   0, 
      0,  85,  85,   0,   0,  88, 
     24,   0,   4,   0, 112,  16, 
      0,   1,   0,   0,   0,  85, 
     85,   0,   0,  98,  16,   0, 
      3, 242,  16,  16,   0,   1, 
      0,   0,   0,  98,  16,   0, 
      3,  50,  16,  16,   0,   2, 
      0,   0,   0,  98,  16,   0, 
      3, 194,  16,  16,   0,   2, 
      0,   0,   0,  98,  16,   0, 
      3, 114,  16,  16,   0,   3, 
      0,   0,   0,  98,  16,   0, 
      3, 114,  16,  16,   0,   4, 
      0,   0,   0, 101,   0,   0, 
      3, 242,  32,  16,   0,   0, 
      0,   0,   0, 104,   0,   0, 
      2,   3,   0,   0,   0,  16, 
      0,   0,   7,  18,   0,  16, 
      0,   0,   0,   0,   0,  70, 
     18,  16,   0,   4,   0,   0, 
      0,  70,  18,  16,   0,   4, 
      0,   0,   0,  68,   0,   0, 
      5,  18,   0,  16,   0,   0, 
      0,   0,   0,  10,   0,  16, 
      0,   0,   0,   0,   0,  56, 
      0,   0,   7, 114,   0,  16, 
      0,   0,   0,   0,   0,   6, 
      0,  16,   0,   0,   0,   0, 
      0,  70,  18,  16,   0,   4, 
      0,   0,   0,  16,   0,   0, 
      7, 130,   0,  16,   0,   0, 
      0,   0,   0,  70,  18,  16, 
      0,   3,   0,   0,   0,  70, 
     18,  16,   0,   3,   0,   0, 
      0,  68,   0,   0,   5, 130, 
      0,  16,   0,   0,   0,   0, 
      0,  58,   0,  16,   0,   0, 
      0,   0,   0,  56,   0,   0, 
      7, 114,   0,  16,   0,   1, 
      0,   0,   0, 246,  15,  16, 
      0,   0,   0,   0,   0,  70, 
     18,  16,   0,   3,   0,   0, 
      0,  16,   0,   0,   7,  18, 
      0,  16,   0,   0,   0,   0, 
      0,  70,   2,  16,   0,   0, 
      0,   0,   0,  70,   2,  16, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   7,  18,   0,  16, 
      0,   0,   0,   0,   0,  10, 
      0,  16,   0,   0,   0,   0, 
      0,   1,  64,   0,   0, 205, 
    204,  76, 190,  56,   0,   0, 
      7,  18,   0,  16,   0,   0, 
      0,   0,   0,  10,   0,  16, 
      0,   0,   0,   0,   0,   1, 
     64,   0,   0,   0,   0,   0, 
     65,  54,  32,   0,   5, 114, 
      0,  16,   0,   0,   0,   0, 
      0,   6,   0,  16,   0,   0, 
      0,   0,   0,  69,   0,   0, 
      9, 242,   0,  16,   0,   1, 
      0,   0,   0,  70,  16,  16, 
      0,   2,   0,   0,   0,  70, 
    126,  16,   0,   0,   0,   0, 
      0,   0,  96,  16,   0,   0, 
      0,   0,   0,  69,   0,   0, 
      9, 242,   0,  16,   0,   2, 
      0,   0,   0, 230,  26,  16, 
      0,   2,   0,   0,   0,  70, 
    126,  16,   0,   1,   0,   0, 
      0,   0,  96,  16,   0,   0, 
      0,   0,   0,  56,   0,   0, 
      7, 242,   0,  16,   0,   1, 
      0,   0,   0,  70,  14,  16, 
      0,   1,   0,   0,   0,  70, 
     14,  16,   0,   2,   0,   0, 
      0,  56,   0,   0,   7, 242, 
      0,  16,   0,   1,   0,   0, 
      0,  70,  14,  16,   0,   1, 
      0,   0,   0,  70,  30,  16, 
      0,   1,   0,   0,   0,  54, 
      0,   0,   5, 130,   0,  16, 
      0,   0,   0,   0,   0,   1, 
     64,   0,   0,   0,   0, 128, 
     63,  56,   0,   0,   7, 242, 
     32,  16,   0,   0,   0,   0, 
      0,  70,  14,  16,   0,   0, 
      0,   0,   0,  70,  14,  16, 
      0,   1,   0,   0,   0,  62, 
      0,   0,   1,  83,  84,  65, 
     84, 116,   0,   0,   0,  17, 
      0,   0,   0,   3,   0,   0, 
      0,   0,   0,   0,   0,   6, 
      0,   0,   0,  12,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   3,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  45,  10,   0,   0,   0, 
      0,   0,   0,   4,   0,   0, 
      0,  32,   1,   0,   0,   0, 
      0,   0,   0,   7,   0,   0, 
      0, 255, 255, 255, 255,   0, 
      0,   0,   0,  54,   0,   0, 
      0,  26,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 103,   0,   0,   0,  75, 
      0,   0,   0,   0,   0,   0, 
      0,  16,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 118, 
      0,   0,   0,  75,   0,   0, 
      0,   0,   0,   0,   0,  80, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 126,   0,   0, 
      0,  75,   0,   0,   0,   0, 
      0,   0,   0, 144,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 133,   0,   0,   0,  75, 
      0,   0,   0,   0,   0,   0, 
      0, 208,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 177, 
      0,   0,   0, 149,   0,   0, 
      0,   0,   0,   0,   0,  16, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 187,   0,   0, 
      0, 149,   0,   0,   0,   0, 
      0,   0,   0,  20,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 236,   0,   0,   0, 208, 
      0,   0,   0,   0,   0,   0, 
      0, 255, 255, 255, 255,   0, 
      0,   0,   0, 247,   0,   0, 
      0, 208,   0,   0,   0,   0, 
      0,   0,   0, 255, 255, 255, 
    255,   0,   0,   0,   0,  40, 
      1,   0,   0,  12,   1,   0, 
      0,   0,   0,   0,   0, 255, 
    255, 255, 255,   3,   0,   0, 
      0,  46,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,  55,   1,   0,   0,  47, 
      0,   0,   0,   0,   0,   0, 
      0,   1,   0,   0,   0,  67, 
      1,   0,   0,  45,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,  79,   1,   0, 
      0,   0,   0,   0,   0,  91, 
      1,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,  97, 
      1,   0,   0,   2,   0,   0, 
      0,   0,   0,   0,   0,   6, 
      0,   0,   0,   0,   0,   0, 
      0,   7,   0,   0,   0,  37, 
     10,   0,   0,   7,   0,   0, 
      0,   0,   0,   0,   0,   7, 
      0,   0,   0, 233,  14,   0, 
      0
};
