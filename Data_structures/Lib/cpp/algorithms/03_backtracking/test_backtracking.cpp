#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Backtracking.h"
#include "check.h"

using VV = std::vector<std::vector<int>>;
using namespace backtracking;

static bool valid_sudoku(const std::vector<std::string>& b)
{
    for (int i = 0; i < 9; ++i) {
        std::set<char> r, c, box;
        for (int j = 0; j < 9; ++j) {
            r.insert(b[i][j]);
            c.insert(b[j][i]);
            box.insert(b[i / 3 * 3 + j / 3][i % 3 * 3 + j % 3]);
        }
        if (r.size() != 9 || c.size() != 9 || box.size() != 9 || r.count('.'))
            return false;
    }
    return true;
}

int main()
{
    TEST_SECTION("recursion");
    CHECK(factorial(0) == 1 && factorial(10) == 3628800);
    std::vector<std::string> moves;
    hanoi(3, 'A', 'B', 'C', moves);
    CHECK(moves.size() == 7 && moves.front() == "A->C" && moves.back() == "A->C");
    moves.clear();
    hanoi(10, 'A', 'B', 'C', moves);
    CHECK(moves.size() == 1023);

    TEST_SECTION("permutations");
    auto p = permutations({1, 2, 3});
    CHECK(p.size() == 6 && p.front() == (std::vector<int>{1, 2, 3}) && p.back() == (std::vector<int>{3, 2, 1}));
    auto pu = permutations_unique({1, 1, 2});
    CHECK(pu == (VV{{1, 1, 2}, {1, 2, 1}, {2, 1, 1}}));

    TEST_SECTION("subsets / combinations");
    auto s = subsets({1, 2, 3});
    CHECK(s.size() == 8 && s.front().empty());
    auto c = combinations(4, 2);
    CHECK(c == (VV{{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}}));
    CHECK(combinations(10, 3).size() == 120);
    auto cs = combination_sum({2, 3, 6, 7}, 7);
    CHECK(cs == (VV{{2, 2, 3}, {7}}));

    TEST_SECTION("N queens");
    auto q4 = n_queens(4);
    CHECK(q4.size() == 2);
    CHECK(q4[0] == (std::vector<std::string>{".Q..", "...Q", "Q...", "..Q."}));
    CHECK(n_queens(8).size() == 92);
    CHECK(n_queens_count(8) == 92 && n_queens_count(10) == 724 && n_queens_count(1) == 1);

    TEST_SECTION("sudoku");
    std::vector<std::string> board{
        "53..7....", "6..195...", ".98....6.", "8...6...3", "4..8.3..1",
        "7...2...6", ".6....28.", "...419..5", "....8..79",
    };
    const auto original = board;
    CHECK(solve_sudoku(board) && valid_sudoku(board));
    bool kept = true;
    for (int r = 0; r < 9; ++r)
        for (int col = 0; col < 9; ++col)
            if (original[r][col] != '.' && original[r][col] != board[r][col])
                kept = false;
    CHECK(kept);

    TEST_SECTION("parentheses");
    auto gp = generate_parentheses(3);
    CHECK(gp == (std::vector<std::string>{"((()))", "(()())", "(())()", "()(())", "()()()"}));
    CHECK(generate_parentheses(5).size() == 42); // 卡特兰数

    return CHECK_REPORT();
}
