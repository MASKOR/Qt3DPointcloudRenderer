import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0 as Q3D
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import pcl 1.0

ApplicationWindow {
    id: window
    title: qsTr("Map Visualization")
    width: 1200
    height: 800
    visible: true

//    PointcloudReader {
//        id: readerBunny
//        filename: "data/bunny.pcd"
//    }
//    PointcloudReader {
//        id: readerBunnyNormal
//        filename: "data/bunny_normal.pcd"
//    }

    GridLayout {
        anchors.fill: parent
        Scene3D {
            id: scene3d
            Layout.minimumWidth: 50
            Layout.fillWidth: true
            Layout.fillHeight: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
            focus: true
            Q3D.Entity {
                id: sceneRoot

                Camera {
                    id: mainCamera
                    projectionType: CameraLens.PerspectiveProjection
                    fieldOfView: 75
                    aspectRatio: scene3d.width/scene3d.height
                    nearPlane : 0.1
                    farPlane : 1000.0
                    position: Qt.vector3d( 0.0, 0.0, -20.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                }

                FirstPersonCameraController {
                //OrbitCameraController {
                    camera: mainCamera
                }

                components: [
                    RenderSettings {
                        activeFrameGraph: Viewport {
                            id: viewport
                            normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0) // From Top Left
                            RenderSurfaceSelector {
                                CameraSelector {
                                    id : cameraSelector
                                    camera: mainCamera
                                    FrustumCulling {
                                        ClearBuffers {
                                            buffers : ClearBuffers.ColorDepthBuffer
                                            clearColor: "white"
                                            NoDraw {}
                                        }
                                        LayerFilter {
                                            layers: solidLayer
                                        }
                                        LayerFilter {
                                            layers: pointLayer
                                            RenderStateSet {
                                                renderStates: [
                                                    // If this is uncommented, following pointsizes are ignored in Qt5.7
                                                    PointSize { sizeMode: PointSize.Fixed; value: 10.0 }, // exception when closing application in qt 5.7. Moreover PointSize
                                                    //PointSize { sizeMode: PointSize.Programmable }, //supported since OpenGL 3.2
                                                    DepthTest { depthFunction: DepthTest.Less }
                                                    //DepthMask { mask: true }
                                                ]
                                            }
                                        }
//                                        LayerFilter {
//                                            layers: surfelLayer
//                                            RenderStateSet {
//                                                renderStates: [
//                                                    PointSize { sizeMode: PointSize.Programmable }, //supported since OpenGL 3.2
//                                                    DepthTest { depthFunction: DepthTest.Less }
//                                                    //DepthMask { mask: true }
//                                                ]
//                                            }
//                                        }
                                    }
                                }
                            }
                        }
                    },
                    // Event Source will be set by the Qt3DQuickWindow
                    InputSettings {
                        eventSource: window
                        enabled: true
                    }
                ]

                PhongMaterial {
                    id: phongMaterial
                }

                TorusMesh {
                    id: torusMesh
                    radius: 5
                    minorRadius: 1
                    rings: 100
                    slices: 20
                }

                Q3D.Transform {
                    id: torusTransform
                    scale3D: Qt.vector3d(2.5, 2.5, 2.5)
                    //rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
                }

                Layer {
                    id: solidLayer
                }
                Layer {
                    id: pointLayer
                }
                Q3D.Entity {
                    id: torusEntity
                    components: [ solidLayer, torusMesh, phongMaterial, torusTransform, pointLayer ]
                }

//                Buffer {
//                    property var vertices: [[0.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0, -10.0, 0.0], [0.0, 2.0, 2.0], [-0.1, 0.0, -0.1]]
//                    function buildVertexBufferData() {
//                        var vertexArray = new Float32Array(vertices.length * 3);
//                        var i = 0;

//                        vertices.forEach(function(vec3) {
//                            vertexArray[i++] = vec3[0];
//                            vertexArray[i++] = vec3[1];
//                            vertexArray[i++] = vec3[2];
//                        });

//                        return vertexArray;
//                    }
//                    id: vertexBufferPoints
//                    type: Buffer.VertexBuffer
//                    data: buildVertexBufferData()
//                }

                Q3D.Entity {
                    id: pointcloud
                    property var meshTransform: Q3D.Transform {
                            id: pointcloudTransform
                            property real userAngle: rotator.rotationAnimation
                            scale: 20
                            translation: Qt.vector3d(0, -2, 0)
                            rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
                        }
//                    property GeometryRenderer pointcloudMesh: GeometryRenderer {
//                                instanceCount: 1
//                                indexOffset: 0
//                                firstInstance: 0
//                                geometry: Geometry {
//                                    Attribute {
//                                        //name: defaultPositionAttributeName()
//                                        name: "vertexPosition" // Name of attribute in the shader
//                                        attributeType: Attribute.VertexAttribute
//                                        vertexBaseType: Attribute.Float
//                                        vertexSize: 3
//                                        byteOffset: 0 // See OpenGL doc for details about these properties
//                                        byteStride: 3 * 4
//                                        count: 5//vertexBufferPoints.count
//                                        buffer: vertexBufferPoints // The actual QBuffer which holds the vertex data
//                                    }
//                                }
//                                primitiveType: GeometryRenderer.Points
//                            }


                    property GeometryRenderer pointcloudMesh: GeometryRenderer {
                                primitiveType: GeometryRenderer.Points
                                geometry: PointcloudGeometry {
                                    pointcloud: Pointcloud { //readerBunny.pointcloud
                                        fields: [
                                            Pointfield {
                                                name: "x"
                                                offset: 0
                                                datatype: Pointfield.FLOAT32
                                                count: 1
                                            },
                                            Pointfield {
                                                name: "y"
                                                offset: 4
                                                datatype: Pointfield.FLOAT32
                                                count: 1
                                            },
                                            Pointfield {
                                                name: "z"
                                                offset: 8
                                                datatype: Pointfield.FLOAT32
                                                count: 1
                                            }/*,
                                            Pointfield {
                                                name: "rgb"
                                                offset: 12
                                                datatype: Pointfield.FLOAT32
                                            }*/
                                        ]
                                        point_step: 12 // 24
                                        width: 5
                                        property var vertices: [[0.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0, -10.0, 0.0], [0.0, 2.0, 2.0], [-0.1, 0.0, -0.1]]
                                        function buildVertexBufferData() {
                                            var arrayBuffer = new ArrayBuffer(vertices.length * 3 * 4);
                                            var vertexArray = new Float32Array(arrayBuffer);//vertices.length * 3);
                                            var i = 0;
                                            vertices.forEach(function(vec3) {
                                                vertexArray[i++] = vec3[0];
                                                vertexArray[i++] = vec3[1];
                                                vertexArray[i++] = vec3[2];
                                            });

                                            return arrayBuffer;
                                        }
                                        data: buildVertexBufferData()
                                    }
                                }
                            }
                    property Material materialPoint: Material {
                        id: matPoint
                        effect: Effect {
                            techniques: Technique {
                                renderPasses: RenderPass {
                                    shaderProgram: ShaderProgram {
                                        vertexShaderCode: loadSource("qrc:/shader/pointcloud.vert")
                                        fragmentShaderCode: loadSource("qrc:/shader/pointcloud.frag")
                                    }
                                }
                            }
                        }
                        parameters: Parameter { name: "pointSize"; value: 0.7 }
                    }
                    //property Material materialPoint: PerVertexColorMaterial {}
                    components: [ pointcloudMesh, matPoint, meshTransform, pointLayer ]
                }

                Q3D.Entity {
                    id: pointcloudSurfel
//                    property Layer layerPoints: Layer {
//                            id: surfelLayer
//                        }
//                    property var meshTransform: Q3D.Transform {
//                            id: pointcloudSurfelTransform
//                            property real userAngle: rotator.rotationAnimation
//                            scale: 20
//                            translation: Qt.vector3d(0, 2, 0)
//                            rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
//                        }
//                    property GeometryRenderer surfelMesh: GeometryRenderer {
//                            geometry: PointcloudGeometry { pointcloud: readerBunnyNormal.pointcloud }
//                            primitiveType: GeometryRenderer.Points
//                        }
//                    property Material materialSurfel: Material {
//                        effect: Effect {
//                            techniques: Technique {
//                                renderPasses: RenderPass {
//                                    shaderProgram: ShaderProgram {
//                                        vertexShaderCode: loadSource("qrc:/shader/surfel.vert")
//                                        fragmentShaderCode: loadSource("qrc:/shader/surfel.frag")
//                                    }
//                                }
//                            }
//                        }
//                        parameters: [
//                            Parameter { name: "pointSize"; value: 0.06 },
//                            Parameter { name: "fieldOfView"; value: mainCamera.fieldOfView },
//                            Parameter { name: "fieldOfViewVertical"; value: mainCamera.fieldOfView/mainCamera.aspectRatio },
//                            Parameter { name: "nearPlane"; value: mainCamera.nearPlane },
//                            Parameter { name: "farPlane"; value: mainCamera.farPlane },
//                            Parameter { name: "width"; value: scene3d.width },
//                            Parameter { name: "height"; value: scene3d.height }
//                        ]
//                    }
//                    components: [ surfelMesh, materialSurfel, meshTransform, surfelLayer ]
                }
            }
        }
    }
    NumberAnimation {
        id: rotator
        property real rotationAnimation
        target: rotator
        property: "rotationAnimation"
        duration: 10000
        from: -180
        to: 180

        loops: Animation.Infinite
        running: true
    }

    SystemPalette {
        id: palette
    }
}
