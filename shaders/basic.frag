#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

uniform int fogCond;


//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

uniform int muzzleFlash;
uniform float intensity;

//lumini punctiforme
float constant = 0.01f;
float linear = 0.0f;
float quadratic = 0.03f;

vec3 muzzle = vec3(0.132f, -0.082f, -1.028f);
vec3 lightPosColor= vec3(1.0f,0.67f,0.0f);


void computeDirLight()
{
    //compute eye space coordinates
    //vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    //vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(fNormal, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, fNormal);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

void computePosLight()//lumina punctiforma
{
    //viewer is situated at the origin
    //vec3 cameraPosEye = vec3(0.0f);

    //compute eye space coordinates
    //vec4 fPosEye = vec4(fPosition, 1.0f);
    //vec3 normalEye = normalize(fNormal);

    //normalize lightPosEye
    //vec3 lightPosEye = vec3(normalize(view * vec4(lightPos, 0.0f)));

    //compute light direction
    vec3 lightDirN = normalize(muzzle - fPosEye.xyz);

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(-fPosEye.xyz);

    //compute half vector
    vec3 halfVector = normalize(lightDirN + viewDir);

    //compute specular light
    float specCoeff = pow(max(dot(fNormal, halfVector), 0.0f), 32.0f);

    //compute distance to light
    float dist = length(muzzle - fPosEye.xyz);
    //compute attenuation
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist))*0.03f*intensity;

    //compute ambient light
    ambient += att * ambientStrength * lightPosColor;
    //compute diffuse light
    diffuse += att * max(dot(fNormal, lightDirN), 0.0f) * lightPosColor;
    //specular += att * specularStrength * specCoeff * lightPosColor;
}

float computeShadow() {
    //perform perspective divide
    //return position of the current fragment in [-1,1]
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    if (normalizedCoords.z > 1.0f) {
        return 0.0f;
    }

    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;

    // Check whether current frag pos is in shadow
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float computeFog(){
    float fogDensity = 0.05f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}


void main()
{
    computeDirLight();

    if(muzzleFlash==1)
        computePosLight();
    float shadow = computeShadow();

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);


    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

    //compute final vertex color
    vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);

    if (fogCond == 0)
        fColor = fogColor *(1 - fogFactor) + vec4(color, 0.0f) * fogFactor;
    else
        fColor = vec4(color, 1.0f);

    //fColor = vec4(color, 1.0f);
}
