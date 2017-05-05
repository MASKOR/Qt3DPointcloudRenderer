#include "qpointcloudgeometry.h"
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/qbufferdatagenerator.h>
#include <QHash>
#include <iomanip>
#include <pcl/PCLPointCloud2.h>
#include <QSharedPointer>

//QByteArray createPointcloudVertexData(pcl::PCLPointCloud2 *pointcloud)
//{
//    if(pointcloud == NULL || pointcloud->data.size() == 0)
//        return QByteArray();
//    //QByteArray arr;
//    //arr.append(reinterpret_cast<char*>(&pointcloud->data[0]), pointcloud->data.size());
//    return QByteArray(reinterpret_cast<char*>(&pointcloud->data[0]), pointcloud->data.size());//arr;
//}

class PointcloudVertexDataGenerator : public Qt3DRender::QBufferDataGenerator /*Qt3DRender::QBufferFunctor*/
{
public:
    PointcloudVertexDataGenerator(QPointcloudGeometry *geometry)
        : m_geometry(geometry)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return m_geometry->pointcloud()->data();
    }

    bool operator ==(const Qt3DRender::QBufferDataGenerator &other) const Q_DECL_OVERRIDE
    {
        const PointcloudVertexDataGenerator *otherFunctor = Qt3DRender::functor_cast<PointcloudVertexDataGenerator>(&other);
        if (otherFunctor != NULL)
            return otherFunctor->m_geometry == m_geometry;
        return false;
    }

    QT3D_FUNCTOR(PointcloudVertexDataGenerator)

private:
    QPointcloudGeometry *m_geometry;
};

class QPointcloudGeometryPrivate
{
public:
    QPointcloudGeometryPrivate()
        :m_vertexBuffer(NULL),
         m_pointcloud(NULL){}
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

Qt3DRender::QAttribute::VertexBaseType pclTypeToAttributeType(const QPointfield::PointFieldTypes &inp)
{
    switch(inp)
    {
    case QPointfield::INT8:
        return Qt3DRender::QAttribute::Byte;
    case QPointfield::INT16:
        return Qt3DRender::QAttribute::Short;
    case QPointfield::INT32:
        return Qt3DRender::QAttribute::Int;
    case QPointfield::UINT8:
        return Qt3DRender::QAttribute::UnsignedByte;
    case QPointfield::UINT16:
        return Qt3DRender::QAttribute::UnsignedShort;
    case QPointfield::UINT32:
        return Qt3DRender::QAttribute::UnsignedInt;
    case QPointfield::FLOAT32:
        return Qt3DRender::QAttribute::Float;
    case QPointfield::FLOAT64:
        return Qt3DRender::QAttribute::Double;
    default:
        Q_ASSERT(false);
        return Qt3DRender::QAttribute::Float;
    }
}

void QPointcloudGeometry::updateVertices()
{
    if(
#ifdef WITH_PCL
           (  m_p->m_pointcloud == NULL
           || m_p->m_pointcloud->pointcloud() == NULL
           || m_p->m_pointcloud->pointcloud()->data.size() == 0)
       &&
#endif
           (  m_p->m_pointcloud->data().length() == 0))
        return;

//    if(m_p->m_vertexBuffer)
//    {
//        m_p->m_vertexBuffer->deleteLater();
//    }

    //m_p->m_vertexBuffer->setBufferFunctor(Qt3DRender::QBufferFunctorPtr(new PointcloudVertexDataFunctor(m_p->m_pointcloud->pointcloud())));

    updateAttributes();
    //QMetaObject::invokeMethod(this, "updateAttributes", Qt::QueuedConnection);
    m_p->m_vertexBuffer->setDataGenerator(Qt3DRender::QBufferDataGeneratorPtr(new PointcloudVertexDataGenerator(this)));
}

QPointcloud *QPointcloudGeometry::pointcloud() const
{
    return m_p->m_pointcloud;
}

void QPointcloudGeometry::updateAttributes()
{
    // completely rebuild attribute list and remove all previous attributes
    QVector<Qt3DRender::QAttribute *> atts = attributes();
    Q_FOREACH(Qt3DRender::QAttribute *attr, atts)
    {
        if(attr->attributeType() == Qt3DRender::QAttribute::VertexAttribute)
        {
            removeAttribute(attr);
            attr->deleteLater();
        }
        else
        {
            qDebug() << "skipped index";
        }
    }

    // Prepare hash table to query attribute names easily
    QHash<QString, QPointfield* > pfs;
    m_p->m_pointcloud->updateAttributes();

    const QList<QPointfield *> &fieldList = m_p->m_pointcloud->getFields();
    for(auto fieldIter = fieldList.cbegin(); fieldIter != fieldList.cend() ; ++fieldIter)
    {
        pfs.insert((*fieldIter)->name(), *fieldIter);
    }

    // parse point fields and make reasonable attributes out of them
    QHash<QString, QPointfield* >::const_iterator pf(pfs.find("x"));
    if(pf != pfs.cend())
    {
        int num = 1 + (pfs.contains("y")?1:0) + (pfs.contains("z")?1:0) + (pfs.contains("w")?1:0);
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype()));
        attrib->setDataSize(num);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset());
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
        setBoundingVolumePositionAttribute(attrib);
    }
    pf = pfs.find("r");
    if(pf != pfs.cend())
    {
        int num = 1 + (pfs.contains("g")?1:0) + (pfs.contains("b")?1:0) + (pfs.contains("A")?1:0);
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype()));
        attrib->setDataSize(num);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset());
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
    pf = pfs.find("normal_x");
    if(pf != pfs.cend())
    {
        int num = 1 + (pfs.contains("normal_y")?1:0) + (pfs.contains("normal_z")?1:0) + (pfs.contains("curvature")?1:0);
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype()));
        attrib->setDataSize(num);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset());
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
    pf = pfs.find("intensity");
    if(pf != pfs.cend())
    {
        Qt3DRender::QAttribute* attrib = new Qt3DRender::QAttribute(this);
        attrib->setName("intensity");
        attrib->setDataType(pclTypeToAttributeType((*pf)->datatype()));
        attrib->setDataSize(1);
        attrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        attrib->setBuffer(m_p->m_vertexBuffer);
        attrib->setByteStride(m_p->m_pointcloud->point_step());
        attrib->setByteOffset((*pf)->offset());
        attrib->setCount(m_p->m_pointcloud->width() * m_p->m_pointcloud->height());
        addAttribute(attrib);
    }
}

void QPointcloudGeometry::setPointcloud(QPointcloud *pointcloud)
{
    if (m_p->m_pointcloud == pointcloud)
        return;

    m_p->m_pointcloud = pointcloud;
    updateVertices();
    Q_EMIT pointcloudChanged(pointcloud);
}
