OUTNAME = runshark 
SRC_DIR = src

deploy:
	mv $(SRC_DIR)/$(OUTNAME) ./

runshark:
	cd $(SRC_DIR); make runshark 
	make deploy

runshark64:
	cd $(SRC_DIR); make runshark64 
	make deploy

debug:
	cd $(SRC_DIR); make debug  
	make deploy

debug64:
	cd $(SRC_DIR); make debug64 
	make deploy 

.PHONY: clean

clean: 
	rm -f ./$(OUTNAME)


