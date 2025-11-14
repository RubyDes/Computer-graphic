#pragma once

#include "../libgl/GLApplication.h"
#include "../libgl/Shaders.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char* title, int width, int height);
	~CMyApplication() = default;

protected:
	virtual void OnDisplay();
	virtual void OnInit();
	virtual void OnReshape(int width, int height);

private:
	void InitShaders();

	CProgram m_program;
	CShader m_vertexShader;
};
