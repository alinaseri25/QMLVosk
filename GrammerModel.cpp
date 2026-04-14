#include "GrammerModel.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

GrammerModel::GrammerModel(QObject *parent)
    : QAbstractListModel(parent) {}

int GrammerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant GrammerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size()) return {};
    if (role == DateTimeRole)    return m_items[index.row()].first;
    if (role == StringValueRole) return m_items[index.row()].second;
    return {};
}

QHash<int, QByteArray> GrammerModel::roleNames() const
{
    return {
        { DateTimeRole,    "dateTime"    },
        { StringValueRole, "stringValue" }
    };
}

void GrammerModel::addItem(const QString &dateTime, const QString &value)
{
    beginInsertRows({}, m_items.size(), m_items.size());
    m_items.append({ dateTime, value });
    endInsertRows();
}

void GrammerModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

void GrammerModel::loadFromJson(const QByteArray &jsonData)
{
    QJsonArray arr = QJsonDocument::fromJson(jsonData).array();

    beginResetModel();
    m_items.clear();
    for (const QJsonValue &val : arr) {
        QJsonObject obj = val.toObject();
        m_items.append({ obj["dateTime"].toString(), obj["stringValue"].toString() });
    }
    endResetModel();
}

QString GrammerModel::toJson() const
{
    QJsonArray arr;
    for (const auto &item : m_items) {
        QJsonObject obj;
        obj["dateTime"]    = item.first;
        obj["stringValue"] = item.second;
        arr.append(obj);
    }
    return QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
}

const QStringList &GrammerModel::toList()
{
    for (const auto &item : m_items){
        grammerList.append(item.second);
    }
    return grammerList;
}
