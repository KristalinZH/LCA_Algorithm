#include <gtest/gtest.h>
#include "Tree.hpp"
#include "LCA.hpp"

// Test Fixture за дървото
class TreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Създаваме дървото за всеки тест наново
        // Структура:
        //         a
        //        / \
        //       b   e
        //      / \   \
        //     c   d   g
        //        / \
        //       h   f
        
        root = new Tree<std::string>("a");
        b = new Tree<std::string>("b");
        c = new Tree<std::string>("c");
        d = new Tree<std::string>("d");
        e = new Tree<std::string>("e");
        f = new Tree<std::string>("f");
        g = new Tree<std::string>("g");
        h = new Tree<std::string>("h");
        
        // Построяваме дървото
        d->addSubtree(h);
        d->addSubtree(f);
        b->addSubtree(c);
        b->addSubtree(d);
        e->addSubtree(g);
        root->addSubtree(b);
        root->addSubtree(e);
    }
    
    void TearDown() override {
        // Изтриваме дървото
        delete root;  // Това ще изтрие цялото дърво рекурсивно
    }
    
    Tree<std::string>* root;
    Tree<std::string>* b;
    Tree<std::string>* c;
    Tree<std::string>* d;
    Tree<std::string>* e;
    Tree<std::string>* f;
    Tree<std::string>* g;
    Tree<std::string>* h;
};

// =================== ТЕСТОВЕ ЗА TREE КЛАС ===================

TEST_F(TreeTest, ConstructorAndBasicAccess) {
    EXPECT_EQ(root->root(), "a");
    EXPECT_EQ(b->root(), "b");
    EXPECT_EQ(c->root(), "c");
}

TEST_F(TreeTest, TreeSize) {
    EXPECT_EQ(root->size(), 8);
    EXPECT_EQ(b->size(), 5);    // b, c, d, h, f
    EXPECT_EQ(d->size(), 3);    // d, h, f
    EXPECT_EQ(e->size(), 2);    // e, g
    EXPECT_EQ(c->size(), 1);    // само c
}

TEST_F(TreeTest, ChildrenAccess) {
    // Проверка деца на root
    auto& rootChildren = root->children();
    EXPECT_EQ(rootChildren.size(), 2);
    EXPECT_EQ((*rootChildren.begin())->root(), "b");
    EXPECT_EQ((*std::next(rootChildren.begin()))->root(), "e");
    
    // Проверка деца на b
    auto& bChildren = b->children();
    EXPECT_EQ(bChildren.size(), 2);
    EXPECT_EQ((*bChildren.begin())->root(), "c");
    EXPECT_EQ((*std::next(bChildren.begin()))->root(), "d");
}

TEST_F(TreeTest, SearchFunction) {
    // Търсене по стойност
    EXPECT_TRUE(root->search("a"));
    EXPECT_TRUE(root->search("b"));
    EXPECT_TRUE(root->search("c"));
    EXPECT_TRUE(root->search("d"));
    EXPECT_TRUE(root->search("e"));
    EXPECT_TRUE(root->search("f"));
    EXPECT_TRUE(root->search("g"));
    EXPECT_TRUE(root->search("h"));
    
    // Несъществуващи стойности
    EXPECT_FALSE(root->search("x"));
    EXPECT_FALSE(root->search("y"));
    EXPECT_FALSE(root->search("z"));
    
    // Търсене в поддърво
    EXPECT_TRUE(b->search("c"));
    EXPECT_TRUE(b->search("f"));
    EXPECT_TRUE(d->search("h"));
    EXPECT_FALSE(d->search("c"));  // c не е под d
}

TEST_F(TreeTest, EqualityOperators) {
    // Равенство на същия обект
    EXPECT_EQ(*root, *root);
    EXPECT_EQ(*b, *b);
    
    // Неравенство на различни възли
    EXPECT_NE(*root, *b);
    EXPECT_NE(*c, *d);
    
    // Копиране и проверка за равенство
    Tree<std::string> c_copy(*c);
    EXPECT_EQ(c_copy, *c);
    EXPECT_EQ(c_copy.root(), "c");
    EXPECT_EQ(c_copy.size(), 1);
}

TEST_F(TreeTest, MoveSemantics) {
    Tree<std::string> original("test");
    Tree<std::string>* child1 = new Tree<std::string>("child1");
    Tree<std::string>* child2 = new Tree<std::string>("child2");
    original.addSubtree(child1);
    original.addSubtree(child2);
    
    // Move конструктор
    Tree<std::string> moved(std::move(original));
    EXPECT_EQ(moved.root(), "test");
    EXPECT_EQ(moved.size(), 3);
    
    // Move assignment
    Tree<std::string> moved2("temp");
    moved2 = std::move(moved);
    EXPECT_EQ(moved2.root(), "test");
    EXPECT_EQ(moved2.size(), 3);
}

TEST_F(TreeTest, OutputStreamOperator) {
    std::ostringstream oss;
    oss << *c;
    EXPECT_EQ(oss.str(), "(c)");
    
    oss.str("");
    oss << *d;
    // Може да е "(d (h) (f))" или "(d (f) (h))" в зависимост от реда
    EXPECT_TRUE(oss.str() == "(d (h) (f))" || oss.str() == "(d (f) (h))");
    
    oss.str("");
    oss << *root;
    // Сложен output - тестваме само че съдържа ключови елементи
    std::string output = oss.str();
    EXPECT_TRUE(output.find("(a") == 0);  // Започва с (a
    EXPECT_TRUE(output.find("(b") != std::string::npos);
    EXPECT_TRUE(output.find("(e") != std::string::npos);
}

// =================== ТЕСТОВЕ ЗА LCA КЛАС ===================

class LCATest : public TreeTest {
protected:
    void SetUp() override {
        TreeTest::SetUp();  // Извикваме SetUp на родителя
        lca = new LCA<std::string>(root);
    }
    
    void TearDown() override {
        delete lca;
        TreeTest::TearDown();  // Извикваме TearDown на родителя
    }
    
    LCA<std::string>* lca;
};

TEST_F(LCATest, BasicLCAs) {
    // Основни тестове от примера ти
    EXPECT_EQ(lca->getLCA(c, f)->root(), "b");
    EXPECT_EQ(lca->getLCA(b, e)->root(), "a");
    EXPECT_EQ(lca->getLCA(h, f)->root(), "d");
    EXPECT_EQ(lca->getLCA(g, f)->root(), "a");
    EXPECT_EQ(lca->getLCA(g, e)->root(), "e");
    EXPECT_EQ(lca->getLCA(h, h)->root(), "h");
    EXPECT_EQ(lca->getLCA(root, root)->root(), "a");
}

TEST_F(LCATest, LCASelf) {
    // LCA на възел със себе си
    EXPECT_EQ(lca->getLCA(c, c), c);
    EXPECT_EQ(lca->getLCA(d, d), d);
    EXPECT_EQ(lca->getLCA(root, root), root);
}

TEST_F(LCATest, LCAParentChild) {
    // LCA на родител и дете
    EXPECT_EQ(lca->getLCA(b, c)->root(), "b");  // b е родител на c
    EXPECT_EQ(lca->getLCA(d, h)->root(), "d");  // d е родител на h
    EXPECT_EQ(lca->getLCA(d, f)->root(), "d");  // d е родител на f
    EXPECT_EQ(lca->getLCA(root, b)->root(), "a");  // a е родител на b
}

TEST_F(LCATest, LCASiblings) {
    // LCA на братя/сестри
    EXPECT_EQ(lca->getLCA(c, d)->root(), "b");  // c и d са деца на b
    EXPECT_EQ(lca->getLCA(h, f)->root(), "d");  // h и f са деца на d
    EXPECT_EQ(lca->getLCA(b, e)->root(), "a");  // b и e са деца на a
}

TEST_F(LCATest, LCACousins) {
    // LCA на "братовчеди" (възли на същата дълбочина, различни родители)
    EXPECT_EQ(lca->getLCA(c, g)->root(), "a");  // c (под b) и g (под e)
    EXPECT_EQ(lca->getLCA(h, g)->root(), "a");  // h (под d под b) и g (под e)
}

TEST_F(LCATest, InvalidInput) {
    // nullptr входни данни
    EXPECT_THROW(lca->getLCA(nullptr, c), std::runtime_error);
    EXPECT_THROW(lca->getLCA(b, nullptr), std::runtime_error);
    EXPECT_THROW(lca->getLCA(nullptr, nullptr), std::runtime_error);
}

TEST_F(LCATest, NodesNotInTree) {
    // Възли, които не са част от дървото
    Tree<std::string>* outsideNode = new Tree<std::string>("outside");
    
    EXPECT_THROW(lca->getLCA(outsideNode, c), std::runtime_error);
    EXPECT_THROW(lca->getLCA(c, outsideNode), std::runtime_error);
    
    delete outsideNode;
}

TEST_F(LCATest, SingleNodeTree) {
    // Тест за дърво само с един възел
    Tree<std::string>* single = new Tree<std::string>("single");
    LCA<std::string> single_lca(single);
    
    EXPECT_EQ(single_lca.getLCA(single, single)->root(), "single");
    
    delete single;
}

TEST_F(LCATest, TwoNodeTree) {
    // Тест за дърво с два възела
    Tree<std::string>* parent = new Tree<std::string>("parent");
    Tree<std::string>* child = new Tree<std::string>("child");
    parent->addSubtree(child);
    
    LCA<std::string> small_lca(parent);
    
    EXPECT_EQ(small_lca.getLCA(parent, child)->root(), "parent");
    EXPECT_EQ(small_lca.getLCA(child, child)->root(), "child");
    
    delete parent;
}

// Тест за всички възможни двойки (exhaustive test)
TEST_F(LCATest, AllPossiblePairs) {
    std::vector<Tree<std::string>*> all_nodes = {root, b, c, d, e, f, g, h};
    std::vector<std::string> expected_results[8][8];
    
    // Очаквани резултати
    // Индекси: 0:a, 1:b, 2:c, 3:d, 4:e, 5:f, 6:g, 7:h
    std::string expected[8][8] = {
        // a     b     c     d     e     f     g     h
        {"a", "a", "a", "a", "a", "a", "a", "a"},  // a
        {"a", "b", "b", "b", "a", "b", "a", "b"},  // b
        {"a", "b", "c", "b", "a", "b", "a", "b"},  // c
        {"a", "b", "b", "d", "a", "d", "a", "d"},  // d
        {"a", "a", "a", "a", "e", "a", "e", "a"},  // e
        {"a", "b", "b", "d", "a", "f", "a", "d"},  // f
        {"a", "a", "a", "a", "e", "a", "g", "a"},  // g
        {"a", "b", "b", "d", "a", "d", "a", "h"}   // h
    };
    
    // Тестваме всички двойки
    for (size_t i = 0; i < all_nodes.size(); i++) {
        for (size_t j = 0; j < all_nodes.size(); j++) {
            EXPECT_EQ(lca->getLCA(all_nodes[i], all_nodes[j])->root(), expected[i][j]);
        }
    }
}

// Edge case: много дълбоко дърво
TEST(LCADeepTreeTest, DeepTree) {
    // Създаваме верижно дърво: a -> b -> c -> d -> e
    Tree<std::string>* a = new Tree<std::string>("a");
    Tree<std::string>* b = new Tree<std::string>("b");
    Tree<std::string>* c = new Tree<std::string>("c");
    Tree<std::string>* d = new Tree<std::string>("d");
    Tree<std::string>* e = new Tree<std::string>("e");
    
    a->addSubtree(b);
    b->addSubtree(c);
    c->addSubtree(d);
    d->addSubtree(e);
    
    LCA<std::string> deep_lca(a);
    
    EXPECT_EQ(deep_lca.getLCA(a, e)->root(), "a");
    EXPECT_EQ(deep_lca.getLCA(b, d)->root(), "b");
    EXPECT_EQ(deep_lca.getLCA(c, e)->root(), "c");
    EXPECT_EQ(deep_lca.getLCA(d, e)->root(), "d");
    
    delete a;
}

// Тест за големи дървета (stress test)
TEST(LCAStressTest, LargeBinaryTree) {
    // Създаваме пълно двоично дърво с дълбочина 3 (15 възела)
    // Индекси в масив представяне:
    //          1 (0)
    //        /      \
    //      2 (1)     3 (2)
    //     /   \     /   \
    //   4 (3) 5(4) 6(5) 7(6)
    //  / \   / \   / \  / \
    // 8(7)9(8)...
    
    std::vector<Tree<int>*> nodes(15, nullptr); // 15 възела
    
    // Създаваме всички възли
    for (int i = 0; i < 15; i++) {
        nodes[i] = new Tree<int>(i + 1);  // Стойности 1..15
    }
    
    // Свързваме като пълно двоично дърво
    for (int i = 0; i < 7; i++) {  // Първите 7 възела имат деца
        int left_child = 2*i + 1;
        int right_child = 2*i + 2;
        
        if (left_child < 15) {
            nodes[i]->addSubtree(nodes[left_child]);
        }
        if (right_child < 15) {
            nodes[i]->addSubtree(nodes[right_child]);
        }
    }
    
    Tree<int>* root = nodes[0];  // Възел със стойност 1
    LCA<int> large_lca(root);
    
    // Тестваме различни двойки
    // Индекси: 0:1, 1:2, 2:3, 3:4, 4:5, 5:6, 6:7, 7:8, 8:9, ...
    
    // 2 и 3 -> LCA = 1
    EXPECT_EQ(large_lca.getLCA(nodes[1], nodes[2])->root(), 1);
    
    // 4 и 5 -> LCA = 2
    EXPECT_EQ(large_lca.getLCA(nodes[3], nodes[4])->root(), 2);
    
    // 8 и 9 -> LCA = 4
    EXPECT_EQ(large_lca.getLCA(nodes[7], nodes[8])->root(), 4);
    
    // 4 и 10 -> LCA = 2
    EXPECT_EQ(large_lca.getLCA(nodes[3], nodes[9])->root(), 2);
    
    // 6 и 7 -> LCA = 3
    EXPECT_EQ(large_lca.getLCA(nodes[5], nodes[6])->root(), 3);
    
    // 14 и 15 -> LCA = 7
    EXPECT_EQ(large_lca.getLCA(nodes[13], nodes[14])->root(), 7);
    
    // 5 и 6 -> LCA = 1 (различни родители)
    EXPECT_EQ(large_lca.getLCA(nodes[4], nodes[5])->root(), 1);
    
    // Почистване
    delete root;  // Това ще изтрие цялото дърво
}