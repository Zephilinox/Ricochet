#include "Core/Input/Keys.hpp"

//SELF

//LIBS
#define MAGIC_ENUM_RANGE_MIN (-512) // NOLINT(cppcoreguidelines-macro-usage)
#define MAGIC_ENUM_RANGE_MAX (512)  // NOLINT(cppcoreguidelines-macro-usage)
#include <magic_enum.hpp>

//STD

using namespace core;

Key core::keyFromString(const std::string& str)
{
    return magic_enum::enum_cast<Key>(str).value_or(Key::Unknown);
}

const char* core::keyToString(Key key)
{
    // magic enum guarantees this is a null terminated string
    return magic_enum::enum_name(key).data();
}

Button core::buttonFromString(const std::string& str)
{
    return magic_enum::enum_cast<Button>(str).value_or(Button::Unknown);
}

const char* core::buttonToString(Button button)
{
    // magic enum guarantees this is a null terminated string
    return magic_enum::enum_name(button).data();
}