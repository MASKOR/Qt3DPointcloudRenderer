#ifndef STREAMDATA_H
#define STREAMDATA_H

#include <QObject>
#include <QMutex>

class Streamdata : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Streamdata)
    Q_PROPERTY(int capacity READ capacity WRITE setCapacity NOTIFY capacityChanged)
    Q_PROPERTY(QByteArray data READ data NOTIFY dataChanged)
    Q_PROPERTY(int writeIndex READ writeIndex NOTIFY writeIndexChanged)
    Q_PROPERTY(int readIndex READ readIndex NOTIFY readIndexChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)
    Q_PROPERTY(int divisor READ divisor WRITE setDivisor NOTIFY divisorChanged)
public:
    Streamdata(QObject *parent = nullptr);
    int capacity() const;
    //TODO: Pointer, ref?
    QByteArray data();
    int writeIndex() const;
    int readIndex() const;
    bool isEmpty() const;
    int divisor() const;

public slots:
    void setCapacity(int capacity);
    void setDivisor(int divisor);
    void consume(const char* ptr, int len);
signals:
    void capacityChanged(int capacity);
    void dataChanged();
    void writeIndexChanged(int writeIndex);
    void readIndexChanged(int readIndex);
    void isEmptyChanged(bool isEmpty);
    void divisorChanged(int divisor);

private:
    int m_capacity;
    std::vector<char> m_data;
    int m_writeIndex;
    int m_readIndex;
    int m_divisor;
    bool m_isEmpty;
    QMutex  m_dataMutex;
};

#endif // STREAMDATA_H
