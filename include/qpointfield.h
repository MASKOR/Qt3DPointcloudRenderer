#ifndef QPOINTFIELD_H
#define QPOINTFIELD_H

#include "pcl/PCLPointField.h"
#include <QObject>

class QPointfield : public QObject
{
    Q_OBJECT
    Q_ENUMS(PointFieldTypes)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(quint32 offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(PointFieldTypes datatype READ datatype WRITE setDatatype NOTIFY datatypeChanged)
    Q_PROPERTY(quint32 count READ count WRITE setCount NOTIFY countChanged)
public:
    enum PointFieldTypes { INT8 = pcl::PCLPointField::INT8,
                           UINT8 = pcl::PCLPointField::UINT8,
                           INT16 = pcl::PCLPointField::INT16,
                           UINT16 = pcl::PCLPointField::UINT16,
                           INT32 = pcl::PCLPointField::INT32,
                           UINT32 = pcl::PCLPointField::UINT32,
                           FLOAT32 = pcl::PCLPointField::FLOAT32,
                           FLOAT64 = pcl::PCLPointField::FLOAT64 };

    QPointfield(QObject *parent, pcl::PCLPointField *field);
    QPointfield(pcl::PCLPointField *field);
    //QPointfield(const QPointfield &cpy);
    QString name() const;
    quint32 offset() const;
    PointFieldTypes datatype() const;
    quint32 count() const;

    const pcl::PCLPointField* getPointfield() { return m_pointfield; }
public Q_SLOTS:

    void setName(QString name);
    void setOffset(quint32 offset);
    void setDatatype(PointFieldTypes datatype);
    void setCount(quint32 count);

Q_SIGNALS:
    void nameChanged(QString name);
    void offsetChanged(quint32 offset);
    void datatypeChanged(PointFieldTypes datatype);
    void countChanged(quint32 count);

private:
    pcl::PCLPointField *m_pointfield;
};

#endif
