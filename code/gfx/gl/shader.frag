#version 460

const float EyeToScreen = 80.0f;
const float ScreenWidth = 60.0f;
const float ScreenHeight = 34.0f;

out vec4 Out;
in vec2 ScreenPos;



float sdfSphere(vec3 Position, float Radius)
{
	return length(Position)-Radius;
}

float sdfBox(vec3 Position, vec3 HalfDiagonal)
{
	vec3 Dist = abs(Position) - HalfDiagonal;
	return length(max(Dist,0.0)) + min(max(Dist.x,max(Dist.y,Dist.z)),0.0);
}

float sdfUnion(float d1, float d2) { return min(d1,d2); }
float sdfSubtraction(float d1, float d2) { return max(-d1,d2); }
float sdfIntersection(float d1, float d2) { return max(d1,d2); }

float sdfSmoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

float sdfSmoothSubtraction(float d1, float d2, float k) {
    float h = clamp(0.5 - 0.5 * (d2 + d1) / k, 0.0, 1.0);
    return mix(d2, -d1, h) + k * h * (1.0 - h);
}

float sdfSmoothIntersection(float d1, float d2, float k) {
    float h = clamp(0.5 - 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(d2, d1, h) + k * h * (1.0 - h);
}

float sdfScene(vec3 Position)
{
	float s1 = sdfSphere(Position - vec3(-0.1,0,0.1), 0.3);
	float s2 = sdfSphere(Position - vec3(0.1,0,0), 0.2);

	return sdfSubtraction(s1, s2);
}

float Raymarch(vec3 Origin, vec3 Direction)
{
	float t = 0.0f;
	for (int i = 0; i < 256; ++i)
	{
		float Dist = sdfScene(Origin + Direction * t);
		if (Dist < 0.0003)
			return t;
		t += Dist;
	}
}

vec3 CalcNormal(vec3 Position)
{
    const float h = 0.0001;
    const vec2 k = vec2(1,-1);
    return normalize(k.xyy * sdfScene(Position + k.xyy * h) + 
                     k.yyx * sdfScene(Position + k.yyx * h) + 
                     k.yxy * sdfScene(Position + k.yxy * h) + 
                     k.xxx * sdfScene(Position + k.xxx * h));
}



void main(){
	vec3 CameraPos = vec3(0,0,1);
	vec3 CameraDir = vec3(ScreenPos.x, ScreenPos.y, -1.0);
	CameraDir = CameraDir * vec3(ScreenWidth, ScreenHeight, EyeToScreen);
	CameraDir = normalize(CameraDir);

	float Depth = Raymarch(CameraPos, CameraDir);
	vec3 Normal = CalcNormal(CameraPos + CameraDir * Depth);

	Out.rgb = vec3(Normal);
	Out.rgb = vec3(0,0,1);
}
