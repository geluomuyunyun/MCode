#include "Trie.h"

bool Trie::insert(const std::string& word)
{
    if (contains(word))
        return false;
    Node* cur = &root_;
    ++cur->pass;
    for (char c : word) {
        auto& child = cur->next[c];
        if (!child)
            child = std::make_unique<Node>();
        cur = child.get();
        ++cur->pass;
    }
    cur->end = true;
    ++words_;
    return true;
}

const Trie::Node* Trie::find_node(const std::string& s) const
{
    const Node* cur = &root_;
    for (char c : s) {
        auto it = cur->next.find(c);
        if (it == cur->next.end())
            return nullptr;
        cur = it->second.get();
    }
    return cur;
}

bool Trie::contains(const std::string& word) const
{
    const Node* n = find_node(word);
    return n && n->end;
}

bool Trie::starts_with(const std::string& prefix) const
{
    const Node* n = find_node(prefix);
    return n && n->pass > 0;
}

bool Trie::erase(const std::string& word)
{
    if (!contains(word))
        return false;
    Node* cur = &root_;
    --cur->pass;
    for (char c : word) {
        auto it = cur->next.find(c);
        Node* child = it->second.get();
        if (--child->pass == 0) {
            // 这个分支只属于被删的单词，整棵子树一次释放
            cur->next.erase(it);
            --words_;
            return true;
        }
        cur = child;
    }
    cur->end = false;
    --words_;
    return true;
}

std::size_t Trie::count_prefix(const std::string& prefix) const
{
    const Node* n = find_node(prefix);
    return n ? n->pass : 0;
}

void Trie::collect(const Node* n, std::string& path, std::vector<std::string>& out)
{
    if (n->end)
        out.push_back(path);
    for (const auto& [c, child] : n->next) {
        path.push_back(c);
        collect(child.get(), path, out);
        path.pop_back();
    }
}

std::vector<std::string> Trie::words_with_prefix(const std::string& prefix) const
{
    std::vector<std::string> out;
    const Node* n = find_node(prefix);
    if (!n)
        return out;
    std::string path = prefix;
    collect(n, path, out);
    return out;
}

std::string Trie::longest_common_prefix() const
{
    std::string lcp;
    if (words_ == 0)
        return lcp;
    const Node* cur = &root_;
    // 一直往下走，直到出现分叉或者某个单词在此结束
    while (cur->next.size() == 1 && !cur->end) {
        const auto& [c, child] = *cur->next.begin();
        lcp.push_back(c);
        cur = child.get();
    }
    return lcp;
}

std::string Trie::longest_prefix_of(const std::string& text) const
{
    const Node* cur = &root_;
    std::size_t best = 0;
    bool found = root_.end;
    for (std::size_t i = 0; i < text.size(); ++i) {
        auto it = cur->next.find(text[i]);
        if (it == cur->next.end())
            break;
        cur = it->second.get();
        if (cur->end) {
            best = i + 1;
            found = true;
        }
    }
    return found ? text.substr(0, best) : std::string();
}
