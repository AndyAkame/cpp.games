/*****************************************************************************|
 * Convey's game of life.
 * You can find detailed explanaition of the game rules here:
 *      https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 *
 * This is a console version and is written on C++/14 (clang 3.6, gcc 5.2).
 * Written by github.com/AndyAkame
 *            vk.com/andy.akame
|*****************************************************************************/


#include <vector>
#include <iostream>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>


namespace gol { // gol for Game of Life

    namespace aux { // auxiliary namespace

        bool // returns true with given probability
        rndBool(double probability = 30)
        {
            unsigned pow10 = std::pow(10, int(std::log10(probability) + 1)) + 1;
            probability = probability / pow10;
            double rnd = (double(std::rand() % pow10) / pow10);
            return (rnd < probability) ? true : false;
        }

    }; // aux namespace

    class Cell {

        public: // methods:

            Cell(bool alive)
                : alive(alive)
            {}

            bool
            isAlive(void)
            { return this->alive; }

            void
            setAlive(bool alive)
            { this->alive = alive; }

        private: // fields:

            bool alive;

    }; // Cell class

    using CellsLine = std::vector<Cell>;

    class Line {

        public:

            Line(std::size_t size)
                : cells_line(size, false)
            {}

            void
            initializeRandomly(double probability)
            {
                for (auto&& cell : cells_line) {
                    bool al = aux::rndBool(probability);
                    cell.setAlive(al);
                }
            }

            Cell&
            operator [] (std::size_t xx)
            { return cells_line[ xx ]; }

        private:

            CellsLine cells_line;

    }; // Line class

    using CellsGrid = std::vector<Line>;

    class Grid {
        public:

            Grid(std::size_t size)
                : lines(size, size) // double buffering
                , lines_buffer(size, size)
            {}

            void
            initializeRandomly(double probability)
            {
                for (auto&& line : lines) {
                    line.initializeRandomly(probability);
                }
            }

            void
            go(void)
            {
                this->linesToBuffer();
                this->bufferToLines();
            }

            auto
            isAlive(std::size_t xx, std::size_t yy)
            { return this->lines[ xx ][ yy ].isAlive(); }

            auto
            size(void)
            { return this->lines.size(); }

        private:

            auto // count how many alive cells are around
            countAliveCells(std::size_t ii, std::size_t jj)
            {
                typedef enum { LEFT, RGHT, SIZE, } SIDE;
                std::size_t xx[ SIDE::SIZE ] = {
                    ii == 0 ? lines.size() - 1 : ii - 1, // on the left
                    ii == lines.size() - 1 ? 0 : ii + 1, // on the right
                };
                std::size_t yy[ SIDE::SIZE ] = {
                    jj == 0 ? lines.size() - 1 : jj - 1, // on the left
                    jj == lines.size() - 1 ? 0 : jj + 1, // on the right
                };
                char count = ( // mouhahaha!
                      char(lines[ ii         ][ yy[ LEFT ] ].isAlive())
                    + char(lines[ ii         ][ yy[ RGHT ] ].isAlive())
                    + char(lines[ xx[ LEFT ] ][ jj         ].isAlive())
                    + char(lines[ xx[ LEFT ] ][ yy[ LEFT ] ].isAlive())
                    + char(lines[ xx[ LEFT ] ][ yy[ RGHT ] ].isAlive())
                    + char(lines[ xx[ RGHT ] ][ jj         ].isAlive())
                    + char(lines[ xx[ RGHT ] ][ yy[ LEFT ] ].isAlive())
                    + char(lines[ xx[ RGHT ] ][ yy[ RGHT ] ].isAlive())
                );
                return count;
            }

            void // decide what to do with the cell: kill, spawn, safe
            linesToBuffer(void)
            {
                for (auto ii = ( 0 ); ii < lines.size(); ++ii) {
                    for (auto jj = ( 0 ); jj < lines.size(); ++jj) {
                        auto count = ( this->countAliveCells(ii, jj) );
                        if (lines_buffer[ ii ][ jj ].isAlive()) {
                            switch (count) {
                            case 0: case 1: {
                                lines_buffer[ ii ][ jj ].setAlive(false);
                            } break;
                            case 2: case 3: {
                                lines_buffer[ ii ][ jj ].setAlive(true);
                            } break;
                            default: {
                                lines_buffer[ ii ][ jj ].setAlive(false);
                            } break;
                            }
                        } else if (count == 3) {
                            lines_buffer[ ii ][ jj ].setAlive(true);
                        }
                    }
                }
            }

            void // return image from hiden buffer to the drawn buffer
            bufferToLines(void)
            {
                for (auto ii = ( 0 ); ii < lines.size(); ++ii) {
                    for (auto jj = ( 0 ); jj < lines.size(); ++jj) {
                        lines[ ii ][ jj ].setAlive(
                            lines_buffer[ ii ][ jj ].isAlive()
                        );
                    }
                }
            }

        private:

            CellsGrid lines;
            CellsGrid lines_buffer;

    }; // Grid class

} // namepace gol


void
draw(gol::Grid& grid)
{
    std::system("clear");
    for (auto ii = ( 0 ); ii < grid.size(); ++ii) {
        for (auto jj = ( 0 ); jj < grid.size(); ++jj) {
            std::cout << (grid.isAlive(ii, jj) ? '*' : ' ');
        }
        std::cout << ' ' << ii << std::endl;
    }
}


int
main(int argc, char** argv)
{
    std::srand(std::time(nullptr)); // set up seed for the rand function
    gol::Grid grid{ 20 }; // square grid, number of cells
    grid.initializeRandomly(30); // probability in %'s
    for (;;) {
        draw(grid);
        grid.go(); // logic here
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


