#include "Backtracking.h"

#include <algorithm>
#include <functional>

namespace backtracking {

long long factorial(int n)
{
    return n <= 1 ? 1 : n * factorial(n - 1);
}

void hanoi(int n, char from, char via, char to, std::vector<std::string>& moves)
{
    if (n == 0)
        return;
    hanoi(n - 1, from, to, via, moves); // 上面 n-1 个先挪到中转柱
    moves.push_back(std::string(1, from) + "->" + to);
    hanoi(n - 1, via, from, to, moves); // 再从中转柱挪到目标柱
}

std::vector<std::vector<int>> permutations(const std::vector<int>& nums)
{
    std::vector<std::vector<int>> out;
    std::vector<int> path;
    std::vector<bool> used(nums.size(), false);
    std::function<void()> dfs = [&]() {
        if (path.size() == nums.size()) {
            out.push_back(path);
            return;
        }
        for (std::size_t i = 0; i < nums.size(); ++i) {
            if (used[i])
                continue;
            used[i] = true;
            path.push_back(nums[i]);
            dfs();
            path.pop_back();
            used[i] = false;
        }
    };
    dfs();
    return out;
}

std::vector<std::vector<int>> permutations_unique(std::vector<int> nums)
{
    std::sort(nums.begin(), nums.end());
    std::vector<std::vector<int>> out;
    std::vector<int> path;
    std::vector<bool> used(nums.size(), false);
    std::function<void()> dfs = [&]() {
        if (path.size() == nums.size()) {
            out.push_back(path);
            return;
        }
        for (std::size_t i = 0; i < nums.size(); ++i) {
            if (used[i])
                continue;
            // 剪枝：相同的数只允许按顺序使用，前一个相同的数没用过就跳过
            if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1])
                continue;
            used[i] = true;
            path.push_back(nums[i]);
            dfs();
            path.pop_back();
            used[i] = false;
        }
    };
    dfs();
    return out;
}

std::vector<std::vector<int>> subsets(const std::vector<int>& nums)
{
    std::vector<std::vector<int>> out;
    std::vector<int> path;
    std::function<void(std::size_t)> dfs = [&](std::size_t start) {
        out.push_back(path); // 每个结点都是一个子集
        for (std::size_t i = start; i < nums.size(); ++i) {
            path.push_back(nums[i]);
            dfs(i + 1);
            path.pop_back();
        }
    };
    dfs(0);
    return out;
}

std::vector<std::vector<int>> combinations(int n, int k)
{
    std::vector<std::vector<int>> out;
    std::vector<int> path;
    std::function<void(int)> dfs = [&](int start) {
        if (static_cast<int>(path.size()) == k) {
            out.push_back(path);
            return;
        }
        // 剪枝：剩下的数不够凑满 k 个就不用继续
        int need = k - static_cast<int>(path.size());
        for (int i = start; i <= n - need + 1; ++i) {
            path.push_back(i);
            dfs(i + 1);
            path.pop_back();
        }
    };
    dfs(1);
    return out;
}

std::vector<std::vector<int>> combination_sum(std::vector<int> candidates, int target)
{
    std::sort(candidates.begin(), candidates.end());
    std::vector<std::vector<int>> out;
    std::vector<int> path;
    std::function<void(std::size_t, int)> dfs = [&](std::size_t start, int remain) {
        if (remain == 0) {
            out.push_back(path);
            return;
        }
        for (std::size_t i = start; i < candidates.size(); ++i) {
            if (candidates[i] > remain)
                break; // 已排序，后面的更大，全部剪掉
            path.push_back(candidates[i]);
            dfs(i, remain - candidates[i]); // 传 i 而不是 i+1：同一个数可以重复选
            path.pop_back();
        }
    };
    dfs(0, target);
    return out;
}

std::vector<std::vector<std::string>> n_queens(int n)
{
    std::vector<std::vector<std::string>> out;
    std::vector<std::string> board(n, std::string(n, '.'));
    std::vector<bool> col(n, false), diag1(2 * n, false), diag2(2 * n, false);
    std::function<void(int)> dfs = [&](int row) {
        if (row == n) {
            out.push_back(board);
            return;
        }
        for (int c = 0; c < n; ++c) {
            // 同一条主对角线 row - c 相同，副对角线 row + c 相同
            if (col[c] || diag1[row - c + n] || diag2[row + c])
                continue;
            col[c] = diag1[row - c + n] = diag2[row + c] = true;
            board[row][c] = 'Q';
            dfs(row + 1);
            board[row][c] = '.';
            col[c] = diag1[row - c + n] = diag2[row + c] = false;
        }
    };
    dfs(0);
    return out;
}

int n_queens_count(int n)
{
    // cols / ld / rd 的每一位表示该列是否被占用（已考虑对角线在下一行的投影）
    const unsigned full = (1u << n) - 1;
    std::function<int(unsigned, unsigned, unsigned)> dfs = [&](unsigned cols, unsigned ld, unsigned rd) {
        if (cols == full)
            return 1;
        int count = 0;
        unsigned avail = full & ~(cols | ld | rd);
        while (avail) {
            unsigned bit = avail & (~avail + 1); // 取最低位的 1
            avail ^= bit;
            count += dfs(cols | bit, (ld | bit) << 1, (rd | bit) >> 1);
        }
        return count;
    };
    return n <= 0 ? 0 : dfs(0, 0, 0);
}

bool solve_sudoku(std::vector<std::string>& board)
{
    // 用位掩码记录每行 / 每列 / 每宫已用的数字
    int row[9] = {0}, col[9] = {0}, box[9] = {0};
    std::vector<std::pair<int, int>> empty;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board[r][c] == '.') {
                empty.push_back({r, c});
            } else {
                int bit = 1 << (board[r][c] - '1');
                int b = r / 3 * 3 + c / 3;
                if ((row[r] | col[c] | box[b]) & bit)
                    return false; // 初始盘面就有冲突
                row[r] |= bit;
                col[c] |= bit;
                box[b] |= bit;
            }
        }
    }
    std::function<bool(std::size_t)> dfs = [&](std::size_t k) {
        if (k == empty.size())
            return true;
        // 启发式：优先填候选数最少的格子（MRV），大幅减少搜索量
        std::size_t best = k;
        int best_count = 10;
        for (std::size_t i = k; i < empty.size(); ++i) {
            auto [r, c] = empty[i];
            int used = row[r] | col[c] | box[r / 3 * 3 + c / 3];
            int cnt = 9;
            for (int x = used; x; x &= x - 1)
                --cnt;
            if (cnt < best_count) {
                best_count = cnt;
                best = i;
            }
        }
        std::swap(empty[k], empty[best]);
        auto [r, c] = empty[k];
        int b = r / 3 * 3 + c / 3;
        int avail = ~(row[r] | col[c] | box[b]) & 0x1FF;
        for (; avail; avail &= avail - 1) {
            int bit = avail & -avail;
            int d = 0;
            while ((1 << d) != bit)
                ++d;
            row[r] |= bit; col[c] |= bit; box[b] |= bit;
            board[r][c] = static_cast<char>('1' + d);
            if (dfs(k + 1))
                return true;
            row[r] ^= bit; col[c] ^= bit; box[b] ^= bit;
            board[r][c] = '.';
        }
        std::swap(empty[k], empty[best]);
        return false;
    };
    return dfs(0);
}

std::vector<std::string> generate_parentheses(int n)
{
    std::vector<std::string> out;
    std::string path;
    std::function<void(int, int)> dfs = [&](int open, int close) {
        if (static_cast<int>(path.size()) == 2 * n) {
            out.push_back(path);
            return;
        }
        if (open < n) {
            path.push_back('(');
            dfs(open + 1, close);
            path.pop_back();
        }
        if (close < open) { // 右括号数不能超过左括号数
            path.push_back(')');
            dfs(open, close + 1);
            path.pop_back();
        }
    };
    dfs(0, 0);
    return out;
}

} // namespace backtracking
