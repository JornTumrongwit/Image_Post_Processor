#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

// Selective Desaturation

//----------Constant-----------------------
vec3 fullDesaturate = vec3(0.0);
//----------Variables----------------------
//the value of the saturation
vec3 Saturation = vec3(1.0, 0.0, 0.0);

//degree of freedom
float freedom = 1.;

//------------------------------------------------------------------------
//------------Taken directly from Timothy Lottes' work-------------------
// sRGB to Linear.
// Assuing using sRGB typed textures this should not be needed.
float ToLinear1(float c){return(c<=0.04045)?c/12.92:pow((c+0.055)/1.055,2.4);}
vec3 ToLinear(vec3 c){return vec3(ToLinear1(c.r),ToLinear1(c.g),ToLinear1(c.b));}

// Linear to sRGB.
// Assuing using sRGB typed textures this should not be needed.
float ToSrgb1(float c){return(c<0.0031308?c*12.92:1.055*pow(c,0.41666)-0.055);}
vec3 ToSrgb(vec3 c){return vec3(ToSrgb1(c.r),ToSrgb1(c.g),ToSrgb1(c.b));}

// Nearest emulated sample given floating point position and texel offset.
// Also zero's off screen.
vec3 Fetch(vec2 pos,vec2 off){
    pos = pos + off;
    return ToLinear(texture(texture1,pos).rgb);
}

//Get Luminance of the color
float Luminance(vec3 color) {
    return max(0.00001f, dot(color, vec3(0.2127f, 0.7152f, 0.0722f)));
}

//Desaturate the color
vec3 Desaturate(vec3 color){
    vec3 blendcolor = mix(vec3(Luminance(color)), color, Saturation);
    return distance(blendcolor, Saturation) < freedom ? blendcolor: mix(vec3(Luminance(color)), color, fullDesaturate);
}

//------------Modified from taken work-------------------
// Entry.
void main(){
  // Unmodified.
  vec2 pos = TexCoord.xy;
  FragColor.rgb=Fetch(pos, vec2(0, 0)).rgb;
  //fragColor.rgb = fragColor.rgb;
  FragColor.rgb = Desaturate(FragColor.rgb);
  FragColor.a=1.0;
  FragColor.rgb=ToSrgb(FragColor.rgb);
}

