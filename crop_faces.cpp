// Licence

/*******************************************************************************
 * @brief face cropping program                                                *
 *                                                                             *
 * @file    crop_faces.cpp                                                     *
 * @author  Gergő Bogacsovics                                                  *
 *                                                                             *
 * @section LICENSE                                                            *
 *                                                                             *
 * Copyright (C) 2017 Gergő Bogacsovics, bgeri74@gmail.com                     *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
 *                                                                             *
 *                                                                             *
 * The following sites/projects were used as references:                       *
 * 1. The opencv website                                                       *
 * - http://opencv.org/                                                        *
 * - http://docs.opencv.org/2.4/doc/tutorials/tutorials.html                   *
 * - http://docs.opencv.org/2.4/modules/refman.html                            *
 * 2. The SamuCam project                                                      *
 * - https://github.com/nbatfai/SamuCam                                        *
 *******************************************************************************/

// Usage: ./app location_of_video

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace cv;

CascadeClassifier faceClassifier;

// The current index of the images.
// It starts from 0.
int INDEX_OF_IMAGE = 0;

// The size of the output image in pixels.
const Size OUTPUT_IMAGE_SIZE(280,280);

/*
  @param  location - the location of the image to be processed
  @return true if the image was successfully loaded, false if it was not

  Processes an image by its location.
  The function loads the image from the given location
  and then detects the faces.
  The detected faces are stored in the save folder given by the user.
*/
bool processImg(string location){

  cv::Mat frame, originalFrame;

  frame = imread(location);
  originalFrame = frame;

  if (frame.empty())
    return false;

  std::vector<cv::Rect> faces;
  cv::Mat grayFrame;

  cv::cvtColor ( frame, grayFrame, cv::COLOR_BGR2GRAY );
  cv::equalizeHist ( grayFrame, grayFrame );

  faceClassifier.detectMultiScale ( grayFrame, faces, 1.1, 4, 0, cv::Size ( 60, 60 ) );

  for(int i = 0; i < faces.size(); i++){

    Point pt1(faces[i].x-1, faces[i].y-1);
    Point pt2(faces[i].x + faces[i].width + 2, faces[i].y + faces[i].height+2);

    // Crop the images
    Mat croppedImg = frame(Rect(pt1.x, pt1.y, (pt2.x - pt1.x), (pt2.y - pt1.y)));

    resize ( frame, frame, OUTPUT_IMAGE_SIZE, 0, 0, INTER_CUBIC );

    imwrite("cropped_" + to_string(INDEX_OF_IMAGE + 1) + ".jpg", croppedImg);

    INDEX_OF_IMAGE++;

  }

  return true;

}

/*
  @param  frame - a Mat variable in which the frame is stored
  @param  saveDir - the save directory
  @return true if the image was successfully loaded, false if it was not

  Processes an image given in a Mat variable and stores it in a save directory.
  The function loads the image from the given location
  and then detects the faces.
  The detected faces are stored in the save folder given by the user.
*/
bool processImg(const Mat& frame, const string& saveDir){

  cv::Mat originalFrame;

  originalFrame = frame;

  if (frame.empty())
    return false;

  mkdir(saveDir.c_str(),S_IRUSR | S_IWUSR | S_IXUSR);

  std::vector<cv::Rect> faces;
  cv::Mat grayFrame;

  cv::cvtColor ( frame, grayFrame, cv::COLOR_BGR2GRAY );
  cv::equalizeHist ( grayFrame, grayFrame );

  faceClassifier.detectMultiScale ( grayFrame, faces, 1.1, 4, 0, cv::Size ( 60, 60 ) );


  for(int i = 0; i < faces.size(); i++){

    Point pt1(faces[i].x-1, faces[i].y-1);
    Point pt2(faces[i].x + faces[i].width + 2, faces[i].y + faces[i].height+2);

    // Crop the images
    Mat croppedImg = frame(Rect(pt1.x, pt1.y, (pt2.x - pt1.x), (pt2.y - pt1.y)));

    resize ( croppedImg, croppedImg, OUTPUT_IMAGE_SIZE);
    imwrite(saveDir + "/cropped_" + to_string(INDEX_OF_IMAGE + 1) + ".jpg", croppedImg);

    INDEX_OF_IMAGE++;

  }

  return true;

}

/*
  Prints the correct usage of the program.
*/
void usage(){
  cout << "The usage of the program is the following:\n";
  cout << "./app input_video save_directory" << endl;
}

int main( int argc, char *argv[] ){

  if(argc < 3){
    cout << "Wrong number of arguments. The program terminates." << endl;

    usage();

    return -1;
  }

  string location = argv[1];
  string saveDirectory = argv[2];

  string faceXML = "lbpcascade_frontalface.xml"; // https://github.com/Itseez/opencv/tree/master/data/lbpcascades

  if (!faceClassifier.load(faceXML)){
    cerr << "An error occured while processing the xml file. The program terminates." << endl;
    return -1;
  }

  VideoCapture cap(location);

  if(!cap.isOpened()){
    cerr << "An error occured while trying to open the video. The program terminates." << endl;

    usage();

    return -1;
  }

  Mat frame;
  cap >> frame;

  while(!frame.empty()){
      processImg(frame, saveDirectory);
      cap >> frame;
  }

  return 0;
}
