#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

int main(int argc, char* argv[]) {
    std::string arquivo;
    if (argc == 1) {
        arquivo = "filmagem1.mp4";
    } else {
        arquivo = "filmagem2.mp4";
    }
    cv::VideoCapture pendulo(arquivo);

    if (!pendulo.isOpened()) {
        std::cout << "Erro ao abrir o video '" << arquivo << "'!\n";
        return 1;
    }

    double fps = pendulo.get(cv::CAP_PROP_FPS);
    std::cout << "video " << arquivo << " de " << fps << " fps carregado!\n\n";

    std::ofstream saida("dados.txt");
    int i = 0;
    while (true) {
        cv::Mat frame;
        pendulo >> frame;
        if (frame.empty()) {
            break;
        }
        ++i;
        cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);
        cv::Mat hsv_image;
        cv::cvtColor(frame, hsv_image, cv::COLOR_BGR2HSV);
        cv::Mat lower_red_hue_range;
        cv::Mat upper_red_hue_range;
        cv::inRange(hsv_image, cv::Scalar(0, 110, 35), cv::Scalar(20, 255, 255), lower_red_hue_range);
        cv::inRange(hsv_image, cv::Scalar(150, 110, 35), cv::Scalar(179, 255, 255), upper_red_hue_range);

        cv::Mat red_hue_image;
        cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(red_hue_image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 1);
        cv::Moments m;
        if (contours.size() != 1) {
            std::cout << "aviso: frame " << i << " ignorado!\n";
            cv::imshow("Frame", frame);
            cv::waitKey(3000);
        } else {
            m           = cv::moments(contours[0]);
            double x_cm = m.m10 / m.m00;
            // double y_cm = m.m01 / m.m00; não usado
            saida << x_cm << " | " << i * 1.0 / fps << '\n';
        }
    }
    std::cout << "Dados escritos em dados.txt\n";
    return 0;
}