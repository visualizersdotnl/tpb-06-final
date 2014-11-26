
inline DWORD AlphaToVtxColor(float alpha, unsigned int RGB = 0xffffff)
{
	const unsigned char iAlpha = int(alpha*255.f);
	return iAlpha<<24|RGB;
}

// Floating point random (FIXME: to be deprecated because it has very poor distribution).
inline float randf(float range)
{
	return range*((float) rand() / RAND_MAX);
}

// UV multiplier for when you're tiling square textures.
const float kTileMul = PIMPPLAYER_RENDER_ASPECT_RATIO;

// Create dynamic shader parameter, bound to any material that uses it.
// No need to release as it's a world element.
static Pimp::MaterialParameter *CreateDynShaderParam(
	const std::string &name,
	Pimp::MaterialParameter::ValueType valueType = Pimp::MaterialParameter::VT_Value)
{
	Pimp::MaterialParameter *newParam = new Pimp::MaterialParameter(s_pWorld);
	s_pWorld->GetElements().push_back(newParam);
	newParam->SetValueType(Pimp::MaterialParameter::VT_Value);
	newParam->SetName(name.c_str());
	return newParam;
}
