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
    float4x4 viewProjMatrixInv;         // Offset:   80, size:   64
    float4x4 mWorld;                    // Offset:  144, size:   64
    float4x4 mWorldInv;                 // Offset:  208, size:   64
}

//
// 3 local object(s)
//
Texture2D textureMap;
Texture2D projMap;
SamplerState samplerTexture
{
    AddressU = uint(CLAMP /* 3 */);
    AddressV = uint(CLAMP /* 3 */);
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
            //   float4x4 viewProjMatrixInv;        // Offset:   80 Size:    64 [unused]
            //   float4x4 mWorld;                   // Offset:  144 Size:    64
            //   float4x4 mWorldInv;                // Offset:  208 Size:    64
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
            //
            vs_4_0
            dcl_constantbuffer cb0[16], immediateIndexed
            dcl_input v0.xyzw
            dcl_input v1.xyz
            dcl_output_siv o0.xyzw, position
            dcl_output o1.xyzw
            dcl_output o2.xy
            dcl_output o2.zw
            dcl_temps 2
            dp4 r0.x, v0.xyzw, cb0[9].xyzw
            dp4 r0.y, v0.xyzw, cb0[10].xyzw
            dp4 r0.z, v0.xyzw, cb0[11].xyzw
            dp4 r0.w, v0.xyzw, cb0[12].xyzw
            dp4 o0.x, r0.xyzw, cb0[1].xyzw
            dp4 o0.y, r0.xyzw, cb0[2].xyzw
            dp4 o0.z, r0.xyzw, cb0[3].xyzw
            dp4 o0.w, r0.xyzw, cb0[4].xyzw
            add r0.x, -v0.x, cb0[13].z
            add r0.y, -v0.y, cb0[14].z
            add r0.z, -v0.z, cb0[15].z
            dp3 r0.w, r0.xyzx, r0.xyzx
            rsq r0.w, r0.w
            mul r0.xyz, r0.wwww, r0.xyzx
            add r1.xyz, r0.xyzx, r0.xyzx
            dp3 r0.x, v1.xyzx, r0.xyzx
            dp3 r0.y, r1.xyzx, r1.xyzx
            rsq r0.y, r0.y
            mul r0.yzw, r0.yyyy, r1.xxyz
            dp3 r0.y, v1.xyzx, r0.yzwy
            max r0.xy, r0.xyxx, l(0.000000, 0.000000, 0.000000, 0.000000)
            log r0.y, r0.y
            mul r0.y, r0.y, l(9.000000)
            exp r0.y, r0.y
            mul r0.y, r0.y, l(1.500000)
            mad o1.xyz, r0.xxxx, l(1.200000, 1.200000, 1.200000, 0.000000), r0.yyyy
            mov o1.w, l(1.000000)
            dp3 r0.x, v1.xyzx, cb0[9].xyzx
            dp3 r0.y, v1.xyzx, cb0[10].xyzx
            mad o2.xy, r0.xyxx, l(0.500000, 0.500000, 0.000000, 0.000000), l(0.500000, 0.500000, 0.000000, 0.000000)
            mov o2.zw, l(0,0,0,0)
            ret 
            // Approximately 32 instruction slots used
                    
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
            // TEXCOORD                 1     zw        2     NONE   float       
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
            dcl_input_ps linear v1.xyzw
            dcl_input_ps linear v2.xy
            dcl_output o0.xyzw
            dcl_temps 1
            sample r0.xyzw, v2.xyxx, t0.xyzw, s0
            mul o0.xyzw, r0.xyzw, v1.xyzw
            ret 
            // Approximately 3 instruction slots used
                    
        };
    }

}

#endif

const BYTE gCompiledShader_Blobs[] =
{
     68,  88,  66,  67, 103,  12, 
    210, 170, 206, 191,  62,  62, 
    208,  74, 175, 236, 185,  47, 
    215, 215,   1,   0,   0,   0, 
     12,  13,   0,   0,   1,   0, 
      0,   0,  36,   0,   0,   0, 
     70,  88,  49,  48, 224,  12, 
      0,   0,   1,  16, 255, 254, 
      1,   0,   0,   0,   5,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  72,  11, 
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
    114, 105, 120,   0, 118, 105, 
    101, 119,  80, 114, 111, 106, 
     77,  97, 116, 114, 105, 120, 
     73, 110, 118,   0, 109,  87, 
    111, 114, 108, 100,   0, 109, 
     87, 111, 114, 108, 100,  73, 
    110, 118,   0,  84, 101, 120, 
    116, 117, 114, 101,  50,  68, 
      0, 153,   0,   0,   0,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  12,   0,   0,   0, 116, 
    101, 120, 116, 117, 114, 101, 
     77,  97, 112,   0, 112, 114, 
    111, 106,  77,  97, 112,   0, 
     83,  97, 109, 112, 108, 101, 
    114,  83, 116,  97, 116, 101, 
      0, 210,   0,   0,   0,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  21,   0,   0,   0, 115, 
     97, 109, 112, 108, 101, 114, 
     84, 101, 120, 116, 117, 114, 
    101,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  21,   0, 
      0,   0,  66, 108, 111,  98, 
    115,   0,  68, 101, 102,  97, 
    117, 108, 116,   0,  56,   7, 
      0,   0,  68,  88,  66,  67, 
     41, 137, 186,  80,   3,  77, 
     11,  34, 224,  76, 235, 226, 
    183,  78, 250, 157,   1,   0, 
      0,   0,  56,   7,   0,   0, 
      5,   0,   0,   0,  52,   0, 
      0,   0, 168,   1,   0,   0, 
    248,   1,   0,   0, 132,   2, 
      0,   0, 188,   6,   0,   0, 
     82,  68,  69,  70, 108,   1, 
      0,   0,   1,   0,   0,   0, 
     76,   0,   0,   0,   1,   0, 
      0,   0,  28,   0,   0,   0, 
      0,   4, 254, 255,   0,   1, 
      0,   0,  59,   1,   0,   0, 
     60,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 112,  97, 114,  97, 
    109, 115,  79, 110, 108, 121, 
     79, 110,  99, 101,   0, 171, 
     60,   0,   0,   0,   5,   0, 
      0,   0, 100,   0,   0,   0, 
     16,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    220,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0, 232,   0, 
      0,   0,   0,   0,   0,   0, 
    248,   0,   0,   0,  16,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0,   8,   1, 
      0,   0,   0,   0,   0,   0, 
     24,   1,   0,   0,  80,   0, 
      0,   0,  64,   0,   0,   0, 
      0,   0,   0,   0,   8,   1, 
      0,   0,   0,   0,   0,   0, 
     42,   1,   0,   0, 144,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0,   8,   1, 
      0,   0,   0,   0,   0,   0, 
     49,   1,   0,   0, 208,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0,   8,   1, 
      0,   0,   0,   0,   0,   0, 
    114, 101, 110, 100, 101, 114, 
     83,  99,  97, 108, 101,   0, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 118, 105, 
    101, 119,  80, 114, 111, 106, 
     77,  97, 116, 114, 105, 120, 
      0, 171,   3,   0,   3,   0, 
      4,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    118, 105, 101, 119,  80, 114, 
    111, 106,  77,  97, 116, 114, 
    105, 120,  73, 110, 118,   0, 
    109,  87, 111, 114, 108, 100, 
      0, 109,  87, 111, 114, 108, 
    100,  73, 110, 118,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  57,  46, 
     51,  48,  46,  57,  54,  48, 
     46,  56,  50,  50,  57,   0, 
     73,  83,  71,  78,  72,   0, 
      0,   0,   2,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,  15,   0,   0,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   7,   0,   0,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  78,  79,  82,  77,  65, 
     76,   0,  79,  83,  71,  78, 
    132,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
    122,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,  12,   0,   0, 
    122,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  12,   3,   0,   0, 
     83,  86,  95,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  83,  72, 
     68,  82,  48,   4,   0,   0, 
     64,   0,   1,   0,  12,   1, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   1,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 194,  32,  16,   0, 
      2,   0,   0,   0, 104,   0, 
      0,   2,   2,   0,   0,   0, 
     17,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  30,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  17,   0,   0,   8, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  17,   0, 
      0,   8,  66,   0,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,  11,   0,   0,   0, 
     17,   0,   0,   8, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  30,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,  12,   0, 
      0,   0,  17,   0,   0,   8, 
     18,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  17,   0, 
      0,   8,  34,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     17,   0,   0,   8,  66,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  17,   0,   0,   8, 
    130,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   9,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
     13,   0,   0,   0,   0,   0, 
      0,   9,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,  16, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
     14,   0,   0,   0,   0,   0, 
      0,   9,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,  16, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  16,   0, 
      0,   7, 130,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  68,   0,   0,   5, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      0,   0,   0,   0, 246,  15, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     16,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  18,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  68,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 226,   0,  16,   0, 
      0,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
      6,   9,  16,   0,   1,   0, 
      0,   0,  16,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      1,   0,   0,   0, 150,   7, 
     16,   0,   0,   0,   0,   0, 
     52,   0,   0,  10,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  47,   0, 
      0,   5,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,  16,  65,  25,   0, 
      0,   5,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 192,  63,  50,   0, 
      0,  12, 114,  32,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0, 154, 153, 
    153,  63, 154, 153, 153,  63, 
    154, 153, 153,  63,   0,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     16,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  18,  16,   0,   1,   0, 
      0,   0,  70, 130,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  16,   0,   0,   8, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      1,   0,   0,   0,  70, 130, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  50,   0, 
      0,  15,  50,  32,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,  63,   0,   0,   0,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 194,  32,  16,   0, 
      2,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     32,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,  27,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  60,   1,   0,   0, 
      0,   0,   0,   0, 188,   2, 
      0,   0,  68,  88,  66,  67, 
    212, 244, 216, 136,  95, 111, 
     52, 107, 200, 205, 156,  29, 
    136, 215,  40, 167,   1,   0, 
      0,   0, 188,   2,   0,   0, 
      5,   0,   0,   0,  52,   0, 
      0,   0, 228,   0,   0,   0, 
    112,   1,   0,   0, 164,   1, 
      0,   0,  64,   2,   0,   0, 
     82,  68,  69,  70, 168,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  28,   0,   0,   0, 
      0,   4, 255, 255,   0,   1, 
      0,   0, 118,   0,   0,   0, 
     92,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 107,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     12,   0,   0,   0, 115,  97, 
    109, 112, 108, 101, 114,  84, 
    101, 120, 116, 117, 114, 101, 
      0, 116, 101, 120, 116, 117, 
    114, 101,  77,  97, 112,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  57, 
     46,  51,  48,  46,  57,  54, 
     48,  46,  56,  50,  50,  57, 
      0, 171,  73,  83,  71,  78, 
    132,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
    122,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   3,   0,   0, 
    122,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  12,   0,   0,   0, 
     83,  86,  95,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     97, 114, 103, 101, 116,   0, 
    171, 171,  83,  72,  68,  82, 
    148,   0,   0,   0,  64,   0, 
      0,   0,  37,   0,   0,   0, 
     90,   0,   0,   3,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   0,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   1,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,   8,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     16,   1,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  54,   0,   0,   0, 
     26,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    103,   0,   0,   0,  75,   0, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 118,   0, 
      0,   0,  75,   0,   0,   0, 
      0,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 136,   0,   0,   0, 
     75,   0,   0,   0,   0,   0, 
      0,   0, 144,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    143,   0,   0,   0,  75,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 191,   0, 
      0,   0, 163,   0,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    202,   0,   0,   0, 163,   0, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 251,   0,   0,   0, 
    223,   0,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      3,   0,   0,   0,  46,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  10,   1, 
      0,   0,  47,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  22,   1,   0,   0, 
     45,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     34,   1,   0,   0,   0,   0, 
      0,   0,  46,   1,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  52,   1,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0, 120,   8,   0,   0, 
      7,   0,   0,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
     64,  11,   0,   0
};
