#ifndef COLOR_CPP
#define COLOR_CPP

#include <ostream>

enum class Color
{
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,

    BrightBlack,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite,
};

std::ostream &operator<<(std::ostream &os, const Color &color)
{
    switch (color)
    {
    case Color::Black:
        return os << "\033[30m";
    case Color::Red:
        return os << "\033[31m";
    case Color::Green:
        return os << "\033[32m";
    case Color::Yellow:
        return os << "\033[33m";
    case Color::Blue:
        return os << "\033[34m";
    case Color::Magenta:
        return os << "\033[35m";
    case Color::Cyan:
        return os << "\033[36m";
    case Color::White:
        return os << "\033[37m";
    case Color::BrightBlack:
        return os << "\033[90m";
    case Color::BrightRed:
        return os << "\033[91m";
    case Color::BrightGreen:
        return os << "\033[92m";
    case Color::BrightYellow:
        return os << "\033[93m";
    case Color::BrightBlue:
        return os << "\033[94m";
    case Color::BrightMagenta:
        return os << "\033[95m";
    case Color::BrightCyan:
        return os << "\033[96m";
    case Color::BrightWhite:
        return os << "\033[97m";
    default:
        return os;
    }
}

enum class BackgroundColor
{
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,

    BrightBlack,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite,
};

std::ostream &operator<<(std::ostream &os, const BackgroundColor &color)
{
    switch (color)
    {
    case BackgroundColor::Black:
        return os << "\033[40m";
    case BackgroundColor::Red:
        return os << "\033[41m";
    case BackgroundColor::Green:
        return os << "\033[42m";
    case BackgroundColor::Yellow:
        return os << "\033[43m";
    case BackgroundColor::Blue:
        return os << "\033[44m";
    case BackgroundColor::Magenta:
        return os << "\033[45m";
    case BackgroundColor::Cyan:
        return os << "\033[46m";
    case BackgroundColor::White:
        return os << "\033[47m";
    case BackgroundColor::BrightBlack:
        return os << "\033[100m";
    case BackgroundColor::BrightRed:
        return os << "\033[101m";
    case BackgroundColor::BrightGreen:
        return os << "\033[102m";
    case BackgroundColor::BrightYellow:
        return os << "\033[103m";
    case BackgroundColor::BrightBlue:
        return os << "\033[104m";
    case BackgroundColor::BrightMagenta:
        return os << "\033[105m";
    case BackgroundColor::BrightCyan:
        return os << "\033[106m";
    case BackgroundColor::BrightWhite:
        return os << "\033[107m";
    default:
        return os;
    }
}

enum class Style
{
    Reset,
    Bold,
    Dim,
    Italic,
    Underline,
    Blink,
    FastBlink, // Note: Fast blink is not widely supported
    Reverse,
    Hidden,
    Strikethrough
};

std::ostream &operator<<(std::ostream &os, const Style &style)
{
    switch (style)
    {
    case Style::Reset:
        return os << "\033[0m";
    case Style::Bold:
        return os << "\033[1m";
    case Style::Dim:
        return os << "\033[2m";
    case Style::Italic:
        return os << "\033[3m";
    case Style::Underline:
        return os << "\033[4m";
    case Style::Blink:
        return os << "\033[5m";
    case Style::FastBlink:
        return os << "\033[6m"; // Note: Fast blink is not widely supported
    case Style::Reverse:
        return os << "\033[7m";
    case Style::Hidden:
        return os << "\033[8m";
    case Style::Strikethrough:
        return os << "\033[9m";
    default:
        return os;
    }
}

#endif