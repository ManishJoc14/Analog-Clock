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
    // Calculate the direction vector and its length.
    sf::Vector2f point1(x1, y1);
    sf::Vector2f point2(x2, y2);
    sf::Vector2f direction = point2 - point1;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Create a rectangle to represent the line.
    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setPosition(point1);
    line.setFillColor(color);

    // Compute the angle (in degrees) and set the rotation.
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
    sf::CircleShape centerCircle(5);
    centerCircle.setFillColor(sf::Color::White);
    centerCircle.setPosition(sf::Vector2f(centerX - 5, centerY - 5));
    window.draw(centerCircle);
}

//------------------------------------------------------------------------------
// Draws the clock numbers (1 to 12) along the circumference of the clock face.
//------------------------------------------------------------------------------
void drawClockNumbers(sf::RenderWindow &window, int centerX, int centerY, int radius, const sf::Font &font)
{
    for (int i = 1; i <= 12; i++)
    {
        // Calculate the position for each number.
        int x = centerX + (radius - 25) * sin(i * M_PI / 6);
        int y = centerY - (radius - 25) * cos(i * M_PI / 6);

        sf::Text text(font);
        text.setString(std::to_string(i));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        // Center the text by adjusting its origin based on its local bounds.
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                        textRect.position.y + textRect.size.y / 2.0f});
        text.setPosition(sf::Vector2f(x, y));

        window.draw(text);
    }
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

    // Determine the lengths for the hour, minute, and second hands.
    int h_length = radius * 0.5;
    int m_length = radius * 0.7;
    int s_length = radius * 0.9;

    // Calculate end points for each hand.
    int h_x2 = centerX + h_length * sin(h_rad);
    int h_y2 = centerY - h_length * cos(h_rad);
    int m_x2 = centerX + m_length * sin(m_rad);
    int m_y2 = centerY - m_length * cos(m_rad);
    int s_x2 = centerX + s_length * sin(s_rad);
    int s_y2 = centerY - s_length * cos(s_rad);

    // Draw the hands with different thicknesses and colors.
    drawLine(window, centerX, centerY, h_x2, h_y2, 3, sf::Color::Red);
    drawLine(window, centerX, centerY, m_x2, m_y2, 2, sf::Color::Green);
    drawLine(window, centerX, centerY, s_x2, s_y2, 1, sf::Color::White);
}

//------------------------------------------------------------------------------
// Draws the digital time text at the top of the window.
//------------------------------------------------------------------------------
void drawTimeText(sf::RenderWindow &window, int hour, int minute, int second, const sf::Font &font)
{
    std::ostringstream timeStr;
    // Format the time with leading zeros (e.g., "09:05:03").
    timeStr << std::setw(2) << std::setfill('0') << hour << ":"
            << std::setw(2) << std::setfill('0') << minute << ":"
            << std::setw(2) << std::setfill('0') << second;

    sf::Text timeText(font);
    timeText.setString(timeStr.str());
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::White);
    timeText.setStyle(sf::Text::Bold);
    timeText.setPosition({200, 0});

    window.draw(timeText);
}

//------------------------------------------------------------------------------
// Main function: sets up the window, loads resources, and runs the main loop.
//------------------------------------------------------------------------------
int main()
{
    // Create an SFML window.
    sf::RenderWindow window(sf::VideoMode({500, 500}), "Clock");

    // Define clock parameters.
    int radius = 180;
    int centerX = 250, centerY = 250;

    // Load the font.
    sf::Font font;
    if (!font.openFromFile("Montserrat-Regular.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    // Main loop.
    while (window.isOpen())
    {
        // Process events.
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Clear the window.
        window.clear();

        // Get the current time.
        time_t now = time(0);
        tm *ltm = localtime(&now);
        int hour = ltm->tm_hour;
        int minute = ltm->tm_min;
        int second = ltm->tm_sec;

        // Draw the clock face, numbers, hands, and digital time.
        drawClockFace(window, centerX, centerY, radius);
        drawClockNumbers(window, centerX, centerY, radius, font);
        drawClockHands(window, centerX, centerY, radius, hour, minute, second);
        drawTimeText(window, hour, minute, second, font);

        // Display the updated frame.
        window.display();

        // Pause for 1 second.
        sf::sleep(sf::seconds(1));
    }

    return 0;
}
