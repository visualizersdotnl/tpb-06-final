#include "SceneTools.h"
#include "gWorld.h"

// 
// Misc. scene graph helpers.
//

void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent)
{
	Pimp::World::StaticAddChildToParent(nodeChild, nodeParent);
}

static void AddAnimCurveKey(FixedSizeList<Pimp::AnimCurve::Pair>& keys, float time, float value)
{
	Pimp::AnimCurve::Pair key;

	key.time = time;
	key.value = value;
	key.inTangentY = key.outTangentY = 0;

	keys.Add(key);
}

static void RecursiveRemoveNode(Pimp::Node* node)
{
	FixedSizeList<Pimp::Element*>& elems = node->GetOwnerWorld()->GetElements();
	elems.Remove(node);

	for (int i=0; i<node->GetChildren().Size(); ++i)
		RecursiveRemoveNode(node->GetChildren()[i]);
}

void RemoveNodeFromWorld(Pimp::Node* node)
{
	Pimp::World* world = node->GetOwnerWorld();

	FixedSizeList<Pimp::Node*>& parents = node->GetParents();
	for (int i=0; i<parents.Size(); ++i)
	{
		parents[i]->GetChildren().Remove(node);
	}
	
	RecursiveRemoveNode(node);
}

void DuplicateTransformTransformedHierarchy(
	Pimp::World* w,
	Pimp::Node* source, Pimp::Node* dest)
{
	for (int i=0; i<source->GetChildren().Size(); ++i)
	{
		Pimp::Node* n = source->GetChildren()[i];

		if (n->GetType() == Pimp::ET_Xform)
		{
			Pimp::Xform* x = new Pimp::Xform(*(Pimp::Xform*)n);
			w->GetElements().Add(x);

			AddChildToParent(x, dest);

			DuplicateTransformTransformedHierarchy(w, n, x);
		}
		else if (n->GetType() == Pimp::ET_PolyMesh)
		{
			// Instance polymesh!
			AddChildToParent(n, dest);
		}
	}
}

//
// Effect compilation.
//

#if PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE

struct MaterialJobInfo
{
	const unsigned char* effectAscii;
	int effectAsciiSize;
	unsigned char** outCompiledEffectBuffer;
	int* compiledEffectLength;
	int index;
	bool ready;
};

#define MAX_NUM_MATERIALCOMPILETHREADS 64

static FixedSizeList<MaterialJobInfo>* materialCompilationJobs;
static FixedSizeList<HANDLE>* materialCompilationThreadHandles;

#endif

void InitMaterialCompilationSystem()
{
#if PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE
	materialCompilationJobs = new FixedSizeList<MaterialJobInfo>(MAX_NUM_MATERIALCOMPILETHREADS);
	materialCompilationThreadHandles = new FixedSizeList<HANDLE>(MAX_NUM_MATERIALCOMPILETHREADS);
#endif
}

DWORD WINAPI MaterialCompilerThreadProc(void* parameter)
{
	MaterialJobInfo* jobInfo = (MaterialJobInfo*)parameter;

#ifdef _DEBUG
	Stopwatch sw;
#endif

	bool success = 
		Pimp::gD3D->CompileEffect(
		jobInfo->effectAscii, 
		jobInfo->effectAsciiSize, 
		jobInfo->outCompiledEffectBuffer, 
		jobInfo->compiledEffectLength);
	ASSERT(success);

	jobInfo->ready = true;

#ifdef _DEBUG
	{
		char s[512];
		sprintf_s(s,512, "Material compilation job %d has finished! Took %f seconds.\n", jobInfo->index, sw.GetSecondsElapsedAndReset());
		OutputDebugString(s);
	}
#endif

	return 1;
}

void StartMaterialCompilationJob(const unsigned char* effectAscii, int effectAsciiSize, unsigned char** outCompiledEffectBuffer, int* compiledEffectLength)
{
#if PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE
	// Schedule a new job
	materialCompilationJobs->AddMultipleNoninitialized(1);
	MaterialJobInfo* newJob = materialCompilationJobs->GetItemPtr(materialCompilationJobs->Size()-1);
	newJob->effectAscii = effectAscii;
	newJob->effectAsciiSize = effectAsciiSize;
	newJob->outCompiledEffectBuffer = outCompiledEffectBuffer;
	newJob->compiledEffectLength = compiledEffectLength;
	newJob->index = materialCompilationJobs->Size()-1;
	newJob->ready = false;

	HANDLE h = CreateThread(NULL, 0, MaterialCompilerThreadProc, newJob, 0, NULL);
	materialCompilationThreadHandles->Add(h);

	DrawLoadProgress(false);
#else

#ifdef _DEBUG
	Stopwatch sw;
#endif

	bool success = Pimp::gD3D->CompileEffect(effectAscii, effectAsciiSize, outCompiledEffectBuffer, compiledEffectLength);
	ASSERT(success);

#ifdef _DEBUG
	{
		char s[512];
		sprintf_s(s,512, "Material compilation job has finished! Took %f seconds.\n", sw.GetSecondsElapsedAndReset());
		OutputDebugString(s);
	}
#endif

#endif
}

void WaitForMaterialCompilationJobsToFinish()
{
#if PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE

#ifdef _DEBUG
	Stopwatch sw;
#endif

	while (true)
	{
		// Wait for all threads to complete
		DWORD res = WaitForMultipleObjects(materialCompilationThreadHandles->Size(), materialCompilationThreadHandles->GetItemsPtr(), TRUE, 1000);

		DrawLoadProgress(true);

		if (res != WAIT_TIMEOUT)
			break;
	}

#ifdef _DEBUG
	{
		char s[512];
		sprintf_s(s,512, "Total material compilation time: %f seconds.\n", sw.GetSecondsElapsedAndReset());
		OutputDebugString(s);
	}
#endif

#else
	// no need to wait
#endif
}

static int gNumTotalMaterialCompilationJobs = 1;

void SetNumTotalMaterialCompilationJobs(int count)
{
	gNumTotalMaterialCompilationJobs = count;

	DrawLoadProgress(false);
}

//
// Loading bar.
//

void DrawLoadProgress( bool texturesLoaded )
{
	if (gWorld == NULL)
	{	
		Pimp::gD3D->ClearBackBuffer();
		Pimp::gD3D->Flip();
		return;
	}

	int numStepsTotal = gNumTotalMaterialCompilationJobs+1; // 1 extra for the texture generator
	int numStepsDone = texturesLoaded ? 1 : 0;

	for (int i=0; i<materialCompilationJobs->Size(); ++i)
		if ((*materialCompilationJobs)[i].ready)
			numStepsDone++;

	float progress = (numStepsTotal > 0) ? ((float)numStepsDone / (float)numStepsTotal) : 0.0f;
	if (progress > 1.0f)
		progress = 1.0f;	

	Pimp::PostProcess* postProcess = gWorld->GetPostProcess();
	postProcess->SetLoadProgress(progress);
	postProcess->Clear();
	postProcess->BindForRenderScene();

	// Flip once to get rid of the white screen
	Pimp::gD3D->ClearBackBuffer();
	postProcess->RenderPostProcess();
	Pimp::gD3D->Flip();
}
