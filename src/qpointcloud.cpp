#include "qpointcloud.h"
#include "pcl/PCLPointCloud2.h"

class QPointcloudPrivate
{
public:
    QPointcloudPrivate(QPointcloud* p):m_parent(p) {}
    QPointcloud *m_parent;
    pcl::PCLPointCloud2 *m_pointcloud;
    QList<QPointfield*> m_fields;

    static void fields_append(QQmlListProperty<QPointfield> *self, QPointfield* f);
    static int fields_count(QQmlListProperty<QPointfield> *self);
    static QPointfield* fields_at(QQmlListProperty<QPointfield> *self, int i);
    static void fields_clear(QQmlListProperty<QPointfield> *self);

    void updateFields()
    {
        for(QList<QPointfield*>::iterator qiter(m_fields.begin()) ; qiter != m_fields.end() ; qiter++)
        {
            (*qiter)->deleteLater();
        }
        m_fields.clear();
        for(std::vector< ::pcl::PCLPointField>::iterator iter(m_pointcloud->fields.begin());
            iter != m_pointcloud->fields.end(); iter++)
        {
            m_fields.append( new QPointfield(m_parent, &(*iter) ) );
        }
    }
};

QPointcloud::QPointcloud(QObject *parent)
    :QObject(parent),
     m_priv(new QPointcloudPrivate(this))
{
    m_priv->m_pointcloud = new pcl::PCLPointCloud2();
}

QPointcloud::QPointcloud(pcl::PCLPointCloud2 *pointcloud)
    :m_priv(new QPointcloudPrivate(this))
{
    m_priv->m_pointcloud = pointcloud;
}

QPointcloud::~QPointcloud()
{
    delete m_priv->m_pointcloud;
    delete m_priv;
}

quint32 QPointcloud::height() const
{
    return m_priv->m_pointcloud->height;
}

quint32 QPointcloud::width() const
{
    return m_priv->m_pointcloud->width;
}

QQmlListProperty<QPointfield> QPointcloud::fields()
{
    m_priv->updateFields();
    return QQmlListProperty<QPointfield>(this, static_cast<void*>(m_priv->m_pointcloud), &QPointcloudPrivate::fields_append, &QPointcloudPrivate::fields_count, &QPointcloudPrivate::fields_at, &QPointcloudPrivate::fields_clear);
}

void QPointcloudPrivate::fields_append(QQmlListProperty<QPointfield> *self, QPointfield *f)
{
    Q_ASSERT_X(false, "QPointcloud::fields_append", "Must not be called. ");
    pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(self->data);
    p->fields.push_back(*f->getPointfield());
}

int QPointcloudPrivate::fields_count(QQmlListProperty<QPointfield> *self)
{
    pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(self->data);
    return p->fields.size();
}

QPointfield *QPointcloudPrivate::fields_at(QQmlListProperty<QPointfield> *self, int i)
{
    pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(self->data);
    return new QPointfield(&p->fields.at(i));
}

void QPointcloudPrivate::fields_clear(QQmlListProperty<QPointfield> *self)
{
    pcl::PCLPointCloud2 *p = static_cast<pcl::PCLPointCloud2*>(self->data);
    p->fields.clear();
}

quint8 QPointcloud::is_bigendian() const
{
    return m_priv->m_pointcloud->is_bigendian;
}

quint32 QPointcloud::point_step() const
{
    return m_priv->m_pointcloud->point_step;
}

quint32 QPointcloud::row_step() const
{
    return m_priv->m_pointcloud->row_step;
}

QByteArray QPointcloud::data() const
{
    return QByteArray::fromRawData(reinterpret_cast<const char*>(&m_priv->m_pointcloud->data[0]), m_priv->m_pointcloud->data.size());
}

quint8 QPointcloud::is_dense() const
{
    return m_priv->m_pointcloud->is_dense;
}

pcl::PCLPointCloud2 *QPointcloud::pointcloud()
{
    return m_priv->m_pointcloud;
}

void QPointcloud::setHeight(quint32 height)
{
    if (m_priv->m_pointcloud->height == height)
        return;

    m_priv->m_pointcloud->height = height;
    Q_EMIT heightChanged(height);
}

void QPointcloud::setWidth(quint32 width)
{
    if (m_priv->m_pointcloud->width == width)
        return;

    m_priv->m_pointcloud->width = width;
    Q_EMIT widthChanged(width);
}

void QPointcloud::setIs_bigendian(quint8 is_bigendian)
{
    if (m_priv->m_pointcloud->is_bigendian == is_bigendian)
        return;

    m_priv->m_pointcloud->is_bigendian = is_bigendian;
    Q_EMIT is_bigendianChanged(is_bigendian);
}

void QPointcloud::setPoint_step(quint32 point_step)
{
    if (m_priv->m_pointcloud->point_step == point_step)
        return;

    m_priv->m_pointcloud->point_step = point_step;
    Q_EMIT point_stepChanged(point_step);
}

void QPointcloud::setRow_step(quint32 row_step)
{
    if (m_priv->m_pointcloud->row_step == row_step)
        return;

    m_priv->m_pointcloud->row_step = row_step;
    Q_EMIT row_stepChanged(row_step);
}

void QPointcloud::setData(QByteArray data)
{
    m_priv->m_pointcloud->data.resize(data.size());
    memcpy(&m_priv->m_pointcloud->data[0], data.data(), m_priv->m_pointcloud->data.size());
    Q_EMIT dataChanged(data);
}

void QPointcloud::setIs_dense(quint8 is_dense)
{
    if (m_priv->m_pointcloud->is_dense == is_dense)
        return;

    m_priv->m_pointcloud->is_dense = is_dense;
    Q_EMIT is_denseChanged(is_dense);
}
