CC = g++
CFLAGS = -I/usr/local/include/opencv  -L/usr/local/lib -lcxcore -lcv -lhighgui -lcvaux -lml

COMPANY_LOGO_DETECTOR = CompanyLogoDetector
ROOM_NO_DETECTOR = RoomNumberDetector

all: $(COMPANY_LOGO_DETECTOR) $(ROOM_NO_DETECTOR)

$(COMPANY_LOGO_DETECTOR):
	$(CC) -o $(COMPANY_LOGO_DETECTOR) ./Company\ Logo\ Detector/Main.cpp $(CFLAGS)

$(ROOM_NO_DETECTOR):
	$(CC) -o $(ROOM_NO_DETECTOR) ./Room\ Number\ Detector/Main.cpp $(CFLAGS)

clean:
	$(RM) $(ROOM_NO_DETECTOR) $(COMPANY_LOGO_DETECTOR)	
