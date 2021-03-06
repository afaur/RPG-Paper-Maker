/*
    RPG Paper Maker Copyright (C) 2017 Marie Laporte

    This file is part of RPG Paper Maker.

    RPG Paper Maker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RPG Paper Maker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SYSTEMLANG_H
#define SYSTEMLANG_H

#include <QStandardItemModel>
#include <QMetaType>
#include "superlistitem.h"
#include "langstranslation.h"

// -------------------------------------------------------
//
//  CLASS SystemLang
//
//  A particulary lang (lang).
//
// -------------------------------------------------------

class SystemLang : public SuperListItem
{
public:
    SystemLang();
    SystemLang(int i, LangsTranslation* names);
    virtual ~SystemLang();
    LangsTranslation* names() const;
    virtual void setName(QString n);
    void setDefault();
    virtual bool openDialog();
    virtual SuperListItem* createCopy() const;
    virtual void setCopy(const SystemLang& item);

    virtual void read(const QJsonObject &json);
    virtual void write(QJsonObject &json) const;

protected:
    LangsTranslation* m_names;
};

Q_DECLARE_METATYPE(SystemLang)

#endif // SYSTEMLANG_H
