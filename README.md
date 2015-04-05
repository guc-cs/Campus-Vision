Campus Vision
=============

Campus Vision is two sub-projects implemented during the 9th Semester's computer vision course, Winter Fall 2011. The main idea is to recognize robot-captured images of offices and campus booths at the GUC using C++ and OpenCV. Project description by TA: Ahmed Abdelgayed

Introduction
------------
The Project Campus Vision is an image and feature detection computer vision exercise using C++ and OpenCV v1.1. It is divided into two sub-projects:
* **Company Logo Detector** - Detects company names corresponding to logos found at booths present at the GUC campus despite different images orientation, different surrounding context and potential rotation when the user/robot captures these images.
* **Room No. Detector** - Detects Room numbers of different offices and classrooms at the GUC given some robot-captured images.

Screenshots
-----------
* Room Number Detector
![Screenshot of Room Number Detector](http://i.imgur.com/CBrLD76.jpg)
* Company Logo Detector
![Screenshot of Room Number Detector](http://i.imgur.com/M4qkE3z.jpg)


Features
--------
* Room Number Detector
  * Circle Detection
  * Gaussian Smoothing
  * Morphology closing of gaps to enhance input image
  * ROI extraction
  * Template matching against a set of template images represening images of numbers.
  * Using Normalized Square distance as a scoring function
* Company Logo Detector
  * Chamfer matching
  * Distance Transform
  * Canny Edge Detection
  * Apply Homography matrix and perspective transform on user selected ROI

How To
------
1. Install our patched version of opencv1.1 (Opencv 1.1 is too old now. We patched parts of it so it installs well on modern systems. Currently, we only tested it using Ubuntu 14.04 LTS x64)

  ```bash
  sh Opencv1.1-install.sh
  ```
  
  If the installation has succeeded, the previous command should end with the following two lines
  ```
  -I/usr/local/include/opencv
  -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml
  ```

  
2. Make

  ```bash
  make
  ```
  
3. Run the corresponding binary from the project sub-directory
  * For **Room Number Detector**
  ```bash
  cd Room\ Number\ Detector/
  ./RoomNumberDetector
  ```
  * For **Company Logo Detector**
  ```bash
  cd Company\ Logo\ Detector
  ./CompanyLogoDetector
  ```
  
4. You will be prompted with the location to input image in the command line:

  * For **Room Number Detector**, enter the path to a test image and hit enter:
    ```
    test images/image001.png
    ```
    * Keep hitting enter, it will show you step by step the series of transformations taken and finally print the room number corresponding to the image in the command line.
  
  * For **Company Detector**, enter the path to a dataset image and hit enter:
      ```
      dataset/IMG_0117.png
      ```
      * Use the mouse to make four _clockwise_ clicks corresponding to vertices of a quad surrounding an area where the logo lies.
      * First, you will be presented with the perspective transform (Upright logo), keep pressing enter, it will show you the list of steps taken and finally prints out the name of the best matching template file as well as the matching scores against all templates.
  

Contributors
------------
* Amr Tj. Wallas


License
-------
MIT. See [LICENSE](/LICENSE) file for more infomration
