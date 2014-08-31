#include "SceneTools.h"


void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent)
{
	Pimp::World::StaticAddChildToParent(nodeChild, nodeParent);
}


#define NUM_MAX_ANIMCURVEKEYS_GENTRACK 5120 //2560


static void AddAnimCurveKey(FixedSizeList<Pimp::AnimCurve::Pair>& keys, float time, float value)
{
	Pimp::AnimCurve::Pair key;

	key.time = time;
	key.value = value;
	key.inTangentY = key.outTangentY = 0;

	keys.Add(key);
}

//void GenerateAnimCurveForSoundtrackEvents(
//	Pandora::System* soundSystem,
//	Pimp::AnimCurve* curve, 
//	int channel, float attackTimeSeconds, float decayTimeSeconds,
//	float outValueMin, float outValueMax, float predelay, bool incremental, int eventType, int noteMin, int noteMax)
//{
//	float outRange = outValueMax - outValueMin;
//
//	if (incremental)
//	{
//		attackTimeSeconds = 0;
//	}
//
//	struct Spike
//	{
//		float time;
//		float value;
//	};
//
//	FixedSizeList<Spike> spikes(NUM_MAX_ANIMCURVEKEYS_GENTRACK);
//	FixedSizeList<Pimp::AnimCurve::Pair> keys(NUM_MAX_ANIMCURVEKEYS_GENTRACK*4);
//
//	// Step one: gather relevant spikes
//	
//	const Pandora::Track& track = soundSystem->GetTrack();
//	const Pandora::Track::Event* finger = track.GetEvents();
//
//	unsigned int timeInSamples = 0;
//
//	for (int i=0; i<track.GetNumEvents(); ++i)
//	{
//		timeInSamples += finger->samplesSincePreviousEvent;
//
//		if (finger->type == Pandora::Track::Event::TYPE_NOTEON &&
//			(channel == -1 || finger->channel == channel))
//		{
//			Spike newSpike = {
//				(float)timeInSamples * (1.0f / 44100.0f) + predelay,
//				(finger->velocity * outRange) / 255.0f + (incremental ? 0.0f : outValueMin)
//			};
//
//			spikes.Add(newSpike);
//		}
//
//		++finger;
//	}
//
//	// Now interpolate inbetween
//	
//	if (spikes.Size() > 0)
//	{
//		const float endTime = spikes[spikes.Size()-1].time + decayTimeSeconds + attackTimeSeconds;
//		float t = 0;
//
//		int nextSpikeIndex = 0;
//
//		float valueMin = outValueMin;
//
//		while (t <= endTime)
//		{
//			if (nextSpikeIndex < (int)spikes.Size())
//			{
//				float deltaTime = spikes[nextSpikeIndex].time - t;
//
//				float decayEndTime = t + decayTimeSeconds;
//				float attackStartTime = spikes[nextSpikeIndex].time - attackTimeSeconds;
//
//				if (incremental)
//				{
//					if (nextSpikeIndex == 0)
//					{
//						AddAnimCurveKey(keys, spikes[nextSpikeIndex].time, valueMin);
//					}
//					else if (decayEndTime < attackStartTime)
//					{
//						// Value key
//						AddAnimCurveKey(keys, decayEndTime, valueMin + spikes[nextSpikeIndex].value);
//
//						// Value key
//						AddAnimCurveKey(keys, spikes[nextSpikeIndex].time, valueMin + spikes[nextSpikeIndex].value);
//
//						valueMin += spikes[nextSpikeIndex].value;
//					}
//					else
//					{
//						// decayendtime > attackstarttime!
//
//						float deltaValue = spikes[nextSpikeIndex].value;
//						float alpha = (decayTimeSeconds > 0) ? (deltaTime / decayTimeSeconds) : 0.0f;
//
//						float dv = alpha*deltaValue;
//
//						// Value key
//						AddAnimCurveKey(keys, spikes[nextSpikeIndex].time, valueMin + dv);
//
//						valueMin += dv;
//					}
//
//				}
//				else
//				{
//					if (nextSpikeIndex == 0)
//					{
//						// No decay needed!
//						// Start an attack interval
//
//						// Attack key
//						AddAnimCurveKey(keys, attackStartTime, outValueMin);
//
//						// Value key
//						AddAnimCurveKey(keys, spikes[nextSpikeIndex].time, spikes[nextSpikeIndex].value);
//					}
//					else if (decayEndTime < attackStartTime) 
//					{
//						// First a decay that completely ends, and then an attack
//
//						// Decay key
//						AddAnimCurveKey(keys, decayEndTime, outValueMin);
//
//						// Attack key
//						AddAnimCurveKey(keys, attackStartTime, outValueMin);
//
//						// Value key
//						AddAnimCurveKey(keys, spikes[nextSpikeIndex].time, spikes[nextSpikeIndex].value);
//					}
//					else
//					{
//						// The decay intersects an attack! So it should be prematurely ended!
//
//						float t1 = spikes[nextSpikeIndex-1].time;
//						float t2 = spikes[nextSpikeIndex].time;
//						float v1 = spikes[nextSpikeIndex-1].value;
//						float v2 = spikes[nextSpikeIndex].value;
//
//						ASSERT_MSG( decayEndTime >= attackStartTime, "Impossible intersection parameters" );
//						ASSERT_MSG( decayEndTime >= t1, "Impossible intersection parameters" );
//						ASSERT_MSG( attackStartTime <= t2, "Impossible intersection parameters" );
//						ASSERT_MSG( t2 >= t1, "Impossible intersection parameters" );
//
//						float vDelta = (v1 - v2) / outRange;
//						float tOffset = (t1 / (decayEndTime - t1)) + (t2 / (t2 - attackStartTime));
//						float tScale = (1 / (decayEndTime - t1)) + (1 / (t2 - attackStartTime));
//						float intersectionTime = (vDelta + tOffset) / tScale;
//
//						intersectionTime = min(max(intersectionTime, t1), t2);
//
//						//ASSERT_MSG( intersectionTime >= t1 && intersectionTime <= t2, "Impossible intersection time!" );
//
//						float valueIntersection = v1 - outRange * ((intersectionTime-t1)/(decayEndTime-t1));
//
//						if (valueIntersection < outValueMin)
//							valueIntersection = outValueMin;
//
//						// Intersection key of decay with attack
//						AddAnimCurveKey(keys, intersectionTime, valueIntersection);
//
//						// Value key
//						AddAnimCurveKey(keys, spikes[nextSpikeIndex].time, spikes[nextSpikeIndex].value);
//					}
//				}
//
//
//				t += deltaTime;							
//				nextSpikeIndex++;
//			}
//			else 
//			{
//				if (!incremental)
//				{
//					// The last decay key after our last note.
//					AddAnimCurveKey(keys, t+decayTimeSeconds, outValueMin);
//				}
//
//
//				// And quit by leaving the loop					
//				break;
//			}
//		}
//	}
//
//	// Set keys
//	curve->SetKeysPtr(keys.GetItemsPtr(), keys.Size());
//}


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

extern HWND gHwnd;
extern Pimp::World* gWorld;
static int gNumTotalMaterialCompilationJobs = 1;

void SetNumTotalMaterialCompilationJobs(int count)
{
	gNumTotalMaterialCompilationJobs = count;

	DrawLoadProgress(false);
}






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

