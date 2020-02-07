#include <vector>
#include <glm/glm.hpp>
#include "Surf.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const int N = 50;
static const int NUM_VERTICES = N*N;

GLuint vbo;
GLuint instanceVBO;
GLuint colorVBO;

//The sinc surface
static glm::vec3 surf(int i, int j)
{
   const float center = 0.5f*N;
   const float xy_scale = 20.0f / N;
   const float z_scale = 10.0f;

   float x = xy_scale * (i - center);
   float y = xy_scale * (j - center);

   float r = sqrt(x*x + y * y);
   float z = 1.0f;

   if (r != 0.0f)
   {
      z = sin(r) / r;
   }
   z = z * z_scale;

   return 0.05f*glm::vec3(x, y, z);
}

//Sinc surface normals
static glm::vec3 normal(int i, int j)
{
   glm::vec3 du = surf(i+1, j) - surf(i-1, j);
   glm::vec3 dv = surf(i, j+1) - surf(i, j-1);
   return glm::normalize(glm::cross(du, dv));
}

static glm::vec3 texCoord(int i, int j)
{
	return glm::vec3(float(i) / N, float(j) / N, 0.0f);
}

void create_surf_vbo()
{
   //Declare a vector to hold N*N vertices
   std::vector<glm::vec3> surf_verts;
   surf_verts.reserve(NUM_VERTICES);

   std::vector<unsigned int> vertex_indices;
   surf_verts.reserve(2*NUM_VERTICES);

   std::vector<glm::mat4> offsets;
   std::vector<glm::vec3> colors;

   for (int i = -1; i < 2; i++)
   {
	   for (int j = -1; j < 2; j++)
	   {
		   glm::mat4 trans = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(1.0f));
		   offsets.push_back(glm::translate(trans, glm::vec3(2 * i, 2 * j, 0.0f)));
		   colors.push_back(glm::vec3(0.5f + 0.3f * i, 0.5f + 0.3f * j, 0.5f + 0.2f * (i+j)));
	   }
   }

   for(int i=0; i<N; i++)
   {
      for (int j = 0; j < N; j++)
      {
         surf_verts.push_back(surf(i,j));
		 surf_verts.push_back(texCoord(i,j));
		 surf_verts.push_back(normal(i,j));

		 //if (i < N - 1) {
			 vertex_indices.push_back((unsigned int)(i * N + j));
			 vertex_indices.push_back((unsigned int)((i+1) * N + j));
		 //}
      }
	  vertex_indices.push_back((unsigned int)(65535));
   }

   std::cout << vertex_indices.size() << std::endl;

   GLuint ebo;
   glGenBuffers(1, &ebo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertex_indices.size(), vertex_indices.data(), GL_STATIC_DRAW);

   glGenBuffers(1, &vbo); //Generate vbo to hold vertex attributes for triangle.
   glBindBuffer(GL_ARRAY_BUFFER, vbo); //Specify the buffer where vertex attribute data is stored.
   
   //Upload from main memory to gpu memory.
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*surf_verts.size(), surf_verts.data(), GL_STATIC_DRAW);

   glGenBuffers(1, &instanceVBO);
   glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 9, offsets.data(), GL_STATIC_DRAW);

   glGenBuffers(1, &colorVBO);
   glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 9, colors.data(), GL_STATIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, 0);

   return;
}

GLuint create_surf_vao()
{
   GLuint vao;

   //Generate vao id to hold the mapping from attrib variables in shader to memory locations in vbo
   glGenVertexArrays(1, &vao);

   //Binding vao means that bindbuffer, enablevertexattribarray and vertexattribpointer state will be remembered by vao
   glBindVertexArray(vao);

   create_surf_vbo();

   const GLint pos_loc = 0; 
   const GLint tex_coord_loc = 1;
   const GLint normal_loc = 2;
   const GLint color_loc = 3;
   const GLint offset_loc = 4;

   glBindBuffer(GL_ARRAY_BUFFER, vbo);

   glEnableVertexAttribArray(pos_loc); //Enable the position attribute.
   glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, 9*sizeof(float), 0);

   glEnableVertexAttribArray(tex_coord_loc); //Enable the position attribute.
   glVertexAttribPointer(tex_coord_loc, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(3 * sizeof(float)));

   glEnableVertexAttribArray(normal_loc); //Enable the position attribute.
   glVertexAttribPointer(normal_loc, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(6 * sizeof(float)));

   glEnableVertexAttribArray(offset_loc);
   glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

   for (int i = 0; i < 4; i++)
   {
	   glVertexAttribPointer(offset_loc + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
	   glEnableVertexAttribArray(offset_loc + i);
	   glVertexAttribDivisor(offset_loc + i, 1);
   }
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   glEnableVertexAttribArray(color_loc);
   glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
   glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glVertexAttribDivisor(color_loc, 1);

   glBindVertexArray(0); //unbind the vao

   return vao;
}

//Draw the set of points on the surface
void draw_surf(GLuint vao)
{
	glBindVertexArray(vao);
	//glDrawArrays(GL_POINTS, 0, NUM_VERTICES);

	//glDrawElements(GL_TRIANGLE_STRIP, 2 * (N-1) * N + N - 1, GL_UNSIGNED_INT, (void*)(0));
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, 2 * (N - 1) * N + N - 1, GL_UNSIGNED_INT, (void*)(0), 9);
}