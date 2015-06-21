#pragma once

namespace Silk
{
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T    F O R W A R D    L I G H T I N G  *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * */
    static string DefaultFragmentShaderBase_0 =
    string("\t") +
    "if(u_LightCount > 0)\n" +
    "\t{\n" +
    "\t\tfor(int l = 0;l < u_LightCount;l++)\n" +
    "\t\t{\n" +
    "\t\t\tint t = u_Light[l].Type;\n" +
    "\t\t\tif(t == 0) //Point\n" + //Point
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
    string("\t\t\t\tvec3 Dir = u_Light[l].Position - ") + PositionOutName + ";\n" +
    "\t\t\t\tfloat Dist = length(Dir);\n" +
    "\t\t\t\tfloat FadeFactor = 1.0;\n" +
    "\t\t\t\tfloat MaxDist = 1.0 / sqrt(u_Light[l].QAttenuation * 0.001);\n" +
    "\t\t\t\tif(Dist > MaxDist) discard;\n" +
    "\t\t\t\telse FadeFactor = 1.0 - ((Dist / MaxDist) * 2.0);\n" +
    "\t\t\t\tDir *= (1.0 / Dist);\n\n" +
    "\t\t\t\t//Compute specular power\n" +
    "\t\t\t\tfloat ndotl = max(dot(o_Normal,Dir),0.0);\n" +
    "\t\t\t\tfloat SpecularPower = 0.0;\n" +
    "\t\t\t\tif(ndotl > 0.0)\n" +
    "\t\t\t\t{\n" +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - " + PositionOutName + ");\n" +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n" +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir," + NormalOutName + "),0.0);\n" +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,1.0 / u_Roughness);\n" +
    "\t\t\t\t}\n\n" +
    "\t\t\t\t//Light equation\n" +
    "\t\t\t\t" + FragmentColorOutputName + " = (u_AmbientFactor * " + ColorOutName + " * u_Light[l].Ambient) + //Ambient\n" +
    "\t\t\t\t       (ndotl           * o_Color  * u_Light[l].Diffuse) + //Diffuse\n" +
    "\t\t\t\t       (SpecularPower   * u_Light[l].Color * u_LightSpecular);  //Specular\n\n" +
    "\t\t\t\t//Attenuation\n" +
    "\t\t\t\t" + FragmentColorOutputName + " *= 1.0 / (u_Light[l].CAttenuation + (u_Light[l].LAttenuation * Dist) + (u_Light[l].QAttenuation * (Dist * Dist)));\n" +
    "\t\t\t\t" + FragmentColorOutputName + " *= u_Light[l].Intensity * FadeFactor;\n";

    static string DefaultFragmentShaderSpotLight =
    string("\t\t\t\tvec3 Dir = u_Light[l].Position - ") + PositionOutName + ";\n" +
    "\t\t\t\tfloat Dist = length(Dir);\n" +
    "\t\t\t\tfloat FadeFactor = 1.0;\n" +
    "\t\t\t\tfloat MaxDist = 1.0 / sqrt(u_Light[l].QAttenuation * 0.001);\n" +
    "\t\t\t\tif(Dist > MaxDist) discard;\n" +
    "\t\t\t\telse FadeFactor = 1.0 - ((Dist / MaxDist) * 2.0);\n" +
    "\t\t\t\tDir *= (1.0 / Dist);\n\n" +
    "\t\t\t\tfloat cosLightAngle = dot(u_Light[l].Direction,Dir);\n" +
    "\t\t\t\tfloat cosLightCutoffAngle = cos(u_Light[l].CutoffAngle * 0.5);\n" +
    "\t\t\t\tif(cosLightAngle < cosLightCutoffAngle) discard;\n\n" +
    "\t\t\t\t//Compute specular power\n" +
    "\t\t\t\tfloat ndotl = max(dot(" + NormalOutName + ",Dir),0.0);\n" +
    "\t\t\t\tfloat cosSoftenRadius = cos((u_LightCutoffAngle * (1.0 - u_Light[l].CutoffSoften)) * 0.5);\n" +
    "\t\t\t\tfloat Soften = 1.0f;\n\n" +
    "\t\t\t\tif(cosLightAngle < cosSoftenRadius)\n" +
    "\t\t\t\t{\n" +
    "\t\t\t\t\tSoften = (cosLightAngle - cosLightCutoffAngle) / (cosSoftenRadius - cosLightCutoffAngle);\n" +
    "\t\t\t\t}\n" +
    "\t\t\t\tfloat SpecularPower = 0.0;\n" +
    "\t\t\t\tif(ndotl > 0.0)\n" +
    "\t\t\t\t{\n" +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - " + PositionOutName + ");\n" +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n" +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir," + NormalOutName + "),0.0);\n" +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,1.0 / u_Roughness);\n" +
    "\t\t\t\t}\n" +
    "\t\t\t\t//Light equation\n" +
    "\t\t\t\t" + FragmentColorOutputName + " = (u_AmbientFactor * " + ColorOutName + " * u_Light[l].Ambient) + //Ambient\n" +
    "\t\t\t\t       (ndotl           * " + ColorOutName + " * u_Light[l].Diffuse) + //Diffuse\n" +
    "\t\t\t\t       (SpecularPower   * u_Light[l].Color * u_LightSpecular);  //Specular\n" +
    "\t\t\t\t//Attenuation\n" +
    "\t\t\t\t" + FragmentColorOutputName + " *= 1.0 / (u_Light[l].CAttenuation + (u_Light[l].LAttenuation * Dist) + (u_Light[l].QAttenuation * (Dist * Dist)));\n" +
    "\t\t\t\t" + FragmentColorOutputName + " *= u_Light[l].Intensity * FadeFactor;\n";

    static string DefaultFragmentShaderDirectionalLight =
    string("\t\t\t\tfloat ndotl = max(dot(") + NormalOutName + ",u_Light[l].Direction),0.0);\n" +
    "\t\t\t\tfloat SpecularPower = 0.0;\n\n" +
    "\t\t\t\tif(ndotl > 0.0)\n" +
    "\t\t\t\t{\n" +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - " + PositionOutName + ");\n" +
    "\t\t\t\t\tvec3 HalfDir        = normalize(u_Light[l].Direction + PosToCam);\n" +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir," + NormalOutName + "),0.0);\n" +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,1.0 / u_Roughness);\n" +
    "\t\t\t\t}\n\n" +
    "\t\t\t\t/* Light Equation */\n" +
    "\t\t\t\t" + FragmentColorOutputName + " = u_Light[l].Intensity * ((u_AmbientFactor * u_Light[l].Ambient * " + ColorOutName + ") + (ndotl * " + ColorOutName + " * u_Light[l].Diffuse) + (SpecularPower * u_Light[l].Specular));\n";


    /* * * * * * * * * * * * * * * * * * * * * * * *\
     *  D E F A U L T    F L A T    S H A D I N G  *
    \* * * * * * * * * * * * * * * * * * * * * * * */
    static string DefaultFlatFragmentShader =
    string("\t") + FragmentColorOutputName + " = " + ColorOutName + ";\n";
    
    /* * * * * * * * * * * * * * * * * * * * * * *\
     *  L I G H T   D A T A   S T R U C T U R E  *
    \* * * * * * * * * * * * * * * * * * * * * * */
    
    /*
    Vec3 m_Direction;
    Vec3 m_Position ;
    Vec3 m_Color    ;
    f32 m_Cutoff    ;
    f32 m_Power     ;
    f32 Constant    ;
    f32 Linear      ;
    f32 Exponential ;
    */
    static string LightDataStructure =
    string("struct Light\n")            +
    "{\n"                               +
    "\tvec4 Position;\n"                +
    "\tvec4 Direction;\n"               +
    "\tvec4 Color;\n"                   +
    "\tfloat Cutoff;\n"                 +
    "\tfloat Soften;\n"                 +
    "\tfloat Power;\n"                  +
    "\tfloat ConstantAttenuation;\n"    +
    "\tfloat LinearAttenuation;\n"      +
    "\tfloat QuadraticAttenuation;\n"   +
    "\tint Type;\n"                     +
    "\tfloat _Padding;\n"               +
    "};\n";
};
