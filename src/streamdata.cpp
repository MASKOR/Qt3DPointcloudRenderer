#include "streamdata.h"
#include <QMutexLocker>
#include <iostream>
#include <cmath>

Streamdata::Streamdata(QObject *parent)
    : QObject(parent)
    , m_capacity(1)
    , m_data()//m_capacity, Qt::Uninitialized)
    , m_writeIndex(0)
    , m_readIndex(0)
    , m_divisor(24)
    , m_isEmpty(true)
    , m_dataMutex(QMutex::Recursive)
{

}

int Streamdata::capacity() const
{
    return m_capacity;
}

QByteArray Streamdata::data()
{
    QMutexLocker lock{&m_dataMutex};
    // deep copy
    return QByteArray(m_data.data(), m_data.size());
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
        emit dataChanged();
    }
    emit capacityChanged(m_capacity);
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
    QMutexLocker lock{&m_dataMutex};
    if(len > m_capacity || len > m_data.size()) {
        // TODO: remove lots of uneccesary copying and locking
        setCapacity(len);
    }
    auto spaceLeft = static_cast<int>(m_data.size())-m_writeIndex;
    auto spaceLeftDivisor = spaceLeft % m_divisor;
    spaceLeft -= spaceLeftDivisor;

    if(len > spaceLeft)
    {
        auto overhang = len-spaceLeft;
        //m_data.replace(m_writeIndex, spaceLeft, ptr);
        //m_data.replace(0, overhang, ptr + spaceLeft);
        memcpy(m_data.data() + m_writeIndex, ptr, static_cast<size_t>(spaceLeft));
        memcpy(m_data.data(), ptr + spaceLeft, static_cast<size_t>(overhang));
        m_writeIndex = overhang;

    }
    else
    {
        //m_data.replace(m_writeIndex, len, ptr);
        memcpy(m_data.data() + m_writeIndex, ptr, static_cast<size_t>(len));
        if(len == spaceLeft)
        {
            m_writeIndex = 0;
        }
        else
        {
            m_writeIndex += len;
        }
    }
    emit dataChanged();
}
