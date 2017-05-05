#ifndef QPOINTCLOUD_H
#define QPOINTCLOUD_H

#include <QObject>
#include <QQmlListProperty>
#include "qpointfield.h" 


#ifdef WITH_PCL
namespace pcl
{
class PCLPointCloud2;
}
#endif

#ifdef WITH_LAS
namespace liblas
{
class Reader;
}
#endif

class QPointcloudPrivate;
class QPointcloud : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(quint32 width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QQmlListProperty<QPointfield> fields READ fields NOTIFY fieldsChanged)
    Q_PROPERTY(quint8 is_bigendian READ is_bigendian WRITE setIs_bigendian NOTIFY is_bigendianChanged)
    Q_PROPERTY(quint32 point_step READ point_step WRITE setPoint_step NOTIFY point_stepChanged)
    Q_PROPERTY(quint32 row_step READ row_step WRITE setRow_step NOTIFY row_stepChanged)
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(quint8 is_dense READ is_dense WRITE setIs_dense NOTIFY is_denseChanged)
public:
    QPointcloud(QObject *parent = NULL);
    ////
    /// \brief QPointcloud takes ownership of the given pointcloud
    /// \param pointcloud will be deleted in destructor. Do not use shared pointer pcl::PCLPointcloud2Ptr from outside.
    ///
    QPointcloud(pcl::PCLPointCloud2 *pointcloud);
    ~QPointcloud();

    void updateAttributes();

    quint32 height() const;
    quint32 width() const;

    QQmlListProperty<QPointfield> fields();

    quint8 is_bigendian() const;
    quint32 point_step() const;
    quint32 row_step() const;
    QByteArray data() const;
    quint8 is_dense() const;

    const QList<QPointfield *> &getFields();

#ifdef WITH_PCL
    pcl::PCLPointCloud2* pointcloud() const;
    void setPointcloud(const pcl::PCLPointCloud2 &copy);
#endif
#ifdef WITH_LAS
    void read(liblas::Reader* reader, bool demean, bool normalize, float normalizeScale, bool flipYZ);
#endif

public Q_SLOTS:
    void setHeight(quint32 height);
    void setWidth(quint32 width);
    void setIs_bigendian(quint8 is_bigendian);
    void setPoint_step(quint32 point_step);
    void setRow_step(quint32 row_step);
    void setData(QByteArray data);
    void setIs_dense(quint8 is_dense);

Q_SIGNALS:
    void heightChanged(quint32 height);
    void widthChanged(quint32 width);
    void fieldsChanged(QQmlListProperty<QPointfield> fields);
    void is_bigendianChanged(quint8 is_bigendian);
    void point_stepChanged(quint32 point_step);
    void row_stepChanged(quint32 row_step);
    void dataChanged(QByteArray data);
    void is_denseChanged(quint8 is_dense);

private:
    QPointcloudPrivate *m_priv;
};



#endif
