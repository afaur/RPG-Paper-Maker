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

#include "floors.h"

// -------------------------------------------------------
//
//
//  ---------- FLOORDATAS
//
//
// -------------------------------------------------------

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

FloorDatas::FloorDatas() :
    FloorDatas(new QRect)
{

}

FloorDatas::FloorDatas(QRect* texture) :
    m_textureRect(texture)
{

}

FloorDatas::~FloorDatas()
{
    delete m_textureRect;
}

QRect *FloorDatas::textureRect() const { return m_textureRect; }

MapEditorSubSelectionKind FloorDatas::getKind() const{
    return MapEditorSubSelectionKind::Floors;
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void FloorDatas::read(const QJsonObject & json){
    QJsonArray tab = json["t"].toArray();

    m_textureRect->setLeft(tab[0].toInt());
    m_textureRect->setTop(tab[1].toInt());
    m_textureRect->setWidth(tab[2].toInt());
    m_textureRect->setHeight(tab[3].toInt());
}

// -------------------------------------------------------

void FloorDatas::write(QJsonObject &json) const{
    QJsonArray tab;

    tab.append(m_textureRect->left());
    tab.append(m_textureRect->top());
    tab.append(m_textureRect->width());
    tab.append(m_textureRect->height());
    json["t"] = tab;
}

// -------------------------------------------------------
//
//
//  ---------- FLOOR
//
//
// -------------------------------------------------------

QVector3D Floor::verticesQuad[4]{
                                  QVector3D(0.0f, 0.0f, 0.0f),
                                  QVector3D(1.0f, 0.0f, 0.0f),
                                  QVector3D(1.0f, 0.0f, 1.0f),
                                  QVector3D(0.0f, 0.0f, 1.0f)
                                };

GLuint Floor::indexesQuad[6]{0, 1, 2, 0, 2, 3};

int Floor::nbVerticesQuad(4);

int Floor::nbIndexesQuad(6);

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

Floor::Floor() :
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_indexBuffer(QOpenGLBuffer::IndexBuffer),
    m_programStatic(nullptr)
{

}

Floor::~Floor()
{

}

// -------------------------------------------------------
//
//  GL
//
// -------------------------------------------------------

void Floor::clearGL(){
    m_vertices.clear();
    m_indexes.clear();
    m_count = 0;
}

// -------------------------------------------------------

void Floor::initializeVertices(int squareSize, int width, int height,
                               Position3D &p, FloorDatas *floor)
{
    QVector3D pos(p.x() * squareSize, 0.0f, p.z() * squareSize);
    QVector3D size(squareSize, 0.0, squareSize);

    float x = (float)(floor->textureRect()->x() * squareSize) / width;
    float y = (float)(floor->textureRect()->y() * squareSize) / height;
    float w = (float)(floor->textureRect()->width() * squareSize) / width;
    float h = (float)(floor->textureRect()->height() * squareSize) / height;

    // Vertices
    m_vertices.append(Vertex(Floor::verticesQuad[0] * size + pos,
                      QVector2D(x, y)));
    m_vertices.append(Vertex(Floor::verticesQuad[1] * size + pos,
                      QVector2D(x + w, y)));
    m_vertices.append(Vertex(Floor::verticesQuad[2] * size + pos,
                      QVector2D(x + w, y + h)));
    m_vertices.append(Vertex(Floor::verticesQuad[3] * size + pos,
                      QVector2D(x, y + h)));

    // indexes
    int offset = m_count * Floor::nbVerticesQuad;
    for (int i = 0; i < Floor::nbIndexesQuad; i++)
        m_indexes.append(Floor::indexesQuad[i] + offset);

    m_count++;
}

// -------------------------------------------------------

void Floor::initializeGL(QOpenGLShaderProgram *programStatic){
    if (m_programStatic == nullptr){
        initializeOpenGLFunctions();

        // Programs
        m_programStatic = programStatic;
    }
}

// -------------------------------------------------------

void Floor::updateGL(){

    // If existing VAO or VBO, destroy it
    if (m_vao.isCreated())
        m_vao.destroy();
    if (m_vertexBuffer.isCreated())
        m_vertexBuffer.destroy();
    if (m_indexBuffer.isCreated())
        m_indexBuffer.destroy();

    // Create new VBO for vertex
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.allocate(m_vertices.constData(), m_vertices.size() *
                            sizeof(Vertex));

    // Create new VBO for indexes
    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.allocate(m_indexes.constData(), m_indexes.size() *
                           sizeof(GLuint));

    // Create new VAO
    m_vao.create();
    m_vao.bind();
    m_programStatic->enableAttributeArray(0);
    m_programStatic->enableAttributeArray(1);
    m_programStatic->setAttributeBuffer(0, GL_FLOAT,
                                        Vertex::positionOffset(),
                                        Vertex::positionTupleSize,
                                        Vertex::stride());
    m_programStatic->setAttributeBuffer(1, GL_FLOAT,
                                        Vertex::texOffset(),
                                        Vertex::texCoupleSize,
                                        Vertex::stride());
    m_indexBuffer.bind();

    // Releases
    m_vao.release();
    m_indexBuffer.release();
    m_vertexBuffer.release();
}

// -------------------------------------------------------

void Floor::paintGL(){
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexes.size(), GL_UNSIGNED_INT, 0);
    m_vao.bind();
}

// -------------------------------------------------------
//
//
//  ---------- FLOORS
//
//
// -------------------------------------------------------

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

Floors::Floors() :
    m_programStatic(nullptr)
{
    for (int i = 0; i < Position::LAYERS_NUMBER; i++)
        m_floorsGL[i] = new Floor;
}

Floors::~Floors()
{
    QHash<Position, LandDatas*>::iterator i;
    for (i = m_lands.begin(); i != m_lands.end(); i++)
        delete i.value();

    for (int i = 0; i < Position::LAYERS_NUMBER; i++)
        delete m_floorsGL[i];
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

bool Floors::isEmpty() const{
    return m_lands.size() == 0;
}

// -------------------------------------------------------

LandDatas* Floors::getLand(Position& p){
    LandDatas* datas = m_lands.value(p);
    // TODO : autotiles

    return datas;
}

// -------------------------------------------------------

void Floors::setLand(Position& p, LandDatas *land){
    m_lands.insert(p, land);
}

// -------------------------------------------------------

LandDatas *Floors::removeLand(Position& p){
    LandDatas* land = m_lands.value(p);

    if (land != nullptr)
        m_lands.remove(p);

    return land;
}

// -------------------------------------------------------

bool Floors::addLand(Position& p, LandDatas *land){
    if (land->getKind() == MapEditorSubSelectionKind::Floors){
        LandDatas* previousLand = removeLand(p);

        if (previousLand != nullptr)
            delete previousLand;

        setLand(p, land);
    }

    return true;
}

// -------------------------------------------------------

bool Floors::deleteLand(Position& p){
    LandDatas* previousLand = removeLand(p);

    if (previousLand != nullptr)
        delete previousLand;

    return true;
}

// -------------------------------------------------------
//
//  GL
//
// -------------------------------------------------------

void Floors::initializeVertices(int squareSize, int width, int height){
    for (int j = 0; j < Position::LAYERS_NUMBER; j++)
        m_floorsGL[j]->clearGL();

    QHash<Position, LandDatas*>::iterator i;
    for (i = m_lands.begin(); i != m_lands.end(); i++){
        LandDatas* land = i.value();
        Position p = i.key();

        switch (land->getKind()){
        case MapEditorSubSelectionKind::Floors:
            m_floorsGL[p.layer()]->initializeVertices(squareSize, width, height,
                                                      p, (FloorDatas*) land);
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------------

void Floors::initializeGL(QOpenGLShaderProgram *programStatic){
    if (m_programStatic == nullptr){
        initializeOpenGLFunctions();

        m_programStatic = programStatic;
    }

    // Floors
    for (int i = 0; i < Position::LAYERS_NUMBER; i++)
        m_floorsGL[i]->initializeGL(programStatic);
}

// -------------------------------------------------------

void Floors::updateGL(){

    // Floors
    for (int i = 0; i < Position::LAYERS_NUMBER; i++)
        m_floorsGL[i]->updateGL();
}

// -------------------------------------------------------

void Floors::paintGL(){

    // Floors
    for (int i = 0; i < Position::LAYERS_NUMBER; i++)
        m_floorsGL[i]->paintGL();
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void Floors::read(const QJsonObject & json){
    QJsonArray tabFloors = json["floors"].toArray();

    // Floors
    for (int i = 0; i < tabFloors.size(); i++){
        QJsonObject obj = tabFloors.at(i).toObject();
        Position p;
        p.read(obj["k"].toArray());
        QJsonObject objLand = obj["v"].toObject();
        FloorDatas* floor = new FloorDatas;
        floor->read(objLand);
        m_lands[p] = floor;
    }
}

// -------------------------------------------------------

void Floors::write(QJsonObject & json) const{
    QJsonArray tabFloors;

    QHash<Position, LandDatas*>::const_iterator i;
    for (i = m_lands.begin(); i != m_lands.end(); i++){
        QJsonObject objHash;
        QJsonArray tabKey;
        i.key().write(tabKey);
        LandDatas* land = i.value();
        QJsonObject objLand;
        land->write(objLand);
        objHash["k"] = tabKey;
        objHash["v"] = objLand;

        switch (land->getKind()){
        case MapEditorSubSelectionKind::Floors:
            tabFloors.append(objHash);
            break;
        case MapEditorSubSelectionKind::Autotiles:
            break;
        case MapEditorSubSelectionKind::Water:
            break;
        default:
            break;
        }
    }
    json["floors"] = tabFloors;
}
