#pragma once

#include <vector>

class RenderObject
{
public:
	RenderObject(std::vector<float> points, std::vector<float> color);

	void draw();

private:
	std::vector<float> m_points;
	std::vector<float> m_color;
};

