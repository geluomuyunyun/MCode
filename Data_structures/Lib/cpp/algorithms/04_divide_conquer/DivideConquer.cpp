#include "DivideConquer.h"

#include <algorithm>
#include <array>
#include <climits>
#include <random>
#include <stdexcept>

namespace dc {

long long fast_pow(long long a, long long b, long long m)
{
    long long result = 1 % m;
    a %= m;
    if (a < 0)
        a += m;
    // b 的二进制：a^13 = a^8 * a^4 * a^1
    while (b > 0) {
        if (b & 1)
            result = result * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return result;
}

static long long max_sub(const std::vector<int>& a, int lo, int hi)
{
    if (lo == hi)
        return a[lo];
    int mid = lo + (hi - lo) / 2;
    long long left = max_sub(a, lo, mid);
    long long right = max_sub(a, mid + 1, hi);
    // 跨越中点的最大和 = 从 mid 向左的最大后缀 + 从 mid+1 向右的最大前缀
    long long sum = 0, best_l = LLONG_MIN, best_r = LLONG_MIN;
    for (int i = mid; i >= lo; --i)
        best_l = std::max(best_l, sum += a[i]);
    sum = 0;
    for (int i = mid + 1; i <= hi; ++i)
        best_r = std::max(best_r, sum += a[i]);
    return std::max({left, right, best_l + best_r});
}

long long max_subarray(const std::vector<int>& a)
{
    if (a.empty())
        throw std::invalid_argument("max_subarray: empty input");
    return max_sub(a, 0, static_cast<int>(a.size()) - 1);
}

static long long merge_count(std::vector<int>& a, std::vector<int>& buf, int lo, int hi)
{
    if (hi - lo < 2)
        return 0;
    int mid = lo + (hi - lo) / 2;
    long long inv = merge_count(a, buf, lo, mid) + merge_count(a, buf, mid, hi);
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        if (a[j] < a[i]) {
            inv += mid - i; // a[i..mid) 都比 a[j] 大，各构成一个逆序对
            buf[k++] = a[j++];
        } else {
            buf[k++] = a[i++];
        }
    }
    while (i < mid) buf[k++] = a[i++];
    while (j < hi) buf[k++] = a[j++];
    std::copy(buf.begin() + lo, buf.begin() + hi, a.begin() + lo);
    return inv;
}

long long count_inversions(std::vector<int> a)
{
    std::vector<int> buf(a.size());
    return merge_count(a, buf, 0, static_cast<int>(a.size()));
}

int quick_select(std::vector<int> a, int k)
{
    if (k < 0 || k >= static_cast<int>(a.size()))
        throw std::out_of_range("quick_select: k out of range");
    std::mt19937 rng(12345);
    int lo = 0, hi = static_cast<int>(a.size()) - 1;
    while (lo < hi) {
        // 随机枢轴 + 三路划分，只需要继续处理包含第 k 个的那一段
        int pivot = a[lo + static_cast<int>(rng() % static_cast<unsigned>(hi - lo + 1))];
        int lt = lo, i = lo, gt = hi;
        while (i <= gt) {
            if (a[i] < pivot) std::swap(a[lt++], a[i++]);
            else if (a[i] > pivot) std::swap(a[i], a[gt--]);
            else ++i;
        }
        if (k < lt) hi = lt - 1;
        else if (k > gt) lo = gt + 1;
        else return pivot;
    }
    return a[k];
}

using Point = std::pair<long long, long long>;

static long long dist2(const Point& a, const Point& b)
{
    long long dx = a.first - b.first, dy = a.second - b.second;
    return dx * dx + dy * dy;
}

// pts[lo, hi) 已按 x 排序；返回后该区间按 y 排序（顺便做归并）
static long long closest(std::vector<Point>& pts, std::vector<Point>& buf, int lo, int hi)
{
    if (hi - lo <= 3) {
        long long best = LLONG_MAX;
        for (int i = lo; i < hi; ++i)
            for (int j = i + 1; j < hi; ++j)
                best = std::min(best, dist2(pts[i], pts[j]));
        std::sort(pts.begin() + lo, pts.begin() + hi,
                  [](const Point& a, const Point& b) { return a.second < b.second; });
        return best;
    }
    int mid = lo + (hi - lo) / 2;
    long long mid_x = pts[mid].first;
    long long best = std::min(closest(pts, buf, lo, mid), closest(pts, buf, mid, hi));
    std::merge(pts.begin() + lo, pts.begin() + mid, pts.begin() + mid, pts.begin() + hi, buf.begin() + lo,
               [](const Point& a, const Point& b) { return a.second < b.second; });
    std::copy(buf.begin() + lo, buf.begin() + hi, pts.begin() + lo);

    // 只需检查离分割线 x 距离小于 sqrt(best) 的"带状区域"，每个点最多和后面几个点比较
    std::vector<Point> strip;
    for (int i = lo; i < hi; ++i) {
        long long dx = pts[i].first - mid_x;
        if (dx * dx < best)
            strip.push_back(pts[i]);
    }
    for (std::size_t i = 0; i < strip.size(); ++i) {
        for (std::size_t j = i + 1; j < strip.size(); ++j) {
            long long dy = strip[j].second - strip[i].second;
            if (dy * dy >= best)
                break;
            best = std::min(best, dist2(strip[i], strip[j]));
        }
    }
    return best;
}

long long closest_pair_dist2(std::vector<Point> pts)
{
    if (pts.size() < 2)
        throw std::invalid_argument("closest_pair: need at least two points");
    std::sort(pts.begin(), pts.end());
    std::vector<Point> buf(pts.size());
    return closest(pts, buf, 0, static_cast<int>(pts.size()));
}

long long fibonacci_mod(long long n, long long m)
{
    // [F(n+1) F(n); F(n) F(n-1)] = [1 1; 1 0]^n
    using Mat = std::array<std::array<long long, 2>, 2>;
    auto mul = [m](const Mat& x, const Mat& y) {
        Mat r{};
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                    r[i][j] = (r[i][j] + x[i][k] * y[k][j]) % m;
        return r;
    };
    Mat result{{{1 % m, 0}, {0, 1 % m}}};
    Mat base{{{1, 1}, {1, 0}}};
    for (; n > 0; n >>= 1) {
        if (n & 1)
            result = mul(result, base);
        base = mul(base, base);
    }
    return result[0][1];
}

} // namespace dc
