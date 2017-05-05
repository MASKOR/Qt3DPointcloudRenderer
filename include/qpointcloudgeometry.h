#ifndef QPOINTCLOUDGEOMETRY_H
#define QPOINTCLOUDGEOMETRY_H

#include <Qt3DRender/qgeometry.h>
#include "qpointcloud.h"

class QPointcloudGeometryPrivate;

class QPointcloudGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(QPointcloud *pointcloud READ pointcloud WRITE setPointcloud NOTIFY pointcloudChanged)

public:
    explicit QPointcloudGeometry(QNode *parent = NULL);
    ~QPointcloudGeometry();
    void updateVertices();

    QPointcloud *pointcloud() const;


public Q_SLOTS:
    void setPointcloud(QPointcloud *pointcloud);
private Q_SLOTS:
    void updateAttributes();
Q_SIGNALS:
    void pointcloudChanged(QPointcloud *pointcloud);

private:
    QPointcloudGeometryPrivate *m_p;
};


#endif
