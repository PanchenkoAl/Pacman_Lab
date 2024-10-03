// Packman_Lab_01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <math.h>
#include <stack>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <queue>

const int W_WIDTH = 840;
const int W_HEIGHT = 930;
const int CELL_SIZE = 30;
const int PACMAN_SIZE = 11;
const float PACMAN_SPEED = 4.5;
const float GHOST_SPEED = 3;
const char EMPTY = ' ';
const char WALL = '#';
const char START = 'S';
const char EXIT = 'E';
const char FOOD = '.';
int MAX_FPS = 30; 

class Pos
{
public:
    float x;
    float y;

    Pos()
    {

    }

    Pos(int m_x, int m_y):
        x(m_x),
        y(m_y)
    {}

    sf::RectangleShape toShape(sf::Color color)
    {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(CELL_SIZE / 2, CELL_SIZE / 2));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        return shape;
    }

    int calculateDistance(Pos position)
    {
        return abs(x - position.x) / CELL_SIZE + abs(y - position.y) / CELL_SIZE;
    }

    int locateSide(Pos position)
    {
        if (position.x <= x && position.y <= y)
        {
            return 14;
        }
        if (position.x >= x && position.y <= y)
        {
            return 12;
        }
        if (position.x <= x && position.y >= y)
        {
            return 34;
        }
        if (position.x >= x && position.y >= y)
        {
            return 32;
        }
    }
};

class Node
{
    Pos m_position;
    bool m_inOpen;
    bool m_inClosed;
    bool m_isObstacle;
    int m_g;
    int m_parentG;
    Pos m_parentPosition;

public:
    Node(Pos position, bool inOpen, bool inClosed, int g, Pos parentPosition) :
        m_position(position),
        m_inOpen(inOpen),
        m_inClosed(inClosed),
        m_g(g),
        m_parentPosition(parentPosition)
    {}

    Pos getPosition()
    {
        return m_position;
    }

    void setObstacle(bool obs)
    {
        m_isObstacle = obs;
    }

    bool getObstacle()
    {
        return m_isObstacle;
    }

    int getG()
    {
        return m_g;
    }

    void setParentG(int g)
    {
        m_parentG = g;
    }

    void setOpen(bool open)
    {
        m_inOpen = open;
    }

    void setClosed(bool closed)
    {
        m_inClosed = closed;
    }

    bool getOpen()
    {
        return m_inOpen;
    }

    bool getClosed()
    {
        return m_inClosed;
    }

    void setParent(Pos position)
    {
        m_parentPosition = position;
    }

    Pos getParent()
    {
        return m_parentPosition;
    }

    float calculateH(Node destination)
    {
        return abs((m_position.x - destination.getPosition().x) / CELL_SIZE) + abs((m_position.y - destination.getPosition().y) / CELL_SIZE);
    }

    float calculateF(Node destination)
    {
        if (m_parentPosition.x == -1 || m_parentPosition.y == -1)
            return 0;
        return calculateH(destination) + m_parentG + 1;
    }

    sf::RectangleShape toShape()
    {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        shape.setPosition(m_position.x, m_position.y);
        shape.setFillColor(sf::Color::Green);
        return shape;
    }

    sf::RectangleShape toShape(int color)
    {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        shape.setPosition(m_position.x, m_position.y);
        shape.setFillColor(sf::Color::Red);
        return shape;
    }

    Pos findClosestValid(std::vector<std::vector<Node>> map)
    {
        Pos currentPos = m_position;
        std::vector<Pos> toCheck;
        toCheck.push_back(currentPos);

        while (map[currentPos.y / CELL_SIZE][currentPos.x / CELL_SIZE].getObstacle())
        {
            if (currentPos.y / CELL_SIZE > 0)
            {
                toCheck.push_back(Pos(currentPos.x, currentPos.y - CELL_SIZE));
            }
            if (currentPos.x / CELL_SIZE < W_WIDTH / CELL_SIZE)
            {
                toCheck.push_back(Pos(currentPos.x + CELL_SIZE, currentPos.y));
            }
            if (currentPos.y / CELL_SIZE < W_HEIGHT / CELL_SIZE)
            {
                toCheck.push_back(Pos(currentPos.x, currentPos.y + CELL_SIZE));
            }
            if (currentPos.x / CELL_SIZE > 0)
            {
                toCheck.push_back(Pos(currentPos.x - CELL_SIZE, currentPos.y));
            }

            currentPos = toCheck[1];
            toCheck.erase(toCheck.begin());
        }

        return currentPos;  
    }
};

class IDAStarNode {
    Pos m_position;     
    int m_g;        
    int m_f;        

public:
    IDAStarNode(Pos position, int g, int f) :
        m_position(position),
        m_g(g),
        m_f(f)
    {}

    Pos getPosition()
    {
        return m_position;
    }

    int getF()
    {
        return m_f;
    }

    void setF(int f)
    {
        m_f = f;
    }

    void setG(int g)
    {
        m_g = g;
    }

    int getG()
    {
        return m_g;
    }

    IDAStarNode(Node node)
    {
        m_position = Pos(node.getPosition().x / CELL_SIZE, node.getPosition().y / CELL_SIZE);
        m_g = 0, m_f = 0;
    }
};

class WaveNode
{
    Pos m_position;
    int m_isObstacle;
    int m_d;
    bool m_isMarked;

public:
    WaveNode(Pos position, bool obst, int d, int isMarked) :
        m_position(position),
        m_isObstacle(obst),
        m_d(d),
        m_isMarked(isMarked)
    {}

    WaveNode()
    {}


    bool isMarked()
    {
        return m_isMarked;
    }

    void setD(int d)
    {
        m_d = d;
    }

    int getD()
    {
        return m_d;
    }

    void setMarked(bool marked)
    {
        m_isMarked = marked;
    }

    Pos getPosition()
    {
        return m_position;  
    }

    void setObstacle(bool obst)
    {
        m_isObstacle = obst;
    }

    bool isObstacle()
    {
        return m_isObstacle;
    }

    WaveNode(Node node)
    {
        m_position = node.getPosition();
        m_isObstacle = node.getObstacle();
        m_d = -2;
        m_isMarked = false;
    }
};

class Cell
{
    int width;
    int height;
    bool empty;
    Pos position;

public:
    Cell(int m_width, int m_height, bool m_empty, Pos m_position) : 
        width(m_width), 
        height(m_height), 
        empty(m_empty),
        position(m_position) 
    {}

    bool isWall()
    {
        return !empty;
    }

    Node toNode()
    { 
        return Node(position, false, false, 0, Pos(-1, -1));
    }


    sf::RectangleShape cellToShape()
    {
        sf::RectangleShape shape;
        if(!empty)
        {
            shape.setSize(sf::Vector2f(width, height));
            shape.setPosition(position.x, position.y);
            shape.setFillColor(sf::Color::Blue);
        }   
        return shape;
    }
};

class Pacman
{
    Pos position;
    int m_direction = 0;

public:
    Pacman(Pos m_position): 
        position(m_position)
    {}

    sf::CircleShape toShape()
    {
        sf::CircleShape shape(PACMAN_SIZE);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(sf::Vector2f(PACMAN_SIZE, PACMAN_SIZE));
        shape.setPosition(position.x, position.y);
        return shape;
    }

    Cell locateCell(const std::vector<Cell>& cells)
    {
        int w_i = position.x / CELL_SIZE;
        int h_i = position.y / CELL_SIZE;

        return cells[h_i * (W_WIDTH / CELL_SIZE) + w_i];
    }

    Cell locateCell(float p_x, float p_y, const std::vector<Cell>& cells)
    {
        int w_i = p_x / CELL_SIZE;
        int h_i = p_y / CELL_SIZE;

        return cells[h_i * (W_WIDTH / CELL_SIZE) + w_i];
    }

    int getDirection()
    {
        return m_direction;
    }

    bool collision(int direction, const std::vector<Cell>& cells)
    {
        m_direction = direction;
        switch (direction)
        {
        case 1:
            return locateCell(position.x + PACMAN_SIZE + PACMAN_SPEED, position.y, cells).isWall() //RIGHT
                || locateCell(position.x + PACMAN_SIZE + PACMAN_SPEED, position.y - PACMAN_SIZE, cells).isWall()
                || locateCell(position.x + PACMAN_SIZE + PACMAN_SPEED, position.y + PACMAN_SIZE, cells).isWall();
            break;
        case 2:
            return locateCell(position.x, position.y - PACMAN_SIZE - PACMAN_SPEED, cells).isWall() //UP
                || locateCell(position.x - PACMAN_SIZE, position.y - PACMAN_SIZE - PACMAN_SPEED, cells).isWall()
                || locateCell(position.x + PACMAN_SIZE, position.y - PACMAN_SIZE - PACMAN_SPEED, cells).isWall();
            break;
        case 3:
            return locateCell(position.x - PACMAN_SIZE - PACMAN_SPEED, position.y, cells).isWall() //LEFT
                || locateCell(position.x - PACMAN_SIZE - PACMAN_SPEED, position.y - PACMAN_SIZE, cells).isWall()
                || locateCell(position.x - PACMAN_SIZE - PACMAN_SPEED, position.y + PACMAN_SIZE, cells).isWall();
            break;
        case 4:
            return locateCell(position.x, position.y + PACMAN_SIZE + PACMAN_SPEED, cells).isWall() //DOWN
                || locateCell(position.x - PACMAN_SIZE, position.y + PACMAN_SIZE + PACMAN_SPEED, cells).isWall()
                || locateCell(position.x + PACMAN_SIZE, position.y + PACMAN_SIZE + PACMAN_SPEED, cells).isWall();
            break;
        }
    }

    Pos getPosition()
    {
        return position;
    }

    void update(const std::vector<Cell>& cells)
    {
        if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if (position.x < W_WIDTH - PACMAN_SIZE && !collision(1, cells))
                position.x += PACMAN_SPEED;
        }

        if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            if (position.y > PACMAN_SIZE && !collision(2, cells))
                position.y -= PACMAN_SPEED;
        }

        if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            if(position.x > PACMAN_SIZE && !collision(3, cells))
                position.x -= PACMAN_SPEED;
        }

        if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            if (position.y < W_HEIGHT - PACMAN_SIZE && !collision(4, cells))
                position.y += PACMAN_SPEED;
        }
    }
};

class Dot
{
    Pos m_position;
    int m_points;
    bool m_isAlive;
public:
    Dot(Pos position, int points) :
        m_position(position),
        m_points(points)
    {
        m_isAlive = true;
    }

    void setDead()
    {
        m_isAlive = false;
    }

    bool isALive()
    {
        return m_isAlive;
    }

    void checkPacman(Pacman pacman, int& score)
    {
        if (abs(pacman.getPosition().x - m_position.x) < 15 && abs(pacman.getPosition().y - m_position.y) < 15 && m_isAlive)
        {
            score += 10;
            setDead();
        }
    }

    sf::CircleShape toShape()
    {
        sf::CircleShape shape(PACMAN_SIZE / 3);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(sf::Vector2f(0, 0));
        shape.setPosition(m_position.x - PACMAN_SIZE / 3, m_position.y - PACMAN_SIZE / 3);
        return shape;
    }
};

class Ghost
{

    Pos m_position;
    int m_direction = 0;
public:

    Ghost (Pos position):
        m_position(position)
    {}

    Pos getPosition()
    {
        return m_position;
    }

    sf::RectangleShape toShape(int color)
    {
        /*sf::CircleShape shape(PACMAN_SIZE);
        shape.setFillColor(sf::Color::Magenta);
        shape.setOrigin(sf::Vector2f(0, 0));
        shape.setPosition(m_position.x, m_position.y);*/

        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(CELL_SIZE - 6, CELL_SIZE - 6));
        shape.setPosition(m_position.x, m_position.y);
        switch (color)
        {
        case 1:
            shape.setFillColor(sf::Color(255, 0, 0));
            break;
        case 2:
            shape.setFillColor(sf::Color(255, 192, 203));
            break;
        case 3:
            shape.setFillColor(sf::Color(0, 255, 255));
            break;
        case 4:
            shape.setFillColor(sf::Color(255, 184, 82));
            break;
        }


        return shape;
    }

    void update(Pos target, int frame) 
    {
        int mgpx = m_position.x / CELL_SIZE;
        int mgpy = m_position.y / CELL_SIZE;
        int mtpx = target.x / CELL_SIZE;
        int mtpy = target.y / CELL_SIZE;

        if (frame == 1)
        {
            if (mgpx < mtpx)
            {
                m_direction = 2;
            }
            else if (mgpx > mtpx)
            {
                m_direction = 4;
            }
            else if (mgpy < mtpy)
            {
                m_direction = 3;
            }
            else if (mgpy > mtpy)
            {
                m_direction = 1;
            }
        }
        move(m_direction);
    }

    void move(int direction)
    {
        switch (direction)
        {
        case 1:
            m_position.y -= GHOST_SPEED;
            break;
        case 2:
            m_position.x += GHOST_SPEED;
            break;
        case 3:
            m_position.y += GHOST_SPEED;
            break;
        case 4:
            m_position.x -= GHOST_SPEED;
            break;
        default:
            break;
        }
    }
};

class GhostManager
{
    std::array<Ghost, 4> m_ghosts;
    std::array<std::vector<Pos>, 4> m_paths;
    std::array<bool, 4> m_updFlags = {true, true, true, true};
    std::array<int, 4> m_iterators = { 0,0,0,0 };

public: 
    GhostManager(std::array<Ghost, 4> ghosts) :
        m_ghosts(ghosts)
    {}

    void draw(sf::RenderWindow& i_window)
    {
        for (int i = 0; i < m_ghosts.size(); i++)
        {
            i_window.draw(m_ghosts[i].toShape(i + 1));
        }
    }

    void draw(sf::RenderWindow& i_window, std::vector<Pos> path, sf::Color color)
    {
        for (int i = 0; i < path.size(); i++)
        {
            i_window.draw(path[i].toShape(color));
        }
    }

    int findLowestF(std::vector<Node>& openList, Node dest)
    {
        int minF = openList[0].calculateF(dest);
        int index = 0;

        for (size_t i = 0; i < openList.size() - 1; i++)
            if (openList[i + 1].calculateF(dest) < openList[i].calculateF(dest))
            {
                index = i + 1; minF = openList[i + 1].calculateF(dest);
            }

        return index;
    }

    std::vector<Pos> aStar(std::vector<std::vector<Node>> map, const Node& start, Node destination)
    {
        std::vector<Node> closedList;
        std::vector<Node> openList;
        std::vector<Pos> path;

        openList.push_back(start);


        while (openList.size() > 0)
        {
            float lowestF = findLowestF(openList, destination);
            Node currentNode = openList[lowestF];

            if (currentNode.getPosition().x == destination.getPosition().x
                && currentNode.getPosition().y == destination.getPosition().y)
            {
                path.push_back(currentNode.getPosition());
                while (currentNode.getParent().x != -1 && currentNode.getParent().y != -1)
                {
                    Pos parent = currentNode.getParent();
                    path.push_back(parent);
                    currentNode = map[parent.y / CELL_SIZE][parent.x / CELL_SIZE];
                }
                return path;
            }

            if (currentNode.getPosition().y / CELL_SIZE > 0) //UP
            {
                Node neibNode = map[(currentNode.getPosition().y / CELL_SIZE) - 1][currentNode.getPosition().x / CELL_SIZE];

                if (!neibNode.getClosed() && !neibNode.getObstacle())
                {
                    if (!neibNode.getOpen() || neibNode.calculateF(destination) < lowestF)
                    {
                        neibNode.setParent(currentNode.getPosition());
                        if (!neibNode.getOpen())
                        {
                            neibNode.setOpen(true);
                            openList.push_back(neibNode);
                        }
                    }
                }
                map[(currentNode.getPosition().y / CELL_SIZE) - 1][currentNode.getPosition().x / CELL_SIZE] = neibNode;
            }

            if (currentNode.getPosition().x / CELL_SIZE < W_WIDTH / CELL_SIZE - 1) //RIGHT
            {
                Node neibNode = map[(currentNode.getPosition().y / CELL_SIZE)][currentNode.getPosition().x / CELL_SIZE + 1];

                if (!neibNode.getClosed() && !neibNode.getObstacle())
                {
                    if (!neibNode.getOpen() || neibNode.calculateF(destination) < lowestF)
                    {
                        neibNode.setParent(currentNode.getPosition());
                        if (!neibNode.getOpen())
                        {
                            neibNode.setOpen(true);
                            openList.push_back(neibNode);
                        }
                    }
                }
                map[(currentNode.getPosition().y / CELL_SIZE)][currentNode.getPosition().x / CELL_SIZE + 1] = neibNode;
            }

            if (currentNode.getPosition().y / CELL_SIZE < W_HEIGHT / CELL_SIZE - 1) //DOWN
            {
                Node neibNode = map[(currentNode.getPosition().y / CELL_SIZE) + 1][currentNode.getPosition().x / CELL_SIZE];

                if (!neibNode.getClosed() && !neibNode.getObstacle())
                {
                    if (!neibNode.getOpen() || neibNode.calculateF(destination) < lowestF)
                    {
                        neibNode.setParent(currentNode.getPosition());
                        if (!neibNode.getOpen())
                        {
                            neibNode.setOpen(true);
                            openList.push_back(neibNode);
                        }
                    }
                }
                map[(currentNode.getPosition().y / CELL_SIZE) + 1][currentNode.getPosition().x / CELL_SIZE] = neibNode;
            }

            if (currentNode.getPosition().x / CELL_SIZE > 0) //LEFT
            {
                Node neibNode = map[(currentNode.getPosition().y / CELL_SIZE)][currentNode.getPosition().x / CELL_SIZE - 1];

                if (!neibNode.getClosed() && !neibNode.getObstacle())
                {
                    if (!neibNode.getOpen() || neibNode.calculateF(destination) < lowestF)
                    {
                        neibNode.setParent(currentNode.getPosition());
                        if (!neibNode.getOpen())
                        {
                            neibNode.setOpen(true);
                            openList.push_back(neibNode);
                        }
                    }
                }
                map[(currentNode.getPosition().y / CELL_SIZE)][currentNode.getPosition().x / CELL_SIZE - 1] = neibNode;
            }
            /////////////

            closedList.push_back(currentNode);
            currentNode.setOpen(false);
            currentNode.setClosed(true);
            openList.erase(openList.begin() + lowestF);
            map[(currentNode.getPosition().y / CELL_SIZE)][currentNode.getPosition().x / CELL_SIZE] = currentNode;
        }

        return path;
    }

    std::vector<Pos> waveLee(std::vector <std::vector<Node>> mapN, WaveNode start, WaveNode finish)
    {
        std::vector<Pos> path;
        std::vector<WaveNode> list;
        std::vector <std::vector<WaveNode>> map;
        std::vector<std::vector<int>> dArr;

        for (int i = 0; i < mapN.size(); i++)
        {
            std::vector<int> temp;
            for (int j = 0; j < mapN[0].size(); j++)
            {
                temp.push_back(0);
            }
            dArr.push_back(temp);
        }

        for (int i = 0; i < mapN.size(); i++)
        {
            std::vector<WaveNode> temp;
            for (int j = 0; j < mapN[0].size(); j++)
            {
                temp.push_back(WaveNode(mapN[i][j]));
            }
            map.push_back(temp);
        }

        start.setD(0);
        start.setMarked(true);
        list.push_back(start);

        WaveNode currentNode = WaveNode();

        while ((list.size() > 0)
            && ((currentNode.getPosition().x / CELL_SIZE != finish.getPosition().x / CELL_SIZE)
                || (currentNode.getPosition().y / CELL_SIZE != finish.getPosition().y / CELL_SIZE)))
        {
            currentNode = list[0];

            if (currentNode.getPosition().y / CELL_SIZE > 0)
            {
                WaveNode neibNode = map[currentNode.getPosition().y / CELL_SIZE - 1][currentNode.getPosition().x / CELL_SIZE];
                if (!neibNode.isMarked() && !neibNode.isObstacle())
                {
                    neibNode.setMarked(true);
                    neibNode.setD(currentNode.getD() + 1);
                    dArr[currentNode.getPosition().y / CELL_SIZE - 1][currentNode.getPosition().x / CELL_SIZE] = currentNode.getD() + 1;
                    list.push_back(neibNode);
                }
                map[currentNode.getPosition().y / CELL_SIZE - 1][currentNode.getPosition().x / CELL_SIZE] = neibNode;
            }

            if (currentNode.getPosition().x / CELL_SIZE < W_WIDTH / CELL_SIZE - 1)
            {
                WaveNode neibNode = map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE + 1];
                if (!neibNode.isMarked() && !neibNode.isObstacle())
                {
                    neibNode.setMarked(true);
                    neibNode.setD(currentNode.getD() + 1);
                    dArr[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE + 1] = currentNode.getD() + 1;
                    list.push_back(neibNode);
                }
                map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE + 1] = neibNode;
            }

            if (currentNode.getPosition().y / CELL_SIZE < W_HEIGHT / CELL_SIZE - 1)
            {
                WaveNode neibNode = map[currentNode.getPosition().y / CELL_SIZE + 1][currentNode.getPosition().x / CELL_SIZE];
                if (!neibNode.isMarked() && !neibNode.isObstacle())
                {
                    neibNode.setMarked(true);
                    neibNode.setD(currentNode.getD() + 1);
                    dArr[currentNode.getPosition().y / CELL_SIZE + 1][currentNode.getPosition().x / CELL_SIZE] = currentNode.getD() + 1;
                    list.push_back(neibNode);
                }
                map[currentNode.getPosition().y / CELL_SIZE + 1][currentNode.getPosition().x / CELL_SIZE] = neibNode;
            }

            if (currentNode.getPosition().x / CELL_SIZE > 0)
            {
                WaveNode neibNode = map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE - 1];
                if (!neibNode.isMarked() && !neibNode.isObstacle())
                {
                    neibNode.setMarked(true);
                    neibNode.setD(currentNode.getD() + 1);
                    dArr[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE - 1] = currentNode.getD() + 1;
                    list.push_back(neibNode);
                }
                map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE - 1] = neibNode;
            }

            map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE] = currentNode;
            list.erase(list.begin());
        }

        path.push_back(currentNode.getPosition());

       /* for (int i = 0; i < mapN.size(); i++)
        {
            for (int j = 0; j < mapN[0].size(); j++)
            {
                std::cout << dArr[i][j] << " ";
            }
            std::cout << std::endl;
        }*/

        while (currentNode.getD() > 0)
        {
            if (currentNode.getPosition().y / CELL_SIZE > 0)
            {
                if (map[currentNode.getPosition().y / CELL_SIZE - 1][currentNode.getPosition().x / CELL_SIZE].getD() == currentNode.getD() - 1)
                {
                    currentNode = map[currentNode.getPosition().y / CELL_SIZE - 1][currentNode.getPosition().x / CELL_SIZE];
                    path.push_back(currentNode.getPosition());
                    continue;
                }
            }

            if (currentNode.getPosition().x / CELL_SIZE < W_WIDTH / CELL_SIZE - 1)
            {
                if (map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE + 1].getD() == currentNode.getD() - 1)
                {
                    currentNode = map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE + 1];
                    path.push_back(currentNode.getPosition());
                    continue;
                }
            }

            if (currentNode.getPosition().y / CELL_SIZE < W_HEIGHT / CELL_SIZE - 1)
            {
                if (map[currentNode.getPosition().y / CELL_SIZE + 1][currentNode.getPosition().x / CELL_SIZE].getD() == currentNode.getD() - 1)
                {
                    currentNode = map[currentNode.getPosition().y / CELL_SIZE + 1][currentNode.getPosition().x / CELL_SIZE];
                    path.push_back(currentNode.getPosition());
                    continue;
                }
            }

            if (currentNode.getPosition().x / CELL_SIZE > 0)
            {
                if (map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE - 1].getD() == currentNode.getD() - 1)
                {
                    currentNode = map[currentNode.getPosition().y / CELL_SIZE][currentNode.getPosition().x / CELL_SIZE - 1];
                    path.push_back(currentNode.getPosition());
                    continue;
                }
            }
        }

        return path;
    }

    int heuristic(Pos pos1, Pos pos2) {
        return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
    }

    bool isValid(Pos pos, int N) {
        return (pos.x >= 0 && pos.y >= 0 && pos.x < N && pos.y < N);
    }

    bool isValid(Pos pos, int N, int M) {
        return (pos.x >= 1 && pos.y >= 1 && pos.x < N - 1 && pos.y < M - 1);
    }

    int dfs(IDAStarNode current, int threshold, Pos goal, std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& visited, std::vector<Pos>& path, int& depth) 
    {
        depth++;
        if (depth > 1000)
            return -2;
        //std::cout << "dfs" << std::endl;
        int h = heuristic(current.getPosition(), goal);
        current.setF(current.getG() + h);

        if (current.getF() > threshold) 
        {
            return current.getF();
        }

        if (current.getPosition().x == goal.x && current.getPosition().y == goal.y) 
        {
            path.push_back(current.getPosition());  
            return -1;  
        }

        //std::cout << "visited 1" << std::endl;
        //if (current.getPosition().y < visited.size() && current.getPosition().x < visited[current.getPosition().y].size())
        visited[current.getPosition().y][current.getPosition().x] = true;
        //std::cout << "visited 2" << std::endl;
        path.push_back(current.getPosition());

        std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        int minCost = std::numeric_limits<int>::max();

        for (auto& dir : directions) 
        {
            Pos newPos = Pos(current.getPosition().x + dir.first, current.getPosition().y + dir.second);

            if (isValid(newPos, grid[0].size(), grid.size()) && !visited[newPos.y][newPos.x] && grid[newPos.y][newPos.x] == 0)
            {
                int result = dfs(IDAStarNode(newPos, current.getG() + 1, 0), threshold, goal, grid, visited, path, depth);
                //std::cout << "after dfs" << std::endl;
                if (result == -1) {
                    return -1;  
                }
                minCost = std::min(minCost, result);
            }
        }

        //std::cout << "visited 3" << std::endl;
        //if(current.getPosition().y < visited.size() && current.getPosition().x < visited[current.getPosition().y].size())
        visited[current.getPosition().y][current.getPosition().x] = false;
        //std::cout << "visited 4" << std::endl;
        path.pop_back();
        return minCost;
    }

    std::vector<Pos> IDAStar(std::vector <std::vector<Node>> mapN, IDAStarNode start, IDAStarNode finish)
    {
        int depth = 0;
        int threshold = heuristic(start.getPosition(), finish.getPosition());
        std::vector<std::vector<int>> grid;

        for (int i = 0; i < mapN.size(); i++)
        {
            std::vector<int> temp;
            for (int j = 0; j < mapN[0].size(); j++)
            {
                temp.push_back(mapN[i][j].getObstacle());
            }
            grid.push_back(temp);
        }

        while (true) 
        {
            std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));
            std::vector<Pos> path;  

            //std::cout << "pre-dfs" << std::endl;
            int result = dfs(IDAStarNode(start.getPosition(), 0, 0), threshold, finish.getPosition(), grid, visited, path, depth);
            //std::cout << "past-dfs" << std::endl;

            if (result == -2)
                return path;
            if (result == -1) 
            {
                if(path.size() > 0)
                {
                    std::reverse(path.begin(), path.end());
                    for (int i = 0; i < path.size(); i++)
                    {
                        path[i] = Pos(path[i].x * CELL_SIZE, path[i].y * CELL_SIZE);
                    }
                }
                return path; 
            }

            if (result == std::numeric_limits<int>::max()) 
            {
                return {};  
            }

            threshold = result;
        }
    }

    Pos findClosestCrossroad(Pacman pacman, int dir, std::vector<std::vector<Node>> map)
    {
        Pos retval;

        switch (dir)
        {
        case 1:

            break;
        case 2:

            break;
        case 3:

            break;
        case 4:

            break;
        }
    }

    Pos selectTarget(int index, Pacman pacman, std::vector<std::vector<Node>> map)
    {
        Pos currentTarget = pacman.getPosition();
        int c_x = currentTarget.x / CELL_SIZE;
        int c_y = currentTarget.y / CELL_SIZE;
        std::vector<Pos> path;
        switch (index)
        {
        case 1: //CHASE
            return pacman.getPosition();
            break;
        case 2: //INTERCEPT
            if (m_ghosts[index - 1].getPosition().calculateDistance(pacman.getPosition()) >= 6)
            {
                std::cout << "intercept in" << std::endl;
                for (int i = 0; i < 5; i++)
                {
                    switch (pacman.getDirection())
                    {
                    case 1: //RIGHT
                        if (c_x < W_WIDTH / CELL_SIZE - 1 && !map[c_y][c_x + 1].getObstacle())
                            currentTarget = Pos(currentTarget.x + CELL_SIZE, currentTarget.y);
                        else if (c_y < W_HEIGHT / CELL_SIZE - 1 && !map[c_y + 1][c_x].getObstacle())
                            currentTarget = Pos(currentTarget.x, currentTarget.y + CELL_SIZE);
                        else if (c_y > 0 && !map[c_y - 1][c_x].getObstacle())
                            currentTarget = Pos(currentTarget.x, currentTarget.y - CELL_SIZE);
                        else
                            currentTarget = pacman.getPosition();
                        break;
                    case 2: //UP
                        if (c_y > 0 && !map[c_y - 1][c_x].getObstacle())
                            currentTarget = Pos(currentTarget.x, currentTarget.y - CELL_SIZE);
                        else if (c_x < W_WIDTH / CELL_SIZE - 1 && !map[c_y][c_x + 1].getObstacle())
                            currentTarget = Pos(currentTarget.x + CELL_SIZE, currentTarget.y);
                        else if (c_x > 0 && !map[c_y][c_x - 1].getObstacle())
                            currentTarget = Pos(currentTarget.x - CELL_SIZE, currentTarget.y);
                        else
                            currentTarget = pacman.getPosition();
                        break;
                    case 3: //LEFT
                        if (c_x > 0 && !map[c_y][c_x - 1].getObstacle())
                            currentTarget = Pos(currentTarget.x - CELL_SIZE, currentTarget.y);
                        else if (c_y < W_HEIGHT / CELL_SIZE - 1 && !map[c_y + 1][c_x].getObstacle())
                            currentTarget = Pos(currentTarget.x, currentTarget.y + CELL_SIZE);
                        else if (c_y > 0 && !map[c_y - 1][c_x].getObstacle())
                            currentTarget = Pos(currentTarget.x, currentTarget.y - CELL_SIZE);
                        else
                            currentTarget = pacman.getPosition();
                        break;
                    case 4: //DOWN
                        if (c_y < W_HEIGHT / CELL_SIZE - 1 && !map[c_y + 1][c_x].getObstacle())
                            currentTarget = Pos(currentTarget.x, currentTarget.y - CELL_SIZE);
                        else if (c_x < W_WIDTH / CELL_SIZE - 1 && !map[c_y][c_x + 1].getObstacle())
                            currentTarget = Pos(currentTarget.x + CELL_SIZE, currentTarget.y);
                        else if (c_x > 0 && !map[c_y][c_x - 1].getObstacle())
                            currentTarget = Pos(currentTarget.x - CELL_SIZE, currentTarget.y);
                        else
                            currentTarget = pacman.getPosition();
                        break;
                    default:
                        currentTarget = pacman.getPosition();
                        break;
                    }
                }
                std::cout << "intercept out" << std::endl;
                return currentTarget;
            }
            else
            {
                return pacman.getPosition();
            }

            break;
        case 3: //PATROL
            //path = aStar(map, map[m_ghosts[2].getPosition().y / CELL_SIZE][m_ghosts[2].getPosition().x / CELL_SIZE], map[pacman.getPosition().y / CELL_SIZE][pacman.getPosition().x / CELL_SIZE]);
            if (m_ghosts[index - 1].getPosition().calculateDistance(pacman.getPosition()) >= 6)
            {
                currentTarget = m_ghosts[2].getPosition();
                switch (currentTarget.locateSide(pacman.getPosition()))
                {
                case 14:
                    std::cout << "top left c" << std::endl;
                    return map[1][1].getPosition();
                    break;
                case 12:
                    std::cout << "top right c" << std::endl;
                    return map[1][W_WIDTH / CELL_SIZE - 2].getPosition();
                    break;
                case 34:
                    std::cout << "bottom left c" << std::endl;
                    return map[W_HEIGHT / CELL_SIZE - 2][1].getPosition();
                    break;
                case 32:
                    std::cout << "bottom right c" << std::endl;
                    return map[W_HEIGHT / CELL_SIZE - 2][W_WIDTH / CELL_SIZE - 2].getPosition();
                    break;
                }
            }
            else
            {
                std::cout << "chasing pacman" << std::endl;
                return pacman.getPosition();
            }
            break;
        case 4: //WAIT
            /*if (m_ghosts[index - 1].getPosition().calculateDistance(pacman.getPosition()) >= 5)
            {
                currentTarget = m_ghosts[3].getPosition();
                switch (currentTarget.locateSide(pacman.getPosition()))
                {
                case 14:
                    std::cout << "top left c" << std::endl;
                    return map[1][1].getPosition();
                    break;
                case 12:
                    std::cout << "top right c" << std::endl;
                    return map[1][W_WIDTH / CELL_SIZE - 2].getPosition();
                    break;
                case 34:
                    std::cout << "bottom left c" << std::endl;
                    return map[W_HEIGHT / CELL_SIZE - 2][1].getPosition();
                    break;
                case 32:
                    std::cout << "bottom right c" << std::endl;
                    return map[W_HEIGHT / CELL_SIZE - 2][W_WIDTH / CELL_SIZE - 2].getPosition();
                    break;
                }
            }
            else
            {
                std::cout << "chasing pacman" << std::endl;
                return pacman.getPosition();
            }*/

            return pacman.getPosition();
            break;
        }
    }

    bool getPermission(Pacman pacman, int index)
    {
        switch (index)
        {
        case 1:
            return true;
            break;
        case 2:
            if(m_ghosts[index - 1].getPosition().calculateDistance(pacman.getPosition()) < 17)
                return true;
            return false;
            break;
        case 3:
            if (m_ghosts[index - 1].getPosition().calculateDistance(pacman.getPosition()) < 6)
                return true;
            return false;
            break;
        case 4:
            return true;
            break;
        }
    }
        
    void update(Pos dest, std::vector<std::vector<Node>> map, std::array<Pos, 4>& startPositions, Pacman pacman, int currentFrame, std::vector<Pos>& ppath, std::vector<Pos>& ppath2, bool& GAME_OVER)
    {
        for (int i = 1; i < m_ghosts.size() + 1; i++)
        {
            if (abs(m_ghosts[i - 1].getPosition().x - pacman.getPosition().x) < 15 && abs(m_ghosts[i - 1].getPosition().y - pacman.getPosition().y) < 15)
            {
                GAME_OVER = true;
                return;
            }

            if(currentFrame == 1)
            {
                std::cout << "current ghost is: " << i << std::endl;
                Pos target = selectTarget(i, pacman, map);
                startPositions[i - 1] = m_ghosts[i - 1].getPosition();
                
                if (getPermission(pacman, i) || m_paths[i - 1].size() < 1)
                {
                    if(target.x / CELL_SIZE < W_WIDTH / CELL_SIZE && target.x / CELL_SIZE >= 0 && target.y / CELL_SIZE < W_HEIGHT / CELL_SIZE && target.y / CELL_SIZE >= 0)
                        m_paths[i - 1] = aStar(map, map[startPositions[i - 1].y / CELL_SIZE][startPositions[i - 1].x / CELL_SIZE], map[target.y / CELL_SIZE][target.x / CELL_SIZE]); //AStar
                        //m_paths[i - 1] = waveLee(map, map[startPositions[i - 1].y / CELL_SIZE][startPositions[i - 1].x / CELL_SIZE], map[target.y / CELL_SIZE][target.x / CELL_SIZE]); //Wave Lee
                        //m_paths[i - 1] = IDAStar(map, map[startPositions[i - 1].y / CELL_SIZE][startPositions[i - 1].x / CELL_SIZE], map[target.y / CELL_SIZE][target.x / CELL_SIZE]); //IDAStar
                }
                else
                {
                    m_paths[i - 1].erase(m_paths[i - 1].begin() + m_paths[i - 1].size() - 1);
                }
            }          

            if(m_paths[i - 1].size() > 1)
            {
                m_ghosts[i - 1].update(m_paths[i - 1][m_paths[i - 1].size() - 2], currentFrame);
            }

            if (i == 2)
                ppath2 = m_paths[i - 1];

            if (i == 3)
                ppath = m_paths[i - 1];

            if (currentFrame == 10)
            {
                startPositions[i - 1] = m_ghosts[i - 1].getPosition();
            }

            //std::cout << "update out" << std::endl;
        }
    }
};

std::vector<std::vector<Node>> makeMap(std::vector<Cell> cells)
{
    std::vector<std::vector<Node>> map;
    for (int i = 0; i < W_HEIGHT / CELL_SIZE; i++)
    {
        std::vector<Node> temp;
        for (int j = 0; j < W_WIDTH / CELL_SIZE; j++)
        {
            Node node = cells[i * (W_WIDTH / CELL_SIZE) + j].toNode();
            node.setObstacle(cells[i * (W_WIDTH / CELL_SIZE) + j].isWall());
            temp.push_back(node);
        }
        map.push_back(temp);
    }

    return map;
}

std::vector <sf::RectangleShape> shapeCells(const std::array<std::string, 31>& mask, std::vector<Cell>& cells)
{
    std::vector <sf::RectangleShape> rectangles;

    for (int i = 0; i < mask.size(); i++)
    {
        for (int j = 0; j < mask[0].length(); j++)
        {
            if ('#' == mask[i][j])
            {
                Cell cell = Cell(CELL_SIZE, CELL_SIZE, false, Pos(j * CELL_SIZE, i * CELL_SIZE));
                cells.push_back(cell);
                rectangles.push_back(cell.cellToShape());
            }
            else
            {
                cells.push_back(Cell(CELL_SIZE, CELL_SIZE, true, Pos(j * CELL_SIZE, i * CELL_SIZE)));
            }
        }
    }

    return rectangles;
}

bool isValid(Pos pos, const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& visited) {
    return pos.x >= 0 && pos.x < grid[0].size() && pos.y >= 0 && pos.y < grid.size() && grid[pos.y][pos.x] == 0 && !visited[pos.y][pos.x];
}

bool hasPath(std::vector<std::vector<int>>& grid, Pos start, Pos end) 
{
    
    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { 1, -1, 0, 0 };

    if (grid[start.y][start.x] == 1 || grid[end.y][end.x] == 1) {
        return false;
    }

    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));

    std::queue<std::pair<int, int>> q;
    q.push({ start.x, start.y });
    visited[start.y][start.x] = true;

    while (!q.empty()) {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        if (x == end.x && y == end.y) {
            return true;
        }

        for (int i = 0; i < 4; ++i) {
            int newX = x + dx[i];
            int newY = y + dy[i];

            if (isValid(Pos(newX, newY), grid, visited)) {
                q.push({ newX, newY });
                visited[newY][newX] = true;
            }
        }
    }

    return false;
}

int getRandom(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

bool isCellTrapped(const std::vector<std::vector<char>>& map, int x, int y)
{
    int wallCount = 0;
    if (map[y - 1][x] == WALL) wallCount++; 
    if (map[y + 1][x] == WALL) wallCount++; 
    if (map[y][x - 1] == WALL) wallCount++; 
    if (map[y][x + 1] == WALL) wallCount++; 
    return wallCount >= 3; 
}

std::vector<std::vector<char>> generateMap(int width, int height, int complexity)
{
    std::vector<std::vector<char>> map(height, std::vector<char>(width, EMPTY));

    for (int i = 0; i < height; ++i)
    {
        map[i][0] = WALL;
        map[i][width - 1] = WALL;
    }
    for (int j = 0; j < width; ++j)
    {
        map[0][j] = WALL;
        map[height - 1][j] = WALL;
    }

    for (int i = 1; i < height - 1; ++i)
    {
        for (int j = 1; j < width - 1; ++j)
        {
            if (getRandom(0, 100) < complexity)
            {
                map[i][j] = WALL;
            }
        }
    }

    for (int i = 1; i < height - 1; ++i)
    {
        for (int j = 1; j < width - 1; ++j)
        {
            if (map[i][j] == WALL && !isCellTrapped(map, j, i))
            {
                if (getRandom(0, 100) < 50)
                {
                    map[i][j] = EMPTY;
                }
            }
        }
    }

    return map;
}

void printMap(const std::vector<std::string>& map)
{
    for (int i = 0; i < map.size(); i++)
    {
        std::cout << map[i] << std::endl;
    }
}

std::array<std::string, 31> RMG()
{
    srand(static_cast<unsigned>(time(0)));

    int width, height, complexity;

    width = 28; height = 31; complexity = 35;

    std::vector<std::vector<char>> map = generateMap(width, height, complexity);

    std::array<std::string, 31> randomMap;

    for (int i = 0; i < map.size(); i++)
    {
        std::string temp = "";
        for (int j = 0; j < map[0].size(); j++)
        {
            temp += map[i][j];
        }
        randomMap[i] = temp;
    }

    return randomMap;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT + 40), "SFML works!");

    std::array<std::string, 31> mask = {
        "############################",
        "#            ##            #",
        "# #### ##### ## ##### #### #",
        "# #### ##### ## ##### #### #",
        "# #### ##### ## ##### #### #",
        "#                          #",
        "# #### ## ######## ## #### #",
        "# #### ## ######## ## #### #",
        "#      ##    ##    ##      #",
        "###### ##### ## ##### ######",
        "     # ##### ## ##### #     ",
        "     # ##          ## #     ",
        "     # ##          ## #     ",
        "###### ## ###  ### ## ######",
        "          #      #          ",
        "###### ## ######## ## ######",
        "     # ##          ## #     ",
        "     # ##          ## #     ",
        "     # ## ######## ## #     ",
        "###### ## ######## ## ######",
        "#            ##            #",
        "# #### ##### ## ##### #### #",
        "# #### ##### ## ##### #### #",
        "#   ##                ##   #",
        "### ## ## ######## ## ## ###",
        "### ## ## ######## ## ## ###",
        "#      ##    ##    ##      #",
        "# ########## ## ########## #",
        "# ########## ## ########## #",
        "#                          #",
        "############################"
    };

    std::vector <sf::RectangleShape> rects;
    std::vector <Cell> cells;
    std::vector<std::vector<Node>> map;
    std::vector<Pos> path;
    std::vector<Pos> path2;

    std::vector<std::vector<Dot>> dots;

    //mask = RMG();

    rects = shapeCells(mask, cells);


    Pacman pacman = Pacman(Pos(W_WIDTH/2 - 15, 525));

    std::array<Ghost, 4> ghosts = {
        Pos(W_WIDTH / 2 - 28 - 60, 423), 
        Pos(W_WIDTH / 2 - 28 - 30, 423), 
        Pos(W_WIDTH / 2 - 28 + 60, 423),
        Pos(W_WIDTH / 2 - 28 + 90, 423)
        };

    window.setFramerateLimit(MAX_FPS);

    std::vector<std::vector<int>> grid;
    for (int i = 0; i < W_HEIGHT / CELL_SIZE; i++)
    {
        std::vector <int> vec;
        for (int j = 0; j < W_WIDTH / CELL_SIZE; j++)
        {
            if (cells[i * (W_WIDTH / CELL_SIZE) + j].isWall())
                vec.push_back(1);
            else
                vec.push_back(0);
        }
        grid.push_back(vec);
    }

    for (int i = 0; i < W_HEIGHT / CELL_SIZE; i++)
    {
        std::vector <Dot> vec;
        for (int j = 0; j < W_WIDTH / CELL_SIZE; j++)
        {
            std::cout << "i = " << i << " j = " << j << std::endl;
            Dot dot = Dot(Pos(j * CELL_SIZE + CELL_SIZE / 2, i * CELL_SIZE + CELL_SIZE / 2), 10);
            if (cells[i * (W_WIDTH / CELL_SIZE) + j].isWall() || !hasPath(grid, Pos(pacman.getPosition().x / CELL_SIZE, pacman.getPosition().y / CELL_SIZE), Pos(j, i)))
                dot.setDead();
            vec.push_back(dot);
        }
        dots.push_back(vec);
    }

    GhostManager ghostManager = GhostManager(ghosts);
    std::array<Pos, 4> startPositions = {
        ghosts[0].getPosition(),
        ghosts[1].getPosition(),
        ghosts[2].getPosition(),
        ghosts[3].getPosition()
    };

    int frame = 0;
    int score = 0;

    bool GAME_OVER = false;

    sf::Text text;
    sf::Font m_font;

    m_font.loadFromFile("C:\\Users\\dwarf\\source\\repos\\Packman_Lab_01\\arial.ttf");

    text.setFont(m_font);
    text.setString("SCORE: ");
    text.setPosition(0, W_HEIGHT);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(30);

    sf::Text go_text;
    go_text.setFont(m_font);
    go_text.setString("GAME OVER");
    go_text.setPosition(W_WIDTH / 4 - 5, W_HEIGHT / 2 - 70);
    go_text.setFillColor(sf::Color::White);
    go_text.setCharacterSize(70);

    sf::Text sc_text;
    sc_text.setFont(m_font);

    sc_text.setPosition(W_WIDTH / 4 + 50, W_HEIGHT / 2 + 10);
    sc_text.setFillColor(sf::Color::White); 
    sc_text.setCharacterSize(45);

    while (window.isOpen())
    {
        frame++;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (GAME_OVER)
        {
            sc_text.setString("SCORE: " + std::to_string(score));
            window.draw(sc_text);
            window.draw(go_text);
            window.display();
            continue;
        }

        if (frame == 11)
            frame = 1;

        pacman.update(cells);

        window.clear();
        for(int i = 0; i < rects.size(); i ++)
            window.draw(rects[i]);
        
        map = makeMap(cells);

        std::cout << "frame: " << frame << std::endl;
        ghostManager.update(pacman.getPosition(), map, startPositions, pacman, frame, path, path2, GAME_OVER);
        ghostManager.draw(window);

        for (int i = 0; i < dots.size(); i++)
        {
            for (int j = 0; j < dots[0].size(); j++)
            {
                dots[i][j].checkPacman(pacman, score);
                if (dots[i][j].isALive())
                {
                    window.draw(dots[i][j].toShape());
                }
            }
        }
        window.draw(map[W_HEIGHT / CELL_SIZE - 2][1].toShape(0));
        window.draw(pacman.toShape());
        text.setString("SCORE: " + std::to_string(score));
        window.draw(text);

        sf::RectangleShape r = pacman.locateCell(cells).cellToShape();
        r.setFillColor(sf::Color::Green);

        window.display();
    }

    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
