#include <Engine/CoDUO.h>
#include "ScrVar.h"

ScrVar::ScrVar() { SetUndefined(); }

void ScrVar::SetUndefined() { Type = VarType::Undefined; Value = 0; }
void ScrVar::SetInt(int32_t value) { Type = VarType::Integer; Value = value; }
void ScrVar::SetFloat(float value) { Type = VarType::Float; Value = value; }
void ScrVar::SetString(const char* value, bool localized) { Type = localized ? VarType::LocalizedString : VarType::String; Value = std::string(value); }
void ScrVar::SetVector(const vec3_t& value) { Type = VarType::Vector; Value = value; }
void ScrVar::SetInternal(int32_t value, VarType type) { Type = type; Value = value; }

VarType ScrVar::GetType() const { return Type; }
const char* ScrVar::GetTypeName() const { return CoDUO::Scr_GetTypeName(Type); };

bool ScrVar::IsType(VarType type) const { return Type == type; }
bool ScrVar::IsNumber() const { return Type == VarType::Integer || Type == VarType::Float; }
bool ScrVar::IsString() const { return Type == VarType::String || Type == VarType::LocalizedString; }

std::string ScrVar::GetString() const
{
	if (Type == VarType::String)
		return std::get<std::string>(Value);
	return std::string();
}

const std::string& ScrVar::GetStringRef() const
{
	if (Type == VarType::String)
		return std::get<std::string>(Value);

	static const std::string empty;
	return empty;
}

vec3_t ScrVar::GetVector() const
{
	if (Type == VarType::Vector)
		return std::get<vec3_t>(Value);
	return vec3_t();
}

const vec3_t& ScrVar::GetVectorRef() const
{
	if (Type == VarType::Vector)
		return std::get<vec3_t>(Value);

	static const vec3_t zero{};
	return zero;
}