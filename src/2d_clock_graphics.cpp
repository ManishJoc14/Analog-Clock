#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <ctime>
#include <stdio.h>

void drawLine(int x1, int y1, int x2, int y2, int thickness, int color)
{
    setcolor(color);
    setlinestyle(SOLID_LINE, 0, thickness);
    line(x1, y1, x2, y2);
}

int main()
{
    initwindow(500, 500, "Clock");

    int R = 180;
    int x1 = 250, y1 = 250;

    int hour, minute, second;
    int h_x2, h_y2, m_x2, m_y2, s_x2, s_y2;
    double h_deg, m_deg, s_deg;
    int page = 0;

    while (1)
    {
        cleardevice(); // Clear the screen first

        // Get current time
        time_t now = time(0);
        tm *ltm = localtime(&now);

        hour = ltm->tm_hour;
        minute = ltm->tm_min;
        second = ltm->tm_sec;

        // Convert to angles (in radians)
        h_deg = (hour % 12) * (M_PI / 6) + (minute * M_PI / 360) + (second * M_PI / 21600);
        m_deg = minute * (M_PI / 30) + (second * M_PI / 1800);
        s_deg = second * (M_PI / 30);

        // Define different hand lengths
        int h_length = R * 0.5;
        int m_length = R * 0.7;
        int s_length = R * 0.9;

        // Calculate end points
        h_x2 = x1 + h_length * sin(h_deg);
        h_y2 = y1 - h_length * cos(h_deg);

        m_x2 = x1 + m_length * sin(m_deg);
        m_y2 = y1 - m_length * cos(m_deg);

        s_x2 = x1 + s_length * sin(s_deg);
        s_y2 = y1 - s_length * cos(s_deg);

        // Draw outer and center circles
        setcolor(WHITE);
        circle(x1, y1, R);
        circle(x1, y1, 5);

        // Draw numbers in bold
        settextstyle(BOLD_FONT, HORIZ_DIR, 2);
        for (int i = 1; i <= 12; i++)
        {
            int x = x1 + (R - 25) * sin(i * M_PI / 6);
            int y = y1 - (R - 25) * cos(i * M_PI / 6);

            char num[3];
            sprintf(num, "%d", i);

            int textWidth = textwidth(num);
            int textHeight = textheight(num);

            // Adjust position to center the numbers
            outtextxy(x - textWidth / 2, y - textHeight / 2, num);
        }

        // Display time at the top
        char timeStr[10];
        sprintf(timeStr, "%02d:%02d:%02d", hour, minute, second);
        settextstyle(BOLD_FONT, HORIZ_DIR, 3);
        outtextxy(200, 0, timeStr);

        // Draw hands with thickness
        drawLine(x1, y1, h_x2, h_y2, 3, RED);
        drawLine(x1, y1, m_x2, m_y2, 2, GREEN);
        drawLine(x1, y1, s_x2, s_y2, 1, WHITE);

        if (GetAsyncKeyState(VK_RETURN))
        {
            break;
        }

        delay(1000); // Update every second
        page = 1 - page;
        setactivepage(page);
        setvisualpage(page);
    }

    getch();
    closegraph();
    return 0;
}
