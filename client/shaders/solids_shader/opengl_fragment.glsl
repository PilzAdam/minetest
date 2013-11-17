uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D useNormalmap;

uniform float enableBumpmapping;
uniform float parallaxMappingMode;
uniform float parallaxMappingScale;
uniform float parallaxMappingBias;


uniform vec4 skyBgColor;
uniform float fogDistance;
uniform vec3 eyePosition;

varying vec3 vPosition;
varying vec3 tsEyeVec;
varying vec3 eyeVec;
varying vec4 vColor;

const float e = 2.718281828459;

void main (void)
{
	float use_normalmap = texture2D(useNormalmap,vec2(1.0,1.0)).r;
	float enable_bumpmapping = enableBumpmapping;

	vec3 color;
	vec2 uv = gl_TexCoord[0].st;
	float height;
	vec2 tsEye = tsEyeVec.xy;
	
	if ((parallaxMappingMode == 1.0) && (use_normalmap > 0.0)) {
		float map_height = texture2D(normalTexture, uv).a;
			if (map_height < 1.0){
				float height = parallaxMappingScale * map_height - parallaxMappingBias;
				uv = uv + height * tsEyeVec.xy;
			}
	}

	if ((parallaxMappingMode == 2.0) && (use_normalmap > 0.0)) {
		const float numSteps = 40.0;
		float height = 1.0;
		float step = 1.0 / numSteps;
		vec4 NB = texture2D(normalTexture, uv);
		vec2 delta = tsEye * parallaxMappingScale / numSteps;
		for (float i = 0.0; i < numSteps; i++) {
		if (NB.a < height) {
			height -= step;
			uv += delta;
			NB = texture2D(normalTexture, uv);
		} else {
			break;
		}
		}
	}

	if ((enable_bumpmapping == 1.0) && (use_normalmap > 0.0)) {
		vec4 base = texture2D(baseTexture, uv);
		vec3 vVec = normalize(eyeVec);
		vec3 bump = normalize(texture2D(normalTexture, uv).xyz * 2.0 - 1.0);
		vec3 R = reflect(-vVec, bump);
		vec3 lVec = normalize(vVec*2);
		float diffuse = max(dot(lVec, bump), 0.0);
		float specular = pow(clamp(dot(R, lVec), 0.0, 1.0),1.0);
		color = diffuse * base + 0.1 * specular * diffuse;
	} else {
		color = texture2D(baseTexture, uv).rgb;
	}

	float alpha = texture2D(baseTexture, uv).a;
	float e_const = 2.71828;
	vec4 col = vec4(color.r, color.g, color.b, alpha) * vColor;
	col *= gl_Color;
	col = col * col; // SRGB -> Linear
	col *= 1.8;
	col.r = 1.0 - exp(1.0 - col.r) / e;
	col.g = 1.0 - exp(1.0 - col.g) / e;
	col.b = 1.0 - exp(1.0 - col.b) / e;
	col = sqrt(col); // Linear -> SRGB
	if(fogDistance != 0.0){
		float d = max(0.0, min(vPosition.z / fogDistance * 1.5 - 0.6, 1.0));
		col = mix(col, skyBgColor, d);
	}
    gl_FragColor = vec4(col.r, col.g, col.b, alpha);   
}
