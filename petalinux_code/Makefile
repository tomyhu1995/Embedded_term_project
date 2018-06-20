
all:
	arm-xilinx-linux-gnueabi-g++ ./fast_hdmi.cpp -o ./fast_hdmi.o \
	-I/home/tomyhu1995/Documents/LAB3/opencv-2.4.9/install/include \
	-I/home/tomyhu1995/Documents/LAB3/opencv-2.4.9/install/include/opencv \
	-L/home/tomyhu1995/Documents/LAB3/opencv-2.4.9/install/lib \
	-lopencv_core \
	-lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video \
	-lopencv_features2d -lopencv_calib3d -lopencv_legacy -lopencv_objdetect -lopencv_flann -lpthread

clean:
	rm -rf fast_hdmi.o 
