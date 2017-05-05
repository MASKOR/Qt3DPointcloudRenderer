#include "qpointcloud.h"
#ifdef WITH_PCL
#include <pcl/PCLPointCloud2.h>
#endif
#ifdef WITH_LAS
#include <liblas/reader.hpp>
#endif
#include <QRgb>

class QPointcloudPrivate
{
public:
    QPointcloudPrivate(QPointcloud* p)
        :m_parent(p)
        ,m_pointcloud(nullptr)
        ,m_width(0)
        ,m_height(1)
        ,m_is_bigendian(0)
        ,m_point_step(0)
        ,m_row_step(0)
        ,m_data()
        ,m_is_dense(0)
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

    static void fields_append(QQmlListProperty<QPointfield> *self, QPointfield* f);
    static int fields_count(QQmlListProperty<QPointfield> *self);
    static QPointfield* fields_at(QQmlListProperty<QPointfield> *self, int i);
    static void fields_clear(QQmlListProperty<QPointfield> *self);

//    void updateFields()
//    {
//        for(QList<QPointfield*>::iterator qiter(m_fields.begin()) ; qiter != m_fields.end() ; qiter++)
//        {
//            (*qiter)->deleteLater();
//        }
//        m_fields.clear();
//#ifdef WITH_PCL
//        if(m_pointcloud)
//        {
//            for(std::vector< ::pcl::PCLPointField>::iterator iter(m_pointcloud->fields.begin());
//                iter != m_pointcloud->fields.end(); iter++)
//            {
//                m_fields.append( new QPointfield(m_parent, &(*iter) ) );
//            }
//        }
//    #ifdef WITH_LAS
//        else
//    #endif
//#endif
//#ifdef WITH_LAS
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

QPointcloud::QPointcloud(QObject *parent)
    :QObject(parent),
     m_priv(new QPointcloudPrivate(this))
{
#ifdef WITH_PCL
    //m_priv->m_pointcloud = new pcl::PCLPointCloud2();
#endif
}

#ifdef WITH_PCL
QPointcloud::QPointcloud(pcl::PCLPointCloud2 *pointcloud)
    :m_priv(new QPointcloudPrivate(this))
{
    m_priv->m_pointcloud = pointcloud;
}
#endif

QPointcloud::~QPointcloud()
{
#ifdef WITH_PCL
    if(m_priv->m_pointcloud != nullptr)
    {
        delete m_priv->m_pointcloud;
    }
#endif
    delete m_priv;
}

void QPointcloud::updateAttributes()
{
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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

#ifdef WITH_PCL
pcl::PCLPointCloud2 *QPointcloud::pointcloud() const
{
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

#ifdef WITH_LAS
void QPointcloud::read(liblas::Reader *reader, bool demean = true, bool normalize = true, float normalizeScale = 10.f, bool flipYZ = true)
{
    const liblas::Header header = reader->GetHeader();
    uint32_t points = header.GetPointRecordsCount();

    bool hasData = false;// TODO: check if data is a usefull feature. // header.GetDataRecordLength() >= 3;
    const size_t pointSize = sizeof(float) * 4                    // pos
                           + sizeof(uint8_t)                      // intensity
                           + sizeof(uint32_t)                     // color
                           + sizeof(uint8_t) * (hasData ? 3 : 0); // data
    m_priv->m_data.resize(points*pointSize);

    setPoint_step(pointSize);
    setWidth(points);

    size_t offsetIntens = sizeof(float)*3;
    size_t offsetColor = offsetIntens + sizeof(uint16_t);
    size_t offsetData = offsetColor + sizeof(QRgb);

    uint32_t i=0;
    double scale, cx, cy, cz;
    if(demean || normalize)
    {
        double minX = header.GetMinX();
        double minY = header.GetMinY();
        double minZ = header.GetMinZ();
        double maxX = header.GetMaxX();
        double maxY = header.GetMaxY();
        double maxZ = header.GetMaxZ();

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
        if(flipYZ)
        {
            cy = minY+sy*0.5;
            cz = minZ+sz*0.5;
        }
        else
        {
            cy = minZ+sz*0.5;
            cz = minY+sy*0.5;
        }
    }
    else
    {
        scale = 1.;
        cx = 0.;
        cy = 0.;
        cz = 0.;
    }
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
    while(reader->ReadNextPoint())
    {
        liblas::Point current(reader->GetPoint());
        float value;
        value = (current.GetX()-cx)*scale;
        memcpy(&m_priv->m_data.data()[i], &value, sizeof(float));
        value = ((current.*dim2)()-cy)*scale;
        memcpy(&m_priv->m_data.data()[i+sizeof(float)], &value, sizeof(float));
        value = ((current.*dim3)()-cz)*scale;
        memcpy(&m_priv->m_data.data()[i+sizeof(float)*2], &value, sizeof(float));

        uint16_t intensity = current.GetIntensity();
        memcpy(&m_priv->m_data.data()[i+offsetIntens], &intensity, sizeof(uint16_t));

        liblas::Color color = current.GetColor();
        QRgb rgb(qRgb(color.GetRed(), color.GetGreen(), color.GetBlue()));
        memcpy(&m_priv->m_data.data()[i+offsetColor], &rgb, sizeof(QRgb));

        if(hasData)
        {
            assert(current.GetData().size() >= 3); // In this case header was wrong
            memcpy(&m_priv->m_data.data()[i+offsetData], current.GetData().data(), sizeof(uint8_t)*3);
        }
        assert(offsetData + sizeof(uint8_t)*3 == pointSize);
        i+=pointSize;
    }

    QPointfield *f;
    f = new QPointfield(this, "x", 0, QPointfield::FLOAT32, points);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "y", 4, QPointfield::FLOAT32, points);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "z", 8, QPointfield::FLOAT32, points);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "intensity", 12, QPointfield::UINT16, points);
    m_priv->m_fields.append(f);
    f = new QPointfield(this, "color", 14, QPointfield::UINT32, points);
    m_priv->m_fields.append(f);
    if(hasData)
    {
        f = new QPointfield(this, "datax", 18, QPointfield::UINT8, points);
        m_priv->m_fields.append(f);
        f = new QPointfield(this, "datay", 19, QPointfield::UINT8, points);
        m_priv->m_fields.append(f);
        f = new QPointfield(this, "dataz", 20, QPointfield::UINT8, points);
        m_priv->m_fields.append(f);
    }
}
#endif

void QPointcloud::setHeight(quint32 height)
{
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
#ifdef WITH_PCL
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
