#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <chrono>

using namespace std::chrono;

const int width = 800;
const int rows = 50;

class Spot {
private:
    int m_totalRows;
    int m_row;
    int m_col;
    int m_width;
public:
    int posPixel[2];
    int pos[2];
    std::vector<Spot*> neighbors;
    sf::Color choice;
    Spot* prev = NULL;

    Spot(int row, int col, int width, int totalRows) {
        pos[0] = row;
        pos[1] = col;
        m_row = row;
        m_col = col;
        posPixel[0] = m_row * width;
        posPixel[1] = m_col * width;
        choice = sf::Color::Black;
        m_width = width;
        m_totalRows = totalRows;
    }

    Spot() {
        choice = sf::Color::Black;
    }

    bool isClosed() {return choice == sf::Color::Red;}
    bool isOpen() {return choice == sf::Color::Green;}
    bool isBarrier() {return choice == sf::Color::White;}
    bool isStart() {return choice == sf::Color::Yellow;}
    bool isEnd() {return choice == sf::Color::Cyan;}

    void reset() {choice = sf::Color::Black;}
    void makeStart() {choice = sf::Color::Yellow;}
    void makeClosed() {choice = sf::Color::Red;}
    void makeOpen() {choice = sf::Color::Green;}
    void makeBarrier() {choice = sf::Color::White;}
    void makeEnd() {choice = sf::Color::Cyan;}
    void makePath() {choice = sf::Color::Magenta;}

    void drawSpot(sf::RenderWindow &window) {
        sf::RectangleShape rect(sf::Vector2f(m_width, m_width));
        rect.setPosition(posPixel[0], posPixel[1]);
        rect.setFillColor(choice);
        window.draw(rect);
    }
    void updateNeighbors(std::vector<std::vector<Spot>> &grid) {
        neighbors.clear();
        if (m_row < m_totalRows - 1 && !grid[m_row + 1][m_col].isBarrier()) {
            neighbors.push_back(&grid[m_row + 1][m_col]);
        }
        if (m_row > 0 && !grid[m_row - 1][m_col].isBarrier()) 
            neighbors.push_back(&grid.at(m_row - 1).at(m_col));

        if (m_col < m_totalRows - 1 && !grid[m_row][m_col + 1].isBarrier())
            neighbors.push_back(&grid.at(m_row).at(m_col + 1));
        if (m_col > 0 && !grid[m_row][m_col - 1].isBarrier())
            neighbors.push_back(&grid.at(m_row).at(m_col - 1));

        if (m_row < m_totalRows - 1 && m_col < m_totalRows - 1 && !grid[m_row + 1][m_col + 1].isBarrier()) {
            if (!grid[m_row + 1][m_col].isBarrier() && !grid[m_row][m_col + 1].isBarrier())
                neighbors.push_back(&grid.at(m_row + 1).at(m_col + 1));
        }
        if (m_row < m_totalRows - 1 && m_col > 0 && !grid[m_row + 1][m_col - 1].isBarrier()) {
            if (!grid[m_row + 1][m_col].isBarrier() && !grid[m_row][m_col - 1].isBarrier())
                neighbors.push_back(&grid[m_row + 1][m_col - 1]);
        }
        if (m_row > 0 && m_col < m_totalRows - 1 && m_col > 0 && !grid[m_row - 1][m_col + 1].isBarrier()) {
            if (!grid[m_row - 1][m_col].isBarrier() && !grid[m_row][m_col - 1].isBarrier())
                neighbors.push_back(&grid[m_row - 1][m_col + 1]);
        }
        if (m_row > 0 && m_col > 0 && !grid[m_row - 1][m_col - 1].isBarrier()) {
            if (!grid[m_row - 1][m_col].isBarrier() && !grid[m_row][m_col - 1].isBarrier())
                neighbors.push_back(&grid[m_row - 1][m_col - 1]);
        }
    }

    bool operator==(const Spot &spot) const{
        return (m_row == spot.pos[0] && m_col == spot.pos[1]);
    }

};

class Button {
private:
    int m_width;
    int m_height;
    int m_x;
    int m_y;
public:
    sf::RectangleShape* image;
    Button(int width, int height, int x, int y) {

        sf::RectangleShape rect(sf::Vector2f(width, height));
        rect.setPosition(x, y);
        image = &rect;
    }
    void drawButton(sf::RenderWindow &window) {
        window.draw(*image);
    }
    bool isOver(sf::RenderWindow &window) {
        if (sf::Mouse::getPosition(window).x > m_x && sf::Mouse::getPosition(window).y > m_y && sf::Mouse::getPosition(window).x < m_x + m_width && sf::Mouse::getPosition(window).y < m_y + m_height) {
            return true;
        }
        return false;
    }
};

class HashFunction {
public:
    size_t operator()(const Spot& spot) const { 
        return spot.pos[0] + spot.pos[1];
    }
};

std::vector<std::vector<Spot>> makeGrid(int rows, int width) {
    std::vector<std::vector<Spot>> grid;
    int gap = width / rows;
    for (int i = 0; i < rows; i++) {
        std::vector<Spot> temp;
        for (int j = 0; j < rows; j++) {
            temp.push_back(Spot(i, j, gap, rows));
        }
        grid.push_back(temp);
    } 
    return grid;
}

void drawGrid(sf::RenderWindow &window, int rows, int width) {
    int gap = width / rows;
    for (int i = 0; i < rows; i++) {
        sf::Vertex line[] = {sf::Vector2f(0, i * gap), sf::Vector2f(width, i * gap)};
        window.draw(line, 2, sf::Lines);
        for (int j = 0; j < rows; j++) {
            sf::Vertex line2[] = {sf::Vector2f(j * gap, 0), sf::Vector2f(j * gap, width)};
            window.draw(line2, 2, sf::Lines);
        }
    }
}

void drawMain(sf::RenderWindow &window, std::vector<std::vector<Spot>> grid, int rows, int width) {
    for (std::vector<Spot> row: grid) {
        for (Spot spot: row) {
            spot.drawSpot(window);
        }
    }
    drawGrid(window, rows, width);
}

sf::Vector2i getClickedPos(sf::Vector2i pos, int rows, int width) {
    int gap = width / rows;
    int row = pos.y / gap;
    int col = pos.x / gap;
    sf::Vector2i returnValue(row, col);
    return returnValue;
}

int heuristic(int p1[], int p2[]) {
    return std::abs((p1[0] - p2[0]) + (p1[1] - p2[1]));
}

bool findKey(std::unordered_map<Spot, Spot, HashFunction> &cameFrom, Spot &current) {
    for (auto &elm: cameFrom) {
        if (elm.first == current) return true;
    }
    return false;
}

void reconstructPath(std::unordered_map<Spot, Spot, HashFunction> &cameFrom, Spot &current, std::vector<std::vector<Spot>> &grid) {
    std::vector<Spot*> path;
    Spot* ptr;
    while (findKey(cameFrom, current)) {
        ptr = &cameFrom[current];
        current = *ptr;
        path.push_back(ptr);
    }
    for (Spot* spot: path) {
        grid[spot->pos[0]][spot->pos[1]].makePath();
    }
}

bool isDiagonal(Spot &current, Spot* neighbor) {
    if (current.pos[0] == neighbor->pos[0] - 1 && current.pos[1] == neighbor->pos[1] - 1) {
        return true;
    } 
    else if (current.pos[0] == neighbor->pos[0] - 1 && current.pos[1] == neighbor->pos[1] + 1) {
        return true;
    }
    else if (current.pos[0] == neighbor->pos[0] + 1 && current.pos[1] == neighbor->pos[1] - 1) {
        return true;
    }     
    else if (current.pos[0] == neighbor->pos[0] + 1 && current.pos[1] == neighbor->pos[1] + 1) {
        return true;
    }
    return false;   
}

bool algorithm(std::vector<std::vector<Spot>> &grid, Spot* start, Spot* end, sf::RenderWindow &window, int rows, int width) {
    std::cout << "algorithm started" << std::endl;
    std::vector<Spot> openSet;
    openSet.push_back(*start);
    std::vector<Spot> closedSet;
    std::unordered_map<Spot, Spot, HashFunction> cameFrom;
    std::unordered_map<Spot, int, HashFunction> gScore;
    std::unordered_map<Spot, int, HashFunction> fScore;
    Spot &current = *start;
    for (std::vector<Spot> row: grid) {
        for (Spot spot: row) {
            gScore.insert(std::make_pair(spot, std::numeric_limits<int>::max()));
            fScore.insert(std::make_pair(spot, std::numeric_limits<int>::max()));
        }
    }

    gScore[*start] = 0;
    fScore[*start] = heuristic(start->pos, end->pos);

    while (openSet.size() > 0) {
        int winner = 0;
        for (int i = 0; i < openSet.size(); i++) {
            if (fScore[openSet[i]] < fScore[openSet[winner]]) {
                winner = i;
            }
        }
        current = openSet[winner];
        openSet.erase(std::remove(openSet.begin(), openSet.end(), current));
        closedSet.push_back(current);

        if (current == *end) {
            std::cout << "reached end" << std::endl;
            reconstructPath(cameFrom, current, grid);
            start->makeStart();
            end->makeEnd();
            return true;
        }
        current.updateNeighbors(grid);
        for (Spot* neighbor: current.neighbors) {
            double tempGScore;
            if (std::find(openSet.begin(), openSet.end(), *neighbor) != openSet.end()) continue;
            if (isDiagonal(current, neighbor)) {
                tempGScore = gScore[current] + 2;
            }
            else {
                tempGScore = gScore[current] + 1;
            }
            if (tempGScore < gScore[*neighbor]) {
                cameFrom[*neighbor] = current;
                gScore[*neighbor] = tempGScore;
                fScore[*neighbor] = tempGScore + heuristic(neighbor->pos, end->pos);
                if (!(std::find(openSet.begin(), openSet.end(), *neighbor) != openSet.end())) {
                    openSet.push_back(*neighbor);
                }
            }
        }
    }

    drawMain(window, grid, rows, width);
    return false;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(width, width), "Pathfinding");
    std::vector<std::vector<Spot>> grid = makeGrid(rows, width);
    Spot *start = NULL;
    Spot *end = NULL;
    bool started = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (!started) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i posPixel = sf::Mouse::getPosition(window);
                    sf::Vector2i pos = getClickedPos(posPixel, rows, width);
                    Spot &temp = grid.at(pos.y).at(pos.x);
                    
                    if (start == NULL && &temp != end) {
                        start = &temp;
                        start->makeStart();
                    } else if (end == NULL && &temp != start) {
                        end = &temp;
                        end->makeEnd();
                    } else if (&temp != end && &temp != start) {
                        temp.makeBarrier();
                    }
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    sf::Vector2i posPixel = sf::Mouse::getPosition(window);
                    sf::Vector2i pos = getClickedPos(posPixel, rows, width);
                    Spot &temp = grid.at(pos.y).at(pos.x);
                    temp.reset();
                }
                if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i posPixel = sf::Mouse::getPosition(window);
                    sf::Vector2i pos = getClickedPos(posPixel, rows, width);
                    Spot &temp = grid.at(pos.y).at(pos.x);
                    if (start != NULL && end != NULL && &temp != end && &temp != start) {
                        temp.makeBarrier();
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                        for (std::vector<Spot> row: grid) {
                            for (Spot spot: row) {
                                spot.updateNeighbors(grid);
                            }
                        }
                        started = true;
                        high_resolution_clock::time_point start_time = high_resolution_clock::now(); 
                        algorithm(grid, start, end, window, rows, width);
                        high_resolution_clock::time_point stop_time = high_resolution_clock::now(); 
                        started = false;
                        duration<double> time_span = duration_cast<duration<double>>(stop_time - start_time);
                        std::cout << "Algorithm took " << time_span.count() << " seconds" << std::endl;
                    }
                    else if (event.key.code == sf::Keyboard::C) {
                        start = NULL;
                        end = NULL;
                        grid = makeGrid(rows, width);
                    }
                }
            }

        }

        window.clear();
        drawMain(window, grid, rows, width);
        window.display();
    }

    return 0;
}