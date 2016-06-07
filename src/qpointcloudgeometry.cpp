#include "qpointcloudgeometry.h"
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/qbufferfunctor.h>
#include <QHash>
#include <iomanip>
#include <pcl/PCLPointCloud2.h>

QByteArray createPointcloudVertexData(pcl::PCLPointCloud2 *pointcloud)
{
    return QByteArray(reinterpret_cast<char*>(&pointcloud->data[0]), pointcloud->data.size());
}

class PointcloudVertexDataFunctor : public Qt3DRender::QBufferFunctor /*QBufferDataGenerator*/
{
public:
    PointcloudVertexDataFunctor(pcl::PCLPointCloud2 *pointcloud)
        : m_pointcloud(pointcloud)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createPointcloudVertexData(m_pointcloud);
    }

    bool operator ==(const Qt3DRender::QBufferFunctor /*QBufferDataGenerator*/ &other) const Q_DECL_OVERRIDE
    {
        const PointcloudVertexDataFunctor *otherFunctor = functor_cast<PointcloudVertexDataFunctor>(&other);
        if (otherFunctor != nullptr)
            return otherFunctor->m_pointcloud == m_pointcloud;
        return false;
    }

    QT3D_FUNCTOR(PointcloudVertexDataFunctor)

private:
    pcl::PCLPointCloud2 *m_pointcloud;
};

class QPointcloudGeometryPrivate
{
public:
    Qt3DRender::QBuffer *m_vertexBuffer;
    QPointcloud *m_pointcloud;
};

QPointcloudGeometry::QPointcloudGeometry(Qt3DCore::QNode *parent)
    :m_p(new QPointcloudGeometryPrivate)
{
    m_p->m_vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
}

QPointcloudGeometry::~QPointcloudGeometry()
{
    delete m_p;
}

Qt3DRender::QAttribute::DataType pclTypeToAttributeType(pcl::uint8_t inp)
{
    switch(inp)
    {
    case pcl::PCLPointField::INT8:
        return Qt3DRender::QAttribute::Byte;
    case pcl::PCLPointField::INT16:
        return Qt3DRender::QAttribute::Short;
    case pcl::PCLPointField::INT32:
        return Qt3DRender::QAttribute::Int;
    case pcl::PCLPointField::UINT8:
        return Qt3DRender::QAttribute::UnsignedByte;
    case pcl::PCLPointField::UINT16:
        return Qt3DRender::QAttribute::UnsignedShort;
    case pcl::PCLPointField::UINT32:
        return Qt3DRender::QAttribute::UnsignedInt;
    case pcl::PCLPointField::FLOAT32:
        return Qt3DRender::QAttribute::Float;
    case pcl::PCLPointField::FLOAT64:
        return Qt3DRender::QAttribute::Double;
    default:
        Q_ASSERT(false);
    }
}

void QPointcloudGeometry::updateVertices()
{
    QHash<QString, pcl::PCLPointField*> pfs;
    for(int i=0 ; m_p->m_pointcloud->pointcloud()->fields.size()>i ; ++i)
    {
        pcl::PCLPointField &pf( m_p->m_pointcloud->pointcloud()->fields[i] );
        pfs.insert(QString::fromStdString(pf.name), &pf);
    }
    QHash<QString, pcl::PCLPointField*>::const_iterator pf(pfs.find("x"));
    if(pf != pfs.cend())
    {
        int num = 1 + (pfs.contains("y")?1:0) + (pfs.contains("z")?1:0) + (pfs.contains("w")?1:0);
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype));
        attrib->setDataSize(num);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset);
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
    pf = pfs.find("r");
    if(pf != pfs.cend())
    {
        int num = 1 + (pfs.contains("g")?1:0) + (pfs.contains("b")?1:0) + (pfs.contains("A")?1:0);
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype));
        attrib->setDataSize(num);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset);
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
    pf = pfs.find("normal_x");
    if(pf != pfs.cend())
    {
        int num = 1 + (pfs.contains("normal_y")?1:0) + (pfs.contains("normal_z")?1:0) + (pfs.contains("curvature")?1:0);
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype));
        attrib->setDataSize(num);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset);
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
    pf = pfs.find("intensity");
    if(pf != pfs.cend())
    {
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype));
        attrib->setDataSize(1);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset);
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
    m_p->m_vertexBuffer->setBufferFunctor(Qt3DRender::QBufferFunctorPtr(new PointcloudVertexDataFunctor(m_p->m_pointcloud->pointcloud())));
}

QPointcloud *QPointcloudGeometry::pointcloud() const
{
    return m_p->m_pointcloud;
}

void QPointcloudGeometry::setPointcloud(QPointcloud *pointcloud)
{
    if (m_p->m_pointcloud == pointcloud)
        return;

    m_p->m_pointcloud = pointcloud;
    m_p->m_vertexBuffer->setBufferFunctor(QSharedPointer<PointcloudVertexDataFunctor>::create(m_p->m_pointcloud->pointcloud()));
    updateVertices();
    Q_EMIT pointcloudChanged(pointcloud);
}
