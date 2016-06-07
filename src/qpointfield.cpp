#include "qpointfield.h"

QPointfield::QPointfield(QObject *parent, pcl::PCLPointField *field)
    :QObject(parent),
      m_pointfield(field)
{
}

QPointfield::QPointfield(pcl::PCLPointField *field)
    :m_pointfield(field)
{
}

//QPointfield::QPointfield(const QPointfield &cpy)
//    :m_pointfield(cpy.m_pointfield)
//{
//}

QString QPointfield::name() const
{
    return QString::fromStdString(m_pointfield->name);
}

quint32 QPointfield::offset() const
{
    return m_pointfield->offset;
}

QPointfield::PointFieldTypes QPointfield::datatype() const
{
    return static_cast<QPointfield::PointFieldTypes>(m_pointfield->datatype);
}

quint32 QPointfield::count() const
{
    return m_pointfield->count;
}

void QPointfield::setName(QString name)
{
    std::string stdname(name.toStdString());
    if (m_pointfield->name == stdname)
        return;

    m_pointfield->name = stdname;
    Q_EMIT nameChanged(name);
}

void QPointfield::setOffset(quint32 offset)
{
    if (m_pointfield->offset == offset)
        return;

    m_pointfield->offset = offset;
    Q_EMIT offsetChanged(offset);
}

void QPointfield::setDatatype(QPointfield::PointFieldTypes datatype)
{
    if (m_pointfield->datatype == datatype)
        return;

    m_pointfield->datatype = datatype;
    Q_EMIT datatypeChanged(datatype);
}

void QPointfield::setCount(quint32 count)
{
    if (m_pointfield->count == count)
        return;

    m_pointfield->count = count;
    Q_EMIT countChanged(count);
}
