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
    string("\t") +
    "if(u_LightCount > 0)\n" +
    "\t{\n" +
    "\t\tfor(int l = 0;l < u_LightCount;l++)\n" +
    "\t\t{\n" +
    "\t\t\tint t = u_Lights[l].Type;\n" +
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

    static string DefaultFragmentShaderPointLight = "";

    static string DefaultFragmentShaderSpotLight = "";

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
