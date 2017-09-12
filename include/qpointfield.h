#ifndef QPOINTFIELD_H
#define QPOINTFIELD_H

#if WITH_PCL
namespace pcl {
    class PCLPointField;
}
#endif

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
    enum PointFieldTypes { INT8,
                           UINT8,
                           INT16,
                           UINT16,
                           INT32,
                           UINT32,
                           FLOAT32,
                           FLOAT64};
#if WITH_PCL
    QPointfield(QObject *parent, pcl::PCLPointField *field);
    QPointfield(pcl::PCLPointField *field);
#endif
    QPointfield(QObject *parent, QString name, quint32 offset, PointFieldTypes type, quint32 count);
    //QPointfield(const QPointfield &cpy);
    QString name() const;
    quint32 offset() const;
    PointFieldTypes datatype() const;
    quint32 count() const;

#if WITH_PCL
    const pcl::PCLPointField* getPointfield() { return m_pointfield; }
#endif
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

    QString m_name;
    quint32 m_offset;
    PointFieldTypes m_datatype;
    quint32 m_count;
#if WITH_PCL
    pcl::PCLPointField *m_pointfield;
#endif
};

#endif
