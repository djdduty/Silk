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
    
    static string DefaultRoughnessFunc =
    string("\t") + RoughnessOutName + " = u_Roughness;\n";
    
    static string DefaultMetalnessFunc =
    string("\t") + MetalnessOutName + " = u_Metalness;\n";
    
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T    F O R W A R D    L I G H T I N G  *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * */
    static string DefaultFragmentShaderBase_0 =
    string("\t")                                                                 +
    "if(u_LightCount > 0)\n"                                                     +
    "\t{\n"                                                                      +
    "\t\tfor(int l = 0;l < u_LightCount;l++)\n"                                  +
    "\t\t{\n"                                                                    +
    "\t\t\tint t = u_Lights[l].Type;\n"                                          +
    "\t\t\tvec3  Normal = normalize(" + NormalOutName + ");\n"                   +
    "\t\t\tvec4  Color  = texture(u_DiffuseMap," + TexCoordOutName + ");\n"      +
    "\t\t\tvec3  Dir    = u_Lights[l].Position.xyz - " + PositionOutName + ";\n" +
    "\t\t\tfloat Dist   = length(Dir);\n"                                        +
    "\t\t\tDir *= (1.0 / Dist);\n\n"                                             +
    "\t\t\t//Compute specular power\n"                                           +
    "\t\t\tfloat ndotl = max(dot(Normal,Dir),0.0);\n\n"                          +
    "\t\t\t//Do lighting\n"                                                      +
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
    string("\t\t\t\tfloat SpecularPower = 0.0;\n")                                                                                  +
    "\t\t\t\tif(ndotl > 0.0)\n"                                                                                                     +
    "\t\t\t\t{\n"                                                                                                                   +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - " + PositionOutName + ");\n"                                      +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n"                                                                  +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir,Normal),0.0);\n"                                                               +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,128.0f);\n"                                                                  +
    "\t\t\t\t}\n\n"                                                                                                                 +
    "\t\t\t\t//Light equation\n"                                                                                                    +
    "\t\t\t\tvec4 FinalColor = (0.2f * Color) + (ndotl * Color * u_Lights[l].Color) + (SpecularPower * u_Lights[l].Color * 3.0);\n" +
    "\t\t\t\t//Attenuation\n"                                                                                                       +
    "\t\t\t\tfloat Att = 1.0 / (u_Lights[l].CAtt + (u_Lights[l].LAtt * Dist) + (u_Lights[l].QAtt * (Dist * Dist)));\n"              +
    "\t\t\t\tFinalColor *= u_Lights[l].Power;\n"                                                                                    +
    "\t\t\t\t" + FragmentColorOutputName + " += FinalColor;\n";

    static string DefaultFragmentShaderSpotLight = 
    string("\t\t\t\tfloat SpecularPower = 0.0;\n")                                                                                  +
    "\t\t\t\tif(ndotl > 0.0)\n"                                                                                                     +
    "\t\t\t\t{\n"                                                                                                                   +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - " + PositionOutName + ");\n"                                      +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n"                                                                  +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir,Normal),0.0);\n"                                                               +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,128.0f);\n"                                                                  +
    "\t\t\t\t}\n\n"                                                                                                                 +
    "\t\t\t\tfloat cosLightAngle = dot(-Dir,u_Lights[l].Direction.xyz);\n"                                                          +
    "\t\t\t\tif(cosLightAngle < u_Lights[l].Cutoff) continue;\n"                                                                    +
    "\t\t\t\tfloat Soften = smoothstep(u_Lights[l].Cutoff,u_Lights[l].Soften,cosLightAngle);\n"                                     +
    "\t\t\t\t//Light equation\n"                                                                                                    +
    "\t\t\t\tvec4 FinalColor = (0.2f * Color) + (ndotl * Color * u_Lights[l].Color) + (SpecularPower * u_Lights[l].Color * 3.0);\n" +
    "\t\t\t\t//Attenuation\n"                                                                                                       +
    "\t\t\t\tfloat Att = 1.0 / (u_Lights[l].CAtt + (u_Lights[l].LAtt * Dist) + (u_Lights[l].QAtt * (Dist * Dist)));\n"              +
    "\t\t\t\tFinalColor *= u_Lights[l].Power * Soften;\n"                                                                           +
    "\t\t\t\t" + FragmentColorOutputName + " += FinalColor;\n";

    static string DefaultFragmentShaderDirectionalLight = "";
    /* * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T    F L A T    S H A D I N G  *
    \* * * * * * * * * * * * * * * * * * * * * * * */
    static string DefaultFlatFragmentShader =
    string("\t") + FragmentColorOutputName + " = " + ColorOutName + ";\n";
    
    /* * * * * * * * * * * * * * * * * * * * * * *\
     *  L I G H T   D A T A   S T R U C T U R E  *
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
};
