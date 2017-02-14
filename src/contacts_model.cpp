/*
 * contacts_model.cpp
 *
 *  Created on: ۲۳ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <algorithm>
#include <QDebug>
#include <src/contacts_model.h>
#include <src/qpeerid.h>

contacts_model::contacts_model(QObject *parent): QAbstractListModel(parent)
{
    prop_map["name"] = role_t::Name;
    prop_map["phone_number"] = role_t::PhoneNumber;
    prop_map["contact_group"] = role_t::ContactGroup;
    prop_map["peer_id"] = role_t::PeerID;
    prop_map["profile_pic"] = role_t::ProfilePicture;

    QHash<int, QByteArray> roles;
    for (auto p = prop_map.begin(); p != prop_map.end(); ++p)
        roles[p->second] = p->first;
    setRoleNames(roles);
}

contacts_model::~contacts_model()
{
}

void contacts_model::add_contact(const contact &c)
{
    qDebug(__PRETTY_FUNCTION__);
    QList<contact>::iterator pos = std::upper_bound(m_contacts.begin(),
        m_contacts.end(), c);
    int idx = pos - m_contacts.begin();
    qDebug() << "Insert new contact at pos: " << idx;
    beginInsertRows(QModelIndex(), idx, idx);
    m_contacts.insert(pos, c);
    endInsertRows();
    emit countChanged();
}

Qt::ItemFlags contacts_model::flags(const QModelIndex &index) const
{
    qDebug(__PRETTY_FUNCTION__);
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int contacts_model::rowCount(const QModelIndex &/*parent*/) const
{
    qDebug(__PRETTY_FUNCTION__);
    return m_contacts.count();
}

QVariant contacts_model::data(const QModelIndex &index, int role) const
{
    qDebug(__PRETTY_FUNCTION__);
    if (index.row() < 0 || index.row() > m_contacts.count())
        return QVariant();

    const contact &c = m_contacts[index.row()];
    switch (role)
    {
        case role_t::Name:
            return c.name();
            break;
        case role_t::PhoneNumber:
            return c.phone_number();
            break;
        case role_t::ContactGroup:
            return c.contact_group();
            break;
        case role_t::PeerID:
            return QPeerId::mk_var(c.peer_id());
            break;
    }
    return QVariant();
}

bool contacts_model::setData(const QModelIndex &index, const QVariant &value,
    int role)
{
    qDebug(__PRETTY_FUNCTION__);
    // todo
    emit dataChanged(index, index);
    return false;
}

QVariant contacts_model::getProperty(int idx, QString prop)
{
    qDebug(__PRETTY_FUNCTION__);
    auto p = prop_map.find(prop.toAscii());
    if (p == prop_map.end())
    {
        qDebug() << "ERRRR, property not found: " << prop;
        return QVariant();
    }
    return data(createIndex(idx, 0), p->second);
}

void contacts_model::print()
{
    qDebug(__PRETTY_FUNCTION__);
    for (int i = 0; i < m_contacts.size(); ++i)
    {
        qDebug() << "Contact: " << m_contacts[i].contact_group() << ' '
                << m_contacts[i].name() << ' ' << m_contacts[i].phone_number();
    }
}
