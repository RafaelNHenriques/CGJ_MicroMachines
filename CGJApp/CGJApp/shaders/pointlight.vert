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

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;

	vec4 position;
	vec2 tex_coord;
} DataOut;

void main () {

	vec4 pos = m_viewModel * position;// posição dos vértices

    vec3 n = normalize(m_normal * normal.xyz);//Vetor normal

    vec3 l_dir =  normalize(vec3(l_pos-pos));//Direção da luz

    vec3 e_dir = vec3(-pos); //Direção do eye

	vec3 aux;


	if(texMode == 3 || useNormalMap)  {  //convert eye and light vectors to tangent space

		//Calculate components of TBN basis in eye space
		vec3 t = normalize(m_normal * tangent.xyz);  
		vec3 b = tangent.w * cross(n, t);

		aux.x = dot(l_dir, t);
		aux.y = dot(l_dir, b);
		aux.z = dot(l_dir, n);
		l_dir = normalize(aux);

		aux.x = dot(e_dir, t);
		aux.y = dot(e_dir, b);
		aux.z = dot(e_dir, n);
		e_dir = normalize(aux);
	}


	DataOut.tex_coord = texCoord.st;

	DataOut.normal = n;
	DataOut.lightDir = l_dir;
	DataOut.eye = e_dir;
	DataOut.position = m_viewModel *  position;
	gl_Position = m_pvm * position;	
}