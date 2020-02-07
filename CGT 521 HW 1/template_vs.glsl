#version 430            
layout(location = 0) uniform mat4 PV;
layout(location = 4) uniform mat4 M;

layout(location = 0) in vec3 pos_attrib;
layout(location = 1) in vec3 tex_coord_attrib;
layout(location = 2) in vec3 norm_attrib;
layout(location = 3) in vec3 color_attrib;
layout(location = 4) in mat4 offset_attrib;


out vec2 tex_coord;
out vec3 normal;
out vec3 FragPos;
out vec3 color;

void main(void)
{
   gl_Position = PV * M * offset_attrib * vec4(pos_attrib, 1.0);
   tex_coord = vec2(tex_coord_attrib.x, tex_coord_attrib.y);
   normal = norm_attrib;
   FragPos = vec3(M * vec4(pos_attrib, 1.0));
   color = color_attrib;
}