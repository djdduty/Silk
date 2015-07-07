#pragma once

namespace Silk
{
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T       O U T P U T      S E T T E R S *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * */
    
    static string IsInstancedConditional_0 =
    string("\tif(u_IsInstanced == 1)\n"
           "\t{\n");
    static string IsInstancedConditional_1 =
    string("\t}\n"
           "\telse\n"
           "\t{\n");
    static string IsInstancedConditional_2 =
    string("\t}\n");
    
    static string DefaultGLPositionInstancedFunc =
    string("\t\tvec4 _Pos = (") + InstanceTransformAttribName + " * vec4(" + PositionAttribName + ",1.0));\n" +
           "\t\tgl_Position = u_MVP * _Pos;\n";
    
    static string DefaultPositionInstancedFunc =
    string("\t\t") + PositionOutName + " = _Pos.xyz;\n";
    
    static string DefaultGLPositionNonInstancedFunc =
    string("\t\tgl_Position = u_MVP * vec4(") + PositionAttribName + ",1.0);\n";
    
    static string DefaultPositionNonInstancedFunc =
    string("\t\t") + PositionOutName + " = (u_Model * vec4(" PositionAttribName + ",1.0)).xyz;\n";
    
    static string DefaultNormalInstancedFunc =
    string("\t\t") + NormalOutName + " = (" + InstanceNormalTransformAttribName + " * vec4(" + NormalAttribName + ",1.0)).xyz;\n";
    
    static string DefaultNormalNonInstancedFunc =
    string("\t\t") + NormalOutName + " = (u_Normal * vec4(" + NormalAttribName + ",1.0)).xyz;\n";
    
    static string DefaultTangentInstancedFunc =
    string("\t\t") + TangentOutName + " = " + InstanceNormalTransformAttribName + " * vec4(" + TangentAttribName + ",1.0)).xyz;\n";
    
    static string DefaultTangentNonInstancedFunc =
    string("\t\t") + TangentOutName + " = (u_Normal * vec4(" + TangentAttribName + ",1.0)).xyz;\n";
    
    static string DefaultTexCoordInstancedFunc =
    string("\t\t") + TexCoordOutName + " = (" + InstanceTextureTransformAttribName + " * vec4(" + TexCoordAttribName + ",0.0,1.0)).xy;\n";
    
    static string DefaultTexCoordNonInstancedFunc =
    string("\t\t") + TexCoordOutName + " = (u_Texture * vec4(" + TexCoordAttribName + ",0.0,1.0)).xy;\n";
    
    static string DefaultColorFunc =
    string("\t") + ColorOutName     + " = " + ColorAttribName + ";\n";
    
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T    F O R W A R D    L I G H T I N G  *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * */
    static string DefaultFragmentShaderBase_0 =
    string("\t")                                                                 +
    "if(u_LightCount > 0)\n"                                                     +
    "\t{\n"                                                                      +
    "\t\t" + FragmentColorOutputName + " = vec4(0,0,0,0);\n"                     +
    "\t\tfor(int l = 0;l < u_LightCount;l++)\n"                                  +
    "\t\t{\n"                                                                    +
    "\t\t\tint t = u_Lights[l].Type;\n"                                          +
    "\n\t\t\t//Do lighting\n"                                                    +
    "\t\t\tif(t == 0) //Point\n"                                                 +
    "\t\t\t{\n";


    static string DefaultFragmentShaderBase_1 =
    string("\t\t\t}\n") +
    "\t\t\telse if(t == 1) //Spot\n" + //Spot
    "\t\t\t{\n";

    static string DefaultFragmentShaderBase_2 =
    string("\t\t\t}\n") +
    "\t\t\telse if(t == 2) //Directional\n" + //Directional
    "\t\t\t{\n";

    static string DefaultFragmentShaderBase_3 =
    string("\t\t\t}\n") +
    "\t\t}\n" +
    "\t}\n";

    static string DefaultFragmentShaderPointLight =
    string("\t\t\t\tvec3  Dir    = u_Lights[l].Position.xyz - sPosition;\n")                                                          +
    "\t\t\t\tfloat Dist   = length(Dir);\n"                                                                                           +
    "\t\t\t\tDir *= (1.0 / Dist);\n\n"                                                                                                +
    "\t\t\t\t//Compute specular power\n"                                                                                              +
    "\t\t\t\tfloat ndotl = max(dot(sNormal,Dir),0.0);\n\n"                                                                            +
    "\t\t\t\tfloat SpecularPower = 0.0;\n"                                                                                            +
    "\t\t\t\tif(ndotl > 0.0)\n"                                                                                                       +
    "\t\t\t\t{\n"                                                                                                                     +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - sPosition);\n"                                                      +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n"                                                                    +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir,sNormal),0.0);\n"                                                                +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,u_Shininess);\n"                                                               +
    "\t\t\t\t}\n\n"                                                                                                                   +
    "\t\t\t\t//Light equation\n"                                                                                                      +
    "\t\t\t\tvec4 FinalColor = (ndotl * sColor * u_Lights[l].Color) + (SpecularPower * u_Lights[l].Color * sSpecular);\n" +
    "\t\t\t\t//Attenuation\n"                                                                                                         +
    "\t\t\t\tfloat Att = 1.0 / (u_Lights[l].CAtt + (u_Lights[l].LAtt * Dist) + (u_Lights[l].QAtt * (Dist * Dist)));\n"                +
    "\t\t\t\tFinalColor *= u_Lights[l].Power * Att;\n"                                                                                +
    "\t\t\t\t" + FragmentColorOutputName + " += FinalColor;\n";

    static string DefaultFragmentShaderSpotLight = 
    string("\t\t\t\tvec3  Dir    = (u_Lights[l].Position.xyz - sPosition);\n")                                                       +
    "\t\t\t\tfloat Dist   = length(Dir);\n"                                                                                           +
    "\t\t\t\tDir *= (1.0 / Dist);\n\n"                                                                                                +
    "\t\t\t\tfloat cosLightAngle = dot(Dir,u_Lights[l].Direction.xyz);\n"                                                             +
    "\t\t\t\tif(cosLightAngle < u_Lights[l].Cutoff) continue;\n"                                                                      +
    "\t\t\t\tfloat ndotl = max(dot(sNormal,Dir),0.0);\n\n"                                                                            +
    "\t\t\t\t//Compute specular power\n"                                                                                              +
    "\t\t\t\tfloat SpecularPower = 0.0;\n"                                                                                            +
    "\t\t\t\tif(ndotl > 0.0)\n"                                                                                                       +
    "\t\t\t\t{\n"                                                                                                                     +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - sPosition);\n"                                                      +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n"                                                                    +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir,sNormal),0.0);\n"                                                                +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,u_Shininess);\n"                                                               +
    "\t\t\t\t}\n\n"                                                                                                                   +
    "\t\t\t\tfloat Soften = smoothstep(u_Lights[l].Cutoff,u_Lights[l].Soften,cosLightAngle);\n"                                       +
    "\t\t\t\t//Light equation\n"                                                                                                      +
    "\t\t\t\tvec4 FinalColor = (ndotl * sColor * u_Lights[l].Color) + (SpecularPower * u_Lights[l].Color * sSpecular);\n"             +
    "\t\t\t\t//Attenuation\n"                                                                                                         +
    "\t\t\t\tfloat Att = 1.0 / (u_Lights[l].CAtt + (u_Lights[l].LAtt * Dist) + (u_Lights[l].QAtt * (Dist * Dist)));\n"                +
    "\t\t\t\tFinalColor *= u_Lights[l].Power * Soften * Att;\n"                                                                       +
    "\t\t\t\t" + FragmentColorOutputName + " += FinalColor;\n";

    static string DefaultFragmentShaderDirectionalLight =
    string("\t\t\t\tfloat SpecularPower = 0.0;\n")                                                                                    +
    "\t\t\t\tfloat ndotl = max(dot(sNormal,u_Lights[l].Direction.xyz),0.0);\n\n"                                                      +
    "\t\t\t\tif(ndotl > 0.0)\n"                                                                                                       +
    "\t\t\t\t{\n"                                                                                                                     +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - sPosition);\n"                                                      +
    "\t\t\t\t\tvec3 HalfDir        = normalize(u_Lights[l].Direction.xyz + PosToCam);\n"                                              +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir,sNormal),0.0);\n"                                                                +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,u_Shininess);\n"                                                               +
    "\t\t\t\t}\n\n"                                                                                                                   +
    "\t\t\t\t//Light equation\n"                                                                                                      +
    "\t\t\t\tvec4 FinalColor = (ndotl * sColor * u_Lights[l].Color) + (SpecularPower * u_Lights[l].Color * sSpecular);\n"             +
    "\t\t\t\t//Attenuation\n"                                                                                                         +
    "\t\t\t\tFinalColor *= u_Lights[l].Power;\n"                                                                                      +
    "\t\t\t\t" + FragmentColorOutputName + " += FinalColor;\n";
    
    
    /* * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T    F L A T    S H A D I N G  *
    \* * * * * * * * * * * * * * * * * * * * * * * */
    static string DefaultFlatFragmentShader =
    string("\t") + FragmentColorOutputName + " = " + ColorOutName + ";\n";
    
    /* * * * * * * * * * * * * * * * * * * * * * *\
     * L I G H T    D A T A    S T R U C T U R E *
    \* * * * * * * * * * * * * * * * * * * * * * */
    
    static string LightDataStructure =
    string("struct Light\n")            +
    "{\n"                               +
    "\tvec4 Position;\n"                +
    "\tvec4 Direction;\n"               +
    "\tvec4 Color;\n"                   +
    "\tfloat Cutoff;\n"                 +
    "\tfloat Soften;\n"                 +
    "\tfloat Power;\n"                  +
    "\tfloat CAtt;\n"                   +
    "\tfloat LAtt;\n"                   +
    "\tfloat QAtt;\n"                   +
    "\tint Type;\n"                     +
    //"\tfloat _Padding;\n"               +
    "};\n";
    
    /* * * * * * * * * * * * * * * * * * * * * * *\
     *  F U N C T I O N    D E F I N I T I O N S *
    \* * * * * * * * * * * * * * * * * * * * * * */
    
    static string ReliefParallaxMappingFunction =
string(
"vec2 ParallaxOffset(in vec3 V,in vec2 T,in float Scale,out float Height)\n"
"{\n"
"// determine required number of layers\n"
"const float minLayers = 10;\n"
"const float maxLayers = 15;\n"
"float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));\n"
"\n"
"// height of each layer\n"
"float layerHeight = 1.0 / numLayers;\n"
"\n"
"// depth of current layer\n"
"float currentLayerHeight = 0;\n"
"\n"
"// shift of texture coordinates for each iteration\n"
"vec2 dtex = Scale * V.xy / V.z / numLayers;\n"
"\n"
"// current texture coordinates\n"
"vec2 currentTextureCoords = T;\n"
"\n"
"// depth from heightmap\n"
"float heightFromTexture = texture(u_ParallaxMap, currentTextureCoords).r;\n"
"\n"
"// while point is above surface\n"
"while(heightFromTexture > currentLayerHeight)\n"
"{\n"
"      // go to the next layer\n"
"      currentLayerHeight += layerHeight;\n"
"\n"
"      // shift texture coordinates along V\n"
"      currentTextureCoords -= dtex;\n"
"\n"
"      // new depth from heightmap\n"
"      heightFromTexture = texture(u_ParallaxMap, currentTextureCoords).r;\n"
"   }\n"
"\n"
"   // Start of Relief Parallax Mapping\n"
"\n"
"   // decrease shift and height of layer by half\n"
"   vec2 deltaTexCoord = dtex / 2;\n"
"   float deltaHeight = layerHeight / 2;\n"
"\n"
"   // return to the mid point of previous layer\n"
"   currentTextureCoords += deltaTexCoord;\n"
"   currentLayerHeight -= deltaHeight;\n"
"\n"
"   // binary search to increase precision of Steep Paralax Mapping\n"
"   const int numSearches = 5;\n"
"   for(int i=0; i<numSearches; i++)\n"
"   {\n"
"      // decrease shift and height of layer by half\n"
"      deltaTexCoord /= 2;\n"
"      deltaHeight /= 2;\n"
"\n"
"      // new depth from heightmap\n"
"      heightFromTexture = texture(u_ParallaxMap, currentTextureCoords).r;\n"
"\n"
"      // shift along or agains vector V\n"
"      if(heightFromTexture > currentLayerHeight) // below the surface\n"
"      {\n"
"         currentTextureCoords -= deltaTexCoord;\n"
"         currentLayerHeight += deltaHeight;\n"
"      }\n"
"      else // above the surface\n"
"      {\n"
"         currentTextureCoords += deltaTexCoord;\n"
"         currentLayerHeight -= deltaHeight;\n"
"      }\n"
"   }\n"
"\n"
"   // return results\n"
"   Height = currentLayerHeight;\n"
"   return currentTextureCoords;\n"
"}\n");

    static string ParallaxMappingShadowMultiplier =
string("float ParallaxSoftShadowMultiplier(in vec3 L,in vec2 T,in float Scale,in float Height)\n"
"{\n"
"   float shadowMultiplier = 1;\n"
"\n"
"   const float minLayers = 15;\n"
"   const float maxLayers = 30;\n"
"\n"
"   // calculate lighting only for surface oriented to the light source\n"
"   if(dot(vec3(0, 0, 1), L) > 0)\n"
"   {\n"
"      // calculate initial parameters\n"
"      float numSamplesUnderSurface	= 0;\n"
"      shadowMultiplier	= 0;\n"
"      float numLayers	= mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), L)));\n"
"      float layerHeight	= Height / numLayers;\n"
"      vec2 texStep	= Scale * L.xy / L.z / numLayers;\n"
"\n"
"      // current parameters\n"
"      float currentLayerHeight	= Height - layerHeight;\n"
"      vec2 currentTextureCoords	= T + texStep;\n"
"      float heightFromTexture	= texture(u_ParallaxMap, currentTextureCoords).r;\n"
"      int stepIndex	= 1;\n"
"\n"
"      // while point is below depth 0.0)\n"
"      while(currentLayerHeight > 0)\n"
"      {\n"
"         // if point is under the surface\n"
"         if(heightFromTexture < currentLayerHeight)\n"
"         {\n"
"            // calculate partial shadowing factor\n"
"            numSamplesUnderSurface	+= 1;\n"
"            float newShadowMultiplier	= (currentLayerHeight - heightFromTexture) * (1.0 - stepIndex / numLayers);\n"
"            shadowMultiplier	= max(shadowMultiplier, newShadowMultiplier);\n"
"         }\n"
"\n"
"         // offset to the next layer\n"
"         stepIndex	+= 1;\n"
"         currentLayerHeight	-= layerHeight;\n"
"         currentTextureCoords	+= texStep;\n"
"         heightFromTexture	= texture(u_ParallaxMap, currentTextureCoords).r;\n"
"      }\n"
"\n"
"      // Shadowing factor should be 1 if there were no points under the surface\n"
"      if(numSamplesUnderSurface < 1)\n"
"      {\n"
"         shadowMultiplier = 1;\n"
"      }\n"
"      else\n"
"      {\n"
"         shadowMultiplier = 1.0 - shadowMultiplier;\n"
"      }\n"
"   }\n"
"   return shadowMultiplier;\n"
"}\n");
};
