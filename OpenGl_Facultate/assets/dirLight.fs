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
} fs_in;

out vec4 FragColor;

uniform sampler2D diffuseTex;
uniform bool  useTexture;
uniform vec3  objectColor;
uniform vec3  viewPos;
uniform DirLight light;
uniform float shininess;

vec3 baseColor(){
    return useTexture ? texture(diffuseTex,fs_in.Tex).rgb
                      : objectColor;
}

void main(){
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir  = normalize(viewPos - fs_in.FragPos);

    vec3 ambient  = light.ambient * baseColor();

    float diff    = max(dot(norm,lightDir),0.0);
    vec3 diffuse  = light.diffuse * diff * baseColor();

    vec3 reflectDir = reflect(-lightDir,norm);
    float spec      = pow(max(dot(viewDir,reflectDir),0.0),shininess);
    vec3 specular   = light.specular * spec;

    FragColor = vec4(ambient + diffuse + specular,1.0);
}
