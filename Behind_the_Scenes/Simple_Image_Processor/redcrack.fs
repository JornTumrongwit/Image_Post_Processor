#version 330 core
out vec3 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

#define NEWTON_ITER 1
#define HALLEY_ITER 1

// texture sampler
uniform sampler2D texture1;

const vec3 red = vec3(1.0, 0.1, 0.2);
const vec3 black = vec3(0.0, 0.0, 0.0);
float cbrtf( float x )
{
	float y = sign(x) * uintBitsToFloat( floatBitsToUint( abs(x) ) / 3u + 0x2a514067u );

	for( int i = 0; i < NEWTON_ITER; ++i )
    	y = ( 2. * y + x / ( y * y ) ) * .333333333;

    for( int i = 0; i < HALLEY_ITER; ++i )
    {
    	float y3 = y * y * y;
        y *= ( y3 + 2. * x ) / ( 2. * y3 + x );
    }
    
    return y;
}

//ACES Tonemapping
const mat3 ACESInputMat = mat3(
    0.59719, 0.35458, 0.04823,
    0.07600, 0.90834, 0.01566,
    0.02840, 0.13383, 0.83777
);

// ODT_SAT => XYZ => D60_2_D65 => sRGB
const mat3 ACESOutputMat = mat3(
     1.60475, -0.53108, -0.07367,
    -0.10208,  1.10813, -0.00605,
    -0.00327, -0.07276,  1.07602
);

// sRGB -> XYZ
const mat3 XYZfromRGB = mat3(
     0.4124564,  0.3575761,  0.1804375,
     0.2126729,  0.7151522,  0.0721750,
     0.0193339, 0.1191920,  0.9503041
);

// XYZ -> sRGB
const mat3 XYZtoRGB = mat3(
     3.2404542, -1.5371385, -0.4985314,
    -0.9692660,  1.8760108,  0.0415560,
     0.0556434, -0.2040259,  1.0572252
);

// going from srgb to oklab
vec3 sRGBtoOklab(vec3 c) 
{
    float l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
	float m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
	float s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

    l = cbrtf(l);
    m = cbrtf(m);
    s = cbrtf(s);

    return vec3(
        0.2104542553f*l + 0.7936177850f*m - 0.0040720468f*s,
        1.9779984951f*l - 2.4285922050f*m + 0.4505937099f*s,
        0.0259040371f*l + 0.7827717662f*m - 0.8086757660f*s
    );
}

// going from oklab to srgb
vec3 OklabtosRGB(vec3 c) 
{
    float l_ = c.r + 0.3963377774f * c.g + 0.2158037573f * c.b;
    float m_ = c.r - 0.1055613458f * c.g - 0.0638541728f * c.b;
    float s_ = c.r - 0.0894841775f * c.g - 1.2914855480f * c.b;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    return vec3(
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s
    );
}

vec3 labblack = sRGBtoOklab(black);
vec3 labred = sRGBtoOklab(red);

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
    color = color * ACESInputMat;

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = color * ACESOutputMat;

    // Clamp to [0, 1]
    color = clamp(color, 0.0, 1.0);

    return color;
}
void main()
{
    FragColor = texture(texture1, TexCoord).rgb;
    vec3 oklabcol = sRGBtoOklab(FragColor);
    oklabcol.r = floor((1.0-oklabcol.r) * 32.0) * 3.0 *(1.0-distance(vec2(0.5, 0.5), TexCoord.xy))/ (40.0);
    oklabcol.g = labred.g;
    oklabcol.b = labred.b;
    FragColor = OklabtosRGB(oklabcol);
    /*
    FragColor.rgb = pow(FragColor.rgb, vec3(0.833));
    FragColor.rgb *= 1.07;
	FragColor.rgb = ACESFitted(FragColor.rgb);
    */
}