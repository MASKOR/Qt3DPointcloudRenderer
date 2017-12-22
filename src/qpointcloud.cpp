#include "qpointcloud.h"
#if WITH_PCL
#include <pcl/PCLPointCloud2.h>
#include <pcl/point_types.h>
#include <pcl/common/centroid.h>
#include <pcl/common/common.h>
#endif
#if WITH_LAS
#include <liblas/reader.hpp>
#endif
#include <QRgb>
#include <QFile>
#include <QVector>
#include <limits>
#include <QDebug> //DBG

class QPointcloudPrivate
{
public:
    QPointcloudPrivate(QPointcloud* p)
        : m_parent(p)
        , m_pointcloud(nullptr)
        , m_width(0)
        , m_height(1)
        , m_is_bigendian(0)
        , m_point_step(0)
        , m_row_step(0)
        , m_data()
        , m_is_dense(0)
        , m_minimum()
        , m_maximum()
        , m_centroid()
        , m_offset()
        , m_dirtyMinMax(true)
        , m_dirtyCentroid(true)
    {}
    QPointcloud *m_parent;
    pcl::PCLPointCloud2 *m_pointcloud;
    QList<QPointfield*> m_fields;

    quint32 m_height;
    quint32 m_width;

    quint8 m_is_bigendian;
    quint32 m_point_step;
    quint32 m_row_step;
    QByteArray m_data;
    quint8 m_is_dense;

    QVector3D m_minimum;
    QVector3D m_maximum;
    QVector3D m_centroid;
    QVector3D m_offset;
    bool m_dirtyMinMax;
    bool m_dirtyCentroid;

    static void fields_append(QQmlListProperty<QPointfield> *self, QPointfield* f);
    static int fields_count(QQmlListProperty<QPointfield> *self);
    static QPointfield* fields_at(QQmlListProperty<QPointfield> *self, int i);
    static void fields_clear(QQmlListProperty<QPointfield> *self);

    void updateMinMax()
    {
#if WITH_PCL
        pcl::PointXYZ min;
        pcl::PointXYZ max;
        pcl::PointCloud<pcl::PointXYZ> pc;
        pcl::fromPCLPointCloud2( *m_pointcloud, pc);
        pcl::getMinMax3D(pc, min, max);
        m_minimum = QVector3D(min.x, min.y, min.z);
        m_maximum = QVector3D(max.x, max.y, max.z);
        m_dirtyMinMax = false;
#endif
#if WITH_LAS
    //already done on read
        m_dirtyMinMax = false;
#endif
    }
    void updateCentroid()
    {

    }

//    void updateFields()
//    {
//        for(QList<QPointfield*>::iterator qiter(m_fields.begin()) ; qiter != m_fields.end() ; qiter++)
//        {
//            (*qiter)->deleteLater();
//        }
//        m_fields.clear();
//#if WITH_PCL
//        if(m_pointcloud)
//        {
//            for(std::vector< ::pcl::PCLPointField>::iterator iter(m_pointcloud->fields.begin());
//                iter != m_pointcloud->fields.end(); iter++)
//            {
//                m_fields.append( new QPointfield(m_parent, &(*iter) ) );
//            }
//        }
//    #if WITH_LAS
//        else
//    #endif
//#endif
//#if WITH_LAS
//        {
//            // Standard LAS fields
//            QPointfield *f;
//            f = new QPointfield(m_parent, "x", 0, QPointfield::FLOAT32, 1);
//            f = new QPointfield(m_parent, "y", 8, QPointfield::FLOAT32, 1);
//            f = new QPointfield(m_parent, "z", 16, QPointfield::FLOAT32, 1);
//            f = new QPointfield(m_parent, "intensity", 24, QPointfield::FLOAT32, 1);
//        }
//#endif
//        //else TODO: completely custom format for pointclouds
//    }
};

QPointcloud::QPointcloud(QPointcloud *copy)
    :m_priv(new QPointcloudPrivate(this))
{
#if WITH_PCL
    this->setPointcloud( *copy->m_priv->m_pointcloud );
#endif
}

QPointcloud::QPointcloud(QObject *parent)
    :QObject(parent),
     m_priv(new QPointcloudPrivate(this))
{
#if WITH_PCL
    //m_priv->m_pointcloud = new pcl::PCLPointCloud2();
#endif
}

#if WITH_PCL
QPointcloud::QPointcloud(pcl::PCLPointCloud2 *pointcloud)
    :m_priv(new QPointcloudPrivate(this))
{
    m_priv->m_pointcloud = pointcloud;
}
#endif

QPointcloud::~QPointcloud()
{
#if WITH_PCL
    if(m_priv->m_pointcloud != nullptr)
    {
        delete m_priv->m_pointcloud;
    }
#endif
    delete m_priv;
}

void QPointcloud::updateAttributes()
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        m_priv->m_fields.clear();
        for(int i=0 ; m_priv->m_pointcloud->fields.size() >i ; ++i)
        {
            pcl::PCLPointField &pf( m_priv->m_pointcloud->fields[i] );
            m_priv->m_fields.append(new QPointfield(&pf));
        }
    }
    else
#endif
    {
        // LAS Attributes are read when LAS is read
    }
}

quint32 QPointcloud::height() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return m_priv->m_pointcloud->height;
    }
    else
#endif
    {
        return m_priv->m_height;
    }
}

quint32 QPointcloud::width() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return m_priv->m_pointcloud->width;
    }
    else
#endif
    {
        return m_priv->m_width;
    }
}

QQmlListProperty<QPointfield> QPointcloud::fields()
{
    return QQmlListProperty<QPointfield>(this, static_cast<void*>(m_priv), &QPointcloudPrivate::fields_append, &QPointcloudPrivate::fields_count, &QPointcloudPrivate::fields_at, &QPointcloudPrivate::fields_clear);
}

void QPointcloudPrivate::fields_append(QQmlListProperty<QPointfield> *self, QPointfield *f)
{
    QPointcloudPrivate *that = static_cast<QPointcloudPrivate*>(self->data);
#if WITH_PCL
    if(that->m_pointcloud)
    {
        // This is not typical for PCL. PCL would create a new pointcloud instead.
        // if this is needed, there should be a conversion from PCL Pointcloud to a
        // custom pointcloud format.
        Q_ASSERT_X(false, "QPointcloud::fields_append", "Must not be called.");
        pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(that->m_pointcloud);
        p->fields.push_back(*f->getPointfield());
    }
    else
#endif
    {
        that->m_fields.append(f);
    }
}

int QPointcloudPrivate::fields_count(QQmlListProperty<QPointfield> *self)
{
    QPointcloudPrivate *that = static_cast<QPointcloudPrivate*>(self->data);
#if WITH_PCL
    if(that->m_pointcloud)
    {
        Q_ASSERT_X(that->m_fields.count() == 0, "QPointcloudPrivate::fields_count", "Mixed up pcl and non pcl.");
        pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(that->m_pointcloud);
        return p->fields.size();
    }
    else
#endif
    {
        that->m_fields.count();
    }
}

QPointfield *QPointcloudPrivate::fields_at(QQmlListProperty<QPointfield> *self, int i)
{
    QPointcloudPrivate *that = static_cast<QPointcloudPrivate*>(self->data);
#if WITH_PCL
    if(that->m_pointcloud)
    {
        Q_ASSERT_X(that->m_fields.count() == 0, "QPointcloudPrivate::fields_count", "Mixed up pcl and non pcl.");
        pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(that->m_pointcloud);
        return new QPointfield(&p->fields.at(i));
    }
    else
#endif
    {
        that->m_fields.at(i);
    }
}

void QPointcloudPrivate::fields_clear(QQmlListProperty<QPointfield> *self)
{
    QPointcloudPrivate *that = static_cast<QPointcloudPrivate*>(self->data);
#if WITH_PCL
    if(that->m_pointcloud)
    {
        Q_ASSERT_X(that->m_fields.count() == 0, "QPointcloudPrivate::fields_count", "Mixed up pcl and non pcl.");
        pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(that->m_pointcloud);
        p->fields.clear();
    }
    else
#endif
    {
        that->m_fields.clear();
    }
}

quint8 QPointcloud::is_bigendian() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return m_priv->m_pointcloud->is_bigendian;
    }
    else
#endif
    {
        return m_priv->m_is_bigendian;
    }
}

quint32 QPointcloud::point_step() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return m_priv->m_pointcloud->point_step;
    }
    else
#endif
    {
        return m_priv->m_point_step;
    }
}

quint32 QPointcloud::row_step() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return m_priv->m_pointcloud->row_step;
    }
    else
#endif
    {
        return m_priv->m_row_step;
    }
}

QByteArray QPointcloud::data() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return QByteArray(reinterpret_cast<char*>(&m_priv->m_pointcloud->data[0]), m_priv->m_pointcloud->data.size());
        //return QByteArray::fromRawData(reinterpret_cast<const char*>(&m_priv->m_pointcloud->data[0]), m_priv->m_pointcloud->data.size());
    }
    else
#endif
    {
        return m_priv->m_data;
    }
}

quint8 QPointcloud::is_dense() const
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        return m_priv->m_pointcloud->is_dense;
    }
    else
#endif
    {
        return m_priv->m_is_dense;
    }
}

const QList<QPointfield *> &QPointcloud::getFields()
{
    return m_priv->m_fields;
}

#if WITH_PCL
pcl::PCLPointCloud2 *QPointcloud::pointcloud()
{
    if(nullptr == m_priv->m_pointcloud)
    {
        m_priv->m_pointcloud = new pcl::PCLPointCloud2();
    }
    return m_priv->m_pointcloud;
}

void QPointcloud::setPointcloud(const pcl::PCLPointCloud2& copy)
{
    if(m_priv->m_pointcloud != nullptr)
    {
        delete m_priv->m_pointcloud;
    }
    m_priv->m_pointcloud = new pcl::PCLPointCloud2(copy);
}
#endif

void QPointcloud::readXyz(QString &path, bool demean, bool normalize, float normalizeScale, bool flipYZ)
{
    // Not yet implemented
}

QVector3D QPointcloud::minimum() const
{
    if(m_priv->m_dirtyMinMax)
    {
        m_priv->updateMinMax();
    }
    return m_priv->m_minimum;
}

QVector3D QPointcloud::maximum() const
{
    if(m_priv->m_dirtyMinMax)
    {
        m_priv->updateMinMax();
    }
    return m_priv->m_maximum;
}

QVector3D QPointcloud::centroid() const
{
    if(m_priv->m_dirtyCentroid)
    {
        m_priv->updateCentroid();
    }
    return m_priv->m_centroid;
}

QVector3D QPointcloud::offset() const
{
    return m_priv->m_offset;
}

#if WITH_LAS
void QPointcloud::read(liblas::Reader *reader
                       , const bool useOffset
                       , double *offsetX
                       , double *offsetY
                       , double *offsetZ
                       , bool demean
                       , bool normalize
                       , float normalizeScale
                       , bool flipYZ
                       , int nth
                       )
{
    const liblas::Header header = reader->GetHeader();
    uint32_t points = header.GetPointRecordsCount();

    bool hasData = false;// TODO: check if data is a useful feature. // header.GetDataRecordLength() >= 3;
    const size_t pointSize = sizeof(float) * 4                    // pos
                           + sizeof(uint8_t)                      // intensity
                           + sizeof(uint32_t)                     // color
                           + sizeof(uint8_t) * (hasData ? 3 : 0); // data
    m_priv->m_data.resize(points*pointSize/nth);

    setPoint_step(pointSize);
    setWidth(points/nth);

    size_t offsetIntens = sizeof(float)*3;
    size_t offsetColor = offsetIntens + sizeof(uint16_t);
    size_t offsetData = offsetColor + sizeof(QRgb);

    double scale, cx, cy, cz;
    double minX = header.GetMinX();
    double maxX = header.GetMaxX();
    double minY;
    double maxY;
    double minZ;
    double maxZ;
    if(flipYZ)
    {
        minY = header.GetMinZ();
        maxY = header.GetMaxZ();
        minZ = header.GetMinY();
        maxZ = header.GetMaxY();
    }
    else
    {
        minY = header.GetMinY();
        maxY = header.GetMaxY();
        minZ = header.GetMinZ();
        maxZ = header.GetMaxZ();
    }
    if(demean || normalize)
    {
        double sx = maxX-minX;
        double sy = maxY-minY;
        double sz = maxZ-minZ;
        double maxDim = std::max(std::max(sx, sy), sz);

        if(normalize)
        {
            scale = normalizeScale / maxDim;
        }
        else
        {
            scale = 1.;
        }

        cx = minX+sx*0.5;
        cy = minY+sy*0.5;
        cz = minZ+sz*0.5;
    }
    else
    {
        scale = 1.;
        if(useOffset)
        {
            assert(offsetX != nullptr);
            assert(offsetY != nullptr);
            assert(offsetZ != nullptr);
            cx = *offsetX;
            cy = *offsetY;
            cz = *offsetZ;
        }
        else
        {
            cx = header.GetOffsetX();
            cy = header.GetOffsetY();
            cz = header.GetOffsetZ();
            assert(offsetX != nullptr);
            assert(offsetY != nullptr);
            assert(offsetZ != nullptr);
            *offsetX = cx;
            *offsetY = cy;
            *offsetZ = cz;
        }
    }
    m_priv->m_centroid = QVector3D(cx, cy, cz); // TODO: bounding box center, not really centroid
    m_priv->m_minimum = QVector3D(minX-cx, minY-cy, minZ-cz);
    m_priv->m_maximum = QVector3D(maxX-cx, maxY-cy, maxZ-cz);
    m_priv->m_offset = m_priv->m_centroid;
    m_priv->m_dirtyMinMax = false;
    m_priv->m_dirtyCentroid = false;

    typedef double (liblas::Point::*GetDimension)() const;
    GetDimension dim2, dim3;
    if(flipYZ)
    {
        dim2 = &liblas::Point::GetZ;
        dim3 = &liblas::Point::GetY;
    }
    else
    {
        dim2 = &liblas::Point::GetY;
        dim3 = &liblas::Point::GetZ;
    }
    m_priv->m_offset = QVector3D(cx, cy, cz);

//    double max[3];
//    double min[3];
//    for(int i=0 ; i<3 ; ++i)
//    {
//        min[i] = +std::numeric_limits<double>::infinity();
//        max[i] = -std::numeric_limits<double>::infinity();
//    }
    uint32_t i=0;
    uint32_t pointsPos=0;
    while(reader->ReadNextPoint())
    {
        if(i++%nth != 0)
            continue;
        liblas::Point current(reader->GetPoint());
        float value;
//        qDebug() << "DBG: x " << current.GetX() << " y: " << current.GetY() << " z: " << current.GetZ();
//        qDebug() << "DBG:rx " << current.GetRawX() << " y: " << current.GetRawY() << " z: " << current.GetRawZ();
        double x =  current.GetX();
        double y = (current.*dim2)();
        double z = (current.*dim3)();
//        if(min[0] > x) min[0] = x;
//        if(max[0] < x) max[0] = x;
//        if(min[1] > y) min[1] = y;
//        if(max[1] < y) max[1] = y;
//        if(min[2] > z) min[2] = z;
//        if(max[2] < z) max[2] = z;
//        if(x > maxX) ++outOfBoundsX1;
//        if(x < minX) ++outOfBoundsX2;
//        if(y > maxY) ++outOfBoundsY1;
//        if(y < minY) ++outOfBoundsY2;
//        if(z > maxZ) ++outOfBoundsZ1;
//        if(z < minZ) ++outOfBoundsZ2;
        value = (x-cx)*scale;
        memcpy(&m_priv->m_data.data()[pointsPos], &value, sizeof(float));
        value = (y-cy)*scale;
        memcpy(&m_priv->m_data.data()[pointsPos+sizeof(float)], &value, sizeof(float));
        value = (z-cz)*scale;
        memcpy(&m_priv->m_data.data()[pointsPos+sizeof(float)*2], &value, sizeof(float));

        uint16_t intensity = current.GetIntensity();
        memcpy(&m_priv->m_data.data()[pointsPos+offsetIntens], &intensity, sizeof(uint16_t));

        liblas::Color color = current.GetColor();
        QRgb rgb(qRgb(color.GetRed(), color.GetGreen(), color.GetBlue()));
        memcpy(&m_priv->m_data.data()[pointsPos+offsetColor], &rgb, sizeof(QRgb));

        if(hasData)
        {
            assert(current.GetData().size() >= 3); // In this case header was wrong
            memcpy(&m_priv->m_data.data()[pointsPos+offsetData], current.GetData().data(), sizeof(uint8_t)*3);
        }
        assert(offsetData + sizeof(uint8_t)*3 == pointSize);
        pointsPos+=pointSize;
    }
//    qDebug() << "real bounds: " << min[0] << "|" << max[0] << ","
//                                << min[1] << "|" << max[1] << ","
//                                << min[2] << "|" << max[2] << ";";

    QPointfield *f;
    f = new QPointfield(this, "x", 0, QPointfield::FLOAT32, i);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "y", 4, QPointfield::FLOAT32, i);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "z", 8, QPointfield::FLOAT32, i);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "intensity", 12, QPointfield::UINT16, i);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "color", 14, QPointfield::UINT32, i);
    m_priv->m_fields.append(f);
    if(hasData)
    {
        f = new QPointfield(this, "datax", 18, QPointfield::UINT8, i);
        m_priv->m_fields.append(f);
        f = new QPointfield(this, "datay", 19, QPointfield::UINT8, i);
        m_priv->m_fields.append(f);
        f = new QPointfield(this, "dataz", 20, QPointfield::UINT8, i);
        m_priv->m_fields.append(f);
    }
}
#endif

void QPointcloud::setHeight(quint32 height)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        if (m_priv->m_pointcloud->height == height)
            return;
        m_priv->m_pointcloud->height = height;
    }
    else
#endif
    {
        if (m_priv->m_height == height)
            return;
        m_priv->m_height = height;
    }
    Q_EMIT heightChanged(height);
}

void QPointcloud::setWidth(quint32 width)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        if (m_priv->m_pointcloud->width == width)
            return;
        m_priv->m_pointcloud->width = width;
    }
    else
#endif
    {
        if (m_priv->m_width == width)
            return;
        m_priv->m_width = width;
    }
    Q_EMIT widthChanged(width);
}

void QPointcloud::setIs_bigendian(quint8 is_bigendian)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        if (m_priv->m_pointcloud->is_bigendian == is_bigendian)
            return;
        m_priv->m_pointcloud->is_bigendian = is_bigendian;
    }
    else
#endif
    {
        if (m_priv->m_is_bigendian == is_bigendian)
            return;
        m_priv->m_is_bigendian = is_bigendian;
    }
    Q_EMIT is_bigendianChanged(is_bigendian);
}

void QPointcloud::setPoint_step(quint32 point_step)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        if (m_priv->m_pointcloud->point_step == point_step)
            return;
        m_priv->m_pointcloud->point_step = point_step;
    }
    else
#endif
    {
        if (m_priv->m_point_step == point_step)
            return;
        m_priv->m_point_step = point_step;
    }

    Q_EMIT point_stepChanged(point_step);
}

void QPointcloud::setRow_step(quint32 row_step)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        if (m_priv->m_pointcloud->row_step == row_step)
            return;

        m_priv->m_pointcloud->row_step = row_step;
    }
    else
#endif
    {
        if (m_priv->m_row_step == row_step)
            return;
        m_priv->m_row_step = row_step;
    }
    Q_EMIT row_stepChanged(row_step);
}

void QPointcloud::setData(QByteArray data)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        m_priv->m_pointcloud->data.resize(data.size());
        memcpy(&m_priv->m_pointcloud->data[0], data.data(), m_priv->m_pointcloud->data.size());
    }
    else
#endif
    {
        m_priv->m_data = data;
    }
    Q_EMIT dataChanged(data);
}

void QPointcloud::setIs_dense(quint8 is_dense)
{
#if WITH_PCL
    if(m_priv->m_pointcloud)
    {
        if (m_priv->m_pointcloud->is_dense == is_dense)
            return;

        m_priv->m_pointcloud->is_dense = is_dense;
    }
    else
#endif
    {
        if (m_priv->m_is_dense == is_dense)
            return;
        m_priv->m_is_dense = is_dense;
    }
    Q_EMIT is_denseChanged(is_dense);
}
