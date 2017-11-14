# Qt3DPointcloudRenderer

Adds C++ Classes and Qmltypes for rendering Point Cloud Library Pointcloud2 and libLAS pointclouds. Interfaces to both libraries look the same in qml. PCL or libLAS can be disabled to avoid the dependency to one of the libraries.
This library adds classes to be used with the Qt3D GeometryRenderer-Component. Moreover a Qml-Pointcloud type and pcd/ply-reader exist. The compiled library can be used by a program, that does not need PCL/libLAS-headers.
Pointclouds are read-only. However Qml binding re-evaluate if the internal pointcloud is changed.

## Qml Types

* Pointcloud: Wraps pcl::PCLPointCloud2; Reads LAS files using a provided liblas::Reader (TODO: this is only C++ yet)
* Pointfield: Wraps pcl::PCLPointField
* PointcloudReader: Wraps pcl::PCDReader and pcl::PLYReader
* QPointcloudGeometry: Can be used as Qt3DRender::QGeometry to render a Pointcloud using Qt3DRender::GeometryRenderer

## Usage

Register Qml-Types:

    qmlRegisterType<QPointCloudReader>("pcl", 1, 0, "PointcloudReader");
    qmlRegisterType<QPointcloud>("pcl", 1, 0, "Pointcloud");
    qmlRegisterType<QPointcloudGeometry>("pcl", 1, 0, "PointcloudGeometry");
    qmlRegisterUncreatableType<QPointfield>("pcl", 1, 0, "Pointfield", "Can not yet be created in qml, use PointcloudReader.");

Read Pointcloud in Qml:

    PointcloudReader {
        id: pointcloudreader
        filename: "data/bunny.pcd"
    }

Add your Pointcloud-Entity to the scenegraph.

    Entity {
        id: pointcloud
        property Layer layerPoints: Layer {
                names: "points"
            }
        property GeometryRenderer pointcloudMesh: GeometryRenderer {
                geometry: PointcloudGeometry { pointcloud: pointcloudreader.pointcloud }
                primitiveType: GeometryRenderer.Points
            }
        property Material materialPoint: PerVertexColorMaterial {}
        components: [ pointcloudMesh, materialPoint, meshTransform, layerPoints ]
    }

The layer is needed to identify pointclouds in the framegraph:

    FrameGraph {
      (...)
        LayerFilter {
            layers: ["points"]
            StateSet {
                renderStates: [
                    //PointSize { specification: PointSize.StaticValue; value: 5 /*pixels*/ },
                    PointSize { specification: PointSize.Programmable },
                    DepthTest { func: DepthTest.Less },
                    DepthMask { mask: true }
                ]
            }
        }
      (...)
    }

Instead of PerVertexColorMaterial, a custom shader can be used to enable programmable per vertex point size:

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

