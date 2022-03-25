#include "Map.hpp"
#include <windows.h>

Map::Map(const std::string &path, const sf::RenderWindow &window)
{
    constructorCommons(window);

    // getFileContent(path);
    // generateCellmap();
    // generateVertexmap();
}

Map::Map(const sf::RenderWindow &window)
{
    constructorCommons(window);
}

void Map::constructorCommons(const sf::RenderWindow &window)
{
    _caseSize = 64.0f;
    _windowSize = window.getSize();
    _cellmapWidth = static_cast<int>(std::ceil(_windowSize.x / _caseSize));
    _cellmapSize = static_cast<int>(_cellmapWidth * std::ceil(_windowSize.y / _caseSize));
    _cellmap = new cell_t[_cellmapSize];
    sf::Vector2f position = {0.0f, 0.0f};
    for (int i = 0; i < _cellmapSize; i++) {
        if (position.x > _windowSize.x - _caseSize) {
            position.y += _caseSize;
            position.x = 0.0f;
        }
        _cellmap[i].exist = true;
        _cellmap[i].rect = sf::RectangleShape(sf::Vector2f{_caseSize, _caseSize});
        _cellmap[i].rect.setPosition(position);
        _cellmap[i].rect.setFillColor(sf::Color::Blue);
        for (int j = 0; j < 4; j++) _cellmap[i].isEdge[j] = false;
        for (int j = 0; j < 4; j++) _cellmap[i].edgeId[j] = false;
        position.x += _caseSize;
    }
    _filemap = nullptr;

    float radius = 3.0f;
    _vertice.setRadius(radius);
    _vertice.setOrigin(sf::Vector2f{radius, radius});
    _vertice.setFillColor(sf::Color::Red);

    _line = sf::VertexArray(sf::LinesStrip, 2);
    _line[0].color = sf::Color::White;
    _line[1].color = sf::Color::White;
}

Map::~Map()
{
    // if (_filemap)
    //     delete[] _filemap;
}

int Map::getCellId(const sf::Vector2f &position) const
{
    if (position.x > _windowSize.x || position.x < 0.0f || position.y > _windowSize.y || position.y < 0.0f) return -1;
    return static_cast<int>(std::floor(position.x / _caseSize) + std::floor(position.y / _caseSize) * std::floor(_windowSize.x / _caseSize));
}

void Map::generateCellmap()
{
    sf::Vector2f position = {0.0f, 0.0f};
    for (int i = 0, cellmapIdx; _filemap[i] != '\0'; i++) {
        cellmapIdx = getCellId(position);
        if (_filemap[i] == '#') _cellmap[cellmapIdx].exist = true;
        if (_filemap[i] == '\n') {
            position.x = 0.0f;
            position.y += _caseSize;
        } else position.x += _caseSize;
    }
}

void Map::generateVertexmap()
{
    _vertexmap.clear();

    sf::Vector2f position;
    int cellId = 0;
    int key = 0;
    for (int i = 0; i < _cellmapSize; i++) {
        for (int j = 0; j < 4; j++) _cellmap[i].isEdge[j] = false;
        for (int j = 0; j < 4; j++) _cellmap[i].edgeId[j] = false;
        if (_cellmap[i].exist) {
            position = _cellmap[i].rect.getPosition();
            if (getCellId(sf::Vector2f{position.x + _caseSize, position.y}) == -1 || !_cellmap[i + 1].exist) {
                if (position.y > 0 && _cellmap[i - _cellmapWidth].isEdge[RIGHT]) {
                    _vertexmap[_cellmap[i - _cellmapWidth].edgeId[RIGHT]].B.y += _caseSize;
                    _cellmap[i].isEdge[RIGHT] = true;
                    _cellmap[i].edgeId[RIGHT] = _cellmap[i - _cellmapWidth].edgeId[RIGHT];
                }
                else {
                    _vertexmap.insert(std::make_pair(key, line_t{sf::Vector2f{position.x + _caseSize, position.y}, sf::Vector2f{position.x + _caseSize, position.y + _caseSize}}));
                    _cellmap[i].isEdge[RIGHT] = true;
                    _cellmap[i].edgeId[RIGHT] = key;
                    key++;
                }
            }
            if (getCellId(sf::Vector2f{position.x - _caseSize, position.y}) == -1 || !_cellmap[i - 1].exist) {
                if (position.y > 0 && _cellmap[i - _cellmapWidth].isEdge[LEFT]) {
                    _vertexmap[_cellmap[i - _cellmapWidth].edgeId[LEFT]].B.y += _caseSize;
                    _cellmap[i].isEdge[LEFT] = true;
                    _cellmap[i].edgeId[LEFT] = _cellmap[i - _cellmapWidth].edgeId[LEFT];
                }
                else {
                    _vertexmap.insert(std::make_pair(key, line_t{sf::Vector2f{position.x, position.y}, sf::Vector2f{position.x, position.y + _caseSize}}));
                    _cellmap[i].isEdge[LEFT] = true;
                    _cellmap[i].edgeId[LEFT] = key;
                    key++;
                }
            }
            if (getCellId(sf::Vector2f{position.x, position.y - _caseSize}) == -1 || !_cellmap[i - _cellmapWidth].exist) {
                if (position.x > 0 && _cellmap[i - 1].isEdge[TOP]) {
                    _vertexmap[_cellmap[i - 1].edgeId[TOP]].B.x += _caseSize;
                    _cellmap[i].isEdge[TOP] = true;
                    _cellmap[i].edgeId[TOP] = _cellmap[i - 1].edgeId[TOP];
                }
                else {
                    _vertexmap.insert(std::make_pair(key, line_t{sf::Vector2f{position.x, position.y}, sf::Vector2f{position.x + _caseSize, position.y}}));
                    _cellmap[i].isEdge[TOP] = true;
                    _cellmap[i].edgeId[TOP] = key;
                    key++;
                }
            }
            if (getCellId(sf::Vector2f{position.x, position.y + _caseSize}) == -1 || !_cellmap[i + _cellmapWidth].exist) {
                if (position.x > 0 && _cellmap[i - 1].isEdge[BOTTOM]) {
                    _vertexmap[_cellmap[i - 1].edgeId[BOTTOM]].B.x += _caseSize;
                    _cellmap[i].isEdge[BOTTOM] = true;
                    _cellmap[i].edgeId[BOTTOM] = _cellmap[i - 1].edgeId[BOTTOM];
                }
                else {
                    _vertexmap.insert(std::make_pair(key, line_t{sf::Vector2f{position.x, position.y + _caseSize}, sf::Vector2f{position.x + _caseSize, position.y + _caseSize}}));
                    _cellmap[i].isEdge[BOTTOM] = true;
                    _cellmap[i].edgeId[BOTTOM] = key;
                    key++;
                }
            }
        }
    }
}

void Map::getFileContent(const std::string& path)
{
    std::ifstream file(path);
    if (file.fail())
        throw "Map path does not exist.";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    _filemap = new char[content.size() + 1];
    _filemap = std::strcpy(_filemap, content.c_str());
}

void Map::event(const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        _cellmap[getCellId(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))].exist = !_cellmap[getCellId(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))].exist;
        generateVertexmap();
    }
}

void Map::drawVertexmap(sf::RenderWindow &window)
{
    for (auto it : _vertexmap) {
        _line[0].position = sf::Vector2f{it.second.A.x, it.second.A.y};
        _line[1].position = sf::Vector2f{it.second.B.x, it.second.B.y};
        window.draw(_line);
        _vertice.setPosition(sf::Vector2f{it.second.A.x, it.second.A.y});
        window.draw(_vertice);
        _vertice.setPosition(sf::Vector2f{it.second.B.x, it.second.B.y});
        window.draw(_vertice);
    }
}

void Map::draw(sf::RenderWindow &window)
{
    for (int i = 0; i < _cellmapSize; i++) {
        if (_cellmap[i].exist) window.draw(_cellmap[i].rect);
    }
    drawVertexmap(window);
}