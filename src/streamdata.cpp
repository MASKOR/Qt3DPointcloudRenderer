#include "streamdata.h"
#include <QMutexLocker>

Streamdata::Streamdata(QObject *parent)
    : QObject(parent)
    , m_capacity(100000)
    , m_data(m_capacity, Qt::Uninitialized)
    , m_writeIndex(0)
    , m_readIndex(0)
    , m_isEmpty(true)
    , m_dataMutex(QMutex::Recursive)
    , m_divisor(24)
{

}

int Streamdata::capacity() const
{
    return m_capacity;
}

QByteArray Streamdata::data() const
{
    return m_data;
}

int Streamdata::writeIndex() const
{
    return m_writeIndex;
}

int Streamdata::readIndex() const
{
    return m_readIndex;
}

bool Streamdata::isEmpty() const
{
    return m_isEmpty;
}

int Streamdata::divisor() const
{
    return m_divisor;
}

void Streamdata::setCapacity(int capacity)
{
    if (m_capacity == capacity)
        return;

    {
        QMutexLocker lock{&m_dataMutex};
        m_capacity = capacity;
        m_data.resize(m_capacity);
        setData(data());
    }
    emit capacityChanged(m_capacity);
}

void Streamdata::setData(QByteArray data)
{
    if (m_data == data)
        return;

    QMutexLocker lock{&m_dataMutex};
    m_data = data;
    emit dataChanged(m_data);
}

void Streamdata::setDivisor(int divisor)
{
    if (m_divisor == divisor)
        return;
    m_divisor = divisor;
    emit divisorChanged(m_divisor);
}

void Streamdata::consume(const char *ptr, int len)
{
    if(len > m_capacity) {
        // TODO: remove lots of uneccesary copying and locking
        QMutexLocker lock{&m_dataMutex};
        setCapacity(len);
    }
    auto spaceLeft = m_data.size()-m_writeIndex;
    auto spaceLeftDivisor = spaceLeft % m_divisor;
    spaceLeft -= spaceLeftDivisor;

    if(len > spaceLeft)
    {
        auto overhang = len-spaceLeft;
        memcpy(m_data.data() + m_writeIndex, ptr, static_cast<size_t>(spaceLeft));
        memcpy(m_data.data(), ptr + spaceLeft, static_cast<size_t>(overhang));
        m_writeIndex = overhang;
    }
    else
    {
        memcpy(m_data.data() + m_writeIndex, ptr, static_cast<size_t>(len));
        m_writeIndex += len;
    }
    emit dataChanged(m_data);
}
