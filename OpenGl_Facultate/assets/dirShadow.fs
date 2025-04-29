#version 330 core
struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
    vec4 FragPosLight;
} fs_in;

out vec4 FragColor;

uniform sampler2D diffuseTex;
uniform sampler2D shadowMap;
uniform bool  useTexture;
uniform vec3  objectColor;
uniform vec3  viewPos;
uniform DirLight light;
uniform float shininess;

// ── helpers ────────────────────────────────────────────────────────────
vec3 baseColor(){
    return useTexture ? texture(diffuseTex,fs_in.Tex).rgb : objectColor;
}
float ShadowCalculation(vec4 fragPosLight)
{
    // perspective divide
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;       // [0,1]

    // outside frustum?
    if(projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(normalize(fs_in.Normal), normalize(-light.direction))), 0.003);
    return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
}

void main()
{
    vec3 norm  = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir  = normalize(viewPos - fs_in.FragPos);

    // ambient
    vec3 ambient = light.ambient * baseColor();

    // diffuse
    float diff   = max(dot(norm,lightDir),0.0);
    vec3 diffuse = light.diffuse * diff * baseColor();

    // specular
    vec3 reflectDir = reflect(-lightDir,norm);
    float spec = pow(max(dot(viewDir,reflectDir),0.0), shininess);
    vec3 specular = light.specular * spec;

    float shadow = ShadowCalculation(fs_in.FragPosLight);
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(result,1.0);
}
