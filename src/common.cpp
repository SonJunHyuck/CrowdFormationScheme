#include "common.h"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string& filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open()) 
    {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}

glm::vec3 GetAttenuationCoeff(float distance)
{
    const auto linear_coeff = glm::vec4(8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01);
    const auto quad_coeff = glm::vec4(-7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02);

    float d = 1.0f / distance;
    auto dvec = glm::vec4(1.0f, d, d * d, d * d * d);
    float kc = 1.0f;
    float kl = glm::dot(linear_coeff, dvec);
    float kq = glm::dot(quad_coeff, dvec);

    return glm::vec3(kc, glm::max(kl, 0.0f), glm::max(kq * kq, 0.0f));
}

void ClampVec3(glm::vec3 &InVec3, const float MaxValue)
{
	float Length = glm::length(InVec3);

	if (Length > MaxValue)
	{
		float Multi = (MaxValue / Length);
        InVec3 *= Multi;
	}
}

void ProjectOnVec3(const glm::vec3 &InVec3, const glm::vec3 &InUnitVec3, glm::vec3 &OutVec3)
{
	float Length = glm::dot(InVec3, InUnitVec3);
    OutVec3 = InUnitVec3 * Length;
}