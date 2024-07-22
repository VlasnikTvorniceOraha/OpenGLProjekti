#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include <fstream>
#include <sstream>

class Shader
{
private:
	void checkCompilerErrors(unsigned int shader, std::string type);

public:

	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	~Shader();

	void use();
	void setUniform(const std::string &name, bool value)const;
	void setUniform(const std::string &name, int value)const;
	void setUniform(const std::string &name, float value)const;
	void setUniformMat4(const std::string& name, glm::mat4 matrica)const;
	void setUniformVec3(const std::string& name, glm::vec3 vektor)const;


};

