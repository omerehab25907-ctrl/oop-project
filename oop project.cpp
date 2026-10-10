#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include "image_Class.h"

using namespace std;

void filter_Grayscale(Image& img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            unsigned int avg = 0;
            for (int k = 0; k < 3; ++k) {
                avg += img.getPixel(i, j, k);
            }
            avg /= 3;
            for (int k = 0; k < 3; ++k) {
                img.setPixel(i, j, k, avg);
            }
        }
    }
}

void filter_DetectEdges(Image& img) {
    filter_Grayscale(img);
    Image target(img.width, img.height);
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < img.channels; ++k) {
                target.setPixel(i, j, k, 255);
            }
        }
    }
    int threshold = 30;
    for (int i = 0; i < img.width - 1; ++i) {
        for (int j = 0; j < img.height - 1; ++j) {
            int diffX = abs((int)img.getPixel(i + 1, j, 0) - (int)img.getPixel(i, j, 0));
            int diffY = abs((int)img.getPixel(i, j + 1, 0) - (int)img.getPixel(i, j, 0));
            if (diffX > threshold || diffY > threshold) {
                for (int k = 0; k < img.channels; ++k) {
                    target.setPixel(i, j, k, 0);
                }
            }
        }
    }
    img = target;
}

void filter_EdgeDetection2(Image& img) {
    for (int i = 0; i < img.width - 1; ++i) {
        for (int j = 0; j < img.height - 1; ++j) {
            int p1 = img.getPixel(i, j, 0);
            int p2 = img.getPixel(i + 1, j, 0);

            if (abs(p1 - p2) > 40) {
                img.setPixel(i, j, 0, 0);
                img.setPixel(i, j, 1, 0);
                img.setPixel(i, j, 2, 0);
            }
            else {
                img.setPixel(i, j, 0, 255);
                img.setPixel(i, j, 1, 255);
                img.setPixel(i, j, 2, 255);
            }
        }
    }
}

void filter_Invert(Image& img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < img.channels; ++k) {
                img.setPixel(i, j, k, 255 - img.getPixel(i, j, k));
            }
        }
    }
}

void filter_LightenDarken(Image& img, bool lighten) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < img.channels; ++k) {
                int val = img.getPixel(i, j, k);
                if (lighten) {
                    val = val + (255 - val) * 0.5;
                }
                else {
                    val = val * 0.5;
                }
                img.setPixel(i, j, k, min(255, max(0, val)));
            }
        }
    }
}

void sunlightFilter(Image& img) {
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            int r = img.getPixel(x, y, 0);
            int g = img.getPixel(x, y, 1);
            int b = img.getPixel(x, y, 2);

            r = static_cast<int>(r * 1.15 + 10);
            g = static_cast<int>(g * 1.08 + 10);
            b = static_cast<int>(b * 0.85);

            img.setPixel(x, y, 0, static_cast<unsigned char>(min(255, max(0, r))));
            img.setPixel(x, y, 1, static_cast<unsigned char>(min(255, max(0, g))));
            img.setPixel(x, y, 2, static_cast<unsigned char>(min(255, max(0, b))));
        }
    }
}

void filter_Resize(Image& img, int newWidth, int newHeight) {
    Image target(newWidth, newHeight);
    double scaleX = (double)img.width / newWidth;
    double scaleY = (double)img.height / newHeight;

    for (int i = 0; i < newWidth; ++i) {
        for (int j = 0; j < newHeight; ++j) {
            int srcX = i * scaleX;
            int srcY = j * scaleY;
            for (int k = 0; k < img.channels; ++k) {
                target.setPixel(i, j, k, img.getPixel(srcX, srcY, k));
            }
        }
    }
    img = target;
}

void mergeImages(Image& img1, Image& img2, Image& output, int mode) {
    int w, h;

    if (mode == 1) {
        w = max(img1.width, img2.width);
        h = max(img1.height, img2.height);
    }
    else {
        w = min(img1.width, img2.width);
        h = min(img1.height, img2.height);
    }

    Image result(w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int x1, y1, x2, y2;
            if (mode == 1) {
                x1 = x * img1.width / w;
                y1 = y * img1.height / h;
                x2 = x * img2.width / w;
                y2 = y * img2.height / h;
            }
            else {
                x1 = x2 = x;
                y1 = y2 = y;
            }
            for (int c = 0; c < 3; ++c) {
                int sum = img1.getPixel(x1, y1, c) + img2.getPixel(x2, y2, c);
                result.setPixel(x, y, c, static_cast<unsigned char>(sum / 2));
            }
        }
    }
    output = result;
}

void filter_TVEffect(Image& img) {
    for (int j = 0; j < img.height; ++j) {
        if (j % 2 == 0) {
            for (int i = 0; i < img.width; ++i) {
                for (int k = 0; k < img.channels; ++k) {
                    unsigned char val = img.getPixel(i, j, k);
                    img.setPixel(i, j, k, val * 0.5);
                }
            }
        }
    }
}

void filter_TVEffect2(Image& img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            if (j % 4 == 0) {
                unsigned char r = img.getPixel(i, j, 0);
                unsigned char g = img.getPixel(i, j, 1);
                unsigned char b = img.getPixel(i, j, 2);

                img.setPixel(i, j, 0, r * 0.5);
                img.setPixel(i, j, 1, g * 0.5);
                img.setPixel(i, j, 2, b * 0.5);
            }
        }
    }
}

void rotateImage(Image& img) {
    int choice;
    cout << "Choose angle (90, 180, 270): ";
    cin >> choice;

    int times = choice / 90;
    for (int t = 0; t < times; ++t) {
        Image rotated(img.height, img.width);
        for (int i = 0; i < img.width; ++i) {
            for (int j = 0; j < img.height; ++j) {
                for (int k = 0; k < img.channels; ++k) {
                    rotated.setPixel(j, img.width - 1 - i, k, img.getPixel(i, j, k));
                }
            }
        }
        img = rotated;
    }
}

int main() {
    string filename;
    cout << "Enter input image filename: ";
    cin >> filename;

    Image img(filename);
    int choice;

    while (true) {
        cout << "\n--- Image Processing Menu ---\n";
        cout << "1. Gray Scale\n";
        cout << "2. Invert Image\n";
        cout << "3. Detect Edges\n";
        cout << "4. Lighten / Darken Image\n";
        cout << "5. Merge Image\n";
        cout << "6. Resize Image\n";
        cout << "7. Sunlight\n";
        cout << "8. TV Effect\n";
        cout << "9. Rotate Image\n";
        cout << "10. Alternative Edge Detection\n";
        cout << "11. Alternative TV Effect\n";
        cout << "17. Save and Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        if (choice == 1) {
            filter_Grayscale(img);
            cout << "Gray Scale filter applied.\n";
        }
        else if (choice == 2) {
            filter_Invert(img);
            cout << "Invert filter applied.\n";
        }
        else if (choice == 3) {
            filter_DetectEdges(img);
            cout << "Detect Edges filter applied.\n";
        }
        else if (choice == 4) {
            int subChoice;
            cout << "1. Lighten\n2. Darken\nChoice: ";
            cin >> subChoice;
            filter_LightenDarken(img, subChoice == 1);
            cout << "Lighten/Darken filter applied.\n";
        }
        else if (choice == 5) {
            string filename2;
            cout << "Enter second image filename to load: ";
            cin >> filename2;

            Image img2(filename2);
            Image mergedImage;

            cout << "Choose merge mode:\n";
            cout << "1. Resize both images to the biggest width and height, then merge\n";
            cout << "2. Merge only the common area (smaller width and smaller height)\n";
            cout << "Choice: ";
            int mergeMode;
            cin >> mergeMode;

            mergeImages(img, img2, mergedImage, mergeMode);
            img = mergedImage;
            cout << "Merge filter applied.\n";
        }
        else if (choice == 6) {
            int w, h;
            cout << "Enter new width: ";
            cin >> w;
            cout << "Enter new height: ";
            cin >> h;
            filter_Resize(img, w, h);
            cout << "Resize filter applied.\n";
        }
        else if (choice == 7) {
            sunlightFilter(img);
            cout << "Sunlight filter applied.\n";
        }
        else if (choice == 8) {
            filter_TVEffect(img);
            cout << "TV Effect filter applied.\n";
        }
        else if (choice == 9) {
            rotateImage(img);
            cout << "Rotate filter applied.\n";
        }
        else if (choice == 10) {
            filter_EdgeDetection2(img);
            cout << "Alternative Edge Detection applied.\n";
        }
        else if (choice == 11) {
            filter_TVEffect2(img);
            cout << "Alternative TV Effect applied.\n";
        }
        else if (choice == 17) {
            string outFilename;
            cout << "Enter output image filename: ";
            cin >> outFilename;
            img.saveImage(outFilename);
            cout << "Image saved successfully. Exiting...\n";
            break;
        }
        else {
            cout << "Invalid choice! Try again.\n";
        }
    }

    return 0;
}
