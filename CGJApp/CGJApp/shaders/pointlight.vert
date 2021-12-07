#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

uniform vec4 l_pos;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria

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
	vec4 color;
} DataOut;

void main () {

    vec4 spec = vec4(0.0);
 
    vec3 n = normalize(m_normal * vec3(normal));

    vec3 l_dir =  normalize(vec3(l_pos-(m_viewModel * position)));
 
    float intensity = max(dot(n,l_dir), 0.0);
 
    // if the vertex is lit compute the specular term
    if (intensity > 0.0) {
        // compute position in camera space
        vec3 pos = vec3(m_viewModel * position);
        // compute eye vector and normalize it 
        vec3 eye = normalize(-pos);
        // compute the half vector
        vec3 h = normalize(vec3(l_pos-(m_viewModel * position)) + eye);
 
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = mat.specular * pow(intSpec, mat.shininess);
    }
	DataOut.color = max(intensity *  mat.diffuse + spec, mat.ambient);

	gl_Position = m_pvm * position;	
}