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

#include "systemmapobject.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SystemMapObject::SystemMapObject() :
    SystemMapObject(1, "", Portion(0, 0, 0))
{

}

SystemMapObject::SystemMapObject(int i, QString n, Portion portion) :
    SuperListItem(i,n),
    m_portion(portion)
{

}

SystemMapObject::~SystemMapObject(){

}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void SystemMapObject::read(const QJsonObject &json){
    SuperListItem::read(json);

    m_portion.read(json["p"].toArray());
}

// -------------------------------------------------------

void SystemMapObject::write(QJsonObject &json) const{
    SuperListItem::write(json);

    QJsonArray tab;
    m_portion.write(tab);
    json["p"] = tab;
}
