#include <Engine/CoDUO.h>
#include "ScrVar.h"

using namespace CoDUO;

ScrVar::ScrVar() { SetUndefined(); }

void ScrVar::SetUndefined() { Type = VarType::Undefined; Value = 0; }
void ScrVar::SetInt(int32_t value) { Type = VarType::Integer; Value = value; }
void ScrVar::SetFloat(float value) { Type = VarType::Float; Value = value; }
void ScrVar::SetString(const char* value, bool localized) { Type = localized ? VarType::LocalizedString : VarType::String; Value = std::string(value); }
void ScrVar::SetVector(const vec3_t& value) { Type = VarType::Vector; Value = value; }
void ScrVar::SetEntity(gentity_t* ent) { Type = VarType::Entity; Value = ent; }
void ScrVar::SetInternal(int32_t value, VarType type) { Type = type; Value = value; }

VarType ScrVar::GetType() const { return Type; }
const char* ScrVar::GetTypeName() const { return Scr_GetTypeName(Type); };

bool ScrVar::IsType(VarType type) const { return Type == type; }
bool ScrVar::IsNumber() const { return Type == VarType::Integer || Type == VarType::Float; }
bool ScrVar::IsString() const { return Type == VarType::String || Type == VarType::LocalizedString; }
bool ScrVar::IsVector() const { return Type == VarType::Vector; }
bool ScrVar::IsEntity() const { return Type == VarType::Entity; }

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

	static const std::string empty{};
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

gentity_t* ScrVar::GetEntity() const
{
	if (Type == VarType::Entity)
		return std::get<gentity_t*>(Value);
	return nullptr;
}

ScrVar ScrVar::From(VariableValue* var)
{
	ScrVar scrVar;

	VarType type = Scr_GetVarType(var);
	switch (type)
	{
	case VarType::Undefined:
		scrVar.SetUndefined();
		break;
	case VarType::String:
	case VarType::LocalizedString:
	{
		const char* str = SL_ConvertToString(var->stringValue);
		bool localized = type == VarType::LocalizedString;
		scrVar.SetString(str ? str : "", localized);
		break;
	}
	case VarType::Vector:
		scrVar.SetVector(*var->vectorValue);
		break;
	case VarType::Float:
		scrVar.SetFloat(var->floatValue);
		break;
	case VarType::Integer:
		scrVar.SetInt(var->intValue);
		break;
	case VarType::Entity:
	{
		gentity_t* ent = Scr_GetVarEntity(var);
		scrVar.SetEntity(ent);
		break;
	}
	default:
		scrVar.SetInternal(var->intValue, type);
	}

	return scrVar;
}