#ifndef NTECHLAB_TESTTASK_MUXMAP_HPP
#define NTECHLAB_TESTTASK_MUXMAP_HPP
#include <boost/algorithm/string/split.hpp>
#include <boost/utility/string_view.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>



template<typename Handler>
class MuxMap {
    struct PathNode {
        std::optional<Handler> handler;
        std::string nodeName;
        bool handleChildren = false;
        std::vector<std::shared_ptr<PathNode>> children;
        std::weak_ptr<PathNode> parent;
    };

    std::shared_ptr<PathNode> root = nullptr;

    std::shared_ptr<PathNode> findPathNode(boost::string_view path);

    std::shared_ptr<PathNode> createPathNode(boost::string_view path);

    bool deleteNode(boost::string_view path);

    static std::vector<std::string> splitPath(boost::string_view path);

public:
    MuxMap() = default;
    MuxMap(const MuxMap &) = delete;
    MuxMap(MuxMap &&) = delete;

    template<typename Arg>
    bool insertHandler(boost::string_view path, bool handleChildren, Arg &&handler);

    bool containsHandler(boost::string_view path);

    template<typename... Args>
    bool callHandler(boost::string_view path, Args&&... args);
    bool deleteHandler(boost::string_view path);
};


template<typename Handler>
bool MuxMap<Handler>::containsHandler(boost::string_view path) {
    auto node = findPathNode(path);
    if (!node) return false;
    if (node->handler)
        return true;
    return false;
}

template<typename Handler>
bool MuxMap<Handler>::deleteHandler(boost::string_view path) {
    const auto node = findPathNode(path);
    if (!node) return false;
    node->handler.reset();
    if (node->children.size() == 0)
        return deleteNode(path);
    return true;
}

template<typename Handler>
template<typename... Args>
bool MuxMap<Handler>::callHandler(boost::string_view path, Args&&... args) {
    const auto node = findPathNode(path);
    if (!node) return false;
    if (!node->handler) return false;
    node->handler.value()(std::forward<Args>(args)...);
    return true;
}


template<typename Handler>
template<typename Arg>
bool MuxMap<Handler>::insertHandler(boost::string_view path, bool handleChildren, Arg &&handler) {
    auto node = findPathNode(path);
    if (!node) {
        node = createPathNode(path);
        if (!node) return false;
    }
    if (node->handler) return false;
    node->handleChildren = handleChildren;
    node->handler = std::forward<Arg>(handler);
    return true;
}


template<typename Handler>
bool MuxMap<Handler>::deleteNode(boost::string_view path) {
    const auto node = findPathNode(path);
    if (!node) return false;
    // Root node check:
    if (node->parent.expired()) return false;
    const auto parent = node->parent.lock();
    parent->children.erase(std::remove_if(parent->children.begin(), parent->children.end(),
                                          [&node](const auto &n) { return n == node; }),
                           parent->children.end());
    return true;
}

template<typename Handler>
std::shared_ptr<typename MuxMap<Handler>::PathNode> MuxMap<Handler>::findPathNode(boost::string_view path) {
    if (!root) return nullptr;
    const auto tokens = splitPath(path);
    if (tokens.size() <= 1)
        return root;
    auto currentNode = root;
    for (int i = 1; i < tokens.size(); i++) {
        const auto &token = tokens[i];
        if (currentNode->children.size() == 0)
            return nullptr;// node has no children, so no node found
        for (const auto &child : currentNode->children) {
            if (child->nodeName == token && child->handleChildren)
                // if node's name fits token and node takes handles children's requests, then found
                return child;
            if (child->nodeName == token) {
                // if node's name fits but id does handles children's requests,
                // then check it's children
                currentNode = child;
                break;
            }
            // Dead end, none of currentNode's children fits our path
            return nullptr;
        }
    }
    // If we hit a dead end we would've exited earlier,
    // so since we are here, currentNode is the node we are looking for
    return currentNode;
}


template<typename Handler>
std::vector<std::string> MuxMap<Handler>::splitPath(boost::string_view path) {
    if (path.empty()) return {};
    if (path.length() == 1 && path[0] == '/') return {"/"};
    std::vector<std::string> pathTokens;
    const auto queryStrBeg = path.find('?');
    if (queryStrBeg != path.npos)
        path.remove_suffix(path.size() - queryStrBeg);
    boost::algorithm::split(
            pathTokens, path, [](char c) { return c == '/'; }, boost::token_compress_on);
    pathTokens.erase(std::remove_if(pathTokens.begin(), pathTokens.end(), [](const std::string &s) { return s.empty(); }), pathTokens.end());
    pathTokens.insert(pathTokens.begin(), "/");
    return pathTokens;
}


template<typename Handler>
std::shared_ptr<typename MuxMap<Handler>::PathNode> MuxMap<Handler>::createPathNode(boost::string_view path) {
    if (!root)
        root = std::make_shared<typename MuxMap<Handler>::PathNode>();
    root->nodeName = "/";
    if (path.length() == 1 && path[0] == '/')
        return root;
    if (findPathNode(path)) return nullptr;

    const auto tokens = splitPath(path);
    auto currentNode = root;
    std::shared_ptr<typename MuxMap<Handler>::PathNode> createdNode = nullptr;
    for (int i = 1; i < tokens.size(); i++) {
        const auto &token = tokens[i];
        bool empty = currentNode->children.size() == 0;
        auto foundNodeIt = std::end(currentNode->children);
        if (!empty)
            foundNodeIt = std::find_if(currentNode->children.begin(),
                                       currentNode->children.end(), [&token](const auto &node) { return node->nodeName == token; });

        // Intermediate node exists
        if (foundNodeIt != std::end(currentNode->children)) {
            currentNode = *foundNodeIt;
        } else {
            // Intermediate node does not exist
            // So create new one
            createdNode = std::make_shared<typename MuxMap<Handler>::PathNode>();
            createdNode->nodeName = token;
            createdNode->parent = currentNode;
            currentNode->children.push_back(createdNode);
            currentNode = createdNode;
        }
    }
    return createdNode;
}


#endif//NTECHLAB_TESTTASK_MUXMAP_HPP
