import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0 as Q3D
import Qt3D.Render 2.0

import pcl 1.0

import QtQuick 2.0 as QQ2

ApplicationWindow {
    title: qsTr("Map Visualization")
    width: 1200
    height: 800
    visible: true

    PointcloudReader {
        id: pointcloudreader
        filename: "data/bunny.pcd"
    }

    GridLayout {
        anchors.fill: parent
        Scene3D {
            id: scene3d
            Layout.minimumWidth: 50
            Layout.fillWidth: true
            Layout.fillHeight: true
            aspects: ["render", "logic", "input"]
            focus: true

            Q3D.Entity {
                id: sceneRoot
                Q3D.Camera {
                    id: camera
                    projectionType: Q3D.CameraLens.PerspectiveProjection
                    fieldOfView: 45
                    aspectRatio: scene3d.width/scene3d.height
                    nearPlane : 0.1
                    farPlane : 1000.0
                    position: Qt.vector3d( 0.0, 0.0, -40.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                }

                Q3D.Configuration  {
                    controlledCamera: camera
                }

                components: [
                    FrameGraph {
                        activeFrameGraph: Viewport {
                            id: viewport
                            rect: Qt.rect(0.0, 0.0, 1.0, 1.0) // From Top Left
                            clearColor: "transparent"

                            CameraSelector {
                                id : cameraSelector
                                camera: camera
                                ClearBuffer {
                                    buffers : ClearBuffer.ColorDepthBuffer
                                    LayerFilter {
                                        layers: ["points"]
                                        StateSet {
                                            renderStates: [
                                                //PointSize { specification: PointSize.StaticValue; value: 5 },
                                                PointSize { specification: PointSize.Programmable },
                                                DepthTest { func: DepthTest.Less },
                                                DepthMask { mask: true }
                                            ]
                                        }
                                    }
                                }
                                LayerFilter {
                                    layers: ["solid"]
                                }
                            }
                        }
                    }
                ]

                Q3D.Entity {
                    id: pointcloud
                    property Layer layerPoints: Layer {
                            names: "points"
                        }
                    property var meshTransform: Q3D.Transform {
                            property real userAngle: 0.0
                            scale: 10
                            rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
                        }
                    property GeometryRenderer pointcloudMesh: GeometryRenderer {
                            geometry: PointcloudGeometry { pointcloud: pointcloudreader.pointcloud }
                            primitiveType: GeometryRenderer.Points
                        }
                    property Material materialPoint: Material {
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
                        parameters: Parameter { name: "pointSize"; value: 0.1 }
                    }
                    //property Material materialPoint: PerVertexColorMaterial {}
                    components: [ pointcloudMesh, materialPoint, meshTransform, layerPoints ]
                }

                Q3D.Entity {
                    id: sphereEntity
                    property Layer layerSolid: Layer {
                            names: "solid"
                        }
                    property var meshTransform: Q3D.Transform {
                        id: sphereTransform
                        property real userAngle: 0.0
                        translation: Qt.vector3d(20, 0, 0)
                        rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
                    }
                    property var sphereMesh: SphereMesh {
                        radius: 1
                    }
                    property Material materialPhong: PhongMaterial {
                    }
                    components: [ sphereMesh, materialPhong, meshTransform, layerSolid ]
                }
                NumberAnimation {
                    target: sphereTransform
                    property: "userAngle"
                    duration: 10000
                    from: 0
                    to: 360

                    loops: Animation.Infinite
                    running: true
                }
            }
        }
    }

    SystemPalette {
        id: palette
    }
}
