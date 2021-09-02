//SELF
#include "Client/Game.hpp"

//LIBS
#include <doctest/doctest.h>

//STD

TEST_CASE("test")
{
    CHECK_EQ(rico::client::Game::add(1, 2), 3);
}