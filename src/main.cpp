#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
// Draws a line with a specified thickness and color between two points.
//------------------------------------------------------------------------------
void drawLine(sf::RenderWindow &window, float x1, float y1, float x2, float y2, float thickness, sf::Color color)
{
    sf::Vector2f point1(x1, y1);
    sf::Vector2f point2(x2, y2);
    sf::Vector2f direction = point2 - point1;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setPosition(point1);
    line.setFillColor(color);

    float angle = std::atan2(direction.y, direction.x) * 180 / M_PI;
    line.setRotation(sf::degrees(angle));

    window.draw(line);
}

//------------------------------------------------------------------------------
// Draws the clock face: an outer circle and a center circle.
//------------------------------------------------------------------------------
void drawClockFace(sf::RenderWindow &window, int centerX, int centerY, int radius)
{
    // Outer circle representing the clock's edge.
    sf::CircleShape outerCircle(radius);
    outerCircle.setFillColor(sf::Color::Transparent);
    outerCircle.setOutlineThickness(2);
    outerCircle.setOutlineColor(sf::Color::White);
    outerCircle.setPosition(sf::Vector2f(centerX - radius, centerY - radius));
    window.draw(outerCircle);

    // Small center circle.
    sf::CircleShape centerCircle(10);
    centerCircle.setFillColor(sf::Color::White);
    centerCircle.setPosition(sf::Vector2f(centerX - 10, centerY - 10));
    window.draw(centerCircle);
}

//------------------------------------------------------------------------------
// Draws the clock numbers (1 to 12) along the circumference of the clock face.
//------------------------------------------------------------------------------
void drawClockNumbers(sf::RenderWindow &window, int centerX, int centerY, int radius, const sf::Font &font)
{
    for (int i = 1; i <= 12; i++)
    {
        int x = centerX + (radius - 25) * sin(i * M_PI / 6);
        int y = centerY - (radius - 25) * cos(i * M_PI / 6);

        sf::Text text(font);
        text.setString(std::to_string(i));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                        textRect.position.y + textRect.size.y / 2.0f});
        text.setPosition(sf::Vector2f(x, y));

        window.draw(text);
    }
}

//------------------------------------------------------------------------------
// Draws a realistic clock hand with varying width.
//------------------------------------------------------------------------------
void drawRealisticHand(sf::RenderWindow &window, int centerX, int centerY, float angle, float length, float baseWidth, float tipWidth, sf::Color color)
{
    sf::ConvexShape hand;
    hand.setPointCount(4);

    // Calculate the positions of the hand's vertices.
    float x1 = centerX - baseWidth / 2 * cos(angle);
    float y1 = centerY - baseWidth / 2 * sin(angle);
    float x2 = centerX + baseWidth / 2 * cos(angle);
    float y2 = centerY + baseWidth / 2 * sin(angle);
    float x3 = centerX + length * sin(angle) + tipWidth / 2 * cos(angle);
    float y3 = centerY - length * cos(angle) + tipWidth / 2 * sin(angle);
    float x4 = centerX + length * sin(angle) - tipWidth / 2 * cos(angle);
    float y4 = centerY - length * cos(angle) - tipWidth / 2 * sin(angle);

    hand.setPoint(0, sf::Vector2f(x1, y1));
    hand.setPoint(1, sf::Vector2f(x2, y2));
    hand.setPoint(2, sf::Vector2f(x3, y3));
    hand.setPoint(3, sf::Vector2f(x4, y4));

    hand.setFillColor(color);
    window.draw(hand);
}

//------------------------------------------------------------------------------
// Draws the clock hands based on the provided time (hour, minute, second).
//------------------------------------------------------------------------------
void drawClockHands(sf::RenderWindow &window, int centerX, int centerY, int radius, int hour, int minute, int second)
{
    // Convert current time to angles in radians.
    double h_rad = (hour % 12) * (M_PI / 6) + (minute * M_PI / 360) + (second * M_PI / 21600);
    double m_rad = minute * (M_PI / 30) + (second * M_PI / 1800);
    double s_rad = second * (M_PI / 30);

    // Draw the hour hand (widest at base, narrower at tip).
    drawRealisticHand(window, centerX, centerY, h_rad, radius * 0.5, 16, 3, sf::Color::Red);

    // Draw the minute hand (medium width).
    drawRealisticHand(window, centerX, centerY, m_rad, radius * 0.65, 12, 2, sf::Color::Green);

    // Draw the second hand (thinnest).
    drawRealisticHand(window, centerX, centerY, s_rad, radius * 0.9, 8, 1, sf::Color::White);
}

//------------------------------------------------------------------------------
// Draws the digital time text at the top of the window.
//------------------------------------------------------------------------------
void drawTimeText(sf::RenderWindow &window, int hour, int minute, int second, const sf::Font &font)
{
    std::ostringstream timeStr;
    timeStr << std::setw(2) << std::setfill('0') << hour << ":"
            << std::setw(2) << std::setfill('0') << minute << ":"
            << std::setw(2) << std::setfill('0') << second;

    sf::Text timeText(font);
    timeText.setString(timeStr.str());
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::White);
    timeText.setStyle(sf::Text::Bold);
    timeText.setPosition({350, 0});

    window.draw(timeText);
}

//------------------------------------------------------------------------------
// Main function: sets up the window, loads resources, and runs the main loop.
//------------------------------------------------------------------------------
int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Clock");

    int radius = 250;
    int centerX = 400, centerY = 400;

    sf::Font font;
    if (!font.openFromFile("Montserrat-Regular.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        time_t now = time(0);
        tm *ltm = localtime(&now);
        int hour = ltm->tm_hour;
        int minute = ltm->tm_min;
        int second = ltm->tm_sec;

        drawClockFace(window, centerX, centerY, radius);
        drawClockNumbers(window, centerX, centerY, radius, font);
        drawClockHands(window, centerX, centerY, radius, hour, minute, second);
        drawTimeText(window, hour, minute, second, font);

        window.display();

        sf::sleep(sf::seconds(1));
    }

    return 0;
}