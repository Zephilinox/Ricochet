//SELF
#include "Core/Window/WindowNone.hpp"

//LIBS
#include <doctest/doctest.h>

//STD

TEST_CASE("WindowNone/isOpen & close")
{
    core::WindowNone window({});
    CHECK(window.isOpen());
    window.close();
    CHECK(!window.isOpen());
}

TEST_CASE("WindowNone/Send & Poll")
{
    core::WindowNone window({});
    core::Event event;
    CHECK(std::holds_alternative<core::EventNone>(event));
    CHECK(!window.poll(event));
    CHECK(std::holds_alternative<core::EventNone>(event));

    window.send(core::EventQuit{});
    auto found_event = window.poll(event);
    CHECK(found_event);
    CHECK(std::holds_alternative<core::EventQuit>(event));
    CHECK(!window.poll(event));
}

struct MyEvent : public core::EventCustom
{
    MyEvent(std::string s)
        : core::EventCustom(type_v)
        , super_secret(std::move(s))
    {
    }

    std::string super_secret;
    static constexpr std::int32_t type_v = 1;
};

TEST_CASE("WindowNone/Send & Poll Custom")
{
    core::WindowNone window({});
    core::Event event;
    CHECK(std::holds_alternative<core::EventNone>(event));
    CHECK(!window.poll(event));
    CHECK(std::holds_alternative<core::EventNone>(event));

    window.send(std::make_unique<MyEvent>("Hello World"));
    auto found_event = window.poll(event);
    CHECK(found_event);
    CHECK(std::holds_alternative<std::unique_ptr<core::EventCustom>>(event));

    auto custom_event = std::get<std::unique_ptr<core::EventCustom>>(std::move(event));
    auto* my_event = custom_event->as<MyEvent>();
    REQUIRE(my_event);
    CHECK(my_event->super_secret == "Hello World");

    CHECK(!window.poll(event));
}