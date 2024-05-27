#include "Object.h"

Object::Object(const char* path)
{
	bool res = loadOBJ(path, this->vertices, this->uvs, this->normals);
}

void Object::draw(ShaderProgram* sp)
{
	glEnableVertexAttribArray(sp->a("vertex"));
	glEnableVertexAttribArray(sp->a("normal"));
	glEnableVertexAttribArray(sp->a("texCoord"));

	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, glm::value_ptr(uvs.front()));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, static_cast<float*>(glm::value_ptr(vertices.front())));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, static_cast<float*>(glm::value_ptr(normals.front())));

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord"));
}