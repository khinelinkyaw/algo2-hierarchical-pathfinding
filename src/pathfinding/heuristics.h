#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <algorithm>
#include <cmath>
#include <string>

// Code from Game AI Pathfinding assignment

namespace HP
{
	class Heuristic
	{
	public:
		virtual float Calculate(float x, float y) const = 0;
	};

	class Manhattan final : public Heuristic
	{
	public:
		static inline std::string Name{ "Manhattan" };
		float Calculate(float x, float y) const override
		{
			return x + y;
		}
	};

	class Euclidean final : public Heuristic
	{
	public:
		static inline std::string Name{ "Euclidean" };
		float Calculate(float x, float y) const override
		{
			return sqrtf(x * x + y * y);
		}
	};

	class SqEuclidean final : public Heuristic
	{
	public:
		static inline std::string Name{ "SqEuclidean" };
		float Calculate(float x, float y) const override
		{
			return sqrtf(x * x + y * y);
		}
	};

	class Octile final : public Heuristic
	{
	public:
		static inline std::string Name{ "Octile" };
		float Calculate(float x, float y) const override
		{
			float constexpr f = 0.414213562373095048801f; // == sqrt(2) - 1;
			return x < y ? f * x + y : f * y + x;
		}
	};

	class Chebyshev final : public Heuristic
	{
	public:
		static inline std::string Name{ "Chebyshev" };
		float Calculate(float x, float y) const override
		{
			return std::max(x, y);
		}
	};
};

#endif