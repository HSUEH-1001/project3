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
        int state_value = 0;
        
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
                            // 判斷i-1、i-2、i+2這三個位置是不是在合理的位置
                            if(is_spot_on_board(Point(i-1, j)) && is_spot_on_board(Point(i-2, j)) && is_spot_on_board(Point(i + 2, j))) 
                            {
                                // 判斷i-1的位置為空的且i-2的位置是自己的棋
                                if((get_disc(Point(i-1, j)) == EMPTY && get_disc(Point(i-2, j)) == player))
                                {
                                    // 同時i+2的地方若是空的，即會有機會讓下下步棋形成活四，因此讓state value加多一點
                                    if(get_disc(Point(i+2, j)) == EMPTY)
                                        state_value += 15;
                                    // 若i+2的地方不是空的，還是有機會形成四顆棋連線，因此也讓state value多加一點點
                                    else 
                                        state_value += 5;
                                }
                            }
                            // 同理，這裡為判斷另一個方向是否有機會形成活四的狀態
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
                            // 判斷i-1、i+3的位置是否在合理的地方
                            if(is_spot_on_board(Point(i - 1, j)) && is_spot_on_board(Point(i + 3, j)))
                            {
                                // 若兩個位置皆為空的，即會形成活三的情形，因此讓state value多加5
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
                            // 判斷i-1、i+4的位置是否為合理的位置
                            if(is_spot_on_board(Point(i - 1, j)) && is_spot_on_board(Point(i + 4, j)))
                            {
                                if(get_disc(Point(i - 1, j)) == EMPTY || get_disc(Point(i + 4, j)) == EMPTY)
                                {
                                    // 若兩邊的位置皆為空，即會形成活四的情形，因此讓state value多加20
                                    if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 4, j)) == EMPTY)
                                    {
                                        state_value += 20;
                                    }
                                    // 不會形成活四的話表示一定會被阻擋，有點多浪費一步的感覺，因此state value只加5
                                    else
                                    {
                                        state_value += 5;
                                    }
                                }
                            }
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
                                        state_value += 5;
                                    }
                                }
                            }
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
                                        state_value += 5;
                                    }
                                }
                            }
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
                                        state_value += 5;
                                    }
                                }
                            }
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
                                // 即為活三的情形，若不防守即會形成活四，而活四即無解，因此要多減一些讓此情形不要發生
                                if(get_disc(Point(i - 1, j)) == EMPTY && get_disc(Point(i + 3, j)) == EMPTY)
                                {
                                    state_value -= 45;
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
                                    state_value -= 45;
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
                                    state_value -= 45;
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
                                    state_value -= 45;
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
                                        state_value -= 20;
                                    }
                                    //
                                    else
                                    {
                                        state_value -= 30;
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
                                        state_value -= 20;
                                    }
                                    else
                                    {
                                        state_value -= 30;
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
                                        state_value -= 20;
                                    }
                                    else
                                    {
                                        state_value -= 30;
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
                                        state_value -= 20;
                                    }
                                    else
                                    {
                                        state_value -= 30;
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



int recur(ofstream &fout, Board bd, int depth, int alpha, int beta)
{
    if(bd.checkwin(board.my_player))    // 若自己下一步就要贏了即return最大值 
        return MAX - 1;
    else if(bd.checkwin(board.get_next_player(board.my_player)))
        return MIN + 1;                 // 若是對方下一步就要贏了即return最小值


    if (depth >= 3)                     // 設定當遞迴跑到第三層
    {
        int k = bd.eval(player);        // 呼叫eval計算當時的state_value
        return k;
    }

    bool prune = false;

    if (bd.my_player == player)        
    {
        int best = MIN;                 
        for (int i = 0; i < SIZE; i++)
        {
            for(int j = 0; j < SIZE; j++) {                 // 跑完整個棋盤去做以下的判斷
                if(bd.get_disc(Point(i, j)) != EMPTY)       // 若在這個地方不是空的即繼續以下的判斷
                    continue;
                bool near = false;
                for(Point dir : directions){                
                    Point p = Point(i, j) + dir;            
                    if(!bd.is_spot_on_board(p))
                        continue;
                    if(bd.get_disc(p) != EMPTY){            // 判斷這個點的各個方向是否有旗子
                        near = true;                        // 有的話就把near設為true
                        break;
                    }
                }
                if(!near)                                   // 若near不是true我就不在這裡下棋
                    continue;
                Board tmp = bd;
                Point p(i, j);
                if(tmp.put_disc(p))
                    tmp.my_player = tmp.get_next_player(tmp.my_player);
                else
                    continue;
                int val = recur(fout, tmp, depth + 1, alpha, beta); // depth + 1後繼續遞迴

                if (best < val)
                {
                    if (depth == 0)
                    {
                        write_valid_spot(fout, p.x, p.y);
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
            if(prune)               // prune表示已經在上一層的for迴圈break過，因此若prune是true，即break
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