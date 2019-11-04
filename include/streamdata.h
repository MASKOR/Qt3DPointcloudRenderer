#ifndef STREAMDATA_H
#define STREAMDATA_H

#include <QObject>
#include <QMutex>

class Streamdata : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int capacity READ capacity WRITE setCapacity NOTIFY capacityChanged)
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(int writeIndex READ writeIndex NOTIFY writeIndexChanged)
    Q_PROPERTY(int readIndex READ readIndex NOTIFY readIndexChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)
public:
    Streamdata(QObject *parent = nullptr);
    int capacity() const;
    //TODO: Pointer, ref?
    QByteArray data() const;
    int writeIndex() const;
    int readIndex() const;
    bool isEmpty() const;

public slots:
    void setCapacity(int capacity);
    void setData(QByteArray data);
    void consume(const char* ptr, int len);

signals:
    void capacityChanged(int capacity);
    void dataChanged(QByteArray data);
    void writeIndexChanged(int writeIndex);
    void readIndexChanged(int readIndex);
    void isEmptyChanged(bool isEmpty);

private:
    int m_capacity;
    QByteArray m_data;
    int m_writeIndex;
    int m_readIndex;
    bool m_isEmpty;
    QMutex  m_dataMutex;
};

#endif // STREAMDATA_H
