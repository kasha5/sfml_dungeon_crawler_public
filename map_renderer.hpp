#pragma once

class MapRenderer : public sf::Drawable, public sf::Transformable
{
private:

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    void setTileTexture(sf::Vertex* quad)
    {
        int random = getRandomInRange(0, 100);

        if (random < 85) {
            quad[0].texCoords = sf::Vector2f(0, 0);
            quad[1].texCoords = sf::Vector2f(16, 0);
            quad[2].texCoords = sf::Vector2f(16, 16);
            quad[3].texCoords = sf::Vector2f(0, 16);
        }
        else if (random < 90) {
            quad[0].texCoords = sf::Vector2f(16, 0);
            quad[1].texCoords = sf::Vector2f(32, 0);
            quad[2].texCoords = sf::Vector2f(32, 16);
            quad[3].texCoords = sf::Vector2f(16, 16);
        }
        else if (random < 95) {
            quad[0].texCoords = sf::Vector2f(32, 0);
            quad[1].texCoords = sf::Vector2f(48, 0);
            quad[2].texCoords = sf::Vector2f(48, 16);
            quad[3].texCoords = sf::Vector2f(32, 16);
        }
        else {
            quad[0].texCoords = sf::Vector2f(48, 0);
            quad[1].texCoords = sf::Vector2f(64, 0);
            quad[2].texCoords = sf::Vector2f(64, 16);
            quad[3].texCoords = sf::Vector2f(48, 16);
        }
    }

public:

    bool load(const std::string& tileset, const sf::Vector2u tileSize, const std::vector<Room*> rooms, const std::vector<Corridor*> corridors)
    {
        // load the tileset texture
        if (!m_tileset.loadFromFile(tileset)) return false;

        m_vertices.setPrimitiveType(sf::Quads);

        int total_vertex_count = 0;
        int room_vertex_count = 0;

        // populate the vertex array, with one quad per tile
        for (const Room* room : rooms) {

            room_vertex_count = (room->width * room->height) * 4;
            total_vertex_count += room_vertex_count;
            m_vertices.resize(total_vertex_count);

            for (unsigned int i = room->x; i < room->x + room->width; i++) {
                for (unsigned int j = room->y; j < room->y + room->height; j++) {

                    sf::Vertex* quad = &m_vertices[(total_vertex_count - room_vertex_count) + (((i - room->x) + (j - room->y) * room->width) * 4)];

                    // define its 4 corners
                    quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                    quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                    quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                    quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                    setTileTexture(quad);
                }
            }
        }

        for (const Corridor* corridor : corridors) {

            room_vertex_count = (corridor->width * corridor->height) * 4;
            total_vertex_count += room_vertex_count;
            m_vertices.resize(total_vertex_count);

            for (unsigned int i = corridor->x1; i < corridor->x1 + corridor->width; i++) {
                for (unsigned int j = corridor->y1; j < corridor->y1 + corridor->height; j++) {
                    sf::Vertex* quad = &m_vertices[(total_vertex_count - room_vertex_count) + (((i - corridor->x1) + (j - corridor->y1) * corridor->width) * 4)];

                    quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                    quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                    quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                    quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                    setTileTexture(quad);
                }
            }
        }

        return true;
    }
};

class BackgroundRenderer : public sf::Drawable, public sf::Transformable
{
private:

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

public:

    bool load(const std::string& tileset, const sf::Vector2u tileSize, const unsigned int width, const unsigned int height)
    {
        // load the tileset texture
        if (!m_tileset.loadFromFile(tileset)) return false;

        m_vertices.setPrimitiveType(sf::Quads);

        m_vertices.resize(width * height * 4);

        for (unsigned int i = 0; i < width; i++) {
            for (unsigned int j = 0; j < height; j++) {
                sf::Vertex* quad = &m_vertices[((i + j * width) * 4)];

                quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                int random = getRandomInRange(0, 100);

                if (random < 85) {
                    quad[0].texCoords = sf::Vector2f(0, 0);
                    quad[1].texCoords = sf::Vector2f(16, 0);
                    quad[2].texCoords = sf::Vector2f(16, 16);
                    quad[3].texCoords = sf::Vector2f(0, 16);
                }
                else if (random < 95) {
                    quad[0].texCoords = sf::Vector2f(16, 0);
                    quad[1].texCoords = sf::Vector2f(32, 0);
                    quad[2].texCoords = sf::Vector2f(32, 16);
                    quad[3].texCoords = sf::Vector2f(16, 16);
                }
                else {
                    quad[0].texCoords = sf::Vector2f(32, 0);
                    quad[1].texCoords = sf::Vector2f(48, 0);
                    quad[2].texCoords = sf::Vector2f(48, 16);
                    quad[3].texCoords = sf::Vector2f(32, 16);
                }
            }
        }

        return true;
    }
};