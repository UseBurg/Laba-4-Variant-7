#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <regex>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


const int WIDTH = 800;
const int HEIGHT = 800;
const int RADIUS = 80; 


void generateFlowerOfLifeSVG(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return;

    file << "<svg width=\"" << WIDTH << "\" height=\"" << HEIGHT << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    int centerX = WIDTH / 2;
    int centerY = HEIGHT / 2;


    file << "  <circle cx=\"" << centerX << "\" cy=\"" << centerY << "\" r=\"" << RADIUS << "\" />\n";

   
    for (int ring = 1; ring <= 2; ++ring) { 
        for (int i = 0; i < 6; ++i) {
            double angle = i * M_PI / 3.0;
        
            int nx = centerX + round(ring * RADIUS * cos(angle));
            int ny = centerY + round(ring * RADIUS * sin(angle));
            file << "  <circle cx=\"" << nx << "\" cy=\"" << ny << "\" r=\"" << RADIUS << "\" />\n";

           
            if (ring > 1) {
                double nextAngle = (i + 1) * M_PI / 3.0;
                int midX = (nx + centerX + (int)round(ring * RADIUS * cos(nextAngle))) / 2; 
               
            }
        }
    }

    file << "</svg>";
    file.close();
}


void drawCircleBresenham(vector<vector<int>>& canvas, int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;

    auto plot = [&](int px, int py) {
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
            canvas[py][px] = 1;
        };

    while (y >= x) {
        plot(xc + x, yc + y); plot(xc - x, yc + y);
        plot(xc + x, yc - y); plot(xc - x, yc - y);
        plot(xc + y, yc + x); plot(xc - y, yc + x);
        plot(xc + y, yc - x); plot(xc - y, yc - x);

        if (d < 0) d = d + 4 * x + 6;
        else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}


void processSVG(const string& filename, vector<vector<int>>& canvas) {
    ifstream file(filename);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    regex circle_re("<circle[^>]*cx=\"([^\"]+)\"[^>]*cy=\"([^\"]+)\"[^>]*r=\"([^\"]+)\"");
    smatch match;

    auto start = content.cbegin();
    while (regex_search(start, content.cend(), match, circle_re)) {
        drawCircleBresenham(canvas, stoi(match[1]), stoi(match[2]), stoi(match[3]));
        start = match.suffix().first;
    }

}


void savePBM(const string& filename, const vector<vector<int>>& canvas) {
    ofstream file(filename);
    file << "P1\n" << WIDTH << " " << HEIGHT << "\n";
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            file << canvas[i][j] << (j % 70 == 0 ? "\n" : " ");
        }
        file << "\n";
    }
    file.close();
    cout << "Result in: " << filename << endl;
}

int main() {
    const string svgFile = "generated_flower.svg";
    const string pbmFile = "result.pbm";
    vector<vector<int>> canvas(HEIGHT, vector<int>(WIDTH, 0));

    generateFlowerOfLifeSVG(svgFile); 
    processSVG(svgFile, canvas);     
    savePBM(pbmFile, canvas);        

    return 0;
}
