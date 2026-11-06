#include <optional>
#include <vector>

#include "BinaryTree.h"
#include "check.h"

using V = std::vector<int>;

int main()
{
    //         1
    //       /   \
    //      2     3
    //     / \     \
    //    4   5     6
    //       /
    //      7
    auto none = std::nullopt;
    auto t = BinaryTree<int>::from_level_order({1, 2, 3, 4, 5, none, 6, none, none, 7});

    TEST_SECTION("recursive traversals with lambdas");
    V pre, in, post;
    t.preorder([&](int v) { pre.push_back(v); });
    t.inorder([&](int v) { in.push_back(v); });
    t.postorder([&](int v) { post.push_back(v); });
    CHECK(pre == (V{1, 2, 4, 5, 7, 3, 6}));
    CHECK(in == (V{4, 2, 7, 5, 1, 3, 6}));
    CHECK(post == (V{4, 7, 5, 2, 6, 3, 1}));

    TEST_SECTION("iterative traversals");
    CHECK(t.preorder_iter() == pre);
    CHECK(t.inorder_iter() == in);
    CHECK(t.postorder_iter() == post);
    auto levels = t.level_order();
    CHECK(levels.size() == 4);
    CHECK(levels[1] == (V{2, 3}) && levels[2] == (V{4, 5, 6}) && levels[3] == (V{7}));

    TEST_SECTION("properties");
    CHECK(t.height() == 4 && t.size() == 7);
    CHECK(t.is_balanced()); // 每个结点左右子树高度差都不超过 1
    CHECK(!t.is_symmetric());
    auto skewed = BinaryTree<int>::from_level_order({1, 2, none, 3});
    CHECK(!skewed.is_balanced());
    CHECK(t.max_root_to_leaf_sum() == 1 + 2 + 5 + 7);

    TEST_SECTION("lowest common ancestor");
    CHECK(t.lowest_common_ancestor(4, 7) == 2);
    CHECK(t.lowest_common_ancestor(7, 6) == 1);
    CHECK(t.lowest_common_ancestor(5, 7) == 5);
    CHECK(!t.lowest_common_ancestor(4, 99).has_value());

    TEST_SECTION("rebuild / copy / mirror");
    auto r = BinaryTree<int>::from_pre_in(pre, in);
    CHECK(r == t);
    BinaryTree<int> copy = t; // 深拷贝
    copy.mirror();
    CHECK(!(copy == t));
    CHECK(copy.inorder_iter() == (V{6, 3, 1, 5, 7, 2, 4}));
    copy.mirror();
    CHECK(copy == t);

    auto sym = BinaryTree<int>::from_level_order({1, 2, 2, 3, 4, 4, 3});
    CHECK(sym.is_symmetric() && sym.is_balanced());

    TEST_SECTION("empty tree");
    BinaryTree<int> e;
    CHECK(e.empty() && e.height() == 0 && e.inorder_iter().empty() && e.level_order().empty());

    return CHECK_REPORT();
}
