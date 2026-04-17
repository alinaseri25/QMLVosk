#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QPair>
#include <QString>

class GrammerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        DateTimeRole    = Qt::UserRole + 1,
        StringValueRole
    };

    explicit GrammerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addItem(const QString &dateTime, const QString &value);
    Q_INVOKABLE void clear();

    void loadFromJson(const QByteArray &jsonData);
    void loadFromJson(const QJsonDocument &jsonDoc);
    QString toJson() const;
    const QStringList &toList();

private:
    QList<QPair<QString, QString>> m_items;
    QStringList grammerList;
};
