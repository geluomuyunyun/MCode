#include "MonotonicStack.h"

#include <algorithm>

#include "Stack.h"

namespace mono {

std::vector<int> next_greater_index(const std::vector<int>& a)
{
    std::vector<int> ans(a.size(), -1);
    Stack<int> st; // 存下标，对应的值从栈底到栈顶递减
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        while (!st.empty() && a[st.top()] < a[i]) {
            ans[st.top()] = i;
            st.pop();
        }
        st.push(i);
    }
    return ans;
}

long long largest_rectangle(const std::vector<int>& heights)
{
    // 栈中下标对应的高度递增；某根柱子出栈时，它能向左右延伸的边界就确定了
    Stack<int> st;
    long long best = 0;
    const int n = static_cast<int>(heights.size());
    for (int i = 0; i <= n; ++i) {
        int h = i == n ? 0 : heights[i]; // 末尾放一个高度 0 的哨兵，清空栈
        while (!st.empty() && heights[st.top()] >= h) {
            long long height = heights[st.top()];
            st.pop();
            int left = st.empty() ? -1 : st.top();
            best = std::max(best, height * (i - left - 1));
        }
        st.push(i);
    }
    return best;
}

} // namespace mono
