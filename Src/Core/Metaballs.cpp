
// the world famous TPB CPU blobs
// this is a little bit fucked up since most functions and variables need to be moved into the class
// but it works for now :-)

#include <stdint.h>
#include "Metaballs.h"
#include "mctables.h"

#include "Shaders/Shader_Sprites.h"

struct Vertex
{
	Vector3 position;
	Vector3 normal;
};

// triangle face (used to address index buffer)
struct Face
{
	uint16_t iA;
	uint16_t iB;
	uint16_t iC;
};

const unsigned int kGridDepth = 48;
const unsigned int kGridCubes = kGridDepth-1;
const unsigned int kGridDepthSqr = kGridDepth*kGridDepth;
const float kGridStep = 2.f / (float) kGridCubes;

const unsigned int kMaxFaces = 65536;      // should be sufficient (assert)
const unsigned int kMaxVertices = 10000;   // 65536 is max. for 16-bit indices (assert)
const size_t kVertexBufferSize = kMaxVertices * sizeof(Vertex);

// grid & cache arrays (huge!)
static float s_isoValues[kGridDepth*kGridDepth*kGridDepth];
static uint32_t s_gridCache[kGridCubes*kGridCubes*kGridCubes*3 + 3];

// (relative) indices for a cube in the grid
const unsigned int kCubeIndices[8] =
{
	0,
	1,
	1 + kGridDepthSqr,
	kGridDepthSqr,
	kGridDepth,
	1 + kGridDepth,
	1 + kGridDepth + kGridDepthSqr,
	kGridDepth + kGridDepthSqr,
};

// (relative) offsets for a cube in grid space
const Vector3 kCubeOffsets[8] =
{
	Vector3(      0.f,       0.f,       0.f),
	Vector3(kGridStep,       0.f,       0.f),
	Vector3(kGridStep,       0.f, kGridStep),
	Vector3(      0.f,       0.f, kGridStep),
	Vector3(      0.f, kGridStep,       0.f),
	Vector3(kGridStep, kGridStep,       0.f),	
	Vector3(kGridStep, kGridStep, kGridStep),
	Vector3(      0.f, kGridStep, kGridStep)
};

// temp. variables
static unsigned int s_numBall4s;
static const Pimp::Metaballs::Metaball4 *s_pBall4s;
static float s_surfaceLevel;
static float s_cube[8]; // current grid cube
static unsigned int s_genNumVerts, s_genNumFaces;
static Vertex *s_pVertices;
static Face *s_pFaces;

// linear interpolate
template<typename T> inline const T lerpf(const T &A, const T &B, float factor)
{
	ASSERT(factor >= 0.f && factor <= 1.f);
	return A*(1.f - factor) + B*factor;
}

__forceinline unsigned int GetEdgeTableIndex()
{
	// flip a bit for each of the current grid cube's corners that lie below surface level
	const __m128 cubeVals1 = _mm_load_ps(s_cube);
	const __m128 cubeVals2 = _mm_load_ps(s_cube+4);
	const __m128 threshold = _mm_load1_ps(&s_surfaceLevel);
	const __m128  cmpMask1 = _mm_cmplt_ps(cubeVals1, threshold);
	const __m128  cmpMask2 = _mm_cmplt_ps(cubeVals2, threshold);
	return _mm_movemask_ps(cmpMask1) | _mm_movemask_ps(cmpMask2) << 4;
}

void Triangulate(unsigned int iGrid, float gridX, float gridY, float gridZ, unsigned int iEdgeTab, unsigned int edgeBits);

static float CalculateIsoValue(unsigned int iGrid, float gridX, float gridY, float gridZ)
{
	const __m128 gridXXXX = _mm_load1_ps(&gridX);
	const __m128 gridYYYY = _mm_load1_ps(&gridY);
	const __m128 gridZZZZ = _mm_load1_ps(&gridZ);

	__m128 isoValues = _mm_setzero_ps();
	for (unsigned int iBall4 = 0; iBall4 < s_numBall4s; ++iBall4)
	{
		// process 4 balls in parallel
		// each register carries a single component for each ball, operations are done vertically
		const __m128 xDeltas = _mm_sub_ps(gridXXXX, _mm_load_ps(s_pBall4s[iBall4].X));
		const __m128 yDeltas = _mm_sub_ps(gridYYYY, _mm_load_ps(s_pBall4s[iBall4].Y));
		const __m128 zDeltas = _mm_sub_ps(gridZZZZ, _mm_load_ps(s_pBall4s[iBall4].Z));
		const __m128 xDeltasSqr = _mm_mul_ps(xDeltas, xDeltas);
		const __m128 yDeltasSqr = _mm_mul_ps(yDeltas, yDeltas);
		const __m128 zDeltasSqr = _mm_mul_ps(zDeltas, zDeltas);
		const __m128 oneOverRadSqr = _mm_rcp_ps(_mm_add_ps(_mm_add_ps(xDeltasSqr, yDeltasSqr), zDeltasSqr));
		isoValues = _mm_add_ps(isoValues, oneOverRadSqr);
	}

	// "collapse" -- in 2 passes, add all 4 floats together and store the final result in the first one (SSE3)
	isoValues = _mm_hadd_ps(isoValues, isoValues);
	isoValues = _mm_hadd_ps(isoValues, isoValues);

	// store & return scalar result
	_mm_store_ss(s_isoValues + iGrid, isoValues);
	return isoValues.m128_f32[0];			
}

static void ProcessCube(unsigned int iGrid, unsigned int iX, unsigned int iY, unsigned int iZ)
{
	// must this grid cube still be processed? (i.e. upper 8 bits set)
	if ((s_gridCache[iGrid] & 0xff000000) == 0xff000000)
	{
		// calculate grid space values
		// this penalty is preferred over passing these values around all the time
		const float gridX = -1.f + (float)iX*kGridStep;
		const float gridY = -1.f + (float)iY*kGridStep;
		const float gridZ = -1.f + (float)iZ*kGridStep;

		// flag as processed (erase upper 8 bits, leave possible vertex cache intact)
		s_gridCache[iGrid] &= 0xffffff;
		
		// obtain iso values
		for (unsigned int iCorner = 0; iCorner < 8; ++iCorner)
		{
			const unsigned int iCube = iGrid + kCubeIndices[iCorner];
			
			if (*(reinterpret_cast<int *>(s_isoValues+iCube)) != 0xffffffff)
			{
				// cached: copy
				s_cube[iCorner] = s_isoValues[iCube];
			}
			else
			{
				// uncached: calculate
				const float cubeX = gridX + kCubeOffsets[iCorner].x;
				const float cubeY = gridY + kCubeOffsets[iCorner].y;
				const float cubeZ = gridZ + kCubeOffsets[iCorner].z;
				s_cube[iCorner] = CalculateIsoValue(iCube, cubeX, cubeY, cubeZ);
			}
		}

		// if there's an edge here...
		const unsigned int iEdgeTab = GetEdgeTableIndex();
		if (iEdgeTab != 0)
		{
			// triangulate this cube
			const unsigned int edgeBits = kEdgeTable[iEdgeTab];
			Triangulate(iGrid, gridX, gridY, gridZ, iEdgeTab, edgeBits);
			
			// now recurse neighbouring cubes as needed
			// this is attempted in a cache-friendly order
			
			if (edgeBits & 2+32+512+1024)
			{
				// right neighbour
				if (iX+1 < kGridCubes)
				{
					ProcessCube(iGrid + 1, iX+1, iY, iZ);
				}
			}
			
			if (edgeBits & 8+128+256+2048)
			{
				// left neighbour
				if (iX > 0)
				{
					ProcessCube(iGrid - 1, iX-1, iY, iZ);
				}
			}
			
			if (edgeBits & 16+32+64+128)
			{
				// top neighbour
				if (iY+1 < kGridCubes)
				{
					ProcessCube(iGrid + kGridDepth, iX, iY+1, iZ);
				}
			}
			
			if (edgeBits & 1+2+4+8)
			{
				// bottom neighbour
				if (iY > 0)
				{
					ProcessCube(iGrid - kGridDepth, iX, iY-1, iZ);
				}
			}
			
			if (edgeBits & 4+64+1024+2048)
			{
				// front neighbour
				if (iZ+1 < kGridCubes)
				{
					ProcessCube(iGrid + kGridDepthSqr, iX, iY, iZ+1);
				}
			}
			
			if (edgeBits & 1+16+256+512)
			{
				// back neighbour
				if (iZ > 0)
				{
					ProcessCube(iGrid - kGridDepthSqr, iX, iY, iZ-1);
				}
			}
		}
	}
}

void Triangulate(unsigned int iGrid, float gridX, float gridY, float gridZ, unsigned int iEdgeTab, unsigned int edgeBits)
{
	unsigned int vertexIndices[12];

	// for each possible edge...
	for (unsigned int iEdge = 0; iEdge < 12; ++iEdge)
	{
		// need to generate a vertex?
		if (edgeBits & (1 << iEdge))
		{
			// yes: is it cached?
			const unsigned int iCache = kCubeEdgeToVertexCache[iEdge][1] + 3*(iGrid + kCubeIndices[kCubeEdgeToVertexCache[iEdge][0]]);
			unsigned int iVertex = s_gridCache[iCache] & 0xffffff;
			if (iVertex == 0xffffff) // if lower 24 bits are all set, vertex is not cached!
			{
				// calculate intersection point on edge A-B
				const unsigned int iCubeA = kCubeEdgeIndices[iEdge][0];
				const unsigned int iCubeB = kCubeEdgeIndices[iEdge][1];
				const float aX = kCubeOffsets[iCubeA].x + gridX;
				const float aY = kCubeOffsets[iCubeA].y + gridY;
				const float aZ = kCubeOffsets[iCubeA].z + gridZ;
				const float bX = kCubeOffsets[iCubeB].x + gridX;
				const float bY = kCubeOffsets[iCubeB].y + gridY;
				const float bZ = kCubeOffsets[iCubeB].z + gridZ;
				const float isoA = s_cube[iCubeA]; 
				const float isoB = s_cube[iCubeB];
				const float distance = (s_surfaceLevel-isoA) / (isoB-isoA);
				const float pX = lerpf(aX, bX, distance);
				const float pY = lerpf(aY, bY, distance);
				const float pZ = lerpf(aZ, bZ, distance);

				// vertex: store position
				s_pVertices[s_genNumVerts].position.x = pX;
				s_pVertices[s_genNumVerts].position.y = pY;
				s_pVertices[s_genNumVerts].position.z = pZ;

				// calculate normal
				// works much like CalculateIsoValue()
				// post-loop code still unsatisfactory (was SSE3), but considerably better than it's C/FPU counterpart

				const __m128 pXXXX = _mm_load1_ps(&pX);
				const __m128 pYYYY = _mm_load1_ps(&pY);
				const __m128 pZZZZ = _mm_load1_ps(&pZ);

				__m128 normalXXXX = _mm_setzero_ps(), normalYYYY = _mm_setzero_ps(), normalZZZZ = _mm_setzero_ps();
				for (unsigned int iBall4 = 0; iBall4 < s_numBall4s; ++iBall4)
				{
					const __m128 xDeltas = _mm_sub_ps(pXXXX, _mm_load_ps(s_pBall4s[iBall4].X));
					const __m128 yDeltas = _mm_sub_ps(pYYYY, _mm_load_ps(s_pBall4s[iBall4].Y));
					const __m128 zDeltas = _mm_sub_ps(pZZZZ, _mm_load_ps(s_pBall4s[iBall4].Z));
					const __m128 xDeltasSqr = _mm_mul_ps(xDeltas, xDeltas);
					const __m128 yDeltasSqr = _mm_mul_ps(yDeltas, yDeltas);
					const __m128 zDeltasSqr = _mm_mul_ps(zDeltas, zDeltas);
					const __m128 radSqr = _mm_add_ps(_mm_add_ps(xDeltasSqr, yDeltasSqr), zDeltasSqr);
					const __m128 oneOverRadQuad = _mm_rcp_ps(_mm_mul_ps(radSqr, radSqr));
					normalXXXX = _mm_add_ps(normalXXXX, _mm_mul_ps(oneOverRadQuad, xDeltas));
					normalYYYY = _mm_add_ps(normalYYYY, _mm_mul_ps(oneOverRadQuad, yDeltas));
					normalZZZZ = _mm_add_ps(normalZZZZ, _mm_mul_ps(oneOverRadQuad, zDeltas));
				}
				
				// sum X, Y and Z registers into first component
				// collapse accumulated normal components into a single vector (SSE3)
				const __m128 nX12X34Y12Y34 = _mm_hadd_ps(normalXXXX, normalYYYY);    // | X1+X2 | X3+X4 | Y1+Y2 | Y3+Y4 |
				const __m128 nXYZ12Z34     = _mm_hadd_ps(nX12X34Y12Y34, normalZZZZ); // | X     | Y     | Z1+Z2 | Z3+Z4 |
				const __m128 nXYZZ         = _mm_shuffle_ps(nXYZ12Z34, _mm_hadd_ps(nXYZ12Z34, nXYZ12Z34), _MM_SHUFFLE(1, 1, 1, 0)); 
				
				// normalize X, Y and Z of nXYZZ (the not-so-elegant SSE way)
				const __m128 normSqr = _mm_mul_ps(nXYZZ, nXYZZ);
				__m128 oneOverNormLen = _mm_rsqrt_ss(_mm_add_ss(normSqr, _mm_add_ss(_mm_shuffle_ps(normSqr, normSqr, _MM_SHUFFLE(0, 0, 0, 1)), _mm_shuffle_ps(normSqr, normSqr, _MM_SHUFFLE(0, 0, 0, 2)))));
				oneOverNormLen = _mm_shuffle_ps(oneOverNormLen, oneOverNormLen, 0);
				const __m128 normal = _mm_mul_ps(nXYZZ, oneOverNormLen);

				// vertex: normalize and store normal
				_mm_store_ss(&s_pVertices[s_genNumVerts].normal.x, _mm_mul_ss(oneOverNormLen, normalXXXX));
				_mm_store_ss(&s_pVertices[s_genNumVerts].normal.y, _mm_mul_ss(oneOverNormLen, normalYYYY));
				_mm_store_ss(&s_pVertices[s_genNumVerts].normal.z, _mm_mul_ss(oneOverNormLen, normalZZZZ));

				// vertex: store color
//				s_pVertices[s_genNumVerts].color = 0xffffffff;

				// vertex: store and cache index
				vertexIndices[iEdge] = iVertex = s_genNumVerts++;
				ASSERT(s_genNumVerts < kMaxVertices);
				s_gridCache[iCache] = (s_gridCache[iCache] & 0xff000000) | iVertex;
			}
			else
			{
				// this vertex has already been generated, so use it
				vertexIndices[iEdge] = iVertex;
			}
		}
	}

	const int *pTriIndices = kTriangleTable[iEdgeTab];
	while (*pTriIndices != -1)
	{
		// build face -- reverse order to obtain CCW face
		const unsigned int iC = vertexIndices[*pTriIndices++];
		const unsigned int iB = vertexIndices[*pTriIndices++];
		const unsigned int iA = vertexIndices[*pTriIndices++];
		s_pFaces[s_genNumFaces].iA = iA;
		s_pFaces[s_genNumFaces].iB = iB;
		s_pFaces[s_genNumFaces].iC = iC;
		++s_genNumFaces;
		ASSERT(s_genNumFaces <= kMaxFaces);
	}
}

namespace Pimp {

Metaballs::Metaballs(World *ownerWorld) :
	Geometry(ownerWorld),
	m_pVB(nullptr), m_pIB(nullptr), m_inputLayout(nullptr),
	effect((unsigned char*)gCompiledShader_Sprites, sizeof(gCompiledShader_Sprites)),
	effectTechnique(&effect, "Sprites"),
	effectPass(&effectTechnique, "Default")
{
	// don't impose CPU load by default
	isVisible = false;
}

Metaballs::~Metaballs()
{
	if (nullptr != m_pVB) m_pVB->Release();
	if (nullptr != m_pIB) m_pIB->Release();
	if (nullptr != m_inputLayout) m_inputLayout->Release();
}

bool Metaballs::Initialize()
{
	// FIXME: error check
	gD3D->CreateVertexBuffer(kVertexBufferSize, nullptr, true);
	gD3D->CreateIndexBuffer(kMaxFaces*3, nullptr, true);

	unsigned char* signature;
	int signatureLength;
	effectPass.GetVSInputSignature(&signature, &signatureLength);

	/* static const */ D3D10_INPUT_ELEMENT_DESC elemDesc[2];
	elemDesc[0].SemanticName = "POSITION";
	elemDesc[0].SemanticIndex = 0;
	elemDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elemDesc[0].InputSlot = 0;
	elemDesc[0].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	elemDesc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	elemDesc[0].InstanceDataStepRate = 0;
	elemDesc[1].SemanticName = "NORMAL";
	elemDesc[1].SemanticIndex = 0;
	elemDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elemDesc[1].InputSlot = 0;
	elemDesc[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	elemDesc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	elemDesc[1].InstanceDataStepRate = 0;
	m_inputLayout = gD3D->CreateInputLayout(elemDesc, 2, signature, signatureLength);

	delete [] signature;

	// fix up shader variables
	int varIndexRenderScale = effect.RegisterVariable("renderScale", true);
	const Vector2& visible_area = gD3D->GetRenderScale();
	effect.SetVariableValue(varIndexRenderScale, visible_area);
	varIndexTextureMap = effect.RegisterVariable("textureMap", true);

	return true;
}

void Metaballs::Tick(float deltaTime, unsigned int numBall4s, const Metaball4 *pBall4s, float surfaceLevel)
{
	// Don't do shit if we're not rendering these.
	if (false == isVisible)
		return;

	// (re)set temp. variables
	s_numBall4s = numBall4s;
	s_pBall4s = pBall4s;
	s_surfaceLevel = surfaceLevel;
	s_genNumVerts = 0;
	s_genNumFaces = 0;
	VERIFY(SUCCEEDED(m_pVB->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast<void **>(&s_pVertices))));
	VERIFY(SUCCEEDED(m_pIB->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast<void **>(&s_pFaces))));

	// invalidate grid & vertex cache
	memset(s_isoValues, 0xff, kGridDepth*kGridDepthSqr * sizeof(float));
	memset(s_gridCache, 0xff, (3 + 3*kGridCubes*kGridCubes*kGridCubes) * sizeof(int));

	// for each ball, walk towards it's edge and see if it needs processing
	for (unsigned int iBall4 = 0; iBall4 < numBall4s; ++iBall4)
	{
		for (unsigned int iBall = 0; iBall < 4; ++iBall)
		{
			// transform ball's center to grid index
			float gridX = (pBall4s[iBall4].X[iBall]*0.5f + 0.5f) * kGridCubes;
			float gridY = (pBall4s[iBall4].Y[iBall]*0.5f + 0.5f) * kGridCubes;
			float gridZ = (pBall4s[iBall4].Z[iBall]*0.5f + 0.5f) * kGridCubes;

			// round index down to integer
			// for now, any ball with it's center outside the grid will cause a page fault: fix this!
			unsigned int iX, iY, iZ;
			iX = (unsigned int) gridX;
			iY = (unsigned int) gridY;
			iZ = (unsigned int) gridZ;
			unsigned int iGrid = iZ*kGridDepthSqr + iY*kGridDepth + iX;
			
			// transform rounded down index back to grid space
			gridX = floorf(gridX)/kGridCubes*2.f - 1.f;
			gridY = floorf(gridY)/kGridCubes*2.f - 1.f;
			gridZ = floorf(gridZ)/kGridCubes*2.f - 1.f;
			
			// figure out which way to to walk towards the center of the grid
			// this also needs expansion, for now we "assume" and walk along the X axis
			int walkDir;
			if (gridX < 0.f) walkDir = 1;
			else walkDir = -1;

			// forward, march!
			do
			{
				// must this grid cube still be processed? (i.e. upper 8 bits set)
				if ((s_gridCache[iGrid] & 0xff000000) != 0xff000000)
				{
					// no, so it's safe to assume that this ball's geometry has been generated
					break;
				}

				// obtain iso values for each corner of grid cube
				for (unsigned int iCorner = 0; iCorner < 8; ++iCorner)
				{
					const unsigned int iCube = iGrid + kCubeIndices[iCorner];
					if (*(reinterpret_cast<int *>(s_isoValues+iCube)) != 0xffffffff)
					{
						// cached: copy
						s_cube[iCorner] = s_isoValues[iCube];
					}
					else
					{
						// uncached: calculate
						const float cubeX = gridX + kCubeOffsets[iCorner].x;
						const float cubeY = gridY + kCubeOffsets[iCorner].y;
						const float cubeZ = gridZ + kCubeOffsets[iCorner].z;
						s_cube[iCorner] = CalculateIsoValue(iCube, cubeX, cubeY, cubeZ);
					}
				}

				// hit an edge?
				if (GetEdgeTableIndex() != 0)
				{
					// process this cube and traverse
					ProcessCube(iGrid, iX, iY, iZ);
					break;
				}

				// walk along X axis...
				iX += walkDir;			
				iGrid += walkDir;
				gridX += kGridStep*walkDir;
			}
			while (iX < kGridCubes);
		}
	}

	m_pVB->Unmap();
	m_pIB->Unmap();
}

void Metaballs::Draw()
{
	// Bind buffers.
	gD3D->BindVertexBuffer(0, m_pVB, sizeof(Vertex));
	gD3D->BindInputLayout(m_inputLayout);
	gD3D->BindIndexBuffer(m_pIB);

	// Set shader vars.
	effect.SetVariableValue(varIndexTextureMap, gD3D->GetWhiteTex()->GetShaderResourceView());
	effectPass.Apply();

	// Kick off.
	gD3D->SetBlendMode(D3D::BlendMode::BM_None);
	gD3D->DrawTriList(s_genNumFaces);
}

} // namespace Pimp
