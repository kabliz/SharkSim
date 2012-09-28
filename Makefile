C = g++
CCFLAGS = -O2 -pg
MACFLAGS= -framework OPENGL -framework GLUT 
LINFLAGS32= -lX11 -lglut -lGL -lGLU -lz
LINFLAGS64= -lglut -lGL -lGLU -lz
CCDEBUGFLAGS = -g
OUTNAME = runshark 
SRC_DIR = ./src
HDEP = *.h
FILES =*.cpp


runshark: $(FILES) $(HDEP)
	$(C) $(CCFLAGS) $(SRC_DIR)/*.cpp $(LINFLAGS32) -o $(OUTNAME)

runshark64: $(FILES) $(HDEP)
	$(C) $(CCFLAGS) $(SRC_DIR)/*.cpp $(LINFLAGS64) -o $(OUTNAME)

debug: $(FILES) $(HDEP)
	$(C) $(CCDEBUGFLAGS) $(SRC_DIR)/*.cpp $(LINFLAGS) -o $(OUTNAME) 

debug64: $(FILES) $(HDEP)
	$(C) $(CCDEBUGFLAGS) $(SRC_DIR)/*.cpp $(LINFLAGS64) -o $(OUTNAME) 


clean: 
	rm -f ./$(OUTNAME)
