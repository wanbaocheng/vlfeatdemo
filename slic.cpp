//
//  main.cpp
//  vlfeat-slic
//
//  Created by willard on 15/7/9.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

extern "C" {
#include <vl/generic.h>
#include <vl/slic.h>
}

int main(int argc, const char *argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    VL_PRINT("hello, VLFeat!\n");
    // Read the Lenna image. The matrix 'mat' will have 3 8 bit channels
    // corresponding to BGR color space.
    Mat mat = imread("../data/lena.jpg", cv::IMREAD_COLOR);

    // Convert image to one-dimensional array.
    float *image = new float[mat.rows * mat.cols * mat.channels()];
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            // Assuming three channels ...
            image[j + mat.cols * i + mat.cols * mat.rows * 0] = mat.at<cv::Vec3b>(i, j)[0];
            image[j + mat.cols * i + mat.cols * mat.rows * 1] = mat.at<cv::Vec3b>(i, j)[1];
            image[j + mat.cols * i + mat.cols * mat.rows * 2] = mat.at<cv::Vec3b>(i, j)[2];
        }
    }

    // The algorithm will store the final segmentation in a one-dimensional array.
    vl_uint32 *segmentation = new vl_uint32[mat.rows * mat.cols];
    vl_size height = mat.rows;
    vl_size width = mat.cols;
    vl_size channels = mat.channels();

    // The region size defines the number of superpixels obtained.
    // Regularization describes a trade-off between the color term and the
    // spatial term.
    vl_size region = 30;
    float regularization = 10000.;
    vl_size minRegion = 10;

    vl_slic_segment(segmentation, image, width, height, channels, region, regularization, minRegion);

    // Convert segmentation.
    int **labels = new int *[mat.rows];
    for (int i = 0; i < mat.rows; ++i) {
        labels[i] = new int[mat.cols];

        for (int j = 0; j < mat.cols; ++j) {
            labels[i][j] = (int) segmentation[j + mat.cols * i];
        }
    }

    int label = 0;
    int labelTop = -1;
    int labelBottom = -1;
    int labelLeft = -1;
    int labelRight = -1;

    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {

            label = labels[i][j];

            labelTop = label;
            if (i > 0) {
                labelTop = labels[i - 1][j];
            }

            labelBottom = label;
            if (i < mat.rows - 1) {
                labelBottom = labels[i + 1][j];
            }

            labelLeft = label;
            if (j > 0) {
                labelLeft = labels[i][j - 1];
            }

            labelRight = label;
            if (j < mat.cols - 1) {
                labelRight = labels[i][j + 1];
            }

            if (label != labelTop || label != labelBottom || label != labelLeft || label != labelRight) {
                mat.at<cv::Vec3b>(i, j)[0] = 0;
                mat.at<cv::Vec3b>(i, j)[1] = 0;
                mat.at<cv::Vec3b>(i, j)[2] = 255;
            }
        }
    }

    imwrite("Lenna_contours.png", mat);
    imshow("SLIC image", mat);
    waitKey();

    return 0;
}