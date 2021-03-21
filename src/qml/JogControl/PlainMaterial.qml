import QtQuick 2.0
import QtQuick3D 1.15
import QtQuick3D.Materials 1.15

CustomMaterial {
    property color baseColor: "red"

    property vector3d a_baseColor: Qt.vector3d(baseColor.r, baseColor.g, baseColor.b)
    shaderInfo: ShaderInfo {
        version: "330"
        type: "GLSL"
        shaderKey: ShaderInfo.Glossy
    }
//    Shader {
//        id: vert
//        stage: Shader.Vertex
//        shader: "
//in vec3 attr_pos;
//in vec3 attr_norm;
//in vec2 attr_uv0;
//in vec3 attr_textan;
//in vec3 attr_binormal;

//uniform vec3 cameraPosition;

//uniform mat4 modelMatrix;
//uniform mat4 modelViewProjection;
//uniform mat4 viewMatrix;
//uniform mat3 normalMatrix;
//uniform mat4 viewProjectionMatrix;
//uniform mat4 viewportMatrix;

//out float angle;
//out vec3 outNorm;
//void main() {
//    vec3 campos = normalize(cameraPosition);
//    vec3 normpos = normalize(normalMatrix*attr_norm);
//    angle = dot(normpos, campos);
//    outNorm = normalize(attr_norm);
//    gl_Position = modelViewProjection * vec4(attr_pos, 1.0);
//}"
//    }

//    Shader {
//        id: frag
//        stage: Shader.Fragment
//        shader: "
//out vec4 fragColor;
//in float angle;
//in vec3 outNorm;
//void main() {
//    float angle2 = angle*1.5 + 0.2;
//    fragColor = vec4(1.0 - outNorm.x, 1.0 - outNorm.y, 1.0 - outNorm.z, 1.0);
//}
//"
//    }
    Shader {
        id: vert
        stage: Shader.Vertex
        shader: "
in vec3 attr_pos;
in vec3 attr_norm;
in vec2 attr_uv0;
in vec3 attr_textan;
in vec3 attr_binormal;

uniform vec3 cameraPosition;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjection;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 viewportMatrix;

out float angle;
void main() {
    vec3 campos = normalize(cameraPosition);
    vec3 normpos = normalize(normalMatrix*attr_norm);
    angle = dot(normpos, campos);
    gl_Position = modelViewProjection * vec4(attr_pos, 1.0);
}"
    }

    Shader {
        id: frag
        stage: Shader.Fragment
        shader: "out vec4 fragColor;
in float angle;
void main() {
    float angle2 = angle*1.5 + 0.2;
    fragColor = vec4((baseColor*angle2).rgb, 1.0);
}"
    }
    passes: [
        Pass {
            shaders: [vert, frag]
        }
    ]
}
