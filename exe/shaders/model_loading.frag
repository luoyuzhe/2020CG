#version 330 core
out vec4 FragColor;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
  
} fs_in;

uniform sampler2D diffuseTexture;
uniform float shininess;

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirlight;

struct PointLight{
	vec3 position;
	
	float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pointlight;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{           
   
   
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);   

	vec3 result=CalcDirLight(dirlight,normal,viewDir);
	result+=CalcPointLight(pointlight,normal,fs_in.FragPos,viewDir);
	
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * spec * color;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	// ambient
    vec3 ambient = light.ambient;
    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.diffuse;
    // specular
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	// 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 
                 0.032 * (distance * distance));  
    vec3 specular = spec * light.specular;    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient +  (diffuse + specular)) * color;
}
