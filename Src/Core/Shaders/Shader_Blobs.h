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
}

//
// 2 local object(s)
//
Texture2D textureMap;
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
            // POSITION                 0   xyz         0     NONE   float   xyz 
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
            //
            vs_4_0
            dcl_constantbuffer cb0[5], immediateIndexed
            dcl_input v0.xyz
            dcl_input v1.xyz
            dcl_output_siv o0.xyzw, position
            dcl_output o1.xyzw
            dcl_output o2.xy
            dcl_temps 1
            mov r0.xyz, v0.xyzx
            mov r0.w, l(1.000000)
            dp4 o0.x, r0.xyzw, cb0[1].xyzw
            dp4 o0.y, r0.xyzw, cb0[2].xyzw
            dp4 o0.z, r0.xyzw, cb0[3].xyzw
            dp4 o0.w, r0.xyzw, cb0[4].xyzw
            add r0.xyz, -v0.xyzx, l(0.000000, 0.000000, 1.000000, 0.000000)
            dp3 r0.w, r0.xyzx, r0.xyzx
            rsq r0.w, r0.w
            mul r0.xyz, r0.wwww, r0.xyzx
            dp3 r0.x, v1.xyzx, r0.xyzx
            max r0.x, r0.x, l(0.000000)
            add o1.xyz, r0.xxxx, l(0.250000, 0.250000, 0.250000, 0.000000)
            mov o1.w, l(1.250000)
            mad o2.xy, v1.xyxx, l(0.500000, 0.500000, 0.000000, 0.000000), l(0.500000, 0.500000, 0.000000, 0.000000)
            ret 
            // Approximately 16 instruction slots used
                    
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
     68,  88,  66,  67, 115, 223, 
    246, 101, 232, 219, 140,  11, 
    203, 129, 160,  44, 246, 188, 
    183, 199,   1,   0,   0,   0, 
    229,   9,   0,   0,   1,   0, 
      0,   0,  36,   0,   0,   0, 
     70,  88,  49,  48, 185,   9, 
      0,   0,   1,  16, 255, 254, 
      1,   0,   0,   0,   2,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 137,   8, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
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
    114, 105, 120,   0,  84, 101, 
    120, 116, 117, 114, 101,  50, 
     68,   0, 118,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  12,   0,   0,   0, 
    116, 101, 120, 116, 117, 114, 
    101,  77,  97, 112,   0,  83, 
     97, 109, 112, 108, 101, 114, 
     83, 116,  97, 116, 101,   0, 
    167,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     21,   0,   0,   0, 115,  97, 
    109, 112, 108, 101, 114,  84, 
    101, 120, 116, 117, 114, 101, 
      0,   1,   0,   0,   0,   2, 
      0,   0,   0,   3,   0,   0, 
      0,   1,   0,   0,   0,   2, 
      0,   0,   0,   3,   0,   0, 
      0,   1,   0,   0,   0,   2, 
      0,   0,   0,  21,   0,   0, 
      0,  66, 108, 111,  98, 115, 
      0,  68, 101, 102,  97, 117, 
    108, 116,   0, 188,   4,   0, 
      0,  68,  88,  66,  67,  36, 
     33,  60, 144, 105,  30,   9, 
    233,  21, 136, 205, 235, 209, 
    163,   4, 120,   1,   0,   0, 
      0, 188,   4,   0,   0,   5, 
      0,   0,   0,  52,   0,   0, 
      0,  64,   1,   0,   0, 144, 
      1,   0,   0,   4,   2,   0, 
      0,  64,   4,   0,   0,  82, 
     68,  69,  70,   4,   1,   0, 
      0,   1,   0,   0,   0,  76, 
      0,   0,   0,   1,   0,   0, 
      0,  28,   0,   0,   0,   0, 
      4, 254, 255,   0,   1,   0, 
      0, 208,   0,   0,   0,  60, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,   0,   0,   0, 
      0, 112,  97, 114,  97, 109, 
    115,  79, 110, 108, 121,  79, 
    110,  99, 101,   0, 171,  60, 
      0,   0,   0,   2,   0,   0, 
      0, 100,   0,   0,   0,  80, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 148, 
      0,   0,   0,   0,   0,   0, 
      0,   8,   0,   0,   0,   0, 
      0,   0,   0, 160,   0,   0, 
      0,   0,   0,   0,   0, 176, 
      0,   0,   0,  16,   0,   0, 
      0,  64,   0,   0,   0,   2, 
      0,   0,   0, 192,   0,   0, 
      0,   0,   0,   0,   0, 114, 
    101, 110, 100, 101, 114,  83, 
     99,  97, 108, 101,   0,   1, 
      0,   3,   0,   1,   0,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 118, 105, 101, 
    119,  80, 114, 111, 106,  77, 
     97, 116, 114, 105, 120,   0, 
    171,   3,   0,   3,   0,   4, 
      0,   4,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  57,  46, 
     51,  48,  46,  57,  54,  48, 
     46,  56,  50,  50,  57,   0, 
    171, 171, 171,  73,  83,  71, 
     78,  72,   0,   0,   0,   2, 
      0,   0,   0,   8,   0,   0, 
      0,  56,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   0, 
      0,   0,   0,   7,   7,   0, 
      0,  65,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   1, 
      0,   0,   0,   7,   7,   0, 
      0,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  78,  79, 
     82,  77,  65,  76,   0,  79, 
     83,  71,  78, 108,   0,   0, 
      0,   3,   0,   0,   0,   8, 
      0,   0,   0,  80,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,   3,   0,   0, 
      0,   0,   0,   0,   0,  15, 
      0,   0,   0,  92,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   3,   0,   0, 
      0,   1,   0,   0,   0,  15, 
      0,   0,   0,  98,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   3,   0,   0, 
      0,   2,   0,   0,   0,   3, 
     12,   0,   0,  83,  86,  95, 
     80, 111, 115, 105, 116, 105, 
    111, 110,   0,  67,  79,  76, 
     79,  82,   0,  84,  69,  88, 
     67,  79,  79,  82,  68,   0, 
    171,  83,  72,  68,  82,  52, 
      2,   0,   0,  64,   0,   1, 
      0, 141,   0,   0,   0,  89, 
      0,   0,   4,  70, 142,  32, 
      0,   0,   0,   0,   0,   5, 
      0,   0,   0,  95,   0,   0, 
      3, 114,  16,  16,   0,   0, 
      0,   0,   0,  95,   0,   0, 
      3, 114,  16,  16,   0,   1, 
      0,   0,   0, 103,   0,   0, 
      4, 242,  32,  16,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0, 101,   0,   0,   3, 242, 
     32,  16,   0,   1,   0,   0, 
      0, 101,   0,   0,   3,  50, 
     32,  16,   0,   2,   0,   0, 
      0, 104,   0,   0,   2,   1, 
      0,   0,   0,  54,   0,   0, 
      5, 114,   0,  16,   0,   0, 
      0,   0,   0,  70,  18,  16, 
      0,   0,   0,   0,   0,  54, 
      0,   0,   5, 130,   0,  16, 
      0,   0,   0,   0,   0,   1, 
     64,   0,   0,   0,   0, 128, 
     63,  17,   0,   0,   8,  18, 
     32,  16,   0,   0,   0,   0, 
      0,  70,  14,  16,   0,   0, 
      0,   0,   0,  70, 142,  32, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,  17,   0,   0, 
      8,  34,  32,  16,   0,   0, 
      0,   0,   0,  70,  14,  16, 
      0,   0,   0,   0,   0,  70, 
    142,  32,   0,   0,   0,   0, 
      0,   2,   0,   0,   0,  17, 
      0,   0,   8,  66,  32,  16, 
      0,   0,   0,   0,   0,  70, 
     14,  16,   0,   0,   0,   0, 
      0,  70, 142,  32,   0,   0, 
      0,   0,   0,   3,   0,   0, 
      0,  17,   0,   0,   8, 130, 
     32,  16,   0,   0,   0,   0, 
      0,  70,  14,  16,   0,   0, 
      0,   0,   0,  70, 142,  32, 
      0,   0,   0,   0,   0,   4, 
      0,   0,   0,   0,   0,   0, 
     11, 114,   0,  16,   0,   0, 
      0,   0,   0,  70,  18,  16, 
    128,  65,   0,   0,   0,   0, 
      0,   0,   0,   2,  64,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 128, 
     63,   0,   0,   0,   0,  16, 
      0,   0,   7, 130,   0,  16, 
      0,   0,   0,   0,   0,  70, 
      2,  16,   0,   0,   0,   0, 
      0,  70,   2,  16,   0,   0, 
      0,   0,   0,  68,   0,   0, 
      5, 130,   0,  16,   0,   0, 
      0,   0,   0,  58,   0,  16, 
      0,   0,   0,   0,   0,  56, 
      0,   0,   7, 114,   0,  16, 
      0,   0,   0,   0,   0, 246, 
     15,  16,   0,   0,   0,   0, 
      0,  70,   2,  16,   0,   0, 
      0,   0,   0,  16,   0,   0, 
      7,  18,   0,  16,   0,   0, 
      0,   0,   0,  70,  18,  16, 
      0,   1,   0,   0,   0,  70, 
      2,  16,   0,   0,   0,   0, 
      0,  52,   0,   0,   7,  18, 
      0,  16,   0,   0,   0,   0, 
      0,  10,   0,  16,   0,   0, 
      0,   0,   0,   1,  64,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10, 114,  32,  16, 
      0,   1,   0,   0,   0,   6, 
      0,  16,   0,   0,   0,   0, 
      0,   2,  64,   0,   0,   0, 
      0, 128,  62,   0,   0, 128, 
     62,   0,   0, 128,  62,   0, 
      0,   0,   0,  54,   0,   0, 
      5, 130,  32,  16,   0,   1, 
      0,   0,   0,   1,  64,   0, 
      0,   0,   0, 160,  63,  50, 
      0,   0,  15,  50,  32,  16, 
      0,   2,   0,   0,   0,  70, 
     16,  16,   0,   1,   0,   0, 
      0,   2,  64,   0,   0,   0, 
      0,   0,  63,   0,   0,   0, 
     63,   0,   0,   0,   0,   0, 
      0,   0,   0,   2,  64,   0, 
      0,   0,   0,   0,  63,   0, 
      0,   0,  63,   0,   0,   0, 
      0,   0,   0,   0,   0,  62, 
      0,   0,   1,  83,  84,  65, 
     84, 116,   0,   0,   0,  16, 
      0,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,   5, 
      0,   0,   0,  11,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   4,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  17,   1,   0,   0,   0, 
      0,   0,   0, 164,   2,   0, 
      0,  68,  88,  66,  67, 204, 
    220,  95,  80,  23,   1,  51, 
     36, 205,   2,  33,  64,  17, 
    105, 236, 193,   1,   0,   0, 
      0, 164,   2,   0,   0,   5, 
      0,   0,   0,  52,   0,   0, 
      0, 228,   0,   0,   0,  88, 
      1,   0,   0, 140,   1,   0, 
      0,  40,   2,   0,   0,  82, 
     68,  69,  70, 168,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   2,   0,   0, 
      0,  28,   0,   0,   0,   0, 
      4, 255, 255,   0,   1,   0, 
      0, 118,   0,   0,   0,  92, 
      0,   0,   0,   3,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,   0,   0,   0, 
      0, 107,   0,   0,   0,   2, 
      0,   0,   0,   5,   0,   0, 
      0,   4,   0,   0,   0, 255, 
    255, 255, 255,   0,   0,   0, 
      0,   1,   0,   0,   0,  12, 
      0,   0,   0, 115,  97, 109, 
    112, 108, 101, 114,  84, 101, 
    120, 116, 117, 114, 101,   0, 
    116, 101, 120, 116, 117, 114, 
    101,  77,  97, 112,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  57,  46, 
     51,  48,  46,  57,  54,  48, 
     46,  56,  50,  50,  57,   0, 
    171,  73,  83,  71,  78, 108, 
      0,   0,   0,   3,   0,   0, 
      0,   8,   0,   0,   0,  80, 
      0,   0,   0,   0,   0,   0, 
      0,   1,   0,   0,   0,   3, 
      0,   0,   0,   0,   0,   0, 
      0,  15,   0,   0,   0,  92, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   3, 
      0,   0,   0,   1,   0,   0, 
      0,  15,  15,   0,   0,  98, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   3, 
      0,   0,   0,   2,   0,   0, 
      0,   3,   3,   0,   0,  83, 
     86,  95,  80, 111, 115, 105, 
    116, 105, 111, 110,   0,  67, 
     79,  76,  79,  82,   0,  84, 
     69,  88,  67,  79,  79,  82, 
     68,   0, 171,  79,  83,  71, 
     78,  44,   0,   0,   0,   1, 
      0,   0,   0,   8,   0,   0, 
      0,  32,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   3,   0,   0,   0,   0, 
      0,   0,   0,  15,   0,   0, 
      0,  83,  86,  95,  84,  97, 
    114, 103, 101, 116,   0, 171, 
    171,  83,  72,  68,  82, 148, 
      0,   0,   0,  64,   0,   0, 
      0,  37,   0,   0,   0,  90, 
      0,   0,   3,   0,  96,  16, 
      0,   0,   0,   0,   0,  88, 
     24,   0,   4,   0, 112,  16, 
      0,   0,   0,   0,   0,  85, 
     85,   0,   0,  98,  16,   0, 
      3, 242,  16,  16,   0,   1, 
      0,   0,   0,  98,  16,   0, 
      3,  50,  16,  16,   0,   2, 
      0,   0,   0, 101,   0,   0, 
      3, 242,  32,  16,   0,   0, 
      0,   0,   0, 104,   0,   0, 
      2,   1,   0,   0,   0,  69, 
      0,   0,   9, 242,   0,  16, 
      0,   0,   0,   0,   0,  70, 
     16,  16,   0,   2,   0,   0, 
      0,  70, 126,  16,   0,   0, 
      0,   0,   0,   0,  96,  16, 
      0,   0,   0,   0,   0,  56, 
      0,   0,   7, 242,  32,  16, 
      0,   0,   0,   0,   0,  70, 
     14,  16,   0,   0,   0,   0, 
      0,  70,  30,  16,   0,   1, 
      0,   0,   0,  62,   0,   0, 
      1,  83,  84,  65,  84, 116, 
      0,   0,   0,   3,   0,   0, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   0,   3,   0,   0, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 217, 
      5,   0,   0,   0,   0,   0, 
      0,   4,   0,   0,   0,  80, 
      0,   0,   0,   0,   0,   0, 
      0,   2,   0,   0,   0, 255, 
    255, 255, 255,   0,   0,   0, 
      0,  54,   0,   0,   0,  26, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 103, 
      0,   0,   0,  75,   0,   0, 
      0,   0,   0,   0,   0,  16, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 156,   0,   0, 
      0, 128,   0,   0,   0,   0, 
      0,   0,   0, 255, 255, 255, 
    255,   0,   0,   0,   0, 208, 
      0,   0,   0, 180,   0,   0, 
      0,   0,   0,   0,   0, 255, 
    255, 255, 255,   3,   0,   0, 
      0,  46,   0,   0,   0,   0, 
      0,   0,   0,   1,   0,   0, 
      0, 223,   0,   0,   0,  47, 
      0,   0,   0,   0,   0,   0, 
      0,   1,   0,   0,   0, 235, 
      0,   0,   0,  45,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0, 247,   0,   0, 
      0,   0,   0,   0,   0,   3, 
      1,   0,   0,   1,   0,   0, 
      0,   0,   0,   0,   0,   9, 
      1,   0,   0,   2,   0,   0, 
      0,   0,   0,   0,   0,   6, 
      0,   0,   0,   0,   0,   0, 
      0,   7,   0,   0,   0, 209, 
      5,   0,   0,   7,   0,   0, 
      0,   0,   0,   0,   0,   7, 
      0,   0,   0, 129,   8,   0, 
      0
};