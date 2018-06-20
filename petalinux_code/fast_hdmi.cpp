#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

string cascadeName;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
signed *fbp = 0;

void hdmi_init(){
	int fbfd = 0;
	long int screensize = 0;
	int count, i;
	
	/* Open the file for reading and writing */
        fbfd = open("/dev/fb0", O_RDWR);

        if (!fbfd) {

            printf("Error: cannot open framebuffer device.\n");
            exit(1);

        }

        printf("The framebuffer device was opened successfully.\n");


        /* Get fixed screen information */
        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {

            printf("Error reading fixed information.\n");
            exit(2);

        }


        /* Get variable screen information */
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {

            printf("Error reading variable information.\n");
            exit(3);

        }


        /* Figure out the size of the screen in bytes */
        screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;


        /* Map the device to memory */
            fbp = (signed *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);

        if (fbp == MAP_FAILED) {

            printf("Error: failed to map framebuffer device to memory.\n");
            exit(4);

        }
		
}


static void hdmi_show(Mat& outputimage)
{
	int x = 0, y = 0;
	long int location = 0;
	int width = outputimage.cols , height = outputimage.rows;

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{

			unsigned r, g, b;

			Point p(x % width, y % height);

			location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;

			b = outputimage.at<Vec3b>(p)[0];
			g = outputimage.at<Vec3b>(p)[1];
			r = outputimage.at<Vec3b>(p)[2];

			*(fbp + location) = r << 16 | g << 8 | b << 0;

		}
	}
}

static void detectAndDraw( Mat& img, CascadeClassifier& cascade )
{
    vector<Rect> faces;
    Mat gray;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    //equalizeHist( gray, gray );

    //-- Detect faces
	cascade.detectMultiScale( gray, faces,
        1.3, 5, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    
    
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];

        rectangle( img, cvPoint(cvRound(r.x), cvRound(r.y)),
                   cvPoint(cvRound(r.x + r.width-1), cvRound(r.y + r.height-1)),
                   Scalar(0,255,0), 3, 8, 0);
	
	printf("Detect a people, write test.bmp.\n");
	imwrite("test.bmp",img);

	char buf[1000];
	sprintf(buf," ./mosquitto_pub -h 140.113.216.71 -t final -f test.bmp");
	system(buf);
	printf(buf);
	printf("\n");
        
    }
    
	//-- Show what you got
	// imshow( "result", img );
    hdmi_show(img);
}

static void help()
{
    cout << "Usage:\n"
            "./facedetect [--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
               //"   [--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
               //"   [--try-flip]\n"
               //"   [filename|camera_index]\n\n"
            "example:\n"
            "./facedetect --cascade=\"../../data/haarcascades/haarcascade_frontalface_alt.xml\" \n\n"
            "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}


const char* keys =
{
    "{cascade|../../data/haarcascades/haarcascade_frontalface_alt.xml|}"
    "{@filename||}"
};

int main( int argc, const char** argv )
{
    VideoCapture capture;
    Mat frame;
    string inputName;
    CascadeClassifier cascade;
    
    CommandLineParser parser(argc, argv, keys);
    cascadeName = parser.get<string>("cascade");
    inputName = parser.get<string>("@filename");
    //-- 1. Load the cascades
	if (!cascade.load(cascadeName))
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        help();
        return -1;
    }
    //-- 2. Open the video stream
    if( inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1) )
    {
        int camera = inputName.empty() ? 0 : inputName[0] - '0';
        if(!capture.open(camera))
            cout << "Capture from camera #" <<  camera << " didn't work" << endl;
    }
    else
    {
        help();
        return -1;
    }

    cout << "Video capturing has been started ..." << endl;
	
	hdmi_init();
	
    for(;;)
    {
        //-- 3. Read the video stream
		capture >> frame;
        if( frame.empty() )
            break;

		//-- 4. Apply the classifier to the frame
        detectAndDraw( frame, cascade);

        // char c = (char)waitKey(10);
        // if( c == 27 || c == 'q' || c == 'Q' )
            // break;
    }
    return 0;
}
