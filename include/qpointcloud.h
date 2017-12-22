#ifndef QPOINTCLOUD_H
#define QPOINTCLOUD_H

#include <QObject>
#include <QVector3D>
#include <QQmlListProperty>
#include "qpointfield.h" 


#if WITH_PCL
namespace pcl
{
class PCLPointCloud2;
}
#endif

#if WITH_LAS
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
    Q_PROPERTY(QVector3D minimum READ minimum NOTIFY minimumChanged)
    Q_PROPERTY(QVector3D maximum READ maximum NOTIFY maximumChanged)
    Q_PROPERTY(QVector3D centroid READ centroid NOTIFY centroidChanged)
    Q_PROPERTY(QVector3D offset READ offset NOTIFY offsetChanged)
public:
    QPointcloud(QPointcloud *copy);
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

#if WITH_PCL
    pcl::PCLPointCloud2* pointcloud();
    void setPointcloud(const pcl::PCLPointCloud2 &copy);
#endif
#if WITH_LAS
    ///
    /// \brief read Reads a LAS dataset. LAS files often have big offsets which cannot be expressed using float.
    /// Thus, an offset must be applied in order to have floatingpoint data which can be visualized.
    /// This offset must be either known in advance or must be caluclated in this method
    /// (before conversion to floats happens).
    /// \param reader the liblas::Reader
    /// \param useOffset use the provided offset or output the calculated offset
    /// \param offset this is _substracted_ from every point. Offset of pointcloud to viewing coordinate system.
    /// \param demean calculate new offset. This overrides useOffset.
    /// \param normalize resize the pointcloud to fit a box with size normalizeScale in each direction. This overrides useOffset.
    /// \param normalizeScale size to bring the normalized model to.
    /// \param flipYZ if visualization uses Y axis up and pointcloud uses Z axis up, this must be true. Should not be used.
    /// \param nth only read in every nth point (fake level of detail).
    ///
    void read(liblas::Reader* reader
              , const bool useOffset = false
              , double *offsetX = nullptr
              , double *offsetY = nullptr
              , double *offsetZ = nullptr
              , bool demean = false
              , bool normalize = false
              , float normalizeScale = 1.0f
              , bool flipYZ = false
              , int nth = 1);
#endif

    void readXyz(QString &path, bool demean = true, bool normalize = true, float normalizeScale = 10.f, bool flipYZ = true);
    QVector3D minimum() const;
    QVector3D maximum() const;
    QVector3D centroid() const;
    QVector3D offset() const;

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
    void minimumChanged(QVector3D minimum);
    void maximumChanged(QVector3D maximum);
    void centroidChanged(QVector3D centroid);
    void offsetChanged(QVector3D offset);

private:
    QPointcloudPrivate *m_priv;
};



#endif
