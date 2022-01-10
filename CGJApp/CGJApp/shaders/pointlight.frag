#version 430

out vec4 colorOut;

struct LightProperties {
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	vec4 position;
	vec3 halfVector;
	vec4 coneDirection;
	float spotCosCutoff;
	float spotExponent;
};

const int MaxLights = 9; // 9 number of lights in scene
uniform LightProperties Lights[MaxLights];

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;
uniform sampler2D texmap4;
uniform sampler2D normalMap;
uniform samplerCube cubeMap;


uniform int texMode;
uniform mat4 m_View;

uniform int toggleFog;
uniform bool shadowMode;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;

	vec4 position;

	vec2 tex_coord;
} DataIn;

const float reflect_factor = 0.1;

uniform int diffMapCount;
uniform bool specularMap;
uniform bool useNormalMap;


void main() {

 vec3 n;
 vec3 e = normalize(DataIn.eye);
 
 
 vec3 spec = vec3(0.0);//componente especular


 float totalIntensity = 0.0;
 vec4 totalSpec = vec4(0.0), cube_texel;
 for (int lightId = 0; lightId < MaxLights; ++lightId) {
	LightProperties light = Lights[lightId];
	
	if (!light.isEnabled)
		continue;
	
	vec3 lightDirection = vec3(-light.position); // luz direcional

	if(texMode == 6 || useNormalMap)  // lookup normal from normal map, move from [0,1] to [-1, 1] range, normalize
		n = normalize(2.0 * texture(normalMap, DataIn.tex_coord).rgb - 1.0);
	else
		n = normalize(DataIn.normal);

	vec3 l = normalize(vec3(light.position - DataIn.position));
	float intensity = 0.0f;
	float intSpec = 0.0f;
	vec4 spec = vec4(0.0);


	if (light.isLocal && light.isSpot) // spotlight
	{
		float att = 0; // attenuation

		vec3 sd = normalize(vec3(-light.coneDirection));

		float spotCos = dot(l, sd);
		if(spotCos > light.spotCosCutoff)  {	//inside cone?
			att = pow(spotCos, light.spotExponent);
			intensity = max(dot(n,l), 0.0) * att;
			if (intensity > 0.0) {
				vec3 h = normalize(l + e);
				intSpec = max(dot(h,n), 0.0);
				spec = mat.specular * pow(intSpec, mat.shininess) * att;
			}
		}
	}
	else if (light.isLocal && !light.isSpot) // point light
	{
		intensity = max(dot(n,l), 0.0);
		if (intensity > 0.0) {
			vec3 h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec = mat.specular * pow(intSpec, mat.shininess);
		}
	}
	else if (!light.isLocal) // directional light same has point but origin is point in infinity so no direction calc
    {
        intensity = max(dot(n,lightDirection), 0.0);
        if (intensity > 0.0) {
            vec3 h = normalize(vec3(0.0, -1.0, 0.0)); // FIXME hardcoded half-vector
            intSpec = max(dot(h,n), 0.0);
            spec = mat.specular * pow(intSpec, mat.shininess);
        }
    }

	totalIntensity += intensity;
	totalSpec += spec;
 }

	 totalIntensity *= 0.6;
	 totalSpec *= 0.6;

 	vec4 texel, texel1; 

	if(shadowMode)
		colorOut = vec4(vec3(0.5), 1.0);
	else {
		if(texMode == 0) // no texture applied
		{
			colorOut = max(totalIntensity * mat.diffuse + totalSpec, mat.ambient);
		}
		else if (texMode == 3) // multitexturing
		{
			texel = texture(texmap2, DataIn.tex_coord);
			texel1 = texture(texmap1, DataIn.tex_coord);
			colorOut = vec4(max(totalIntensity * texel.rgb * texel1.rgb + totalSpec.rgb, 0.07 * texel.rgb * texel1.rgb), 0.80);
		}
	}

	//Efeito de nevoeiro
	if (toggleFog == 1)
    {
        vec4 fogColor = vec4(0.9, 0.9, 0.9, 1.0);
        float fogAmount = exp(-length(DataIn.position) * 0.05); // position in camera space
        colorOut = mix(fogColor, colorOut, fogAmount);
    }
}