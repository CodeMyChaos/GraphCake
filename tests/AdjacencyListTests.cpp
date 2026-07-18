#include <gtest/gtest.h>

using namespace testing;

import graphCake.representations;
import graphCake.graph;

namespace graphCake {
    namespace {
        struct AdjacencyListTests : public testing::Test {

            AdjacencyListTests() {
                constructed = 0;
                destroyed = 0;
                moved = 0;
                copied = 0;
            }

            struct Spy {
                Spy() { (*constructed)++; }
                Spy(const Spy&) { (*copied)++; }
                Spy(Spy&& other) noexcept {
                    (*moved)++;
                    other.destroyed = nullptr;
                    other.copied = nullptr;
                    other.moved = nullptr;
                    other.constructed = nullptr;
                }
                ~Spy() { if (destroyed) (*destroyed)++; }
                Spy& operator=(const Spy& other) { if (this == &other) return *this; (*copied)++; return *this; }
                Spy& operator=(Spy&& other) noexcept { (*moved)++; return *this; }
                operator double() const { return 1.0; }

                int* constructed = &(AdjacencyListTests::constructed);
                int* destroyed = &(AdjacencyListTests::destroyed);
                int* moved = &(AdjacencyListTests::moved);
                int* copied = &(AdjacencyListTests::copied);
            };

            static int constructed;
            static int destroyed;
            static int moved;
            static int copied;
        };

        int AdjacencyListTests::constructed = 0;
        int AdjacencyListTests::destroyed = 0;
        int AdjacencyListTests::moved = 0;
        int AdjacencyListTests::copied = 0;
    }

    TEST_F(AdjacencyListTests, defaultGraphDoesNotCreateOrDestroyAnyVertices) {
        {
            auto sut = representations::AdjacencyList<Spy, double>{};
        }
        EXPECT_EQ(constructed, 0);
        EXPECT_EQ(destroyed, 0);
    }

    TEST_F(AdjacencyListTests, graphCorrectlyManagesVerticesLifetime) {
        {
            auto sut = representations::AdjacencyList<Spy, double>{};
            sut.add_vertex();
            sut.add_vertex();
        }
        EXPECT_EQ(constructed, 2);
        EXPECT_GE(moved, 1); // account for reallocation
        EXPECT_EQ(copied, 0);
        EXPECT_EQ(destroyed, 2);
    }

    TEST_F(AdjacencyListTests, graphCorrectlyManagesUnidirectionalEdgesLifetime) {
        {
            auto sut = representations::AdjacencyList<int, Spy>{};
            sut.add_vertex();
            sut.add_vertex();
            (void)sut.add_edge(0, 1);
        }
        EXPECT_EQ(constructed, 1);
        EXPECT_EQ(moved, 0);
        EXPECT_EQ(copied, 0);
        EXPECT_EQ(destroyed, 1);
    }

    TEST_F(AdjacencyListTests, graphCorrectlyManagesBidirectionalEdgesLifetime) {
        {
            auto sut = representations::AdjacencyList<int, Spy>{};
            sut.add_vertex();
            sut.add_vertex();
            (void)sut.add_edge<graph::EdgeType::Bidirectional>(0, 1);
        }
        EXPECT_EQ(constructed, 1);
        EXPECT_GE(moved, 0);
        EXPECT_EQ(copied, 1);
        EXPECT_EQ(destroyed, 2);
    }

    TEST_F(AdjacencyListTests, graphCorrectlyManagesUnidirectionalProvidedEdgeLifetime) {
        {
            Spy spy{};
            auto sut = representations::AdjacencyList<int, Spy>{};
            sut.add_vertex();
            sut.add_vertex();
            (void)sut.add_edge(0, 1, std::move(spy));
        }
        EXPECT_EQ(constructed, 1); // the initial spy object
        EXPECT_EQ(moved, 1);
        EXPECT_EQ(copied, 0);
        EXPECT_EQ(destroyed, 1);
    }

    TEST_F(AdjacencyListTests, graphCorrectlyManagesBidirectionalProvidedEdgeLifetime) {
        {
            Spy spy{};
            auto sut = representations::AdjacencyList<int, Spy>{};
            sut.add_vertex();
            sut.add_vertex();
            (void)sut.add_edge<graph::EdgeType::Bidirectional>(0, 1, std::move(spy));
        }
        EXPECT_EQ(constructed, 1); // the initial spy object
        EXPECT_EQ(moved, 1);
        EXPECT_EQ(copied, 1);
        EXPECT_EQ(destroyed, 2);
    }

    TEST_F(AdjacencyListTests, addingBidirectionalLoopEdgeDoesNotCreateDuplicates) {
        {
            auto sut = representations::AdjacencyList<int, Spy>{};
            sut.add_vertex();
            sut.add_vertex();
            sut.add_edge<graph::EdgeType::Bidirectional>(0, 0);
        }
        EXPECT_EQ(constructed, 1);
        EXPECT_EQ(moved, 0);
        EXPECT_EQ(copied, 0);
        EXPECT_EQ(destroyed, 1);
    }

    TEST_F(AdjacencyListTests, defaultGraphDoesNotCreateOrDestroyAnyEdges) {
        {
            auto sut = representations::AdjacencyList<int, Spy>{};
        }
        EXPECT_EQ(constructed, 0);
        EXPECT_EQ(destroyed, 0);
    }
}
