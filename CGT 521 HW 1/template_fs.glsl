#version 430

layout(location = 1) uniform float time;
layout(location = 2) uniform sampler2D diffuse_color;

out vec4 fragcolor;     

in vec2 tex_coord;
in vec3 normal;
in vec3 FragPos;
in vec3 color;

vec3 lightPos = vec3(1.0f);
      
void main(void)
{   
	vec4 objColor = texture(diffuse_color, tex_coord) * vec4(color, 1.0f);

	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(objColor.xyz);

	// diffuse 
    vec3 nw = normalize(normal);
    vec3 lw = normalize(lightPos - FragPos);
    float diff = max(dot(nw, lw), 0.0);
    vec3 diffuse = diff  * vec3(objColor.xyz);

	//fragcolor = texture(diffuse_color, tex_coord);
	fragcolor = vec4(diffuse + ambient, 1.0f);
}




















