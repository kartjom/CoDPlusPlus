#pragma once
#include <Structs/Engine/gsc.h>
#include <Structs/Engine/gentity.h>
#include <string>
#include <variant>

class ScrVar
{
	VarType Type;
	std::variant<int32_t, float, uint32_t, vec3_t, std::string, gentity_t*> Value;

public:
	static ScrVar From(VariableValue* var);

	ScrVar();

	void SetUndefined();
	void SetInt(int32_t value);
	void SetFloat(float value);
	void SetString(const char* value, bool localized);
	void SetVector(const vec3_t& value);
	void SetEntity(gentity_t* ent);
	void SetInternal(int32_t value, VarType type);

	VarType GetType() const;
	const char* GetTypeName() const;

	bool IsType(VarType type) const;
	bool IsNumber() const;
	bool IsString() const;
	bool IsVector() const;
	bool IsEntity() const;

	std::string GetString() const;
	const std::string& GetStringRef() const;

	vec3_t GetVector() const;
	const vec3_t& GetVectorRef() const;

	gentity_t* GetEntity() const;

	template<typename T>
		requires (std::is_same_v<T, int32_t> || std::is_same_v<T, float>)
	inline T GetNumber() const
	{
		if (std::holds_alternative<int32_t>(Value))
			return (T)std::get<int32_t>(Value);

		if (std::holds_alternative<float>(Value))
			return (T)std::get<float>(Value);

		return T{};
	}
};