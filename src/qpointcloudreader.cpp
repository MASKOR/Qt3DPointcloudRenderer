#include "qpointcloudreader.h"
#include "pcl/io/pcd_io.h"
#include "pcl/io/ply_io.h"
#include <QDebug>

QPointCloudReader::QPointCloudReader()
    :m_pointcloud(new QPointcloud())
{
}

QString QPointCloudReader::filename() const
{
    return m_filename;
}

QPointcloud *QPointCloudReader::pointcloud() const
{
    return m_pointcloud;
}

void QPointCloudReader::setFilename(QString filename)
{
    if (m_filename == filename)
        return;

    if(filename.endsWith(".pcd", Qt::CaseInsensitive))
    {
        pcl::PCDReader reader;
        reader.read(filename.toStdString(), *m_pointcloud->pointcloud());
    }
    else if(filename.endsWith(".ply", Qt::CaseInsensitive))
    {
        pcl::PLYReader reader;
        reader.read(filename.toStdString(), *m_pointcloud->pointcloud());
    }
    qDebug() << "Read Pointcloud" << filename << "with" << ((m_pointcloud->pointcloud()->width) * (m_pointcloud->pointcloud()->height)) << "points.";
    m_filename = filename;
    Q_EMIT filenameChanged(filename);
    Q_EMIT pointcloudChanged(m_pointcloud);
}
