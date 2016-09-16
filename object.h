#ifndef OBJECT_H
#define OBJECT_H

#include <QString>

/*
 * We are not able to inherit from both QObject and QWidget
 */
#define M_OBJECT \
    Q_PROPERTY(QString id MEMBER m_id WRITE setId) \
public: \
    Q_INVOKABLE QString className() const \
    { return QString(metaObject()->className()); } \
    Q_INVOKABLE QString id() const \
    { return m_id; } \
    void setId(const QString &id) \
    { QString lower = id.toLower(); m_id = lower; setObjectName(lower); } \
private: \
    QString m_id; \

#endif // OBJECT_H
