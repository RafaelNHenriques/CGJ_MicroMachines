#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

uniform mat4 m_Model;

uniform int texMode;

uniform bool useNormalMap;

uniform vec4 l_pos;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 color;
in vec4 texCoord;
in vec4 tangent;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;

	vec4 position;
} DataOut;

void main () {

	vec4 pos = m_viewModel * position;// posição dos vértices

    vec3 n = normalize(m_normal * normal.xyz);//Vetor normal

    vec3 l_dir =  normalize(vec3(l_pos-pos));//Direção da luz

    vec3 e_dir = vec3(-pos); //Direção do eye

	DataOut.normal = n;
	DataOut.lightDir = l_dir;
	DataOut.eye = e_dir;
	DataOut.position = m_viewModel *  position;
	gl_Position = m_pvm * position;	
}