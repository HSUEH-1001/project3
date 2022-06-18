#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#define MIN -2100000000
#define MAX 2100000000
using namespace std;

// 1 = debug mode
bool dede = 0;
const int SIZE = 15;
enum SPOT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
struct Point
{
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point &rhs) const
    {
        return !operator==(rhs);
    }
    Point operator+(const Point &rhs) const
    {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point &rhs) const
    {
        return Point(x - rhs.x, y - rhs.y);
    }
};
const std::array<Point, 8> directions{{Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                       Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                       Point(1, -1), Point(1, 0), Point(1, 1)}};

struct Board
{
    array<array<int, SIZE>, SIZE> my_board;
    int empty_count;
    bool done;
    int winner;
    int my_player;

    int &operator()(int i, int j)
    {
        return my_board[i][j];
    }
    Board &operator=(const Board &r)
    {
        my_board = r.my_board;
        empty_count = r.empty_count;
        done = r.done;
        winner = r.winner;
        my_player = r.my_player;
        return *this;
    }
    int get_next_player(int player)
    {
        return 3 - player;
    }
    bool is_spot_on_board(Point p)
    {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p)
    {
        return my_board[p.x][p.y];
    }
    void set_disc(Point p, int disc)
    {
        my_board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc)
    {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center)
    {
        if (get_disc(center) != EMPTY)
            return false;
        return true;
    }
    // cp for current player
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(my_player);
            done = true;
            return false;
        }
        set_disc(p, my_player);
        empty_count--;
        // Check Win
        if (checkwin(my_player)) {
            done = true;
            winner = my_player;
        }
        if (empty_count == 0) {
            done = true;
            winner = EMPTY;
        }

        return true;
    }
    
    bool checkwin(int disc) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (is_disc_at(Point(i, j), disc)){
                    bool iswin = true;
                    if (i + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i+k, j), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (i + 4 < SIZE && j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i+k, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (i - 4 >= 0 && j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i-k, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                }
            }
        }
        return false;
    }

    int eval(int player)
    {
        // my_player = get_next_player(my_player);
        int state_value = 0;
        // for (int i = 0; i < SIZE; i++)
        // {
        //     if (my_board[i][0] == my_player)
        //         state_value += 4;
        //     else if (my_board[i][0] == get_next_player(my_player))
        //         state_value -= 3;
        //     if (my_board[i][7] == my_player)
        //         state_value += 4;
        //     else if (my_board[i][7] == get_next_player(my_player))
        //         state_value -= 3;
        //     if (my_board[0][i] == my_player)
        //         state_value += 4;
        //     else if (my_board[0][i] == get_next_player(my_player))
        //         state_value -= 3;
        //     if (my_board[7][i] == my_player)
        //         state_value += 4;
        //     else if (my_board[7][i] == get_next_player(my_player))
        //         state_value -= 3;
        // }
        // for (int i = 1; i < SIZE - 1; i++)
        // {
        //     if (my_board[1][i] == my_player)
        //         state_value -= 3;
        //     if (my_board[6][i] == my_player)
        //         state_value -= 3;
        //     if (my_board[i][1] == my_player)
        //         state_value -= 3;
        //     if (my_board[i][6] == my_player)
        //         state_value -= 3;
        // }
        
        for(int i = 0; i < SIZE; i++)
        {
            for(int j = 0; j < SIZE; j++)
            {
                if(is_disc_at(Point(i, j), player))
                {
                    if(i + 1 < SIZE)
                    {
                        bool is_two = true;
                        for(int k = 0; k < 2; k++)
                        {
                            if(!is_disc_at(Point(i + k, j), player))
                            {
                                is_two = false;
                                break;
                            }
                        }
                        if(is_two)
                        {
                            if(is_spot_on_board(Point(i-1, j)) && is_spot_on_board(Point(i-2, j)) && is_spot_on_board(Point(i + 2, j)))
                            {
                                if((get_disc(Point(i-1, j)) == EMPTY && get_disc(Point(i-2, j)) == player))
                                {
                                    if(get_disc(Point(i+2, j)) == EMPTY)
                                        state_value += 15;
                                    else 
                                        state_value += 5;
                                }
                            }
                            else if(is_spot_on_board(Point(i+2, j)) && is_spot_on_board(Point(i+3, j)) && is_spot_on_board(Point(i-1, j)))
                            {
                                if(get_disc(Point(i+2, j)) == EMPTY && get_disc(Point(i+3, j)) == player)
                                {
                                    if(get_disc(Point(i-1, j)) == EMPTY)
                                        state_value += 15;
                                    else 
                                        state_value += 5;
                                }
                            }
                            state_value += 2;
                        }
                    }
                    if(j + 1 < SIZE)
                    {
                        bool is_two = true;
                        for(int k = 0; k < 2; k++)
                        {
                            if(!is_disc_at(Point(i, j + k), player))
                            {
                                is_two = false;
                                break;
                            }
                        }
                        if(is_two)
                        {
                            if(is_spot_on_board(Point(i, j-1)) && is_spot_on_board(Point(i, j-2)) && is_spot_on_board(Point(i, j+2)))
                            {
                                if(is_spot_on_board(Point(i, j-1)) && is_spot_on_board(Point(i, j-2)))
                                {
                                    if(get_disc(Point(i, j+2)) == EMPTY)
                                        state_value += 15;
                                    else 
                                        state_value += 5;
                                }
                            }
                            else if(is_spot_on_board(Point(i, j+2)) && is_spot_on_board(Point(i, j+3)) && is_spot_on_board(Point(i, j-1)))
                            {
                                if(get_disc(Point(i, j+2)) == EMPTY && get_disc(Point(i, j+3)) == player)
                                {
                                    if(get_disc(Point(i, j-1)) == EMPTY)
                                        state_value += 15;
                                    else 
                                        state_value += 5;
                                }
                            }
                            state_value += 2;
                        }
                    }
                    if(j + 1 < SIZE && i + 1 < SIZE)
                    {
                        bool is_two = true;
                        for(int k = 0; k < 2; k++)
                        {
                            if(!is_disc_at(Point(i + k, j + k), player))
                            {
                                is_two = false;
                                break;
                            }
                        }
                        if(is_two)
                        {
                            if(is_spot_on_board(Point(i-1, j-1)) && is_spot_on_board(Point(i-2, j-2)) && is_spot_on_board(Point(i+2, j+2)))
                            {
                                if(get_disc(Point(i-1, j-1)) == EMPTY && get_disc(Point(i-2, j-2)) == player)
                                {
                                    if(get_disc(Point(i+2, j+2)) == EMPTY)
                                        state_value += 15;
                                    else 
                                        state_value += 5;
                                }
                            }
                            else if(is_spot_on_board(Point(i+2, j+2)) && is_spot_on_board(Point(i+3, j+3)) && is_spot_on_board(Point(i-1, j-1)))
                            {
                                if(get_disc(Point(i+2, j+2)) == EMPTY && get_disc(Point(i+3, j+3)) == player)
                                {
                                    if(get_disc(Point(i-1, j-1)) == EMPTY)
                                        state_value += 15;
                                    else 
                                        state_value += 5;
                                }
                            }
                            state_value += 2;
                        }
                    }
                    if(j + 1 < SIZE && i - 1 < SIZE)
                    {
                        bool is_two = true;
                        for(int k = 0; k < 2; k++)
                        {
                            if(!is_disc_at(Point(i - k, j + k), player))
                            {
                                is_two = false;
                                break;
                            }
                        }
                        if(is_two)
                        {
                            if(is_spot_on_board(Point(i-1, j+2)) && is_spot_on_board(Point(i-2, j+3)) && is_spot_on_board(Point(i+1, j-1)))
                            {
                                if(get_disc(Point(i-1, j+2)) == EMPTY && get_disc(Point(i-2, j+3)) == player)
                                {
                                    if(get_disc(Point(i+1, j-1)) == EMPTY)
                                        state_value += 15;
                                    else
                                        state_value += 5;
                                }
                            }
                            else if(is_spot_on_board(Point(i+1, j-1)) && is_spot_on_board(Point(i+2, j-2)) && is_spot_on_board(Point(i-1, j +2)))
                            {
                                if(get_disc(Point(i+1, j-1)) == EMPTY && get_disc(Point(i+2, j-2)) == player)
                                {
                                    if(get_disc(Point(i-1, j+2)) == EMPTY)
                                        state_value += 15;
                                    else
                                        state_value += 5;
                                }
                            }
                            state_value += 2;
                        }
                    }
                
                    if(i + 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i + k, j), player))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i - 1, j)) && is_spot_on_board(Point(i + 3, j)))
                            {
                                if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 3, j)) == EMPTY)
                                {
                                    state_value += 5;
                                }
                            }
                            state_value += 5;
                        }
                    }
                    if(j + 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i, j + k), player))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i, j - 1)) && is_spot_on_board(Point(i, j + 3)))
                            {
                                if(get_disc(Point(i, j - 1)) == EMPTY && get_disc(Point(i, j + 3)) == EMPTY)
                                {
                                    state_value += 5;
                                }
                            }
                            state_value += 5;
                        }
                    }
                    if(j + 2 < SIZE && i + 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i + k, j + k), player))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i - 1, j - 1)) && is_spot_on_board(Point(i + 3, j + 3)))
                            {
                                if(get_disc(Point(i - 1, j - 1)) == EMPTY && get_disc(Point(i + 3, j + 3)) == EMPTY)
                                {
                                    state_value += 5;
                                }
                            }
                            state_value += 5;
                        }
                    }
                    if(j + 2 < SIZE && i - 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i - k, j + k), player))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i - 3, j + 3)) && is_spot_on_board(Point(i + 1, j - 1)))
                            {
                                if(get_disc(Point(i - 3, j + 3)) == EMPTY && get_disc(Point(i + 1, j - 1)) == EMPTY)
                                {
                                    state_value += 5;
                                }
                            }
                            state_value += 5;
                        }
                    }

                    if(i + 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i + k, j), player))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i - 1, j)) && is_spot_on_board(Point(i + 4, j)))
                            {
                                if(get_disc(Point(i - 1, j)) == EMPTY || get_disc(Point(i + 4, j)) == EMPTY)
                                {
                                    if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 4, j)) == EMPTY)
                                    {
                                        state_value += 20;
                                    }
                                    else
                                    {
                                        state_value -= 15;
                                    }
                                }
                            }
                            // state_value += 0;
                        }
                    }
                    if(j + 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i, j + k), player))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i, j - 1)) && is_spot_on_board(Point(i, j + 4)))
                            {
                                if(get_disc(Point(i, j - 1)) == EMPTY || get_disc(Point(i, j + 4)) == EMPTY)
                                {
                                    if(get_disc(Point(i, j - 1)) == EMPTY && get_disc(Point(i, j + 4)) == EMPTY)
                                    {
                                        state_value += 20;
                                    }
                                    else 
                                    {
                                        state_value -= 15;
                                    }
                                }
                            }
                            // state_value += 0;
                        }
                    }
                    if(j + 3 < SIZE && i + 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i + k, j + k), player))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i - 1, j - 1)) && is_spot_on_board(Point(i + 4, j + 4)))
                            {
                                if(get_disc(Point(i - 1, j - 1)) == EMPTY || get_disc(Point(i + 4, j + 4)) == EMPTY)
                                {
                                    if(get_disc(Point(i - 1, j - 1)) == EMPTY && get_disc(Point(i + 4, j + 4)) == EMPTY)
                                    {
                                        state_value += 20;
                                    }
                                    else
                                    {
                                        state_value -= 15;
                                    }
                                }
                            }
                            // state_value += 0;
                        }
                    }
                    if(j + 3 < SIZE && i - 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i - k, j + k), player))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i - 1, j + 4)) && is_spot_on_board(Point(i + 4, j - 1)))
                            {
                                if(get_disc(Point(i - 1, j + 4)) == EMPTY || get_disc(Point(i + 4, j - 1)) == EMPTY)
                                {
                                    if(get_disc(Point(i - 1, j + 4)) == EMPTY && get_disc(Point(i + 4, j - 1)) == EMPTY)
                                    {
                                        state_value += 20;
                                    }
                                    else
                                    {
                                        state_value -= 15;
                                    }
                                }
                            }
                            // state_value += 0;
                        }
                    }

                }
                else if(is_disc_at(Point(i, j), get_next_player(player)))
                {
                    int opponent = get_next_player(player);

                    if(i + 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i + k, j), opponent))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i - 1, j)) && is_spot_on_board(Point(i + 3, j)))
                            {
                                if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 3, j)) == EMPTY)
                                {
                                    state_value -= 38;
                                }
                            }
                            state_value -= 12;
                        }
                    }
                    if(j + 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i, j + k), opponent))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i, j - 1)) && is_spot_on_board(Point(i, j + 3)))
                            {
                                if(get_disc(Point(i, j - 1)) == EMPTY && get_disc(Point(i, j + 3)) == EMPTY)
                                {
                                    state_value -= 38;
                                }
                            }
                            state_value -= 12;
                        }
                    }
                    if(j + 2 < SIZE && i + 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i + k, j + k), opponent))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i - 1, j - 1)) && is_spot_on_board(Point(i + 3, j + 3)))
                            {
                                if(get_disc(Point(i - 1, j - 1)) == EMPTY && get_disc(Point(i + 3, j + 3)) == EMPTY)
                                {
                                    state_value -= 38;
                                }
                            }
                            state_value -= 12;
                        }
                    }
                    if(j + 2 < SIZE && i - 2 < SIZE)
                    {
                        bool is_three = true;
                        for(int k = 0; k < 3; k++)
                        {
                            if(!is_disc_at(Point(i - k, j + k), opponent))
                            {
                                is_three = false;
                                break;
                            }
                        }
                        if(is_three)
                        {
                            if(is_spot_on_board(Point(i + 3, j - 1)) && is_spot_on_board(Point(i - 1, j + 3)))
                            {
                                if(get_disc(Point(i + 3, j - 1)) == EMPTY && get_disc(Point(i - 1, j + 3)) == EMPTY)
                                {
                                    state_value -= 38;
                                }
                            }
                            state_value -= 12;
                        }
                    }

                    if(i + 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i + k, j), opponent))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i - 1, j)) && is_spot_on_board(Point(i + 4, j)))
                            {
                                if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 4, j)) == EMPTY)
                                {
                                    if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 4, j)) == EMPTY)
                                    {
                                        state_value -= 30;
                                    }
                                    else
                                    {
                                        state_value -= 20;
                                    }
                                }
                            }
                            state_value -= 20;
                        }
                    }
                    if(j + 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i, j + k), opponent))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i, j - 1)) && is_spot_on_board(Point(i, j + 4)))
                            {
                                if(get_disc(Point(i, j - 1)) == EMPTY || get_disc(Point(i, j + 4)) == EMPTY)
                                {
                                    if(get_disc(Point(i, j - 1)) == EMPTY && get_disc(Point(i, j + 4)) == EMPTY)
                                    {
                                        state_value -= 30;
                                    }
                                    else
                                    {
                                        state_value -= 20;
                                    }
                                }
                            }
                            state_value -= 20;
                        }
                    }
                    if(j + 3 < SIZE && i + 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i + k, j + k), opponent))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i - 1, j - 1)) && is_spot_on_board(Point(i + 4, j + 4)))
                            {
                                if(get_disc(Point(i - 1, j - 1)) == EMPTY || get_disc(Point(i + 4, j + 4)) == EMPTY)
                                {
                                    if(get_disc(Point(i - 1, j - 1)) == EMPTY && get_disc(Point(i + 4, j + 4)) == EMPTY)
                                    {
                                        state_value -= 30;
                                    }
                                    else
                                    {
                                        state_value -= 20;
                                    }
                                }
                            }
                            state_value -= 20;
                        }
                    }
                    if(j + 3 < SIZE && i - 3 < SIZE)
                    {
                        bool is_four = true;
                        for(int k = 0; k < 4; k++)
                        {
                            if(!is_disc_at(Point(i - k, j + k), opponent))
                            {
                                is_four = false;
                                break;
                            }
                        }
                        if(is_four)
                        {
                            if(is_spot_on_board(Point(i + 4, j - 1)) && is_spot_on_board(Point(i - 1, j + 4)))
                            {
                                if(get_disc(Point(i + 4, j - 1)) == EMPTY || get_disc(Point(i - 1, j + 4)) == EMPTY)
                                {
                                    if(get_disc(Point(i + 4, j - 1)) == EMPTY && get_disc(Point(i - 1, j + 4)) == EMPTY)
                                    {
                                        state_value -= 30;
                                    }
                                    else
                                    {
                                        state_value -= 20;
                                    }
                                }
                            }
                            state_value -= 20;
                        }
                    }

                }
            }
        }
        return state_value;
    }
};

int player;
Board board;
array<array<int, SIZE>, SIZE> score;

void read_board(ifstream &fin)
{
    fin >> player;
    board.empty_count = 0;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fin >> board.my_board[i][j];
            if(board.my_board[i][j] == EMPTY)
                board.empty_count++;
        }
    }
}

void write_valid_spot(ofstream &fout, int x, int y)
{
    fout << x << " " << y << endl;
    fout.flush();
}

// bd = board


// void giveAllValue(Board bd) 
// {
//     for(int i = 0; i < SIZE; i++)
//     {
//         for(int j = 0; j < SIZE; j++)
//         {
//             if(bd.get_disc(Point(i, j)) == EMPTY)
//             {
//                 setValue(i, j);
//             }
//         }
//     }
// }

int rec = 0;

int recur(ofstream &fout, Board bd, int depth, int alpha, int beta)
{
    if(bd.checkwin(board.my_player))
        return MAX - 1;
    else if(bd.checkwin(board.get_next_player(board.my_player)))
        return MIN + 1;

    //giveAllValue(bd);

    rec++;


    if (depth >= 3)
    {
        int k = bd.eval(player);
        return k;
    }

    bool prune = false;

    if (bd.my_player == player)
    {
        int best = MIN;
        for (int i = 0; i < SIZE; i++)
        {
            for(int j = 0; j < SIZE; j++) {
                if(bd.get_disc(Point(i, j)) != EMPTY)
                    continue;
                bool near = false;
                for(Point dir : directions){
                    Point p = Point(i, j) + dir;
                    if(!bd.is_spot_on_board(p))
                        continue;
                    if(bd.get_disc(p) != EMPTY){
                        near = true;
                        break;
                    }
                }
                if(!near)
                    continue;
                Board tmp = bd;
                Point p(i, j);
                if(tmp.put_disc(p))
                    tmp.my_player = tmp.get_next_player(tmp.my_player);
                else
                    continue;
                int val = recur(fout, tmp, depth + 1, alpha, beta);

                if (best < val)
                {
                    if (depth == 0)
                    {
                        write_valid_spot(fout, p.x, p.y);
                        //fout << rec << endl;
                        
                        //fout << "val: " << val << endl;
                        rec = 0;
                    }
                    best = val;
                }
                alpha = max(best, alpha);

                
                if (beta <= alpha)
                {
                    prune = true;
                    break;
                }
            }
            if(prune)
                break;
        }
        if (depth != 0)
            return best;
    }
    else
    {
        int best = MAX;
        for (int i = 0; i < SIZE; i++)
        {
            for(int j = 0; j < SIZE; j++) {
                Board tmp = bd;
                Point p(i, j);
                if(tmp.put_disc(p))
                    tmp.my_player = tmp.get_next_player(tmp.my_player);
                else
                    continue;
                int val = recur(fout, tmp, depth + 1, alpha, beta);

                best = min(best, val);
                beta = min(best, beta);
            }
        }
        if (depth != 0)
            return best;
    }
    return 1112;
}

int main(int, char **argv)
{
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    read_board(fin);

    board.my_player = player;
    if(board.empty_count == SIZE * SIZE){
        write_valid_spot(fout, 7, 7);
    }
    else
        recur(fout, board, 0, MIN, MAX);

    fin.close();
    fout.close();
    return 0;
}
