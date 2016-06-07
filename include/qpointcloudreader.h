#ifndef QPOINTCLOUDREADER_H
#define QPOINTCLOUDREADER_H

#include <QObject>
#include "qpointcloud.h"

class QPointCloudReader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QPointcloud *pointcloud READ pointcloud NOTIFY pointcloudChanged)
public:
    QPointCloudReader();

    QString filename() const;

    QPointcloud *pointcloud() const;

public Q_SLOTS:
    void setFilename(QString filename);

Q_SIGNALS:
    void filenameChanged(QString filename);
    void pointcloudChanged(QPointcloud * pointcloud);

private:
    QString m_filename;
    QPointcloud *m_pointcloud;
};

#endif
