sudo apt-get -y install gcc g++ pkg-config libgtk2.0-dev libjpeg-turbo8-dev libtiff4-dev libjasper-dev libpng12-dev zlib1g-dev build-essential

cd ./opencv-1.1.0

sed -i '133s/.*/#else/' ./opencv-1.1.0/cxcore/include/cxmisc.h

sudo ./configure --disable-apps --without-v4l --with-gtk --without-python

sudo make

sudo make install

sudo sh -c 'echo "/usr/local/lib" >  /etc/ld.so.conf.d/opencv.conf'

sudo ldconfig

sudo sh -c 'echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig" >> /etc/bash.bashrc'

sudo sh -c 'echo "export PKG_CONFIG_PATH" >> /etc/bash.bashrc'

sudo pkg-config --cflags opencv

sudo pkg-config --libs opencv
