#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

#define FONT_SIZE 30
#define OUTLINE_THICKNESS 1
#define FONT_STYLE sf::Text::Bold
#define FONT_COLOR sf::Color::White
#define OUTLINE_COLOR sf::Color(255, 255, 255, 50)
#define OUTER_CIRCLE_FILL_COLOR sf::Color::Transparent
#define CENTER_CIRCLE_FILL_COLOR sf::Color::White

// fonts
#define FONT_PATH "assets/fonts/Montserrat-Regular.ttf"

// shaders
#define VERTEXT_SHADER_PATH "assets/shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER_PATH "assets/shaders/fragment_shader.glsl"

// images
#define CLOCK_FACE_PATH "assets/images/clock_face.png"
#define HOUR_HAND_PATH "assets/images/hour_hand.png"
#define MINUTE_HAND_PATH "assets/images/minute_hand.png"
#define SECOND_HAND_PATH "assets/images/second_hand.png"
#define BACKGROUND_IMAGE_PATH "assets/images/background_image.png"

// sounds
#define SOUND_PATH "assets/sounds/tick_sound.wav"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define CLOCK_RADIUS 250
#define CENTER_CIRCLE_RADIUS 10
#define CENTER_X 400
#define CENTER_Y 400

#define BLUR_RADIUS 0.06f

// ------------------------------------------------------------------------------
// SFML (Simple and Fast Multimedia Library)
// ------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Update the view to maintain aspect ratio when the window is resized.
//------------------------------------------------------------------------------
void updateView(sf::RenderWindow &window)
{
    sf::Vector2u windowSize = window.getSize();
    float aspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    // create a square view at (0,0) with size (WINDOW_WIDTH, WINDOW_HEIGHT)
    sf::View view(sf::FloatRect({0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}));

    // ie. when width > height
    if (aspectRatio > 1.0f)
    {
        // change width to maintain aspect ratio
        view.setSize({WINDOW_WIDTH * aspectRatio, WINDOW_HEIGHT});
    }
    else
    {
        // change height to maintain aspect ratio
        view.setSize({WINDOW_WIDTH, WINDOW_HEIGHT / aspectRatio});
    }
    window.setView(view);
}

//------------------------------------------------------------------------------
// Draws a line with a specified thickness and color between two points.
//------------------------------------------------------------------------------
void drawLine(sf::RenderWindow &window, float x1, float y1, float x2, float y2, float thickness, sf::Color color)
{
    sf::Vector2f point1(x1, y1);
    sf::Vector2f point2(x2, y2);
    sf::Vector2f difference = point2 - point1;
    float length = std::sqrt(difference.x * difference.x + difference.y * difference.y);

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setPosition(point1);
    line.setFillColor(color);

    float angle = std::atan2(difference.y, difference.x) * 180 / M_PI;
    line.setRotation(sf::degrees(angle));

    window.draw(line);
}

//------------------------------------------------------------------------------
// Draws the clock face: an outer circle and a center circle.
//------------------------------------------------------------------------------
void drawClockFace(sf::RenderWindow &window)
{
    // Outer circle representing the clock's edge.
    sf::CircleShape outerCircle(CLOCK_RADIUS);
    outerCircle.setFillColor(OUTER_CIRCLE_FILL_COLOR);
    outerCircle.setOutlineThickness(OUTLINE_THICKNESS);
    outerCircle.setOutlineColor(OUTLINE_COLOR);
    outerCircle.setPosition(sf::Vector2f(CENTER_X - CLOCK_RADIUS, CENTER_Y - CLOCK_RADIUS));
    window.draw(outerCircle);

    // Small center circle.
    sf::CircleShape centerCircle(CENTER_CIRCLE_RADIUS);
    centerCircle.setFillColor(CENTER_CIRCLE_FILL_COLOR);
    centerCircle.setPosition(sf::Vector2f(CENTER_X - CENTER_CIRCLE_RADIUS, CENTER_Y - CENTER_CIRCLE_RADIUS));
    window.draw(centerCircle);
}

//------------------------------------------------------------------------------
// Draws the clock numbers (1 to 12) along the circumference of the clock face.
//------------------------------------------------------------------------------
void drawClockNumbers(sf::RenderWindow &window, const sf::Font &font)
{
    int offset = 25;
    for (int i = 1; i <= 12; i++)
    {
        int x = CENTER_X + (CLOCK_RADIUS - offset) * sin(i * M_PI / 6);
        int y = CENTER_Y - (CLOCK_RADIUS - offset) * cos(i * M_PI / 6);

        sf::Text text(font);
        text.setString(std::to_string(i));
        text.setCharacterSize(FONT_SIZE);
        text.setFillColor(FONT_COLOR);
        text.setStyle(FONT_STYLE);

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
void drawRealisticHand(sf::RenderWindow &window, float angle, float length, float baseWidth, float tipWidth, sf::Color color)
{
    sf::ConvexShape hand;
    hand.setPointCount(4);

    // Calculate the positions of the hand's vertices.
    float x1 = CENTER_X - baseWidth / 2 * cos(angle);
    float y1 = CENTER_Y - baseWidth / 2 * sin(angle);
    float x2 = CENTER_X + baseWidth / 2 * cos(angle);
    float y2 = CENTER_Y + baseWidth / 2 * sin(angle);
    float x3 = CENTER_X + length * sin(angle) + tipWidth / 2 * cos(angle);
    float y3 = CENTER_Y - length * cos(angle) + tipWidth / 2 * sin(angle);
    float x4 = CENTER_X + length * sin(angle) - tipWidth / 2 * cos(angle);
    float y4 = CENTER_Y - length * cos(angle) - tipWidth / 2 * sin(angle);

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
void drawClockHands(sf::RenderWindow &window, int hour, int minute, int second)
{
    // Convert current time to angles in radians.
    double h_rad = (hour % 12) * (M_PI / 6) + (minute * M_PI / 360) + (second * M_PI / 21600);
    double m_rad = minute * (M_PI / 30) + (second * M_PI / 1800);
    double s_rad = second * (M_PI / 30);

    // Draw the hour hand (widest at base, narrower at tip).
    drawRealisticHand(window, h_rad, CLOCK_RADIUS * 0.5, 16, 3, sf::Color::Red);

    // Draw the minute hand (medium width).
    drawRealisticHand(window, m_rad, CLOCK_RADIUS * 0.65, 12, 2, sf::Color::Green);

    // Draw the second hand (thinnest).
    drawRealisticHand(window, s_rad, CLOCK_RADIUS * 0.9, 8, 1, sf::Color::White);
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
    timeText.setCharacterSize(FONT_SIZE);
    timeText.setFillColor(FONT_COLOR);
    timeText.setStyle(FONT_STYLE);
    timeText.setPosition({350, 0});

    window.draw(timeText);
}

//------------------------------------------------------------------------------
// Draws the background with circular blur effect
//------------------------------------------------------------------------------
void drawBackground(sf::RenderWindow &window, sf::Sprite &backgroundSprite, sf::Shader &shader)
{
    // Create render texture for blurred version (create once)
    sf::RenderTexture blurTexture({window.getSize().x, window.getSize().y});

    // 1. Draw fully blurred background
    blurTexture.clear();
    shader.setUniform("blur_radius", BLUR_RADIUS); // Use your blur strength
    blurTexture.draw(backgroundSprite, &shader);
    blurTexture.display();

    // 2. Create circular mask for blurred area
    sf::CircleShape blurCircle(CLOCK_RADIUS);
    blurCircle.setTexture(&blurTexture.getTexture()); // Use blurred texture
    blurCircle.setTextureRect(sf::IntRect({CENTER_X - CLOCK_RADIUS,
                                           CENTER_Y - CLOCK_RADIUS},
                                          {CLOCK_RADIUS * 2,
                                           CLOCK_RADIUS * 2}));
    blurCircle.setPosition({CENTER_X - CLOCK_RADIUS, CENTER_Y - CLOCK_RADIUS});

    // 3. Draw final composite
    window.draw(backgroundSprite); // Original sharp background
    window.draw(blurCircle);       // Blurred circular area
}

//------------------------------------------------------------------------------
// Main function: sets up the window, loads resources, and runs the main loop.
//------------------------------------------------------------------------------
int main()
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8; // To get anti - aliased shapes(i.e.shapes with smoothed edges)
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Clock");

    //------------------------------------------------------------------------------
    // load font
    //------------------------------------------------------------------------------
    sf::Font font;
    if (!font.openFromFile(FONT_PATH))
    {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    //------------------------------------------------------------------------------
    // load sound
    //------------------------------------------------------------------------------
    sf::SoundBuffer tickBuffer;
    if (!tickBuffer.loadFromFile(SOUND_PATH))
    {
        std::cerr << "Failed to load tick sound!" << std::endl;
        return -1;
    }
    sf::Sound tickSound(tickBuffer);

    //------------------------------------------------------------------------------
    // load background image
    //------------------------------------------------------------------------------sf::Texture backgroundTexture;
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile(BACKGROUND_IMAGE_PATH))
    {
        std::cerr << "Failed to load background image!" << std::endl;
        return -1;
    }
    auto [width, height] = backgroundTexture.getSize();
    sf::Sprite backgroundSprite(backgroundTexture);

    // Set the scale of the background image to match the window size
    backgroundSprite.setScale(
        {static_cast<float>(window.getSize().x) / width,
         static_cast<float>(window.getSize().y) / height});

    //------------------------------------------------------------------------------
    // load shaders
    //------------------------------------------------------------------------------
    sf::Shader shader;
    if (!shader.loadFromFile(VERTEXT_SHADER_PATH, sf::Shader::Type::Vertex) ||
        !shader.loadFromFile(FRAGMENT_SHADER_PATH, sf::Shader::Type::Fragment))
    {
        std::cerr << "Failed to load shaders!" << std::endl;
        return -1;
    }

    //------------------------------------------------------------------------------
    // For precise timing control
    //------------------------------------------------------------------------------
    sf::Clock loopClock;            // For measuring elapsed time
    float timeAccumulator = 0.0f;   // For accumulating elapsed time
    const float targetDelay = 1.0f; // 1 second delay

    while (window.isOpen())
    {
        //------------------------------------------------------------------------------
        // Measure elapsed time and Accumulate elapsed time until it reaches target delay
        //------------------------------------------------------------------------------
        float elapsedTime = loopClock.restart().asSeconds();
        timeAccumulator += elapsedTime;

        //------------------------------------------------------------------------------
        // handle window close event
        //------------------------------------------------------------------------------
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::Resized>())
                updateView(window);
        }

        //------------------------------------------------------------------------------
        // update clock if accumulator exceeds target delay
        //------------------------------------------------------------------------------
        if (timeAccumulator >= targetDelay)
        {
            //------------------------------------------------------------------------------
            // decrease time accumulator by target delay
            //------------------------------------------------------------------------------
            timeAccumulator -= targetDelay;

            //------------------------------------------------------------------------------
            // Get current time
            //------------------------------------------------------------------------------
            time_t now = time(0);
            tm *ltm = localtime(&now);
            int hour = ltm->tm_hour;
            int minute = ltm->tm_min;
            int second = ltm->tm_sec;

            //------------------------------------------------------------------------------
            // stop prev sound and play new sound
            //------------------------------------------------------------------------------
            tickSound.stop();
            tickSound.play();

            //------------------------------------------------------------------------------
            // Clear and redraw
            //------------------------------------------------------------------------------
            window.clear();
            drawBackground(window, backgroundSprite, shader);
            drawClockFace(window);
            drawClockNumbers(window, font);
            drawClockHands(window, hour, minute, second);
            drawTimeText(window, hour, minute, second, font);
            window.display();
        }

        //------------------------------------------------------------------------------
        // Avoid 100% CPU usage
        //------------------------------------------------------------------------------

        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}