#include "../libgl/pch.h"
#include "Canabola.h"
#define _USE_MATH_DEFINES
#include <math.h>

// доработать программу, чтобы выводилось восьмерка
void Canabola::Draw()
{
	auto y = 0.005f;
	auto z = 1.0f;
	glPointSize(1.0f);
	glBegin(GL_LINES);
	float step = float(M_PI / 1000);

	for (float x = 0; x < 2 * M_PI; x += step)
	{
		glVertex3f(x + step, y, z);
		glVertex3f(x, y, z);
	}
	glEnd();
}